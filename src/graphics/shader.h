#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;


	Shader();
	Shader(bool includeDefaultHeade,
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		const char* geoShaderPath = nullptr);

	void generate(bool includeDefaultHeader,
		const char* vertexShaderPath, 
		const char* fragShaderPath,
		const char* geoShaderPath = nullptr);
	void bind();

	void activate();

	
	GLuint compileShader(bool includeDefaultHeader,const char* filepath, GLenum type);


	//uinform functions
	void setFloat(const std::string& name, float value);
	void set3Float(const std::string& name, glm::vec3 v);
	void set3Float(const std::string& name, float v1, float v2, float v3);
	void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
	void set4Float(const std::string& name, aiColor4D color);
	void set4Float(const std::string& name, glm::vec4 v);
	void setMat4(const std::string& name, glm::mat4 val);
	void setInt(const std::string& name, int valuse);
	void setValue(const std::string& name,float value);
	void setBool(const std::string& name, bool value);
	void setMat3(const std::string& name, glm::mat3 val);

	//static
	//default directory
	static std::string defaultDirectory;


	//stream containing default contnent
	static std::stringstream defaultHeaders;    // позволяет вот так записывать  int number 42 ; stringstram << number; и извлекать stringstram >> number

	//load into deafult header 
	static void loadIntoDefault(const char* filepath);

	//clear default header (after shader compilation)
	static void clearDefaults();

	//utility fucntions
	static std::string loadShadersSrc(bool includeDefaultHeader,const char* filepath);

};


#endif