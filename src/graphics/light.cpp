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

void DirLight::render(Shader shader)
{

	std::string name = "dirLight";

	shader.set3Float(name + ".direction", direction);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

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
