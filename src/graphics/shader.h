#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;


	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	void generate(const char* vertexShaderPath, const char* fragShaderPath);
	void activate();

	//utility fucntions
	std::string loadShadersSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);


	//uinform functions
	void setMat4(const std::string& name, glm::mat4 val);
	void setInt(const std::string& name, int valuse);
	void setValue(const std::string& name,float value);


};


#endif