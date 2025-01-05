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
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"

// Функция обратного вызова для обработки изменения размеров окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// ввод клавиш
void processInput(GLFWwindow* window, float fdeltatime);

float merge = 0.5f;

glm::mat4 transform = glm::mat4(1.0f);

Camera cameras[2] = {

    Camera(glm::vec3(0.0f,0.0f,3.0f)),
    Camera(glm::vec3(10.0f,10.0f,10.0f))
};

int activeCam = 0;

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


 unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;
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

    // Создание окна
    GLFWwindow* Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FatBoys", NULL, NULL);
    if (Window == NULL) // Проверка, удалось ли создать окно
    {
        std::cout << "Not Open Window" << std::endl; // Вывод сообщения об ошибке
        glfwTerminate(); // Завершаем GLFW, если окно не было создано
        return -1;
    }

    // Установка контекста OpenGL для созданного окна
    glfwMakeContextCurrent(Window);

    // Загрузка всех функций OpenGL с помощью GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad" << std::endl; // Сообщение об ошибке, если загрузка не удалась
        glfwTerminate();
        return -1;
    }

    // Установка области отрисовки (всего окна по умолчанию)
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); // Параметры: x, y, ширина, высота

    // Регистрация функции обратного вызова для изменения размеров окна
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

    // Установка обработчика событий клавиатуры (callback-функции)
    glfwSetKeyCallback(Window, keyboard::keyCallback);

    glfwSetCursorPosCallback(Window, Mouse::cursorPosCallback);
    glfwSetMouseButtonCallback( Window, Mouse::mouseButtonCallback);
    glfwSetScrollCallback(Window, Mouse::mouseWheelCallback);


    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // скрыть курсор 


    glEnable(GL_DEPTH_TEST);   // включаем тест глубины 

    /*
 shaders
 */



    Shader shader("Assets/object.vs.glsl", "Assets/object.fs.glsl");
  //  Shader shader2("Assets/vertex_core.glsl", "Assets/fragment_core.glsl");

    //Vertex array object
    float vertices[] = {

        //position           //texCoords
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //VAO, VBO
    unsigned VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

   


    //bind VAO
    glBindVertexArray(VAO);

    //bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   

    //set atribute pointer 

    //positions  Указываем, что атрибут 0 (позиции) это 3 компонента типа float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // устанавливаем атрибутный слой 0
    glEnableVertexAttribArray(0); // Включаем атрибут 0 (позиции) 

   //texture coordinate  //index 2 , send 2 elements(x,y)  8 * sizeof(float) - это пробел между пред и след значениями в матрице
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Textures

    unsigned int texture1,texture2;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);// к текстурному слоту GL_TEXTURE0 с помощью glBindTexture.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //S=X 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //T=Y 
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT); //R=Z

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // настройки для растягивания картинки (линейно просчитываем пиксели
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // настройки для сужения картинки (используем соседние, пикселит при сжатии)

    // Load Image
    int width, Height, nChannels;
    unsigned char* data = stbi_load("Assets/Skull.png", &width, &Height, &nChannels, 0);  //читаем данные из текстуры 

    if (data) {

        if (nChannels ==3)
        {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //Создает двумерную текстуру с указанными размерами и данными пикселей, передаваемыми в data

        }
        else  // Alpha channel in texture 
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Стандартная функция смешивания для прозрачных объектов
        }
        //Мип-маппинг — это метод использования нескольких версий текстуры с разным уровнем детализации
        glGenerateMipmap(GL_TEXTURE_2D);
        // в зависимости от расстояния до камеры.
    }
    else
    {
        std::cout << "Failed to load Texture" << std::endl; 
    }

    stbi_image_free(data);  //освобождаем данные 

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);// к текстурному слоту GL_TEXTURE0 с помощью glBindTexture.


    data = stbi_load("Assets/Rubbit.jpg", &width, &Height, &nChannels, 0);  //читаем данные из текстуры 

    if (data) {


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //Создает двумерную текстуру с указанными размерами и данными пикселей, передаваемыми в data
        glGenerateMipmap(GL_TEXTURE_2D);  // Генерация мип-маппинга
        //Мип-маппинг — это метод использования нескольких версий текстуры с разным уровнем детализации
        // в зависимости от расстояния до камеры.
    }
    else
    {
        std::cout << "Failed to load Texture" << std::endl;
    }


    shader.activate();
    shader.setInt("texture1", 0); //GL_TEXTURE0   текстура должна быть использована из текстурного слота 0.
    shader.setInt("texture2", 1);
    shader.setValue("blend", merge);


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
    while (!glfwWindowShouldClose(Window)) // Выполняем цикл, пока окно не закрыто
    {

        // Получаем время между кадрами
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;


        //input Tracking
        processInput(Window, deltaTime);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0); // выбраем текстурный блок, который будет использоваться для дальнейших операций с текстурами. 31 max
        glBindTexture(GL_TEXTURE_2D, texture1); //вязывает конкретную текстуру с выбранным текстурным блоком.
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        //draw shapes 
        glBindVertexArray(VAO);
      


        merge += deltaTime;
        float merge2 = 0.5f * sin(merge) + 0.5f;




        shader.setValue("blend", merge2);     // позволяет нам смешивать цвета 

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //create transformation on the screen
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, currentTime * glm::radians(-55.0f), glm::vec3(0.5f));
        view = cameras[activeCam].getViewMatrix();
        projection = glm::perspective(glm::radians(cameras[activeCam].getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    
         shader.activate();
         shader.setMat4("model", model);
         shader.setMat4("view", view);
         shader.setMat4("projection", projection);
   
      
        glBindVertexArray(0);

       
        glfwSwapBuffers(Window);  // Отправка нового кадра на экран (обмен буферов)
        glfwPollEvents();    // Обработка событий (ввод с клавиатуры, мыши и т.д.)
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
        
    // Завершение работы GLFW, освобождение ресурсов
    glfwTerminate();

    return 0;
}

// Определение функции обратного вызова для изменения размеров окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Обновление области отрисовки под новый размер окна
    glViewport(0, 0, width, height); // Параметры: x, y, новая ширина, новая высота

    //когда мы скейлим экран отрисовки 
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

}
void processInput(GLFWwindow* window, float fDeltaTime)
{

    if (keyboard::KeyWentUp((GLFW_KEY_ESCAPE)))
    {
        glfwSetWindowShouldClose(window, true);
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
