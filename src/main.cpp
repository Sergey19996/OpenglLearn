// Подключение необходимых библиотек
#include <iostream> // Для вывода сообщений в консоль
#include <glad/glad.h> // Для загрузки функций OpenGL
#include <GLFW/glfw3.h> // Библиотека для работы с окнами и ввода/вывода

#include <fstream> //классы std::ifstream (чтение) и std::ofstream (запись).
#include <sstream> //std::stringstream, std::istringstream и std::ostringstream для преобразования текста в данные.
#include <streambuf> //низкоуровневый интерфейс к буферам ввода/вывода могут быть связаны с файлами или строками.

#include "graphics/cubemap.h"

#include <string>
#include <stack>
#include<stb/stb_image.h>  //download png files  PNG, JPEG, BMP, TGA и другие

#include <glm/glm.hpp> // glm::vec2, glm::vec3, glm::mat4) и базовые математические операции.
#include <glm/gtc/type_ptr.hpp>  //glm::value_ptr Удобно для передачи данных в OpenGL через glUniform*.
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale и glm::perspective).

#include "graphics/rendering/shader.h"
#include "graphics/rendering/texture.h"
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
#include "graphics/objects/model.h"
#include "graphics/models/plane.hpp"
#include "graphics/framememory.hpp"
#include "graphics/models/brickwall.hpp"
#include "graphics/uniformmemory.hpp"


#include "physics/environment.h"

#include "algorithms/States.hpp"
#include "algorithms/ray.h"
#include "scene.h"

#include <ft2build.h>

Scene scene;
// ввод клавиш
void processInput(float fdeltatime);


void renderScene(Shader shader);

Camera cam({0.0f,1.0f,0.0f},90, 0);

glm::mat4 transform = glm::mat4(1.0f);




// unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;


 float lastTime = glfwGetTime();
 float deltaTime = 0.0f;

float theta = 45.0f;
bool flashlight = true;

Sphere sphere(10);
Cube cube(11);
Lamp lamp(4);
Brickwall wall;

std::string Shader::defaultDirectory = "Assets/shaders";


#include "physics/collisionmesh.h"


void launchItem() {
    glm::vec3 fixpos = { cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z };

    RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.25f), 1.0f, fixpos);  // передаем индекс сферы, где она хранится в octree в сцене
    std::cout << rb << std::endl;
    if (rb) {
        //instance generated
        rb->transferEnergy(100.0f, cam.cameraFront);
        rb->applyAcceleration(Environment::gravityAcc);

    }

}

void emitRay() {
    Ray r(cam.cameraPos, cam.cameraFront);
    float tmin = std::numeric_limits<float>::max();
    BoundingRegion* intersected = scene.octree->checkCollisionRay(r, tmin);
    if (intersected) {
        std::cout << " Hits " << intersected->instance->instanceId << "at t = " << tmin << std::endl;
        scene.markForDeletion(intersected->instance->instanceId);
    }
    else {
        std::cout << "No hit" << std::endl;

    }
}
void MousePosChanged(GLFWwindow* window, double _x, double _y) {

}
void MouseWheelChanged(GLFWwindow* window, double dx, double dy) {

}
void MouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {
    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_1)) {
        emitRay();
    }
}
void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {

    switch (action)
    {
    case GLFW_RELEASE:
        std::cout << "Release code" << key << std::endl;
        break;
    case GLFW_PRESS:
        std::cout << "Press code" << key << std::endl;
        break;
    case GLFW_REPEAT:
        std::cout << "Repeat code" << key << std::endl;
        break;

    };

    if (keyboard::KeyWentDown(GLFW_KEY_ESCAPE)) {

        scene.setShouldCloses(true);
    }
    if (keyboard::KeyWentDown(GLFW_KEY_F)) {
        launchItem();
           
    }
    scene.sceneEvents();
}

