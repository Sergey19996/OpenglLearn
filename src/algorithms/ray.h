#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

#include "bounds.h"
#include "../physics/collisionmesh.h"
#include "../physics/rigidbody.h"

class Ray {
public:
	glm::vec3 origin;
	glm::vec3 dir;
	glm::vec3 invDir;

	Ray(glm::vec3 origin, glm::vec3 d);

	bool intersectsBoundingRegion(BoundingRegion br, float& tmin, float& tmax);
	bool intersectsMesh(CollisionMesh* mesh, RigidBody* rb, float& t); // collision mesh -base object, Rigidbody - transformed coordinated of the collision mesh


};


#endif // !RAY_H
