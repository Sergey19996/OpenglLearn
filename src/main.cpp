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
#include "graphics/models/plane.hpp"
#include "graphics/framememory.hpp"


#include "physics/environment.h"

#include "algorithms/States.hpp"
#include "scene.h"

#include <ft2build.h>

Scene scene;
// ввод клавиш
void processInput(float fdeltatime);


void renderScene(Shader shader);

Camera cam;

glm::mat4 transform = glm::mat4(1.0f);




 unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;


 float lastTime = glfwGetTime();
 float deltaTime = 0.0f;

float theta = 45.0f;
bool flashlight = true;

Sphere sphere(10);
Cube cube(10);


int main()
{
    //          version 3.3 opengl
    scene = Scene(3, 3, "Fat Boys", SCR_WIDTH, SCR_HEIGHT);
    if (!scene.init()) {

        std::cout << "Couldn't open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;
    //shaders ============================
    Shader lampShader("Assets/shaders/instanced/instanced.vs.glsl", "Assets/shaders/lamp.fs.glsl");
    Shader shader("Assets/shaders/instanced/instanced.vs.glsl", "Assets/shaders/object.fs.glsl");
    Shader boxShader("Assets/shaders/instanced/box.vs.glsl", "Assets/shaders/instanced/box.fs.glsl");
    Shader textShader("Assets/shaders/text.vs.glsl", "Assets/shaders/text.fs.glsl");
    Shader dirLightShader("Assets/shaders/shadows/directionalshadow.vs.glsl", "Assets/shaders/shadows/shadow.fs.glsl");

    Shader skyBoxShader("Assets/skybox/skybox.vs.glsl", "Assets/skybox/skybox.fs.glsl");
    Shader outlineShader("Assets/shaders/outline.vs.glsl", "Assets/shaders/outline.fs.glsl");
    Shader bufferShader("Assets/shaders/buffer.vs.glsl", "Assets/shaders/buffer.fs.glsl");

  //  skyBoxShader.activate();
  //  skyBoxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
  //  skyBoxShader.set3Float("max", 0.945f, 1.000f, 0.682f);
    //skybox
  //  Cubemap skybox;
  //  skybox.init();
   // skybox.loadTextures("Assets/skybox");



    //Models ==================================
  //  Lamp lamp(4);
   // scene.registerModel(&lamp);
    scene.registerModel(&sphere);

   
    scene.registerModel(&cube);

    Box box;
    box.init();

    //FBO
    //                 direction                   amb                 diff          spec
    DirLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f),
        BoundingRegion(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 20.0f)));

   

    //setup plane to display texture
    Plane map;
    map.init(dirLight.shadowFBO.textures[0]);
    scene.registerModel(&map);



    //load all model data
    scene.loadModels();




  //  glDeleteFramebuffers(1, &fbo);



  
    scene.dirLight = &dirLight;
  
  //  glm::vec3 pointLightPositions[] = {
  //          glm::vec3(0.7f,  0.2f,  2.0f),
  //          glm::vec3(2.3f, -3.3f, -4.0f),
  //          glm::vec3(-4.0f,  2.0f, -12.0f),
  //          glm::vec3(0.0f,  0.0f, -3.0f)
  //  };

  //  glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
  //  glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
  //  glm::vec4 specular = glm::vec4(1.0f);
  //  float k0 = 1.0f;
  //  float k1 = 0.09f;
  //  float k2 = 0.032f;

  //  PointLight pointLights[4];  // создаётся 4 света 

  ////  SpotLight light;
  // 
  //  for (unsigned int i = 0; i < 4; i++) {
  //      pointLights[i] = {pointLightPositions[i],    //устанавливаются значения 
  //          k0,k1,k2,
  //          ambient,diffuse,specular
  //      };
  //      scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);

  //      scene.pointLights.push_back(&pointLights[i]);    // устонавливаются ссылки для рендера света
  //      States::activate(&scene.activePointLights, i);  // в inte  на 32 света (32 bit) устанавливает 1 

  //  }



  //  SpotLight spotLight = {
  //      cam.cameraPos,cam.cameraFront,
  //      glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(20.0f)), //малый большой круги cut 
  //      1.0f,0.07f,0.032f,   // коэфиенты для затухания фанаря 
  //      glm::vec4(0.0f,0.0f,0.0f,1.0f),glm::vec4(1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f)

  //  };
  //  scene.spotLights.push_back(&spotLight);
  //  scene.activeSpotLights = 1; // 0b001


    scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));
    glm::vec3 cubePositions[] = {
    { 1.0f, 3.0f, -5.0f },
    { -7.25f, 2.1f, 1.5f },
    { -15.0f, 2.55f, 9.0f },
    { 4.0f, -3.5f, 5.0f },
    { 2.8f, 1.9f, -6.2f },
    { 3.5f, 6.3f, -1.0f },
    { -3.4f, 10.9f, -5.5f },
    { 10.0f, -2.0f, 13.2f },
    { 2.1f, 7.9f, -8.3f },
    };
    for (unsigned int i = 0; i < 9; i++) {
        scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
    }
    //instatiate texture quad
    scene.generateInstance(map.id, glm::vec3(2.0f, 2.0f, 0.0f), 0.0f, glm::vec3(0.0f));


    //instanciate instances
    scene.initInstances();

    scene.prepare(box); // для octree 





    scene.VariableLog["time"] = (double)0.0;

    scene.defaultFBO.bind(); //rebind default framebuffer
  
    // Главный цикл рендеринга
    while (!scene.shouldClose()) // Выполняем цикл, пока окно не закрыто
    {




        // Получаем время между кадрами
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        scene.VariableLog["time"] += deltaTime;
        scene.VariableLog["fps"] = 1 / deltaTime;


        scene.update();
        //input Tracking

        processInput(deltaTime);

      


        //remove launch objects if too far
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 100.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);


            }
        }
       


        //render scene for direction light FBO
        dirLight.shadowFBO.activate();

        scene.renderDirLightShader(dirLightShader);
        renderScene(dirLightShader);
       
        //render scene normally
        scene.defaultFBO.activate();
        scene.renderShader(shader);
        renderScene(shader);


        //render boxes
       // scene.renderShader(boxShader, false);
       // box.render(boxShader);




        //render skybox
      //  skybox.render(skyBoxShader, &scene);


        //glDepthFunc(GL_ALWAYS);  // change depth function so depth test passes when values are equal to depth buffer's content
        //scene.renderText("comic", textShader, "Hello, Opengl!", 50.0f, 50.f, glm::vec2(1.0f), glm::vec3(0.5f,0.6f,1.0f));
        //scene.renderText("comic", textShader, "Fat Boyes!", SCR_WIDTH-200.0f, 0+70.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));

        //scene.renderText("comic", textShader, "fps: " + scene.VariableLog["fps"].dump(), SCR_WIDTH - 200.0f, SCR_HEIGHT - 70.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        //scene.renderText("comic", textShader, "fps: " + scene.VariableLog["time"].dump(), SCR_WIDTH - 200.0f, SCR_HEIGHT - 100.0f, glm::vec2(1.0f), glm::vec3(0.5f, 0.6f, 1.0f));
        //glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content



        
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
void launchItem(float fdeltatime) {
    glm::vec3 fixpos = { cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z - 0.25f };

   RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.1f), 1.0f, fixpos);  // передаем индекс сферы, где она хранится в octree в сцене
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
        if (flashlight) {
            scene.activeSpotLights = 1;
        }
        else
        {
            scene.activeSpotLights = 0;
        }

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
    if (keyboard::KeyWentDown((GLFW_KEY_T))) {
        scene.dirLightActive = !scene.dirLightActive;
    }
}

void renderScene(Shader shader) {
    if (sphere.currentNoInstances > 0) {
        scene.renderInstances(sphere.id, shader, deltaTime);
    }

    //render    cubes normally
    scene.renderInstances(cube.id, shader, deltaTime);
}




