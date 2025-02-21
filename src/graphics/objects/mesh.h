#ifndef MESH_H  // Если макрос  MESH_H  ещё не определён
#define MESH_H // Определяем макрос 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "../rendering/material.h"
#include "../rendering/texture.h"
#include "../rendering/material.h"

#include "../models/box.hpp"
#include "../vertexmemory.hpp"

#include "../../algorithms/bounds.h"
#include "../../physics/collisionmesh.h"


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;

	static std::vector<struct Vertex> genList(float* vertices, int noVertices);


	//calculate tangent vectors for each face 
	static void calcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices);

};
typedef struct Vertex Vertex;



class Mesh {
public:
	BoundingRegion br;
	// pointer to the attached collision mesh
	CollisionMesh* collision; 

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ArrayObject VAO;  // обвёртка над VAO (arrayobject) 

	std::vector<texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	// default constructor 
	Mesh();

	//initialize with a bounding region
	Mesh(BoundingRegion br);

	// initialize as textured object
	Mesh(BoundingRegion br, std::vector<texture> textures);

	// initialize as material object
	Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);

	//initialize with a material
	Mesh(BoundingRegion br, Material m);

	// load vertex and index data
	void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool pad =false);

	//setup collision mesh
	void loadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);

	//setup tetures
	void setupTextures(std::vector<texture> textures);

	//setup material colors
	void setupColors(aiColor4D diff, aiColor4D spec);

	//set material structures
	void setupMaterial(Material mat);


	//render number of instances using shader
	void render(Shader shader, unsigned int noInstances);

	void cleanup();

private:

	unsigned int VBO, EBO;

	bool noTex;

	void setup();


};


#endif