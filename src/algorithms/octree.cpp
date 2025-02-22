#include "octree.h"
#include "../graphics/models/box.hpp"


void Octree::calculateBounds(BoundingRegion& out, Octant octant, BoundingRegion parentRegion)
{
    // ��������� ����� ������������� �������.
    // ����� ��������� ������������ ������������� ������� �� 8 ��������.
    glm::vec3 center = parentRegion.calculateCenter();

    // ���������, ����� ������ ������, � �� ������ ����� ������� ��������������� ������.
    if (octant == Octant::O1) {
        // ������ ������: �� ������ �� ������������ ��������� ������������� �������.
        out =  BoundingRegion(center, parentRegion.max);
    }
    else if (octant == Octant::O2) {
        // ������ ������: ����� �� ����������� x � ����������� y, z
        // �� ����������� x � ������������ y, z.
        out =  BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, center.z),
            glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O3) {
        // ������ ������: �� ����������� x, y � ����������� z 
        // �� ����������� x, y � ������������ z.
        out =  BoundingRegion(
            glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z),
            glm::vec3(center.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O4) {
        // ��������� ������: �� ����������� x, ����������� y � ����������� z
        // �� ������������ x, y � z.
        out =  BoundingRegion(
            glm::vec3(center.x, parentRegion.min.y, center.z),
            glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O5) {
        // ����� ������: �� ������ �� ������������ ��������� �� x, y,
        // �� ����������� �� z.
        out =  BoundingRegion(
            glm::vec3(center.x, center.y, parentRegion.min.z),
            glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O6) {
        // ������ ������: �� ����������� x, ����������� y � ����������� z
        // �� ����������� x, y � z.
        out =  BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z),
            glm::vec3(center.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O7) {
        // ������� ������: �� ����������� ��������� �� ������.
        out = BoundingRegion(parentRegion.min, center);
    }
    else if (octant == Octant::O8) {
        // ������� ������: �� ����������� x, ����������� y � z
        // �� ������������ ���������.
        out =  BoundingRegion(
            glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z),
            glm::vec3(parentRegion.max.x, center.y, center.z)
        );
    }


}

Octree::node::node() : region(BoundTypes::AABB)
{
}

Octree::node::node(BoundingRegion bounds) : region(bounds)
{
}

Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList) : region(bounds)
{ 
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::node::addToPending(RigidBody* instance, trie::Trie<Model*> models){
    //get all bounding regions of model
    for (BoundingRegion br : models[instance->modelId]->boundingRegions) {
        br.instance = instance;
        br.transform();  // ������������� ������� �������� ������ � �������� 
        queue.push(br);

    }


}

void Octree::node::addToPending(RigidBody* instance, Model* model){
    // get all bounding regions of model and put them in queue
    for (BoundingRegion br : model->boundingRegions) {
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

void Octree::node::build()
{ 
    //variables declarations
    glm::vec3 dimensions = region.calculateDimensions();
    BoundingRegion octants[NO_CHILDREN];
    std::vector<BoundingRegion> octLists[NO_CHILDREN]; // array of lists of objects in each octant
  

    /*
        termination conditions (don't subdivide further)
        - 1 or less objects (ie an empty leaf node or node with 1 object)
        - dimesnions are too small
    */

    // <= 1 objects
    if (objects.size() <= 1) {
        // set state variables
        goto setVars;
    }

    // too small
   
    for (int i = 0; i < 3; i++) {
        if (dimensions[i] < MIN_BOUNDS) {
            goto setVars;
        }
    }

    // create regions
  
    for (int i = 0; i < NO_CHILDREN; i++) {
        calculateBounds(octants[i], (Octant)(1 << i), region);
    }

    

    // determine which octants to place objects in
   
    for (int i = 0, len = (int)objects.size(); i < len; i++) {
        BoundingRegion br = objects[i];
        for (int j = 0; j < NO_CHILDREN; j++) {
            if (octants[j].containsRegion(br)) {
                // octant contains region
                octLists[j].push_back(br);
                objects.erase(objects.begin() + i);
                i--;
                len--;

                break;
            }
        }
    }


    // populate octants
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) {
            // if children go into this octant, generate new child
            children[i] = new node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i); // activate octant
            children[i]->parent = this;
            children[i]->build();
            hasChildren = true;
        }
    }



setVars:
    // set state variables
    treeBuilt = true;
    treeReady = true;

    // set pointer to current cell of each object
    for (int i = 0; i < objects.size(); i++) {
        objects[i].cell = this;
    }
}
void Octree::node::update(Box& box)
{

    // ���� ������ ��������� � ������
    if (treeBuilt && treeReady) {
        box.positions.push_back(region.calculateCenter());
        box.sizes.push_back(region.calculateDimensions());


        // ���� ���� ���� � �� ����� �������� �����
        if (objects.size() == 0) {
            if (!hasChildren) {
                // ���� ���� ������ ����� ����������� �� ��������
                if (currentLifespan == -1) {
                    // ������������� ������� ����� ����
                    currentLifespan = maxLifespan;
                }
                else if (currentLifespan > 0) {
                    // ���������� ������� ����� ����
                    currentLifespan--;
                }
            }
        }
        else {
            // ���� ���� �������� �������
            if (currentLifespan != -1) {
                // ���� ����� ����� ���� ������ ��� ����� 64, �������� ����
                if (maxLifespan <= 64) {
                    maxLifespan <<= 2; // ���������� maxLifespan � 4 ����
                }
                currentLifespan = -1; // ����� ������� �����
            }
        }

        // ������� ������ ��� �������� ��������, ������� ������������� �� ������ ����������� ���������
        std::stack<int> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED)) {
                objects[i].transform(); // �������� ������� BoundingBox � ������ ��������� � ��������
                movedObjects.push(i);
            }
            box.positions.push_back(objects[i].calculateCenter());
            box.sizes.push_back(objects[i].calculateDimensions());
        }

        // �������� "�������" ������
        unsigned char flags = activeOctants;
        for (int i = 0;
            flags > 0;
            flags >>= 1, i++) {
            if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0) {
                if (children[i]->objects.size() > 0) {
                    // ����� ������, �� �������� ������� � ����� ������� �����
                    children[i]->currentLifespan = -1;
                }
                else {
                    // ����� ������, ������� �
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        // �������� "�������" ��������
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::isActive(&objects[i].instance->state, INSTANCE_DEAD)) {
                objects.erase(objects.begin() + i);
                i--;       // ��������� ������ ��� ���������� ���������
                listSize--; // ��������� ������ ������
            }
        }

        // ���� � ���� ���� �������� ����, ��������� ��
        if (children != nullptr) {
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++) {
                if (States::isIndexActive(&flags, 0) && children[i] != nullptr) {
                    children[i]->update(box); // ��������� �������� ����
                }
            }
        }

        // ��������� ������������ ��������
        BoundingRegion movedObj;
        while (movedObjects.size() != 0) {


            movedObj = objects[movedObjects.top()]; // �������� ������� ������ �� �����
            node* current = this;

            // ����������� ����� �� ������, ���� �� ������ ���������� ����
            while (!current->region.containsRegion(movedObj)) {
                if (current->parent != nullptr) {
                    current = current->parent;
                }
                else {
                    break; // �������� ����� ������
                }
            }

            // ������� ������ �� �������� ���� � ���������� � ��������� ����
            objects.erase(objects.begin() + movedObjects.top());
            movedObjects.pop();
            current->insert(movedObj);


            //collision detection
            //itself
            current = movedObj.cell;
            current->checkCollisionsSelf(movedObj);

            //children
            current->checkCollisionsChildren(movedObj);

            //parents
            while (current->parent)
            {
                current = current->parent;
                current->checkCollisionsSelf(movedObj);

            }


        }
    }
    else {
        // ���� ������ ��� �� ������, ������������ ��������� �������
        if (!queue.empty()) {
            processPending();
        }
    }
}
void Octree::node::processPending()
{
    // ���� ������ �� ���������, ��������� ������� �� ������� � ������ �������� ����.
    if (!treeBuilt) {
        // ��������� ��� ������� �� ������� � ������ �������� ����
        while (queue.size() != 0) {
            objects.push_back(queue.front()); // ������� ������� �� ������� � ������ ��������
            queue.pop(); // ������� ������ �� �������
        }
        // ������ ������ ����� ����, ��� ��� ������� ���������
        build();
    }
    else
    { // ���� ������ ��� ���������, ����� ��������� ������� � ��������������� ����
        for (int i = 0, len = queue.size(); i < len; i++) {
            BoundingRegion br = queue.front();
            if (region.containsRegion(br)) {
                //insert objects immediaatly
                insert(br);

            }
            else
            { // return to queue
                br.transform(); // ����������� � ������ ������ � ������ �������
                queue.push(br);
            }
            queue.pop();
      }
    }
}

