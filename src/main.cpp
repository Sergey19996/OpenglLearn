// ����������� ����������� ���������
#include <iostream> // ��� ������ ��������� � �������
#include <glad/glad.h> // ��� �������� ������� OpenGL
#include <GLFW/glfw3.h> // ���������� ��� ������ � ������ � �����/������

#include <fstream> //������ std::ifstream (������) � std::ofstream (������).
#include <sstream> //std::stringstream, std::istringstream � std::ostringstream ��� �������������� ������ � ������.
#include <streambuf> //�������������� ��������� � ������� �����/������ ����� ���� ������� � ������� ��� ��������.

#include <string>
#include <stack>
#include<stb/stb_image.h>  //download png files  PNG, JPEG, BMP, TGA � ������

#include <glm/glm.hpp> // glm::vec2, glm::vec3, glm::mat4) � ������� �������������� ��������.
#include <glm/gtc/type_ptr.hpp>  //glm::value_ptr ������ ��� �������� ������ � OpenGL ����� glUniform*.
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale � glm::perspective).

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"
#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/Sphere.hpp"
#include "graphics/models/box.hpp"
#include "graphics//light.h"
#include "graphics/model.h"
#include "physics/environment.h"

#include "algorithms/States.hpp"
#include "scene.h"

Scene scene;
// ���� ������
void processInput(float fdeltatime);

Camera cam;

glm::mat4 transform = glm::mat4(1.0f);




 unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

 

float theta = 45.0f;
bool flashlight = true;

Sphere sphere(10);

int main()
{
    //          version 3.3 opengl
    scene = Scene(3,3,"Fat Boys", 800, 600);
    if (!scene.init()) {

        std::cout << "Couldn't open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;
 //shaders ============================
    Shader lampShader("Assets/instanced/instanced.vs.glsl", "Assets/lamp.fs.glsl");
    Shader shader("Assets/instanced/instanced.vs.glsl", "Assets/object.fs.glsl");
    Shader boxShader("Assets/instanced/box.vs.glsl", "Assets/instanced/box.fs.glsl");
  //Models ==================================
    Lamp lamp(4);
    scene.registerModel(&lamp);
    scene.registerModel(&sphere);
   
    //load all model data
    scene.loadModels();



    //                 direction                   amb                 diff          spec
    DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec4(0.1f,0.1f,0.1f,1.0f),
        glm::vec4(0.4f,0.4f,0.4f,1.0f),
        glm::vec4(0.75f,0.75f,0.75f,1.0f) };
    scene.dirLight = &dirLight;

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f,  0.2f,  2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.09f;
    float k2 = 0.032f;

    PointLight pointLights[4];  // �������� 4 ����� 

  
   
    for (unsigned int i = 0; i < 4; i++) {
        pointLights[i] = {pointLightPositions[i],    //��������������� �������� 
            k0,k1,k2,
            ambient,diffuse,specular
        };
        scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);

        scene.pointLights.push_back(&pointLights[i]);    // ��������������� ������ ��� ������� �����
        States::activate(&scene.activePointLights, i);  // � inte  �� 32 ����� (32 bit) ������������� 1 

    }



    SpotLight spotLight = {
        cam.cameraPos,cam.cameraFront,
        glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(20.0f)), //����� ������� ����� cut 
        1.0f,0.07f,0.032f,   // ��������� ��� ��������� ������ 
        glm::vec4(0.0f,0.0f,0.0f,1.0f),glm::vec4(1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f)

    };
    scene.spotLights.push_back(&spotLight);
    scene.activeSpotLights = 1; // 0b001

    //instanciate instances
    scene.initInstances();



    float lastTime = glfwGetTime();
    float deltaTime = 0.0f;



  
    // ������� ���� ����������
    while (!scene.shouldClose()) // ��������� ����, ���� ���� �� �������
    {
       
      


        // �������� ����� ����� �������
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

       
        scene.update();
        //input Tracking

        processInput(deltaTime);

        //render
        //screen.update();


        //remove launch objects if too far

      
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 100.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);  


            }
        }
        //render launch objects
        if (sphere.currentNoInstances > 0) {
        scene.renderShader(shader);
        scene.renderInstances(sphere.id, shader, deltaTime);
        }

        //render lamps
        scene.renderShader(lampShader);
        scene.renderInstances(lamp.id, lampShader, deltaTime);


        // sen new frame to window
        scene.clearDeadInstances();

       // screen.newFrame();
        scene.newFrame();
       
    }
   
  
  
    // ���������� ������ GLFW, ������������ ��������
    scene.cleanUp();

    return 0;
}
void launchItem(float fdeltatime) {
    glm::vec3 fixpos = { cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z - 0.25f };

   RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.05f), 1.0f, fixpos);  // �������� ������ �����, ��� ��� �������� � octree � �����
    std::cout << rb << std::endl;
    if (rb) {
        //instance generated
       rb->transferEnergy(100.0f, cam.cameraFront);
       rb->applyAcceleration(Environment::gravityAcc);

    }

}

void processInput(float fDeltaTime)
{
    scene.processInput(fDeltaTime);

    //update flash light
    if (States::isIndexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
    }


    if (keyboard::KeyWentUp((GLFW_KEY_ESCAPE)))
    {
        scene.setShouldCloses(true);
       // screen.setShouldClose(true);
       // glfwSetWindowShouldClose(window, true);
    }

    if (keyboard::KeyWentDown((GLFW_KEY_F)))
    {
        flashlight = !flashlight;

    };

    if (keyboard::KeyWentDown((GLFW_KEY_Q)))
    {
        launchItem(fDeltaTime);

    };

    for (int i = 0; i < 4; i++)
    {
        if (keyboard::KeyWentDown((GLFW_KEY_1 + i)))
        {
            States::toggleIndex(&scene.activePointLights, i);
            
        }
    }
}
