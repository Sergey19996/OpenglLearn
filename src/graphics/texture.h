#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <stb/stb_image.h>

class texture
{
public:
	texture();

	texture(std::string name);


	texture(std::string dir, std::string path, aiTextureType type);

	
	void generate();

	void load(bool flip =true);

	void allocate(GLenum format, GLuint widdth, GLuint height, GLenum type);

	static void setParams(GLenum texMinFilter = GL_NEAREST,
						  GLenum texMagFilter = GL_NEAREST,
						  GLenum wrapS = GL_REPEAT,
						  GLenum wrapT = GL_REPEAT);
	void bind();

	void cleanup();
	
	unsigned int id;
	aiTextureType type;
	std::string name;
	std::string dir;
	std::string path;

};

