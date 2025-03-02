#include "scene.h"

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 2

unsigned int Scene::srcWidth = 0;
unsigned int Scene::srcHeight = 0;



std::string Scene::generateId() {
	for (int i = currenId.length() - 1; i >= 0; i--) {
		if ((int)currenId[i] != (int)'z') {
			currenId[i] = (char)(((int)currenId[i]) + 1);  // берем currid по индексу,переводим в инт добавляем 1 и переводим обратно в чар 
			break;
		}
		else
		{
			currenId[i] = 'a';

		}
	}
	return currenId; // Возвращаем обновленный идентификатор
}
void Scene::framebufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::srcWidth = width;
	Scene::srcHeight = height;
	
}

Scene::Scene() :currenId("aaaaaaaa"),lightUBO(0)
{
}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight)
	:glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), titles(title), activeCamera(-1), activePointLights(0), activeSpotLights(0),
	currenId("aaaaaaaa"), lightUBO(0)
{
	Scene::srcWidth = srcWidth;
	Scene::srcHeight = srcHeight;
	defaultFBO = FramebufferObject(srcWidth, srcHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);  // создаём буфер, который рендерит в текстуру
	
	setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);

}

bool Scene::init()
{
	glfwInit();

	//set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif // _APPLE_

	//initialize window
	window = glfwCreateWindow(srcWidth, srcHeight, titles, NULL, NULL);
	if (window == NULL)
	{
		//not created
		return false;
	}
	glfwMakeContextCurrent(window);

	//set GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;

	}

	//setup screen
	glViewport(0, 0, srcWidth, srcHeight);

	//callBacks

	//frame size
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallBack);
	//key pressed
	glfwSetKeyCallback(window, keyboard::keyCallback);
	//cursor moved
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	//mouse btn pressed
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	//mouse scroll
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);



	//set rendering parametres


	//depth testing
	glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera
	//glDepthFunc(GL_ALWAYS);

	//blending for text  textures 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//stencil testing
	glEnable(GL_STENCIL_TEST);
	//keep fragments if either stencil or depth = fails, replace if they both pass
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	glfwSwapInterval(1);  // для вертикальной синхронизации

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disable cursor

	/*
	   init model/instance trees
   */

	models = avl_createEmptyRoot(strkeycmp);
	instances = trie::Trie<RigidBody*>(trie::ascii_lowercase);

	octree = new Octree::node(BoundingRegion(glm::vec3(-16.0f), glm::vec3(16.0f)));


	//initialize freetype library
	if (FT_Init_FreeType(&ft)){
		std::cout << "could not init FreeType library" << std::endl;

		return false;
	}

	//insert font
	fonts.insert("comic", TextRenderer(32));
	if (!fonts["comic"].loadFont(ft, "Assets/fonts/comic.ttf")) {
		std::cout << "could not load font " << std::endl;
		return false;
	}

	FT_Done_FreeType(ft);

	//setup lighting variable
	VariableLog["useBlinn"] = true;
	VariableLog["useGamma"] = false;
	VariableLog["displayOutlines"] = false;
	VariableLog["noNormalMap"] = false;

	return true;

	//return false;
}
//prepare for main loop (after object generation, etc)
void Scene::prepare(Box& box, std::vector<Shader> shaders){


	octree->update(box);

	//setUp lighting UBO
	lightUBO = UBO::UBO(0, {
		UBO::newStruct({  // dirlight
			UBO::Type::VEC3,  //position

			UBO::Type::VEC4,  // ambient
			UBO::Type::VEC4,  //diffuse
			UBO::Type::VEC4, // specular

			UBO::Type::SCALAR , //farPlane


			UBO::newColMat(4,4) // light space matrix

			}),

			UBO::Type::SCALAR, //no point lights
			UBO::newArray(MAX_POINT_LIGHTS, UBO::newStruct({
			UBO::Type::VEC3,  // position

			UBO::Type::VEC4,  // ambient
			UBO::Type::VEC4,  //diffuse
			UBO::Type::VEC4, // specular

			UBO::Type::SCALAR, //k1
			UBO::Type::SCALAR, //k2
			UBO::Type::SCALAR, //k3

			UBO::Type::SCALAR, // farplane

					})),


			UBO::Type::SCALAR, // no SpotLight
			UBO::newArray(MAX_SPOT_LIGHTS, UBO::newStruct({
			UBO::Type::VEC3,  // position
			UBO::Type::VEC3,  // direction

			UBO::Type::SCALAR, // cutOff
			UBO::Type::SCALAR, // outerCutOff

			UBO::Type::VEC4,  // ambient
			UBO::Type::VEC4,  //diffuse
			UBO::Type::VEC4, // specular

			UBO::Type::SCALAR, //k1
			UBO::Type::SCALAR, //k2
			UBO::Type::SCALAR, //k3
			 
			UBO::Type::SCALAR, //NearPlane
			UBO::Type::SCALAR, //farPlane


			UBO::newColMat(4,4) // light space matrix
				})),




		
		});

	//attach the ubo to specified shaders
	for (Shader s : shaders) {
		lightUBO.attachToShader(s, "Lights");

	}

	//setup memory
	lightUBO.generate();
	lightUBO.bind();
	lightUBO.initNullData(GL_STATIC_DRAW);
	lightUBO.bindRange(); //whar area of memory will be occupied

	//write initial values
	lightUBO.startWrite();


	//directional light
	lightUBO.writeElement<glm::vec3>(&dirLight->direction);
	lightUBO.writeElement<glm::vec4>(&dirLight->ambient);
	lightUBO.writeElement<glm::vec4>(&dirLight->diffuse);
	lightUBO.writeElement<glm::vec4>(&dirLight->specular);
	lightUBO.writeElement<float>(&dirLight->br.max.z);
	lightUBO.writeArrayContainer<glm::mat4, glm::vec4>(&dirLight->lightSpaceMatrix, 4);
	//point light
	noPointLights = std::min<unsigned int>(pointLights.size(), MAX_POINT_LIGHTS);
	lightUBO.writeElement<unsigned int >(&noPointLights);

	unsigned int i = 0;

	for (; i < noPointLights; i++) {
		lightUBO.writeElement<glm::vec3>(&pointLights[i]->position);

		lightUBO.writeElement<glm::vec4>(&pointLights[i]->ambient);
		lightUBO.writeElement<glm::vec4>(&pointLights[i]->diffuse);
		lightUBO.writeElement<glm::vec4>(&pointLights[i]->specular);

		lightUBO.writeElement<float>(&pointLights[i]->k0);
		lightUBO.writeElement<float>(&pointLights[i]->k1);
		lightUBO.writeElement<float>(&pointLights[i]->k2);

		lightUBO.writeElement<float>(&pointLights[i]->farPlane);

	}
	lightUBO.advanceArray(MAX_POINT_LIGHTS - i);// advance to finish array
	
	//spot light
	noSpotLights = std::min<unsigned int>(spotLights.size(), MAX_SPOT_LIGHTS);
	lightUBO.writeElement<unsigned int >(&noSpotLights);
	
	for (i = 0 ; i < noSpotLights; i++) {
		lightUBO.writeElement<glm::vec3>(&spotLights[i]->position);
		lightUBO.writeElement<glm::vec3>(&spotLights[i]->direction);

		lightUBO.writeElement<float>(&spotLights[i]->cutOff);
		lightUBO.writeElement<float>(&spotLights[i]->outerCutOff);


		lightUBO.writeElement<glm::vec4>(&spotLights[i]->ambient);
		lightUBO.writeElement<glm::vec4>(&spotLights[i]->diffuse);
		lightUBO.writeElement<glm::vec4>(&spotLights[i]->specular);

		lightUBO.writeElement<float>(&spotLights[i]->k0);
		lightUBO.writeElement<float>(&spotLights[i]->k1);
		lightUBO.writeElement<float>(&spotLights[i]->k2);


		lightUBO.writeElement<float>(&spotLights[i]->nearPlane);
		lightUBO.writeElement<float>(&spotLights[i]->farPlane);

		lightUBO.writeArrayContainer<glm::mat4, glm::vec4>(&spotLights[i]->lightSpaceMatrix, 4);
	}


	lightUBO.clear();
	
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{ // active camera exist

		//set camera direction
		//if(!Mouse::getDX() == 0 && Mouse::getDY() ==0)
		double dx = Mouse::getDX(), dy = Mouse::getDY();
		if (dx != 0 || dy != 0)
		cameras[activeCamera]->updateCameraDirection(dx, dy);


		//set camera zoom
		double scrollDy = Mouse::getScrollDY();
		if(scrollDy !=0)
		cameras[activeCamera]->updateCameraZoom(scrollDy);


		//set camera pos
		if (keyboard::Key(GLFW_KEY_W)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);



		}
		if (keyboard::Key(GLFW_KEY_S)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);


		}
		if (keyboard::Key(GLFW_KEY_A)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);

		}
		if (keyboard::Key(GLFW_KEY_D)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);

		}
		if (keyboard::Key(GLFW_KEY_SPACE)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
		}
		if (keyboard::Key(GLFW_KEY_LEFT_SHIFT)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
		}

		//update outline parametr if necessary
		if (keyboard::KeyWentDown(GLFW_KEY_O)) {
			VariableLog["displayOutlines"] = !VariableLog["displayOutlines"].val<bool>();
		}

		if (keyboard::KeyWentUp(GLFW_KEY_N)) {
			VariableLog["noNormalMap"] = !VariableLog["noNormalMap"].val<bool>();
		}

		//set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(glm::radians(cameras[activeCamera]->getZoom()), //Fov
			(float)srcWidth /(float)srcHeight,                                     // aspect ration
			0.1f, 100.0f                                                           // near and far bounds
			);

		textProjection = glm::ortho(0.0f, (float)srcWidth, 0.0f, (float)srcHeight); //glm::ortho(left, right, bottom, top);


		//set pos ath the end
		cameraPos = cameras[activeCamera]->cameraPos;

		//update blinn parameter if necessary
		if (keyboard::KeyWentDown(GLFW_KEY_K)) {

			VariableLog["useBlinn"] = !VariableLog["useBlinn"].val<bool>();

		}

		//update gamma parametr
		if (keyboard::KeyWentDown(GLFW_KEY_G)) {
			VariableLog["useGamma"] = !VariableLog["useGamma"].val<bool>();
		}

	}


}

