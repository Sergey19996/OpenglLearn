#ifndef BOUNDS_H  
#define BOUNDS_H
#include <glm/glm.hpp>

enum class BoundTypes : unsigned char{
	AABB = 0x00, // ��� char ������ ��� ��� 8 ���  � 0x00 = 0  axis - aligned bounding box
	SPHERE = 0x01 // 0x01 = 1

};

class  BoundingRegion
{
public:
	BoundTypes type;

	//sphere values
	glm::vec3 center;
	float radius;


	//bounding box
	glm::vec3 min;
	glm::vec3 max;

	//constructors
	   // �������� ��������� ��� ��������� �������� BoundingRegion
	bool operator==(const BoundingRegion& other) const {
		// ���������� ��� ���������� (����������� � ������������ ����������)
		return min == other.min && max == other.max;
	}

	//initialize with type
	BoundingRegion(BoundTypes type = BoundTypes::AABB);

	//initialize as a sphere
	BoundingRegion(glm::vec3 cente, float radius);

	//initialize as  AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);


	//calculating values for the region
	glm::vec3 calculateCenter();
	glm::vec3 calculateDimensions();


	//determine if point inside
	bool containspoint(glm::vec3 point);


	//determine if region inside
	bool containsRegion(BoundingRegion br);

	//determine if region intersects
	bool intersectsWith(BoundingRegion br);


	//operator overload
	bool operator==(BoundingRegion br);

};
#endif // ! 