#include "model.h"
#include "../../physics/environment.h"

#include "../../scene.h"
Model::Model()
{
}

Model::Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags) :
	id(id),boundType(boundType),switches(flags),currentNoInstances(0), maxNoInstances(maxNoInstances),
	collision(nullptr) {}

void Model::init()
{
	
}

RigidBody* Model::generateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rotation)
{
	if (currentNoInstances >= maxNoInstances) {
		//all slots filled
		return nullptr;

	}
	instances.push_back(new RigidBody(id, size, mass, pos, rotation));
	return instances[currentNoInstances++];  // �� ���������� ��������� ������ ����� ����������� ����������
	return 0;
}

void Model::initInstances(){
	//default values
	GLenum usage = GL_DYNAMIC_DRAW;
	glm::mat4* modelData = nullptr;
	glm::mat3* normalModelData = nullptr;


	std::vector<glm::mat4> models(currentNoInstances);
	std::vector<glm::mat3> normalModels(currentNoInstances);

	if (States::isActive(&switches, CONST_INSTANCES)) {
		//set data pointers
		for (unsigned int i = 0; i < currentNoInstances; i++) {
			models[i] = instances[i]->model; // we take matrix from instances (scale, translate,rotate) assign it
			normalModels[i] = instances[i]->normalModel;  // for normal model we don't need translate it does not have influence on normals

		}
		if (currentNoInstances) {
			modelData = &models[0]; // ��������� ������ ��������� �� ������ ������� �������
			normalModelData = &normalModels[0]; //
		}

		usage = GL_STATIC_DRAW;
	}
	
	//generate matrix Vertex buffer object
	modelVBO = BufferObject(GL_ARRAY_BUFFER);
	modelVBO.generate();
	modelVBO.bind();
	modelVBO.setData<glm::mat4>(UPPER_BOUND, modelData, usage);  // UPPER BOUND - 100 � box.hpp

	normalModelVBO = BufferObject(GL_ARRAY_BUFFER);
	normalModelVBO.generate();
	normalModelVBO.bind();
	normalModelVBO.setData<glm::mat3>(UPPER_BOUND, normalModelData, usage);

	//set attribute pointes for each mesh
	for (unsigned int i = 0, size = meshes.size(); i < size; i++)
	{


		meshes[i].VAO.bind();

		//set vertex attrib pointer
		modelVBO.bind();
		modelVBO.setAttPointer<glm::vec4>(4, 4, GL_FLOAT, 4, 0, 1);
		modelVBO.setAttPointer<glm::vec4>(5, 4, GL_FLOAT, 4, 1, 1);
		modelVBO.setAttPointer<glm::vec4>(6, 4, GL_FLOAT, 4, 2, 1);
		modelVBO.setAttPointer<glm::vec4>(7, 4, GL_FLOAT, 4, 3, 1);


		normalModelVBO.bind();
		normalModelVBO.setAttPointer<glm::vec3>(8, 3, GL_FLOAT, 3, 0, 1);
		normalModelVBO.setAttPointer<glm::vec3>(9, 3, GL_FLOAT, 3, 1, 1);
		normalModelVBO.setAttPointer<glm::vec3>(10, 3, GL_FLOAT, 3, 2, 1);
		

		//positions      // stride 1 * sizeof(glm::vec3) ��� ��� ��� ��������� ������   //1 divisor  for reset every instance
		


		ArrayObject::clear();
	}
}

//add a mesh to the list
void Model::addMesh(Mesh* mesh){
	meshes.push_back(*mesh);
	boundingRegions.push_back(mesh->br);
}

