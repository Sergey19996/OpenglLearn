#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "mesh.h"
#include "models/box.hpp"
#include "../physics/rigidbody.h"
#include "../algorithms/bounds.h"
#include "../algorithms/States.hpp"
#include "../scene.h"

//model switches
#define DYNAMIC			 (unsigned int) 1 // 01 
#define CONST_INSTANCES  (unsigned int) 2 // 10
#define NO_TEX			 (unsigned int) 4 //100

class Scene; //forward declaration 

class Model {
public:
	std::string id;

	RigidBody rb;
	glm::vec3 size;

	BoundTypes boundType;

	std::vector<Mesh> meshes;
	std::vector<BoundingRegion> boundingRegions;
	std::vector<RigidBody*> instances;


	
	unsigned int maxNoInstances;
	unsigned int currentNoInstances;

	unsigned int switches;


	Model();
	Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int falgs = 0);

	//initialize method
	virtual void init();

RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos);

	void initInstances();


	
	void loadModel(std::string path);

	virtual void render(Shader shader, float DeltaTime,Scene* scene, bool setmodel = true);


	void CleanUp();

	void removeInstance(unsigned int idx);

	void removeInstance(std::string instanced);

	unsigned int getIdx(std::string id);


protected:
	bool noTex;

	std::string directory;

	std::vector<texture> textures_load;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<texture> loadTextures(aiMaterial* mat, aiTextureType type);


	//VBOs for positions and sizes
	BufferObject posVBO;
	BufferObject sizeVBO;

};

#endif // !MODEL_H
