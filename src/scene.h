#ifndef  SCENE_H
#define SCENE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H


#include "graphics/light.h"
#include "graphics/rendering/shader.h"
#include "graphics/objects/model.h"
#include "graphics/models/box.hpp"
#include "graphics/text.h"
#include "graphics/framememory.hpp"
#include "graphics/uniformmemory.hpp"


#include "io/Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

#include "algorithms/States.hpp"
#include "algorithms/Trie.hpp"
#include "algorithms/octree.h"

#include <jsoncpp/json.hpp>
#include "algorithms/avl.h"

namespace Octree {
	class node;
}
class Model;

class Scene {
public:

	
	avl* models;
	
	trie::Trie<RigidBody*> instances;
	

	//list of instances that should be deleted
	std::vector<RigidBody*> instancesToDelete;

	//pointer to root node in octree
	Octree::node* octree;

	//map for logged variable
	jsoncpp::json VariableLog;

	//freetype library
	FT_Library ft;
	trie::Trie<TextRenderer> fonts;


	FramebufferObject defaultFBO;  //�������� �� � ��������
	UBO::UBO lightUBO; // ������ ��� uniform ��� �����


	//Callbacks
	

	//window resize
	static void  framebufferSizeCallBack(GLFWwindow* window, int width, int height);


	//constructor
	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int srcWidth, unsigned int srcHeight);

	//initialization
	bool init();

	void prepare(Box& box, std::vector<Shader> shaders);

	//main loop methods

	//process input
	void processInput(float dt);

	void sceneEvents();
	//update screen before each frame 
	void update();

	//update screen after each frame
	void newFrame(Box& box);

	//set uniform shader variables;
	void renderShader(Shader shader, bool applyLighting = true);


	//set uniform shader variables for directional light render
	void renderDirLightShader(Shader shader);

	//set uniform shader variables for point light render
	void renderPointLightShader(Shader Shader, unsigned int idx);

	//set uniform shader variables for spot light shader
	void renderSpotLightShader(Shader shader, unsigned int idx);


	void renderInstances(std::string modelId, Shader shader, float dt);

	//render text
	void renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);


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
	RigidBody* generateInstance(std::string modelId, glm::vec3 size = glm::vec3(1.0f), float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f));

	void initInstances();
	void loadModels();

	void removeInstance(std::string instanceId);

	void markForDeletion(std::string instanceId);
	void clearDeadInstances();



	 std::string currenId;
	 std::string generateId();


	//lights
	

	//list of point lights
	 unsigned int noPointLights;
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;
	//list of spor lights
	unsigned int noSpotLights;
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;
	//direction light
	DirLight* dirLight;
	bool dirLightActive = true;


	//Camers
	std::vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 textProjection;
	glm::vec3 cameraPos;
	static unsigned int srcWidth;  //������ ��� ������ callframbufer ������
	static unsigned int srcHeight;

protected:
	//window objects
	GLFWwindow* window;
	//window values
	const char* titles;
	

	float bg[4]; //backgroun colors

	//GLFW info 
	int glfwVersionMajor;
	int glfwVersionMinor;


};
void resetRb(avl* node);
#endif // ! SCENE_H
