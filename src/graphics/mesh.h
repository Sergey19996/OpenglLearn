#ifndef MESH_H  // Если макрос  MESH_H  ещё не определён
#define MESH_H // Определяем макрос 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "models/box.hpp"
#include "../algorithms/bounds.h"
#include "glmemory.hpp"
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;


	static std::vector<struct Vertex> genList(float* vertices, int noVertices);

};
typedef struct Vertex Vertex;



class Mesh {
public:
	BoundingRegion br;


	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ArrayObject VAO;  // обвёртка над VAO (arrayobject) 

	std::vector<texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	// default constructor 
	Mesh();

	// initialize as textured object
	Mesh(BoundingRegion br, std::vector<texture> textures = {});

	// initialize as material object
	Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);

	// load vertex and index data
	void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	
	void render(Shader shader, unsigned int noInstances);

	void cleanup();

private:

	unsigned int VBO, EBO;

	bool noTex;

	void setup();


};


#endif