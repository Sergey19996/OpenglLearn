#ifndef MESH_H  // Если макрос  MESH_H  ещё не определён
#define MESH_H // Определяем макрос 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"


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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;

	std::vector<texture> textures;


	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);


	void render(Shader shader);

	void cleanup();

private:

	unsigned int VBO, EBO;

	void setup();


};


#endif