void Model::render(Shader shader, float deltaTime, Scene* scene){
	
	if (!States::isActive(&switches, CONST_INSTANCES)) {
		// dynamic instances - update VBO data

		//create list of each
		std::vector<glm::mat4> models(currentNoInstances);
		std::vector<glm::mat3> normalModels(currentNoInstances);

		//determine if instances are moving
		bool doUpdate = States::isActive(&switches, DYNAMIC);

		//iterate through each instance
		for (int i = 0; i < currentNoInstances; i++) {
			if (doUpdate) {
				//update Rigid Body
				instances[i]->update(deltaTime);
				//activate moved witch
				States::activate(&instances[i]->state, INSTANCE_MOVED);

			}
			else
			{
				//deactivate movedswitch
				States::deactivate(&instances[i]->state, INSTANCE_MOVED);

			}
			//add update matrices position and sizes
			models[i] = instances[i]->model;
			normalModels[i] = instances[i]->normalModel;

		}

		if (currentNoInstances) {
			//set transformation data
			modelVBO.bind();
			modelVBO.updateData<glm::mat4>(0, currentNoInstances, &models[0]);
			normalModelVBO.bind();
			normalModelVBO.updateData<glm::mat3>(0, currentNoInstances, &normalModels[0]);
		}

	}
	shader.setFloat("material.shininess", 0.5f);

	
	for (unsigned int i = 0, noMeshes = meshes.size(); i < noMeshes; i++) {
		meshes[i].render(shader, currentNoInstances);
	}

}

void Model::CleanUp()
{
	for (Mesh mesh : meshes) {

		mesh.cleanup();
	}

	modelVBO.cleanUp();
	normalModelVBO.cleanUp();

}

void Model::removeInstance(unsigned int idx){
	instances.erase(instances.begin() + idx);
	currentNoInstances--;

}

void Model::removeInstance(std::string instanced)
{
	int idx = getIdx(instanced);
	if (idx != -1) {
		instances.erase(instances.begin() + idx);
		currentNoInstances--;
	}
}

unsigned int Model::getIdx(std::string id){
	for (int i = 0; i < currentNoInstances; i++) {
		if (instances[i]->instanceId == id) {  // �� �� ������������� ��������� == �� ����� ����� �������� rigidbody == string 
			return i;
		}
	}

	return -1;
}

void Model::loadModel(std::string path) {  //����� ��������� 3D-������ �� ���������� �����.
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout <<"Coulde not load file at "<<path <<std::endl << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/")); //����������� ���� � ��������, � ������� ��������� ������ (��� ������ � ����������).
	processNode(scene->mRootNode, scene); //�������������� �������� ���� �����
}

//enable a collision model
void Model::enableCollisionModel(){
	if (!this->collision) {
		this->collision = new CollisionModel(this);
	}

}

