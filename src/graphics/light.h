#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "shader.h"
#include "../algorithms/bounds.h"
#include "framememory.hpp"
struct  PointLight{

	glm::vec3 position;


	//attenuation constans
	float k0;
	float k1;
	float k2;


	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;


	void render(Shader shader,int idx);



};
 
struct DirLight {

	glm::vec3 direction;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	//bounding region for shadow
	BoundingRegion br;

	//transformations for the light space (projection * view)
	glm::mat4 lightSpaceMatrix;


	//FBO for shadows
	FramebufferObject shadowFBO;


	//consturctor
	DirLight(glm::vec3 direction,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		BoundingRegion br);

	void render(Shader shader, unsigned int textureIdx);

	//update light space matrix
	void updateMatrices();

};

struct SpotLight {

	glm::vec3 direction;
	glm::vec3 position;

	float cutOff;
	float outerCutOff;

	//attenuation constans
	float k0;
	float k1;
	float k2;



	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	void render(Shader shader,int idx);
};


#endif // !LIGHT_H