void Scene::update()
{  // set background color
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	// clear occupied bits
	defaultFBO.clear();
	
	defaultFBO.resize(srcWidth, srcHeight);
}

void Scene::newFrame(Box& box)
{  //process pending
	box.positions.clear();
	box.sizes.clear();

	 // process pending objects
	octree->processPending();
	octree->update(box);

	
	
	// send new frame to window
	glfwSwapBuffers(window);
	glfwPollEvents();


}

void Scene::renderShader(Shader shader, bool applyLighting)
{  // activate shader 
	shader.activate();

	//set camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);


	//lightning
	if (applyLighting) {
		unsigned int textureIdx = 31;

		if (dirLightActive) {
			dirLight->render(shader, textureIdx--);
		}
		//point light
		unsigned int noLight = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLight; i++)
		{
			if (States::isIndexActive(&activePointLights, i))
			{  // чекаем биты в activePointLights, если 1 то, active
				pointLights[i]->render(shader, noActiveLights, textureIdx--);
				noActiveLights ++;
			
			}
		}
		shader.setInt("noPointLights", noActiveLights);
		
		//spot lights
		noLight = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLight; i++){
			if (States::isIndexActive(&activeSpotLights, i)) {
				// чекаем биты в activeSpotLights, если 1 то, active
				spotLights[i]->render(shader, noActiveLights, textureIdx--);
				noActiveLights++;


			}
		}
		shader.setInt("noSpotLights", noActiveLights);

		//direction light
		shader.setBool("useDirLight", dirLightActive);
		
		shader.setBool("useBlinn", VariableLog["useBlinn"].val<bool>());
		shader.setBool("useGamma", VariableLog["useGamma"].val<bool>());
		shader.setBool("skipNormalMap", VariableLog["noNormalMap"].val<bool>());
	}

}

