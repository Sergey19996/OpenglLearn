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

RigidBody* Model::generateInstance(glm::vec3 size, float mass, glm::vec3 pos)
{
	if (currentNoInstances >= maxNoInstances) {
		//all slots filled
		return nullptr;

	}
	instances.push_back(new RigidBody(id, size, mass, pos));
	return instances[currentNoInstances++];  // мы возвращаем значенеие только потом возвращение увеличится
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
			positions.push_back(instances[i]->pos);
			sizes.push_back(instances[i]->size);

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
		//positions      // stride 1 * sizeof(glm::vec3) так как это отдельный массив   //1 divisor  for reset every instance
		posVBO.bind();//                                      diviser 1 
		posVBO.setAttPointer<glm::vec3>(4, 3, GL_FLOAT, 1, 0, 1);
		//Size
		sizeVBO.bind();
		sizeVBO.setAttPointer<glm::vec3>(5, 3, GL_FLOAT, 1, 0, 1);


		ArrayObject::clear();
	}
}


void Model::render(Shader shader, float deltaTime, Scene* scene, glm::mat4 model){
	//set model Matrix
	shader.setMat4("model", model);
	shader.setMat3("normalModel", glm::transpose(glm::inverse(glm::mat3(model)))); // подготавливаем матрицу для переводов в мировые координаты

	if (!States::isActive(&switches, CONST_INSTANCES)) {
		//update VBO data

		std::vector<glm::vec3> positions, sizes;
		bool doUpdate = States::isActive(&switches, DYNAMIC);
		for (int i = 0; i < currentNoInstances; i++) {
			if (doUpdate) {
				//update Rigid Body
				instances[i]->update(deltaTime);
				States::activate(&instances[i]->state, INSTANCE_MOVED);

			}
			else
			{
				States::deactivate(&instances[i]->state, INSTANCE_MOVED);

			}
			positions.push_back(instances[i]->pos);
			sizes.push_back(instances[i]->size);

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
		if (instances[i]->instanceId == id) {  // из за перегруженого оператора == мы можем сразу написать rigidbody == string 
			return i;
		}
	}

	return -1;
}

void Model::loadModel(std::string path) {  //Метод загружает 3D-модель из указанного файла.
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout <<"Coulde not load file at "<<path <<std::endl << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/")); //Сохраняется путь к каталогу, в котором находится модель (для работы с текстурами).
	processNode(scene->mRootNode, scene); //Обрабатывается корневой узел сцены
}

void Model::processNode(aiNode* node, const aiScene* scene) //Метод обрабатывает текущий узел и его дочерние узлы.
{//process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		meshes.push_back(newMesh);
		boundingRegions.push_back(newMesh.br);
	}

	//process all child nodes

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) { //Метод вертексы ,нормали, текстуры 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture> textures;

	BoundingRegion br(boundType);
	glm::vec3 min((float)(~0));     //min point = max float  // минимальные координаты начинаются с максимального float
	glm::vec3 max(-(float)(~0));     //max point =min float   // максимальные координаты начинаются с минимального float

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
		else  //если нет текстур 
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
			{ // vertex = sqrtf((vertices - center)*(vertices - center)) только пока корень не высчитываем 
				radiusSquared += (vertices[i].pos[j] - br.center[j]) * (vertices[i].pos[j] - br.center[j]); //квадрат расстояния до центра сферы
			}
			if (radiusSquared > MaxRadiusSquare) { //Если для текущей вершины расстояние больше =обновляем
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

std::vector<texture> Model::loadTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);  //записывает путь к текстуре в str
	


		//prevent duplicate loading 
		bool skip = false;
		for (unsigned int j = 0; j < textures_load.size(); j++)
		{
			//Сравниваем путь текущей текстуры str с путями текстур, которые уже были загружены
			if (std::strcmp(textures_load[j].path.data(), str.C_Str()) == 0) //Если совпадение найдено (std::strcmp возвращает 0), текстура считается дубликатом.
			{
				textures.push_back(textures_load[j]); //Текстура из textures_load добавляется в результирующий вектор textures
				skip = true;
				break;
			}
		}
		

		if (!skip) {
			//not loaded yet
			texture tex(directory, str.C_Str(), type);  
			tex.load(false); // не флипаем координвты 
			textures.push_back(tex);
			textures_load.push_back(tex);

		}

	}

	return textures;
}

