#ifndef BOUNDS_H  
#define BOUNDS_H
#include <glm/glm.hpp>
#include "../physics/rigidbody.h"



//forward declaration
namespace Octree {
	class node;
}

enum class BoundTypes : unsigned char{
	AABB = 0x00, // как char храним так как 8 бит  в 0x00 = 0  axis - aligned bounding box
	SPHERE = 0x01 // 0x01 = 1

};

class  BoundingRegion
{
public:
	BoundTypes type;

	//pointer for quik access to instance
	RigidBody* instance;

	//pointer for quick access to current octree node
	Octree::node* cell;



	//sphere values
	glm::vec3 center;
	float radius;
	
	glm::vec3 ogCenter;
	float ogRadius;



	//bounding box
	glm::vec3 min;
	glm::vec3 max;

	glm::vec3 ogMin;
	glm::vec3 ogMax;

	//constructors
	   // Оператор сравнения для сравнения объектов BoundingRegion
	bool operator==(const BoundingRegion& other) const {
		// Сравниваем все компоненты (минимальные и максимальные координаты)
		return min == other.min && max == other.max;
	}

	//initialize with type
	BoundingRegion(BoundTypes type = BoundTypes::AABB);

	//initialize as a sphere
	BoundingRegion(glm::vec3 cente, float radius);

	//initialize as  AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	//transform for instance
	void transform();




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