bool Octree::node::insert(BoundingRegion obj)
{
    // ������� ���������� �������: ���� ������ �� ���������� � ������� ����
    glm::vec3 dimensions = region.calculateDimensions(); // ��������� ������� �������� �������

    // ���� ���� ������ ��� ������� ������� ��������� ��� ����������� �������
    if (objects.size() == 0 ||
        dimensions.x < MIN_BOUNDS ||
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS){

        obj.cell = this;
        objects.push_back(obj); // ��������� ������ � ������� ����
        return true;
    }

    // ���� ������ �� ���������� � ������� ������, �������� ��� ������������� ����
    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    // ������� ������� ��� �������� �����, ���� ��� ��� �� �������
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (children[i] != nullptr) {
            octants[i] = children[i]->region; // ���� �������� ���� ����������, ���������� ��� ������
        }
        else
        {
            calculateBounds(octants[i], (Octant)(1 << i),region); // ��������� ������� ��� ��������� ����
        }
    }
    objects.push_back(obj);

    //determinate which octants to put objects in

    std::vector<BoundingRegion> octLists[NO_CHILDREN]; // array of list of objects   in each octants
    for (int i = 0, len = objects.size(); i < len; i++) {
        objects[i].cell = this;
        for (int j = 0; j < NO_CHILDREN; j++) {
            if (octants[j].containsRegion(objects[i])) {
                octLists[j].push_back(objects[i]);
                //remove from ojbect list
                objects.erase(objects.begin() + i);
                i--;
                len--;
                break;
            }
        }


    }

    //populate octans
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) {
            //objects exist in this octans
            if (children[i]) {
                for (BoundingRegion br : octLists[i]) {
                    children[i]->insert(br);

                }
            }
            else
            {  //create new node
                children[i] = new node(octants[i], octLists[i]);
                children[i]->parent = this;
                States::activateIndex(&activeOctants, i);
                children[i]->build();
                hasChildren = true;
            }
        }
    }
    return true;
}
//check collisions with all objects in node
void Octree::node::checkCollisionsSelf(BoundingRegion obj){
    for (BoundingRegion br : objects) {
        if (br.intersectsWith(obj)) {
            if (br.instance->instanceId != obj.instance->instanceId) {  // br � obj �� ���� � ��� �� �������
                //different instances collide
                std::cout << " instance " << br.instance->instanceId << "(" << br.instance->modelId << ") collides with" << obj.instance->instanceId << "(" << obj.instance->modelId << ")" << std::endl;
                br.intersectsWith(obj);

            }
        }
    }

}
//check collisiont with all objects in child node
void Octree::node::checkCollisionsChildren(BoundingRegion obj){
    if (children) { // �� ����� nullptr
        for (int flags = activeOctants, i = 0;
            flags > 0;
            flags >>= 1, i++) {

            if (States::isIndexActive(&flags, 0) && children[i]) {
                children[i]->checkCollisionsSelf(obj);
                children[i]->checkCollisionsChildren(obj);
            }
        }
    }

}

void Octree::node::destroy()
{
    // ������� �������� ����, ���� ��� ����������
    if (children != nullptr) {
        for (int flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++)
        {
            // ���� �������� ���� �������
            if (States::isIndexActive(&flags, 0)) {
                // ���� �������� ���� ����������, ���������� ������� ���
                if (children[i] != nullptr) {
                    children[i]->destroy(); // ������� �������� ����
                    children[i] = nullptr; // �������� ���������
                }
            }
        }
    }
    // ������� ������� ����
    objects.clear(); // ������� ������ �������� � ������� ����
    // ������� �������, ���� ��� �� �����
    while (queue.size() != 0)
    {
        queue.pop(); // ������� ������� �� �������
    }
}