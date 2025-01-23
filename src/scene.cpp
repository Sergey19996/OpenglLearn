#include "scene.h"


unsigned int Scene::srcWidth = 0;
unsigned int Scene::srcHeight = 0;



std::string Scene::generateId(){
	for (int i = currenId.length() - 1; i >= 0; i--) {
		if ((int)currenId[i] != (int)'z') {
			currenId[i] = (char)(((int)currenId[i]) + 1);  // берем curridпо индексу,переводим в инт добавляем 1 и переводим обратно в чар 
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

Scene::Scene() :currenId("aaaaaaaa")
{
}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight)
	:glfwVersionMajor(glfwVersionMajor),glfwVersionMinor(glfwVersionMinor),titles(title),activeCamera(-1),activePointLights(0),activeSpotLights(0),
	currenId("aaaaaaaa")
{
	Scene::srcWidth = srcWidth;
	Scene::srcHeight = srcHeight;
	setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);

}

bool Scene::init()
{
	glfwInit();

	//set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

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
	glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disable cursor
	return true;


	//return false;
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

		//set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(glm::radians(cameras[activeCamera]->getZoom()), //Fov
			(float)srcWidth /(float)srcHeight,                                     // aspect ration
			0.1f, 100.0f                                                           // near and far bounds
			);

		//set pos ath the end
		cameraPos = cameras[activeCamera]->cameraPos;

	}


}

void Scene::update()
{
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}

void Scene::newFrame()
{  // send new frame to window
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
		//point light
		unsigned int noLight = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLight; i++)
		{
			if (States::isActive(&activePointLights, i))
			{  // чекаем биты в activePointLights, если 1 то, active
				pointLights[i]->render(shader, noActiveLights);
				noActiveLights ++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);


		//spot lights
		noLight = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLight; i++){
			if (States::isActive(&activeSpotLights, i)) {
				// чекаем биты в activeSpotLights, если 1 то, active
				spotLights[i]->render(shader, noActiveLights);
				noActiveLights++;


			}
		}
		shader.setInt("noSpotLights", noActiveLights);

		//direction light
		dirLight->render(shader);


	}

}

void Scene::renderInstances(std::string modelId, Shader shader, float dt){
	models[modelId]->render(shader, dt, this);


}

void Scene::cleanUp(){
	models.traverse([](Model* model)->void { model->CleanUp();});
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
	models.insert(model->id, model); // Сохраняем модель в Trie

}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos){
	RigidBody* rb= models[modelId]->generateInstance(size, mass, pos);  // мы создаем копию rigidbody по modelId
	if (rb) {
		//successfuly generated
		std::string id = generateId();  // Генерация Id для Rigidbody
		rb->instanceId = id;    //присваиваем её 
		instances.insert(id, rb);
		return rb;

		  
	}

	return nullptr;
}

void Scene::initInstances(){
	models.traverse([](Model* model)->void {model->initInstances(); });
}

void Scene::loadModels(){
	models.traverse([](Model* model)->void {model->init(); });
}

void Scene::removeInstance(std::string instanceId){
	//remove all locations
	//-scene::instances
	//- Model::instances
	std::string targetModel = instances[instanceId]->modelId;

	models[targetModel]->removeInstance(instanceId);

	instances[instanceId] = nullptr;

	instances.erase(instanceId);
	


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