int main()
{


    //          version 3.3 opengl
    scene = Scene(3, 3, "Fat Boys", 1200, 720);
    if (!scene.init()) {

        std::cout << "Couldn't open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;
    //shaders ============================
    Shader::loadIntoDefault("defaultHead.gh");


    Shader shader(true, "instanced/instanced.vs.glsl", "object.fs.glsl");
    Shader boxShader(false, "instanced/box.vs.glsl", "instanced/box.fs.glsl");
    Shader textShader(false, "text.vs.glsl", "text.fs.glsl");
    Shader dirShadowShader(true, "shadows/dirSpotShadow.vs.glsl", "shadows/dirShadow.fs.glsl");
    Shader spotShadowShader(false, "shadows/dirSpotShadow.vs.glsl", "shadows/pointSpotShadow.fs.glsl");
    Shader pointShadowShader(false, "shadows/pointShadow.vs.glsl",
        "shadows/pointSpotShadow.fs.glsl",
        "shadows/pointShadow.gs.glsl");

   // Shader skyBoxShader(false, "skybox/skybox.vs.glsl", "skybox/skybox.fs.glsl");
    //Shader outlineShader(false, "outline.vs.glsl", "outline.fs.glsl");
    //Shader bufferShader(false, "buffer.vs.glsl", "buffer.fs.glsl");
    //Shader lampShader(false, "instanced/instanced.vs.glsl", "lamp.fs.glsl");*/

    Shader::clearDefaults();

   
    
  //  skyBoxShader.activate();
  //  skyBoxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
  //  skyBoxShader.set3Float("max", 0.945f, 1.000f, 0.682f);
    //skybox
  //  Cubemap skybox;
  //  skybox.init();
   // skybox.loadTextures("Assets/skybox");



    //Models ==================================
  
    scene.registerModel(&lamp);
    scene.registerModel(&wall);
    scene.registerModel(&sphere);
    scene.registerModel(&cube);
   
    //scene.registerModel(&cube);

    Box box;
    box.init();

    //FBO


    //load all model data
    scene.loadModels();




  //  glDeleteFramebuffers(1, &fbo);




        //                 direction                   amb                 diff          spec
    DirLight dirLight(glm::vec3(0.2f,-1.0f,0.3f),
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
        glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f),
        BoundingRegion(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 20.0f)));



  
    scene.dirLight = &dirLight;
    //setup plane to display texture
   
    glm::vec3 pointLightPositions[] = {
            glm::vec3(3.3f,  5.3f, 3.0f),
            glm::vec3(4.0f, 2.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, 4.0f),
            glm::vec3(4.0f,  2.0f, 4.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.0014f;
    float k2 = 0.000007f;

    PointLight pointLights[4];  // создаётся 4 света 

  //  SpotLight light;
   
    for (unsigned int i = 0; i < 4; i++) {
        pointLights[i] = PointLight(
            pointLightPositions[i],    //устанавливаются значения 
            k0,k1,k2,
            ambient,diffuse,specular,
            0.5f,50.0f
        );
        //create physical model for each lamp
        scene.generateInstance(lamp.id, glm::vec3(0.15f), 0.25f, pointLightPositions[i]);

      //  scene.pointLights.push_back(&pointLights[i]);    // устонавливаются ссылки для рендера света
        States::activate(&scene.activePointLights, i);  // в inte  на 32 света (32 bit) устанавливает 1 

    }



    SpotLight spotLight (
        scene.getActiveCamera()->cameraPos + glm::vec3(0.0f, -0.25f, 0.0f), scene.getActiveCamera()->cameraFront, scene.getActiveCamera()->cameraUp,  //pos, dir , up
        glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.f)),  // cutoff, outercutoff
        1.0f, 0.0014f, 0.000007f,  // k0 , k1, k2
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f), // ambient, diff, spec
        0.1f, 100.0f); // near , far plane
    scene.spotLights.push_back(&spotLight);
   // scene.activeSpotLights = 1; //0b0000001



    //scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));
    glm::vec3 cubePositions[] = {
    { 1.0f, 3.0f, -5.0f },
    { -7.25f, 2.1f, 1.5f },
    { -15.0f, 2.55f, 9.0f },
    { 4.0f, -3.5f, 5.0f },
    { 2.8f, 1.9f, -6.2f },
    { 3.5f, 6.3f, -1.0f },
    { -3.4f, 10.9f, -5.5f },
    { 10.0f, -2.0f, 13.2f },
    { 2.1f, 7.9f, -8.3f }
    };
    for (unsigned int i = 0; i < 9; i++) {
        scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
    }
    scene.generateInstance(cube.id, glm::vec3(15.25f, 0.01f, 15.25f), 1.0f);
    // instantiate the brickwall plane
    scene.generateInstance(wall.id, glm::vec3(1.0f), 1.0f,
        { 0.0f, 1.0f, 2.0f }, { -1.0f, glm::pi<float>(), 0.0f });

    // instantiate instances
    scene.initInstances();

    // finish preparations (octree, etc)
    scene.prepare(box, { shader });

    keyboard::keyCallbacks.push_back(keyChanged);
    Mouse::mouseButtonCallBacks.push_back(MouseButtonChanged);  // определяем тело функции для вызова в ивенте
    Mouse::mouseWheelCallBacks.push_back(MouseWheelChanged);
    Mouse::cursorPosCallBacks.push_back(MousePosChanged);


    scene.VariableLog["time"] = (double)0.0;

    scene.defaultFBO.bind(); //rebind default framebuffer

  
    // Главный цикл рендеринга
    while (!scene.shouldClose()) // Выполняем цикл, пока окно не закрыто
    {

        std::cout << cam.cameraPos.x << " " << cam.cameraPos.y << " "<<cam.cameraPos.z << " " << std::endl;




        // Получаем время между кадрами
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        scene.VariableLog["time"] += deltaTime;
        scene.VariableLog["fps"] = 1 / deltaTime;
        scene.VariableLog["fdeltaTime"] = deltaTime;

        scene.update();
        //input Tracking

        processInput(deltaTime);

      



        //  remove launch objects if too far
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 100.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);


            }
        }



        ////render scene for direction light FBO
       /* dirLight.shadowFBO.activate();
        scene.renderDirLightShader(dirShadowShader);
        renderScene(dirShadowShader);*/


        //////render scene to point light FBOS
        //for (unsigned int i = 0, len = scene.pointLights.size(); i < len; i++) {
        //    if (States::isIndexActive(&scene.activePointLights, i)) {
        //        scene.pointLights[i]->shadowFBO.activate();
        //        scene.renderPointLightShader(pointShadowShader, i);
        //        renderScene(pointShadowShader);
        //    }
        //}


        //////render scene to spot lightFBP
        //for (unsigned int i = 0, len = scene.spotLights.size(); i < len; i++) {
        //    if (States::isIndexActive(&scene.activeSpotLights, i)) {
        //        scene.spotLights[i]->shadowFBO.activate();
        //        scene.renderSpotLightShader(spotShadowShader, i);
        //        renderScene(spotShadowShader);
        //    }
        //}

      
        //render scene normally
     //   scene.defaultFBO.activate();  // буфер позвоялет рендерить всё последующее в текстуру.
        scene.renderShader(shader);
        renderScene(shader);

     
     //   scene.renderShader(lampShader);
     //   scene.renderInstances(lamp.id, lampShader, deltaTime);

        //render boxes
        scene.renderShader(boxShader, false);
        box.render(boxShader);




        //render skybox
      //  skybox.render(skyBoxShader, &scene);

      
        glDepthFunc(GL_ALWAYS);  // change depth function so depth test passes when values are equal to depth buffer's content
        scene.renderText("comic", textShader, "Hello, Opengl!", 50.0f, 50.f, glm::vec2(1.0f), glm::vec3(0.5f,0.6f,1.0f));
        scene.renderText("comic", textShader, "Fat Boyes!", Scene::srcWidth -200.0f, 0+70.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));

        scene.renderText("comic", textShader, "fps: " + scene.VariableLog["fps"].dump(), Scene::srcWidth - 200.0f, Scene::srcHeight - 150.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        scene.renderText("comic", textShader, "timer: " + scene.VariableLog["time"].dump(), Scene::srcWidth - 200.0f, Scene::srcHeight - 100.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        scene.renderText("comic", textShader, "deltatime: " + scene.VariableLog["fdeltaTime"].dump(), Scene::srcWidth - 240.0f, Scene::srcHeight - 50.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content



        
       //set new frame 
        scene.newFrame(box);

        // sen new frame to window
        scene.clearDeadInstances();

      

      

       
    }
   
  //  skybox.cleanUp();
  //  FBO.cleanup();
    // Завершение работы GLFW, освобождение ресурсов
    scene.cleanUp();

    return 0;
}

void processInput(float fDeltaTime)
{
    scene.processInput(fDeltaTime);

    //update flash light
    if (States::isIndexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
        scene.spotLights[0]->up = scene.getActiveCamera()->cameraUp;
        scene.spotLights[0]->updateMatrices();
    }
   

  

   
}

void renderScene(Shader shader) {
    if (sphere.currentNoInstances > 0) {
        scene.renderInstances(sphere.id, shader, deltaTime);
    }

    //render    cubes normally
    scene.renderInstances(cube.id, shader, deltaTime);

    scene.renderInstances(wall.id, shader, deltaTime);
    scene.renderInstances(lamp.id, shader, deltaTime);
}

