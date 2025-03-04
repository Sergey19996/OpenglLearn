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
#include "../models/box.hpp"
#include "../../physics/collisionmodel.h"
#include "../../physics/rigidbody.h"
#include "../../algorithms/bounds.h"
#include "../../algorithms/States.hpp"
#include "../../scene.h"

//model switches
#define DYNAMIC			 (unsigned int) 1 // 01 
#define CONST_INSTANCES  (unsigned int) 2 // 10
#define NO_TEX			 (unsigned int) 4 //100

class Scene; //forward declaration 

class Model {
public:

	// id of model in scene
	std::string id;

	RigidBody rb;
	glm::vec3 size;



	// list of meshes 
	std::vector<Mesh> meshes;


	//pointer to the collision model
	CollisionModel* collision;

	//list of bounding region (1 for each mesh)
	std::vector<BoundingRegion> boundingRegions;

	//list of instances
	std::vector<RigidBody*> instances;


	//maximum number of indices
	unsigned int maxNoInstances;
	unsigned int currentNoInstances;

	unsigned int switches;


	Model();
	Model(std::string id, unsigned int maxNoInstances, unsigned int falgs = 0);

	//initialize method
	virtual void init();

	RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot);

	void initInstances();


	//load model from path
	void loadModel(std::string path);

	//enable a collision model
	void enableCollisionModel();

	//add a mesh to the list
	void addMesh(Mesh* mesh);

	//render instance(s)
	virtual void render(Shader shader, float DeltaTime,Scene* scene);


	void cleanUp();

	void removeInstance(unsigned int idx);

	void removeInstance(std::string instanced);

	unsigned int getIdx(std::string id);
	void resetRBCalc();

protected:
	bool noTex;
	
	std::string directory;

	//list of loaded textures
	std::vector<texture> textures_load;

	//process node in object file
	void processNode(aiNode* node, const aiScene* scene);

	//process mesh in object file
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//process of custom mesh
	Mesh processMesh(BoundingRegion br, unsigned int noVertices, float* vertices,
		unsigned int noIndices, unsigned int* indices,
		bool calcTanVectors = true,
		unsigned int noCollisionPoints = 0, float* collisionPoints = NULL,
		unsigned int noCollisionFaces = 0, unsigned int* CollisionIndices = NULL,
		bool pad = false);

	std::vector<texture> loadTextures(aiMaterial* mat, aiTextureType type);


	//VBOs for model matrices
	BufferObject modelVBO;
	BufferObject normalModelVBO;

private:
	

};

#endif // !MODEL_H