void Scene::renderDirLightShader(Shader shader){
	shader.activate();
	shader.setMat4("lightSpaceMatrix", dirLight->lightSpaceMatrix); // замена projections ( теперь смотрит от света) 

}

void Scene::renderPointLightShader(Shader Shader, unsigned int idx){

	Shader.activate();

	//light space matrices
	for (unsigned int i = 0; i < 6; i++) {
		Shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", pointLights[idx]->lightSpaceMatrices[i]); //clipspace matrix viwe * proj
	}

	//light position
	Shader.set3Float("lightPos", pointLights[idx]->position);

	//far plane
	Shader.setFloat("farPlane", pointLights[idx]->farPlane);
}

void Scene::renderSpotLightShader(Shader shader, unsigned int idx){
	shader.activate();
	// light space	matrix
	shader.setMat4("lightSpaceMatrix", spotLights[idx]->lightSpaceMatrix);

	//light position
	shader.set3Float("lightPos", spotLights[idx]->position);

	//far plane
	shader.setFloat("farPlane", spotLights[idx]->farPlane);
}

void Scene::renderInstances(std::string modelId, Shader shader, float dt){
	void* val = avl_get(models, (void*)modelId.c_str());

	if (val) {
	shader.activate();
	((Model*)val)->render(shader, dt, this);
	}


}

