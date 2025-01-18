#include "model.h"
#include "../physics/environment.h"


Model::Model()
{
}

Model::Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags) :
	id(id),boundType(boundType),switches(flags),currentNoInstances(0), maxNoInstances(maxNoInstances)
{

}

void Model::init()
{
	
}

unsigned int Model::generateInstance(glm::vec3 size, float mass, glm::vec3 pos)
{
	if (currentNoInstances >= maxNoInstances) {
		//all slots filled
		return -1;

	}
	instances.push_back(RigidBody(&id, size, mass, pos));
	return currentNoInstances++;  // �� ���������� ��������� ������ ����� ����������� ����������
	return 0;
}

void Model::initInstances(){
	glm::vec3* posData = nullptr;
	glm::vec3* sizeData = nullptr;
	GLenum usage = GL_DYNAMIC_DRAW;


	std::vector<glm::vec3>positions, sizes;

	if (States::isActive(&switches, CONST_INSTANCES)) {
		//set data pointers
		for (unsigned int i = 0; i < currentNoInstances; i++) {
			positions.push_back(instances[i].pos);
			sizes.push_back(instances[i].size);

		}
		if (positions.size() > 0) {
			posData = &positions[0];
			sizeData = &sizes[0];
		}

		usage = GL_STATIC_DRAW;
	}
	//generate posiitons VBO
	posVBO = BufferObject(GL_ARRAY_BUFFER);
	posVBO.generate();
	posVBO.bind();
	posVBO.setData<glm::vec3>(UPPER_BOUND, posData, GL_DYNAMIC_DRAW);


	//generate zise VBO

	sizeVBO = BufferObject(GL_ARRAY_BUFFER);
	sizeVBO.generate();
	sizeVBO.bind();
	sizeVBO.setData<glm::vec3>(UPPER_BOUND, sizeData, GL_DYNAMIC_DRAW);

	//set attribute pointes for each mesh
	for (unsigned int i = 0, size = meshes.size(); i < size; i++)
	{


		meshes[i].VAO.bind();

		//set vertex attrib pointer
		//positions      // stride 1* sizeof(glm::vec3) ��� ��� ��� ��������� ������   //1 divisor  for reset every instance
		posVBO.bind();//                                      diviser 1 
		posVBO.setAttPointer<glm::vec3>(3, 3, GL_FLOAT, 1, 0, 1);
		//Size
		sizeVBO.bind();
		sizeVBO.setAttPointer<glm::vec3>(4, 3, GL_FLOAT, 1, 0, 1);


		ArrayObject::clear();
	}
}


void Model::render(Shader shader, float deltaTime, Scene* scene, bool  setModel){
	if (setModel) {
		shader.setMat4("model", glm::mat4(1.0f));

	}
	if (!States::isActive(&switches, CONST_INSTANCES)) {
		//update VBO data

		std::vector<glm::vec3> positions, sizes;
		bool doUpdate = States::isActive(&switches, DYNAMIC);
		for (int i = 0; i < currentNoInstances; i++) {
			if (doUpdate) {
				//update Rigid Body
				instances[i].update(deltaTime);

			}
			positions.push_back(instances[i].pos);
			sizes.push_back(instances[i].size);

		}

		posVBO.bind();
		posVBO.updateData<glm::vec3>(0, currentNoInstances, &positions[0]);

		sizeVBO.bind();
		sizeVBO.updateData<glm::vec3>(0, currentNoInstances, &sizes[0]);

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

	posVBO.cleanUp();
	sizeVBO.cleanUp();

}

void Model::removeInstance(unsigned int idx){
	instances.erase(instances.begin() + idx);
	currentNoInstances--;

}

unsigned int Model::getIdx(std::string id){
	for (int i = 0; i < currentNoInstances; i++) {
		if (instances[i] == id) {  // �� �� ������������� ��������� == �� ����� ����� �������� rigidbody == string 
			return i;
		}
	}

	return -1;
}

void Model::loadModel(std::string path) {  //����� ��������� 3D-������ �� ���������� �����.
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout <<"Coulde not load file at "<<path <<std::endl << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/")); //����������� ���� � ��������, � ������� ��������� ������ (��� ������ � ����������).
	processNode(scene->mRootNode, scene); //�������������� �������� ���� �����
}

void Model::processNode(aiNode* node, const aiScene* scene) //����� ������������ ������� ���� � ��� �������� ����.
{//process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	//process all child nodes

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
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

		vertices.push_back(vertex);
	}

	//process min/max for BR
	if (boundType == BoundTypes::AABB) {
		//assign max and min
		br.max = max;
		br.min = min;

	}
	else
	{	//calculate max distance from center
		br.center = BoundingRegion(min, max).calculateCenter();
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

			ret = Mesh(br, textures);
		}
	}

	ret.loadData(vertices, indices);
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

