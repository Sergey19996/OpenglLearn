#ifndef PLANE_HPP
#define PLANE_HPP

#include "../model.h"

class Plane : public Model {
public:
	Plane() : Model("plane", BoundTypes::AABB, 1, CONST_INSTANCES) {}

	void init(std::vector<texture> tex) {
		int noVertices = 4; 

		float quadVertices[] = {
			//poition				normal			texCoord
			0.5f,0.5f,0.0f,		0.0f,0.0f,1.0f,		1.0f,1.0f, // top right
			0.5f,-0.5f,0.0f,	0.0f,0.0f,1.0f,		1.0f,0.0f, // bottom right
		  -0.5f,-0.5f,0.0f, 	0.0f,0.0f,1.0f,		0.0f,0.0f, // bottom left
		   -0.5f,0.5f,0.0f,		0.0f,0.0f,1.0f,		0.0f,1.0f // top left

		};
		std::vector<unsigned int> indices = {
			0 , 1 ,2 ,
			2,  3, 0
		};

		BoundingRegion br(glm::vec3(-0.5f, -0.5f, 0.0f), (0.5f, 0.5f, 0.0f));  // min max

		Mesh ret(br,tex);
		ret.loadData(Vertex::genList(quadVertices, noVertices), indices, true);

		meshes.push_back(ret);
		boundingRegions.push_back(br);
	}

};
#endif // !PLANE_HPP
