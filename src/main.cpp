// Подключение необходимых библиотек
#include <iostream> // Для вывода сообщений в консоль
#include <glad/glad.h> // Для загрузки функций OpenGL
#include <GLFW/glfw3.h> // Библиотека для работы с окнами и ввода/вывода

#include <fstream> //классы std::ifstream (чтение) и std::ofstream (запись).
#include <sstream> //std::stringstream, std::istringstream и std::ostringstream для преобразования текста в данные.
#include <streambuf> //низкоуровневый интерфейс к буферам ввода/вывода могут быть связаны с файлами или строками.
#include <string>

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
#include "io/screen.h"
#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"



// ввод клавиш
void processInput(float fdeltatime);

float merge = 0.5f;

glm::mat4 transform = glm::mat4(1.0f);

Camera cameras[2] = {

    Camera(glm::vec3(0.0f,0.0f,3.0f)),
    Camera(glm::vec3(10.0f,10.0f,10.0f))
};

int activeCam = 0;

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


 unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

 Screen screen;

float theta = 45.0f;

Joystick mainJ(0);

int main()
{
    int success;
    char infoLog[512];


    // Инициализация GLFW
    glfwInit(); // Запускает библиотеку GLFW, возвращает false, если инициализация не удалась

    // Настройка параметров окна через glfwWindowHint
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Устанавливаем версию OpenGL: 3.x (старшая часть)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Устанавливаем версию OpenGL: 3.3 (младшая часть)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Используем core profile (без устаревших функций OpenGL)

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Специфическая настройка для macOS
#endif

    if (!screen.init()) {

        std::cout << "Not Open Window" << std::endl; // Вывод сообщения об ошибке
        glfwTerminate(); // Завершаем GLFW, если окно не было создано
        return -1;

    }

    // Загрузка всех функций OpenGL с помощью GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad" << std::endl; // Сообщение об ошибке, если загрузка не удалась
        glfwTerminate();
        return -1;
    }

    screen.setParameters();

    glEnable(GL_DEPTH_TEST); // для буфера глубины, что бы корректно отрисовывать элементы, без перекрытия
    /*
 shaders
 */



    Shader shader("Assets/object.vs.glsl", "Assets/object.fs.glsl");
    Shader lampShader("Assets/object.vs.glsl", "Assets/lamp.fs.glsl");

  
    Cube Cube (Material::gold,glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f));
    Cube.init();
    Lamp lamp(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f,-0.0f,-3.25f),glm::vec3(2.0f));
    lamp.init();

    float lastTime = glfwGetTime();
    float deltaTime = 0.0f;



    mainJ.update();
    if (mainJ.isPresent()) {
        std::cout << mainJ.getName() << " is present" << std::endl;

    }
    else
    {
        std::cout << "not present" << std::endl;
    }

  
    // Главный цикл рендеринга
    while (!screen.shouldClose()) // Выполняем цикл, пока окно не закрыто
    {

        merge += deltaTime;
      


        // Получаем время между кадрами
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

       
        //input Tracking
        processInput(deltaTime);

        //render
        screen.update();

        




        shader.bind();
        shader.set3Float("light.position", lamp.pos);
        shader.set3Float("viewPos", cameras[activeCam].cameraPos);
        shader.set3Float("light.ambient", lamp.ambient);
        shader.set3Float("light.diffuse", lamp.diffuse);
        shader.set3Float("light.specular", lamp.specular);

        //create transformation on the screen
      
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

       
        view = cameras[activeCam].getViewMatrix();
                                       //fov                                           aspect ration                   near pl/far plane
        projection = glm::perspective(glm::radians(cameras[activeCam].getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
     
         shader.bind();
       
         shader.setMat4("view", view);  //model матрица в modele типа cube
         shader.setMat4("projection", projection);
   
         Cube.render(shader);   //  <--тут матрица model
         lampShader.activate();
         lampShader.setMat4("view", view);
         lampShader.setMat4("projection", projection);
         lamp.render(lampShader);


        // sen new frame to window
        screen.newFrame();
    }


    Cube.CleanUp();
    lamp.CleanUp();
    // Завершение работы GLFW, освобождение ресурсов
    glfwTerminate();

    return 0;
}


void processInput(float fDeltaTime)
{

    if (keyboard::KeyWentUp((GLFW_KEY_ESCAPE)))
    {
        screen.setShouldClose(true);
       // glfwSetWindowShouldClose(window, true);
    }

    if (keyboard::KeyWentDown((GLFW_KEY_F)))
    {
        merge += 0.05f;
        if (merge > 1.0f)
        {
            merge = 1.0f;
        }

    }

    if (keyboard::KeyWentDown((GLFW_KEY_G)))
    {
        merge -= 0.05f;
        if (merge < 0.0f)
        {
            merge = 0.0f;
        }
    }

    //move camera
    if (keyboard::Key(GLFW_KEY_W)) {
        cameras[activeCam].updateCameraPos(CameraDirection::FORWARD, fDeltaTime);


        
    }
    if (keyboard::Key(GLFW_KEY_S)) {
        cameras[activeCam].updateCameraPos(CameraDirection::BACKWARD, fDeltaTime);

     
    }
    if (keyboard::Key(GLFW_KEY_A)) {
        cameras[activeCam].updateCameraPos(CameraDirection::LEFT, fDeltaTime);
      
    }
    if (keyboard::Key(GLFW_KEY_D)) {
        cameras[activeCam].updateCameraPos(CameraDirection::RIGHT, fDeltaTime);
     
    }
    if (keyboard::Key(GLFW_KEY_SPACE)) {
        cameras[activeCam].updateCameraPos(CameraDirection::UP, fDeltaTime);
    }
    if (keyboard::Key(GLFW_KEY_LEFT_SHIFT)) {
        cameras[activeCam].updateCameraPos(CameraDirection::DOWN, fDeltaTime);
    }


    double dx = Mouse::getDX(), dy = Mouse::getDY();
    if (dx != 0 || dy != 0)
    {
        cameras[activeCam].updateCameraDirection(dx, dy);
    }


    double scrollDy = Mouse::getScrollDY();
    if (scrollDy != 0)
    {
        cameras[activeCam].updateCameraZoom(scrollDy);
    }

    if (keyboard::KeyWentDown(GLFW_KEY_TAB)) {
        activeCam += (activeCam == 0) ? 1 : -1;
    }
   
}
