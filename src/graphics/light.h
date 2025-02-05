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

	

	//bounds 
	float nearPlane;
	float farPlane;

	//list of view matrices
	glm::mat4 lightSpaceMatrices[6];
	
	//FBO for shadows
	FramebufferObject shadowFBO;

	//default consturcor for vectors and arrays
	PointLight();

	//constructor
	PointLight(glm::vec3 pos,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float FarPlane);

	void render(Shader shader,int idx, unsigned int textureIdx);

	//update matrices method
	void updateMatrices();

	//list of directions
	static glm::vec3 directions[6];

	//list of up vectors
	static glm::vec3 up[6];

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

	//default consturcor for vectors and arrays
	DirLight();


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

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 up;

	float cutOff;
	float outerCutOff;

	//attenuation constans
	float k0;
	float k1;
	float k2;



	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;


	//bounds for the shadow
	float nearPlane;
	float farPlane;

	//light space transformation matrix
	glm::mat4 lightSpaceMatrix;

	//FBO for shadows
	FramebufferObject shadowFBO;


	//default consturcor for vectors and arrays
	SpotLight();

	//constructor
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
		float cutoff, float outerCutOff,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void render(Shader shader,int idx, unsigned int textureIdx);

	//update light space matri
	void updateMatrices();
};


#endif // !LIGHT_H