void Scene::renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color){
	shader.activate();
	shader.setMat4("projection", textProjection);

	fonts[font].render(shader, text, x, y, scale, color);


}

void Scene::cleanUp(){
	// clean all models
	avl_postorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->cleanUp();
		});
	avl_free(models);
	instances.cleanup();

	//clean fonts
	fonts.traverse([](TextRenderer tr)-> void {
		tr.cleanUp();
		});
	fonts.cleanup();


	
	octree->destroy();

	glfwTerminate();
}

bool Scene::shouldClose()
{
	return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera()
{
	return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

void Scene::setShouldCloses(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a)
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
	bg[3] = a;
}

void Scene::registerModel(Model* model){
	models = avl_insert(models, (void*)model->id.c_str(), model);
}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot){
	// generate new rigid body
	void* val = avl_get(models, (void*)modelId.c_str());
	if (val) {
		Model* model = (Model*)val;
		RigidBody* rb = model->generateInstance(size, mass, pos, rot);
		if (rb) {
			// successfully generated, set new and unique id for instance
			std::string id = generateId();
			rb->instanceId = id;
			// insert into trie
			instances.insert(rb->instanceId, rb);
			// insert into pending queue
			octree->addToPending(rb, model);
			return rb;
		}
	}
	return nullptr;
}

void Scene::initInstances() {
	// initialize all instances for each model
	avl_inorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->initInstances();
		});
}
void Scene::loadModels(){
	// initialize each model
	avl_inorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->init();
		});
}

void Scene::removeInstance(std::string instanceId){
	RigidBody* instance = instances[instanceId];
	std::string targetModel = instances[instanceId]->modelId;

	Model* model = (Model*)avl_get(models, (void*)targetModel.c_str());

	// delete instance from model
	model->removeInstance(instanceId);

	// remove from tree
	instances[instanceId] = NULL;
	instances.erase(instanceId);
	free(instance);
	


}

void Scene::markForDeletion(std::string instanceId)
{
	States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
	instancesToDelete.push_back(instances[instanceId]);
}

void Scene::clearDeadInstances(){
	for (RigidBody* rb : instancesToDelete) {
		removeInstance(rb->instanceId);

	}
	instancesToDelete.clear();

}



//void Scene::ChagnePointLightColor(glm::vec3 color, int index)
//{
//	
//}
