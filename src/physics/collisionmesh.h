#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H
#include <vector>


#include "../algorithms/bounds.h"
#include "../algorithms/cmathematis/vec.h"

//forward declarations
class CollisionModel;
class CollisionMesh;

typedef struct Face {
	CollisionMesh* mesh;
	unsigned int i1, i2, i3;


	vec baseNormal;
	vec norm;


	bool collidesWidth(struct Face& face);


}Face;

class CollisionMesh {

public:
	CollisionModel* model;
	BoundingRegion br;

	std::vector<vec> points;
	std::vector<Face> faces;


	CollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);

};


#endif // !COLLISIONMESH_H
