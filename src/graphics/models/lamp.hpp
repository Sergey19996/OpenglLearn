#ifndef  LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"
#include "../light.h"

class Lamp : public Cube {

public :
	glm::vec3 lightColor;



	
	Lamp(unsigned int maxNoInstances, glm::vec3 lightColor = glm::vec3(1.0f)):
	Cube(maxNoInstances,Material::white_plastic){
		id = "lamp";
		this->lightColor = lightColor;


	}

	
};


#endif // ! LAMP_HPP
