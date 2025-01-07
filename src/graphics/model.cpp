#include "model.h"

Model::Model()
{
}

void Model::init()
{
}

void Model::render(Shader shader)
{
	for (Mesh mesh : meshes) {

		mesh.render(shader);

	}

}

void Model::CleanUp()
{
	for (Mesh mesh : meshes) {

		mesh.cleanup();
	}
}