void Model::processNode(aiNode* node, const aiScene* scene) //����� ������������ ������� ���� � ��� �������� ����.
{	//process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];  // �������� ��������� �� ��� �� �����
		Mesh newMesh = processMesh(mesh, scene); // ������� �����, ������� ������ ������ mesh �� ������ ������ ����
		addMesh(&newMesh);  // ������� � ��������� meshes � ������� � ��������� bounding region
		
	}

	//process all child nodes

	for (unsigned int i = 0; i < node->mNumChildren; i++)  // �������� � �������� �� ����� ����
	{ 
		processNode(node->mChildren[i], scene);  // ���� ����� ���� �������� ��� �� �������, ������ ��� ��� ������� 
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) { //����� �������� ,�������, �������� 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture> textures;

	BoundingRegion br(boundType);
	glm::vec3 min((float)(~0));     //min point = max float  // ����������� ���������� ���������� � ������������� float
	glm::vec3 max(-(float)(~0));     //max point =min float   // ������������ ���������� ���������� � ������������ float

	//vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//position
		vertex.pos = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z


		);
		//MIN MAX finding for bounding box 
		for (int j = 0; j < 3; j++)
		{	// if smaller than  min
			if (vertex.pos[j] < min[j])
				min[j] = vertex.pos[j];

			//if larger than max
			if (vertex.pos[j] > max[j])
				max[j] = vertex.pos[j];
		}
		//normal vectors
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		//textures
		if (mesh->mTextureCoords[0]) {
			vertex.texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y

				);


		}
		else  //���� ��� ������� 
		{
			vertex.texCoord = glm::vec2(0.0f);
		}

		//tangent vector
		vertex.tangent = {
			mesh->mTangents[i].x,
			mesh->mTangents[i].y,
			mesh->mTangents[i].z
		};

		vertices.push_back(vertex);
	}

	//process min/max for BR
	if (boundType == BoundTypes::AABB) {
		//assign max and min
		br.max = max;
		br.ogMax = max;
		br.min = min;
		br.ogMin = min;

	}
	else
	{	//calculate max distance from center
		br.center = BoundingRegion(min, max).calculateCenter();
		br.ogCenter = br.center;

		float MaxRadiusSquare = 0.0f;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			float radiusSquared = 0.0f; // distance for this vertex
			for (int j = 0; j < 3; j++)
			{ // vertex = sqrtf((vertices - center)*(vertices - center)) ������ ���� ������ �� ����������� 
				radiusSquared += (vertices[i].pos[j] - br.center[j]) * (vertices[i].pos[j] - br.center[j]); //������� ���������� �� ������ �����
			}
			if (radiusSquared > MaxRadiusSquare) { //���� ��� ������� ������� ���������� ������ =���������
				//found new squere radius
				//if a^2 >b^2 -> |a| > |b|
				MaxRadiusSquare = radiusSquared;

			}


		}
		br.radius = sqrt(MaxRadiusSquare);
		br.ogRadius = br.radius;

	}

	//process indicess
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);

		}
	}
	Mesh ret;
	
	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		if (States::isActive<unsigned int>(&switches, NO_TEX)) {
			// 1. diffuse colors
			aiColor4D diff(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
			// 2. specular colors
			aiColor4D spec(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);
		

			ret = Mesh(br, diff, spec);
		}
		else {
			// 1. diffuse maps
			std::vector<texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			//3. normal maps
			//.obj, we use aiTextureType_HEIGHT
			std::vector<texture> normalMaps = loadTextures(material, aiTextureType_NORMALS);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());



			ret = Mesh(br, textures);
		}
	}

	ret.loadData(vertices, indices);
	return ret;
}
//process of custom mesh
Mesh Model::processMesh(BoundingRegion br,
	unsigned int noVertices, float* vertices,
	unsigned int noIndices, unsigned int* indices,
	bool calcTanVectors,
	unsigned int noCollisionPoints, float* collisionPoints,
	unsigned int noCollisionFaces, unsigned int* CollisionIndices,
	bool pad){
	//process vertex array
	std::vector<Vertex> vertexList = Vertex::genList(vertices, noVertices);


	//create index list
	std::vector<unsigned int> indexList(noIndices);
	if (indices) {
		//copy array

		//      ���� ��������    ������     � ����� ������  ( �������� 23 �������� * �� 32 bite)
		memcpy(indexList.data(), indices, noIndices * sizeof(unsigned int));

	}
	else{
		//insert sequential indices
		for (unsigned int i = 0; i < noIndices; i++) {
			indexList[i] = i;	

		}

	}
	//calculate the lighting values
	if (calcTanVectors) {
		Vertex::calcTanVectors(vertexList, indexList);
		
	}

	//ssetup return mesh
	Mesh ret(br);
	ret.loadData(vertexList, indexList, pad);

	//allocate collision mesh if specified
	if (noCollisionPoints) {
		enableCollisionModel();
		ret.loadCollisionMesh(noCollisionPoints,collisionPoints,noCollisionFaces,CollisionIndices);
	}


	return ret;
}

std::vector<texture> Model::loadTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);  //���������� ���� � �������� � str
	


		//prevent duplicate loading 
		bool skip = false;
		for (unsigned int j = 0; j < textures_load.size(); j++)
		{
			//���������� ���� ������� �������� str � ������ �������, ������� ��� ���� ���������
			if (std::strcmp(textures_load[j].path.data(), str.C_Str()) == 0) //���� ���������� ������� (std::strcmp ���������� 0), �������� ��������� ����������.
			{
				textures.push_back(textures_load[j]); //�������� �� textures_load ����������� � �������������� ������ textures
				skip = true;
				break;
			}
		}
		

		if (!skip) {
			//not loaded yet
			texture tex(directory, str.C_Str(), type);  
			tex.load(false); // �� ������� ���������� 
			textures.push_back(tex);
			textures_load.push_back(tex);

		}

	}

	return textures;
}

