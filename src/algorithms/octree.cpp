#include "octree.h"

void Octree::calculateBounds(BoundingRegion* out, Octant octant, BoundingRegion parentRegion)
{
    // ��������� ����� ������������� �������.
    // ����� ��������� ������������ ������������� ������� �� 8 ��������.
    glm::vec3 center = parentRegion.calculateCenter();

    // ���������, ����� ������ ������, � �� ������ ����� ������� ��������������� ������.
    if (octant == Octant::O1) {
        // ������ ������: �� ������ �� ������������ ��������� ������������� �������.
        out = new BoundingRegion(center, parentRegion.max);
    }
    else if (octant == Octant::O2) {
        // ������ ������: ����� �� ����������� x � ����������� y, z
        // �� ����������� x � ������������ y, z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, center.z),
            glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O3) {
        // ������ ������: �� ����������� x, y � ����������� z 
        // �� ����������� x, y � ������������ z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z),
            glm::vec3(center.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O4) {
        // ��������� ������: �� ����������� x, ����������� y � ����������� z
        // �� ������������ x, y � z.
        out = new BoundingRegion(
            glm::vec3(center.x, parentRegion.min.y, center.z),
            glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O5) {
        // ����� ������: �� ������ �� ������������ ��������� �� x, y,
        // �� ����������� �� z.
        out = new BoundingRegion(
            glm::vec3(center.x, center.y, parentRegion.min.z),
            glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O6) {
        // ������ ������: �� ����������� x, ����������� y � ����������� z
        // �� ����������� x, y � z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z),
            glm::vec3(center.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O7) {
        // ������� ������: �� ����������� ��������� �� ������.
        out = new BoundingRegion(parentRegion.min, center);
    }
    else if (octant == Octant::O8) {
        // ������� ������: �� ����������� x, ����������� y � z
        // �� ������������ ���������.
        out = new BoundingRegion(
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

void Octree::node::build()
{
    if (treeBuilt && treeReady) {
        // ���� ������ ��������� � ������, �������� ����������.

        // ������� ������ ��� �������� ��������, ������� ������������� �� ������ ����������� ���������.
        std::vector<BoundingRegion> movedObjects(objects.size());
        // TODO: ���������� ������ ����������� ������������ ��������.

        // ������� "�������" �������, ������� ������ �� ����������.
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            // ������� ������, ���� �� ��������� � ������ ��������� ��������.
            // TODO: ���������� �������� "�������" ��������.
        }

        // ���� � ���� ���� �������� ����, ��������� ��.
        if (children != nullptr) {
            // ���������� ��� �������� ������� (�������� ����).
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++) {
                // ���������, ������� �� ������� ������.
                if (States::isIndexActive(&flags, 0)) {
                    // ���� ������ ������� � ���������� ��������������� �������� ����.
                    if (children[i] != nullptr) {
                        // TODO: ���������� ���������� ��������� ����.
                    }
                }
            }
        }

        // ������������ �������, ������� ������������� � ����� �����.
        BoundingRegion movedObj;
        while (movedObjects.size() != 0)
        {
            // �������� ������ ������������ ������.
            movedObj = movedObjects[0];
            node* current = this; // �������� � �������� ����.

            // ����������� ����� �� ������, ���� �� ������ ����, ������� ��������� �������� ������.
            while (!current->region.containsRegion(movedObj))
            {
                if (current->parent != nullptr) {
                    current = current->parent; // ��������� � ������������� ����.
                }
                else {
                    break; // ���� �������� ����� ������, ������� �� �����.
                }
            }

            // ������� ������ ������ �� ������ ������������ ��������.
            movedObjects.erase(movedObjects.begin());

            // ������� ������ �� �������� ����.
            objects.erase(objects.begin() + List::getIndexOf<BoundingRegion>(objects, movedObj));

            // ��������� ������ � ��������� ���������� ����.
            current->insert(movedObj);

            // TODO: ���������� ����������� ������������, ���� ���������.
        }

    }
    else {
        // ���� ������ ��� �� ������, ������������ ��������� �������.
        if (queue.size() > 0) {
            processPending();
        }
    }
}
void Octree::node::update()
{

    // ���� ������ ��������� � ������
    if (treeBuilt && treeReady) {
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
        std::stack<std::pair<int, BoundingRegion>> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED)) {
                objects[i].transform(); // �������� ������� BoundingBox � ������ ��������� � ��������
                movedObjects.push({ i, objects[i] });
            }
        }

        // �������� "�������" ������
        unsigned char flags = activeOctants;
        for (int i = 0; flags > 0; flags >>= 1, i++) {
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
                    children[i]->update(); // ��������� �������� ����
                }
            }
        }

        // ��������� ������������ ��������
        BoundingRegion movedObj;
        while (!movedObjects.empty()) {
            movedObj = movedObjects.top().second; // �������� ������� ������ �� �����
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
            objects.erase(objects.begin() + movedObjects.top().first);
            movedObjects.pop();
            current->insert(movedObj);
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
        while (queue.size() != 0)
        {
            insert(queue.front()); // ��������� ������ � ������
            queue.pop(); // ������� ������ �� �������
        }
    }
}

bool Octree::node::insert(BoundingRegion obj)
{
    // ������� ���������� �������: ���� ������ �� ���������� � ������� ����
    glm::vec3 dimensions = region.calculateDimensions(); // ��������� ������� �������� �������

    // ���� ���� ������ ��� ������� ������� ��������� ��� ����������� �������
    if (objects.size() == 0 || dimensions.x < MIN_BOUNDS || dimensions.y < MIN_BOUNDS || dimensions.z < MIN_BOUNDS)
    {
        objects.push_back(obj); // ��������� ������ � ������� ����
        return true;
    }

    // ���� ������ �� ���������� � ������� ������, �������� ��� ������������� ����
    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    // ������� ������� ��� �������� �����, ���� ��� ��� �� �������
    BoundingRegion octans[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (children[i] != nullptr) {
            octans[i] = children[i]->region; // ���� �������� ���� ����������, ���������� ��� ������
        }
        else
        {
            calculateBounds(&octans[i], (Octant)(1 << i),region); // ��������� ������� ��� ��������� ����
        }
    }

    // ������� �������� ������, ������� ��������� �������� ������
    bool found = false;
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octans[i].containsRegion(obj)) {
            // ���� �������� ���� ����������, ��������� ������ � ����
            if (children[i] != nullptr) {
                return children[i]->insert(obj);
            }
            else {
                // ������� ����� ���� ��� ��������� ������� � ��������� ������
                children[i] = new node(octans[i], { obj });
                States::activateIndex(&activeOctants, i); // �������� �������� ���� ��� ��������
                return true;
            }
        }
    }

    // ���� �� ���� �������� ���� �� �������, ��������� ������ � ������� ����
    objects.push_back(obj);
    return true;
}

void Octree::node::destroy()
{
    // ������� �������� ����, ���� ��� ����������
    if (children != nullptr) {
        for (int flags = activeOctants, i = 0; flags > 0; flags >> 1, i++)
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