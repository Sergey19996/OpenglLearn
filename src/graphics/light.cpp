#include "light.h"

void PointLight::render(Shader shader, int idx)
{
	std::string name = "pointLight[" + std::to_string(idx) + "]";

	shader.set3Float(name + ".position", position);


	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

}

DirLight::DirLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, BoundingRegion br) : direction(direction),ambient(ambient), diffuse(diffuse),specular(specular),shadowFBO(1024,1024,GL_DEPTH_BUFFER_BIT), br(br)
{
	//setup FBO
	shadowFBO.generate();
	shadowFBO.bind();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	updateMatrices();

}

void DirLight::render(Shader shader, unsigned int textureIdx)
{

	std::string name = "dirLight";

	shader.set3Float(name + ".direction", direction);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

	//set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].bind();
	shader.setInt("dirLight.depthbuffer", textureIdx);

	//set light space matrix
	shader.setMat4("dirLight.lightSpaceMatrix", lightSpaceMatrix);

}

void DirLight::updateMatrices(){
	glm::mat4 proj = glm::ortho(br.min.x, br.max.x, br.min.y, br.max.y, br.min.z, br.max.z);

	glm::vec3 pos = -2.0f * direction;

	glm::mat4 lightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = proj * lightView;
	 


}

void SpotLight::render(Shader shader, int idx)
{


	std::string name = "spotLight[" + std::to_string(idx) + "]";


	shader.set3Float(name + ".position", position);
	shader.set3Float(name + ".direction", direction);

	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);


	shader.setFloat(name + ".cutOff", cutOff);
	shader.setFloat(name + ".outerCutOff", outerCutOff);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

}
