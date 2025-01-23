#ifndef  SCENE_H
#define SCENE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/model.h"

#include "io/Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

#include "algorithms/States.hpp"
#include "algorithms/Trie.hpp"

class Model;

class Scene {
public:

	trie::Trie<Model*>models;
	trie::Trie<RigidBody*> instances;

	std::vector<RigidBody*> instancesToDelete;


	//Callbacks
	

	//window resize
	static void  framebufferSizeCallBack(GLFWwindow* window, int width, int height);


	//constructor
	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight);

	//initialization
	bool init();


	//main loop methods

	//process input
	void processInput(float dt);

	//update screen before each frame 
	void update();

	//update screen after each frame
	void newFrame();

	//set uniform shader variables;
	void renderShader(Shader shader, bool applyLighting = true);

	void renderInstances(std::string modelId, Shader shader, float dt);


	//cleanup method
	void cleanUp();

	//accessors
	bool shouldClose();

	Camera* getActiveCamera();


	//modifiers
	void setShouldCloses(bool shouldclose);

	void setWindowColor(float r, float g, float b, float a);

	void ChagnePointLightColor(glm::vec3 color,int index);

	//model Instance methods
	void registerModel(Model* model);
	RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);

	void initInstances();
	void loadModels();

	void removeInstance(std::string instanceId);

	void markForDeletion(std::string instanceId);
	void clearDeadInstances();


	 std::string currenId;
	 std::string generateId();


	//lights
	

	//list of point lights
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;
	//list of spor lights
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;
	//direction light
	DirLight* dirLight;
	bool dirLightActive;


	//Camers
	std::vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	//window objects
	GLFWwindow* window;
	//window values
	const char* titles;
	static unsigned int srcWidth;  //потому что функци callframbufer статик
	static unsigned int srcHeight;

	float bg[4]; //backgroun colors

	//GLFW info 
	int glfwVersionMajor;
	int glfwVersionMinor;


};

#endif // ! SCENE_H
