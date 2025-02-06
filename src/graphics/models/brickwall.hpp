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

};

#endif // ! BRICKWALL_H
