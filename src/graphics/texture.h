#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <stb/stb_image.h>

class texture
{
public:
	texture();
	texture(std::string dir, std::string path, aiTextureType type);

	void generate();
	void load(bool flip =true);

	void bind();
	
	unsigned int id;
	aiTextureType type;
	std::string dir;
	std::string path;

};

