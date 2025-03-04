#include "light.h"

PointLight::PointLight() {}

//list of directions
glm::vec3 PointLight::directions[6] = {
	{1.0f,0.0f,0.0f},
	{-1.0f,0.0f,0.0f},
	{0.0f,1.0f,0.0f},
	{0.0f,-1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.0f,0.0f,-1.0f},
};

glm::vec3 PointLight::up[6] = {
	{0.0f,-1.0f,0.0f},
	{0.0f,-1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{0.0f,0.0f,-1.0f},
	{0.0f,-1.0f,0.0f},
	{0.0f,-1.0f,0.0f},

};
PointLight::PointLight(
	glm::vec3 pos,
	float k0, float k1, float k2,
	glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
	float nearPlane, float FarPlane) :
	position(pos),
	k0(k0), k1(k1), k2(k2),
	ambient(ambient), diffuse(diffuse), specular(specular),
	nearPlane(nearPlane), farPlane(farPlane), shadowFBO(1024,1024,GL_DEPTH_BUFFER_BIT){
	shadowFBO.generate();

	shadowFBO.bind();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachCubemap(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	updateMatrices();

}

void PointLight::render(Shader shader, int idx, unsigned int textureIdx)
{
	//set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);  //occupies texture slot 
	shadowFBO.cubemap.bind(); // bind this texture slot with data
	shader.setInt("pointLightBuffers[" + std::to_string(idx) + "]", textureIdx); // send this data in shader
}
//updae light space matrices
void PointLight::updateMatrices() {                                                      //proj = [
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), //FOV						//[cot(FOV / 2) / aspect, 0, 0, 0],
		(float)shadowFBO.height / (float)shadowFBO.width, // aspect ratio               //[0, cot(FOV/2), 0, 0],
		nearPlane, farPlane); //near plane												//[0, 0, -(far + near)/(far - near), -2*far*near/(far - near)],
		//[-1;+1]																		//[0, 0, -1, 0] //координаты нормализованы и готовы для отсечения и последующего преобразования в экранные координаты.



	for (unsigned int i = 0; i < 6; i++) {                                              //view = [ 1.Камера становится центром координат. 2.Если камера движется вправо, объекты в мире движутся влево.
		lightSpaceMatrices[i] = proj * glm::lookAt(position,                            //[x.x, x.y, x.z, -dot(x, position)],
			position + PointLight::directions[i],                                       //[y.x, y.y, y.z, -dot(y, position)],
			PointLight::up[i]);                                                         //[z.x, z.y, z.z, -dot(z, position)],
	}                                                                                   // [0,   0,   0,    1]


}
DirLight::DirLight()
{
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

	//set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].bind();
	shader.setInt("dirLightBuffer", textureIdx);


}

void DirLight::updateMatrices(){

	glm::mat4 proj = glm::ortho(br.min.x, br.max.x, br.min.y, br.max.y, br.min.z, br.max.z);

	glm::vec3 pos = -2.0f * direction;

	glm::mat4 lightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = proj * lightView;
	 


}

void SpotLight::render(Shader shader, int idx,unsigned int textureIdx)
{


	//set depth texture 
	// Устанавливаем активный текстурный юнит (выбираем, куда привязывать текстуру)
	glActiveTexture(GL_TEXTURE0 + textureIdx); 

	// Привязываем текстуру из FBO к активному текстурному юниту
	shadowFBO.textures[0].bind();

	// Передаём индекс текстурного юнита в шейдер
	shader.setInt("spotLightBuffers[" + std::to_string(idx) + "]", textureIdx);

}

SpotLight::SpotLight()
{
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
	float cutoff, float outerCutOff,
	float k0, float k1, float k2,
	glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
	float nearPlane, float farPlane) : position(position), direction(direction), up(up),
	cutOff(cutoff), outerCutOff(outerCutOff),
	k0(k0), k1(k1),k2(k2),
	ambient(ambient), diffuse(diffuse), specular(specular),
	nearPlane(nearPlane),farPlane(farPlane), shadowFBO(1024,1024,GL_DEPTH_BUFFER_BIT){

	shadowFBO.generate();  // генерируем фрамебуфер
	shadowFBO.bind();  // связываем с контекстом GL_FRAME_BUFFER
	shadowFBO.disableColorBuffer();  // отключаем COLOR REAAD и COLOR WRITE для оптимизации 
	shadowFBO.allocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);  // создаём текстуру, передаём параметры, прикрпляем её к контексту

	updateMatrices();  // передаём матрицу проекции от позиции освещение 

}



void SpotLight::updateMatrices(){
	glm::mat4 proj = glm::perspective(glm::acos(outerCutOff) * 2.0f,  //FOV
		(float)shadowFBO.height / (float)shadowFBO.width, // aspect ratio
		nearPlane, farPlane);

	glm::mat4 lightView = glm::lookAt(position, position + direction, up);

	lightSpaceMatrix = proj * lightView;
}
