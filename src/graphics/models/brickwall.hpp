#ifndef  BRICKWALL_H
#define BRICKWALL_H

#include "plane.hpp"

class Brickwall : public Plane{
public:
	void init() {
		std::vector<texture> textures{
			texture("Assets/textures","brickwall.jpg", aiTextureType_DIFFUSE),
			texture("Assets/textures","brickwall_specular.jpg", aiTextureType_SPECULAR),
			texture("Assets/textures","brickwall_normal.jpg", aiTextureType_NORMALS),

		};
		for (texture t : textures) {
			t.load();

		}
		Plane::init(textures);
	}
	void render(Shader shader, float deltaTime, Scene* scene, glm::mat4 model = glm::mat4(1.0f)) {

		model = glm::rotate(model, glm::radians(45.0f), { 1.0f,0.0f,0.0f });

		Model::render(shader, deltaTime, scene);
	}
};

#endif // ! BRICKWALL_H
