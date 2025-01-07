#ifndef  LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"


class Lamp : public Cube {

public :
	glm::vec3 lightColor;


	//light strength values
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;


	Lamp(glm::vec3 lightcolor, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 pos, glm::vec3 size)
		:lightColor(lightcolor), ambient(ambient), diffuse(diffuse), specular(specular), 
		Cube(Material::white_plastic, pos, size) {}


	void render(Shader shader) {
		//set light color
		shader.set3Float("lightColor", lightColor);


		Cube::render(shader);


	}

};




#endif // ! LAMP_HPP
