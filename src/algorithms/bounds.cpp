#include "bounds.h"
// ����������� ��� ������������� ���� �������
BoundingRegion::BoundingRegion(BoundTypes type) : type(type) {}

// ����������� ��� �����, ����� ����� � ������
BoundingRegion::BoundingRegion(glm::vec3 center, float radius) :
    type(BoundTypes::SPHERE), center(center), radius(radius) {}

// ����������� ��� AABB, ����� ����������� � ������������ �����
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max) :
    type(BoundTypes::AABB), min(min), max(max) {}

// ����� ��� ���������� ������ �������
glm::vec3 BoundingRegion::calculateCenter() {
    // ���� ��� AABB, ����� � ������� �������������� ����� min � max
    // ���� ��� �����, ������������ � �����
    return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

// ����� ��� ���������� �������� �������
glm::vec3 BoundingRegion::calculateDimensions() {
    // ��� AABB � �������� ����� max � min   // ��� �������� ������� (�����) ��� ������ ������� ��������� ������������ ����� 
    // ��� ����� � ������� (2 * ������) ���������� �� ���� ����
    return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

// ����� ��� ��������, ���������� �� ����� ������ �������
bool BoundingRegion::containspoint(glm::vec3 point) {
    if (type == BoundTypes::AABB) {
        // ��� AABB ����� ������ ���� ������ ��� ����� min � ������ ��� ����� max �� ���� ����
        return (point.x >= min.x) && (point.x <= max.x) &&
            (point.y >= min.y) && (point.y <= max.y) &&
            (point.z >= min.z) && (point.z <= max.z);
    }
    else {
        // ��� ����� ���������� �� ����� �� ������ ������ ���� ������ ��� ����� �������
        float distSquare = 0.0f;
        for (int i = 0; i < 3; i++) {
            // ��������� ������� ���������� (x^2 + y^2 + z^2)
            distSquare += (center[i] - point[i]) * (center[i] - point[i]);
        }
        return distSquare <= (radius * radius);
    }
}

// ����� ��� ��������, ��������� �� ������ `br` ���������� ������ �������� �������
bool BoundingRegion::containsRegion(BoundingRegion br) {
    if (br.type == BoundTypes::AABB) {
        // ��� AABB: ������� ������ ������ ��������� ��� ����� `min` � `max` ������� �������
        return containspoint(br.min) && containspoint(br.max);
    }
    else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        // ��� ���� ����: ���������� ����� �� �������� + ������ ��������� ����� 
        // ������ ���� ������ ������� ������� �����
        return glm::length(center - br.center) + br.radius <= radius;
    }
    else {
        // ���� ������� ������ � AABB, � ��������� ������ � �����
        if (!containspoint(br.center)) {
            // ���� ����� ����� �� ��������� AABB, ���������� `false`
            return false;
        }

        // ��������� ���������� �� ������ ����� �� ������ ������� AABB
        for (int i = 0; i < 3; i++) {
            // ���� ����� ����� ������� ������ � ����� ����� AABB
            // (������ ������� �����), ������, ����� ������� �� ������� AABB
            if (abs(max[i] - br.center[i]) < br.radius || abs(br.center[i] - min[i]) < br.radius) {
                return false;
            }
        }
    }

    // ���� �� ���� �� ������� ���� �� ��������, ������ ����������
    return true;
}

 //�������� ���������� �������
bool BoundingRegion::intersectsWith(BoundingRegion br)
{// overlap on all axes
    if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
        //both boxes  //  calculateDimensions() ���������� ������� ������ ������� 
        glm::vec3 rad = calculateDimensions() / 2.0f;  // radius of this box;
        glm::vec3 radBr = br.calculateDimensions() / 2.0f; // radius of br

        glm::vec3 center = calculateCenter();           //center of this box
        glm::vec3 centerBr = br.calculateCenter();      //center of other box

        glm::vec3 dist = abs(center - centerBr);

        for (int i = 0; i < 3; i++)
        {
            if (dist[i] > rad[i] + radBr[i]) {
                //no overlap on this axis
                return false; // ���� ���������� ������ ����� �������� �� ����� �� ����, ����������� ���.

            }
        }
        //failed to prove wrong on each axes
        return true;
    }
    else if (type == BoundTypes::SPHERE && br.type ==BoundTypes::SPHERE)
    {  // both spheres
        return glm::length(center - br.center) < (radius + br.radius);
    }
    else if(type == BoundTypes::SPHERE)
    {  //this is a sphere  br is a box
        
        //determinate if sphere is above top, below bottom,etc
        // find the distance(squared) to the closet plane
        float distSquared = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            if (center[i] < br.min[i]) {  //����� ����� ��������� "�� ��������" �������, ����� � ����������� ������.
                // ����� ����� �� ��������� ����������� ������� ������� �� ��� i.
                distSquared += (br.min[i] - center[i]) * (br.min[i] - center[i]);

            }
            else if (center[i] > br.max[i])
            {   // ����� ����� �� ��������� ������������ ������� ������� �� ��� i.
                distSquared += (center[i] - br.max[i]) * (center[i] - br.max[i]);

            }
            // ���� ����� ����� ����� min � max, ������ �� ���������.

        }
        return distSquared < (radius * radius);  // ���� ������� ���������� ������ �������� �������, ����� ���������� �������.
    }
    else
    {//this is a box , br is a sphere
        //call algorithm for br (defined in preceding if block)
        return br.intersectsWith(*this);  //������ ������� ��� ����� �� �� ����� � br box

    }


    //failed to prove wrong on each axis
	return false;
}

bool BoundingRegion::operator==(BoundingRegion br){
    if (type != br.type) {
        return false;
    }
    if (type == BoundTypes::AABB) {

        return min == br.min && max == br.max;
    }
    else
    {
        return center == br.center && radius ==br.radius;
    }
    return true;
}
