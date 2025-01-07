#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

class texture
{
public:
	texture();
	texture(const char* path, const char* name, bool defaultparam =true);

	void generate();
	void load(bool flip =true);

	void setFilters(GLenum all);
	void setFilters(GLenum mag, GLenum min);

	void setWrap(GLenum all);
	void setWrap(GLenum s, GLenum t);

	void bind();
	bool checkAlpha() { return HasAlpha; };

	//texture object
	 int id;
	 unsigned int tex;
	 const char* name;


private:
	static int currId;

	const char* path;
	int width;
	int height;
	int nChannels;
	bool HasAlpha =false;
};

