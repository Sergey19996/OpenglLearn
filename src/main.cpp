// Подключение необходимых библиотек
#include <iostream> // Для вывода сообщений в консоль
#include <glad/glad.h> // Для загрузки функций OpenGL
#include <GLFW/glfw3.h> // Библиотека для работы с окнами и ввода/вывода

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include<stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"


// Функция обратного вызова для обработки изменения размеров окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// ввод клавиш
void processInput(GLFWwindow* window);


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
    GLFWwindow* Window = glfwCreateWindow(800, 600, "FatBoys", NULL, NULL);
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
    glViewport(0, 0, 800, 600); // Параметры: x, y, ширина, высота

    // Регистрация функции обратного вызова для изменения размеров окна
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

    /*
 shaders
 */

    Shader shader("Assets/vertex_core.glsl", "Assets/fragment_core.glsl");
    Shader shader2("Assets/vertex_core.glsl", "Assets/fragment_core2.glsl");

    //Vertex array object
    float vertices[] = {
        //First triangle
        //Positions         colors
        -0.2f,-0.5f, 0.0f, 0.3f,1.0f,0.5f,// leftDown
         0.0f,0.5f,0.0f,   0.5f,1.0f,0.75f,  // Left Up
         0.0f,0.0f,0.0f,   0.6f,1.0f,1.0f, //Right Down

        0.2f,-0.5f,0.0f,    1.0f,0.1f,0.5f   // Right up


    };
    unsigned int indices[] = {
        0,1,2,  //first triangle 
        3,1,2  //second triangle 


    };

    //VAO, VBO
    unsigned VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);  // инизиализируем буфер для индексов точек (оптимизация)


    //bind VAO
    glBindVertexArray(VAO);

    //bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    //set atribute pointer 

    //positions  Указываем, что атрибут 0 (позиции) это 3 компонента типа float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);  // устанавливаем атрибутный слой 0
    glEnableVertexAttribArray(0); // Включаем атрибут 0 (позиции) 

    //color   // Указываем, что атрибут 1 (цвет) это 3 компонента типа float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // атрибутный сло 1  location ==1
    glEnableVertexAttribArray(1);// Включаем атрибут 1 (позиции)

    //set up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glm::mat4 trans = glm::mat4(1.0f); //uint matrix 
    float degree = 0.0f;  //45 degree
    trans = glm::rotate(trans, degree, glm::vec3(0.0f, 0.0f, 1.0f));
    shader.activate();
    shader.setMat4("transform", trans); //transform это нода с таким же именем в вертекс шейдере

    glm::mat4 trans2 = glm::mat4(1.0f);
   // trans2 = glm::scale(trans2, glm::vec3(1.5f));
    trans2 = glm::rotate(trans2, degree , glm::vec3(0.0f, 0.0f, 1.0f));

    shader2.activate();
    shader2.setMat4("transform", trans2);


    float lastTime = glfwGetTime();
    float deltaTime = 0.0f;

    // Главный цикл рендеринга
    while (!glfwWindowShouldClose(Window)) // Выполняем цикл, пока окно не закрыто
    {

        // Получаем время между кадрами
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;


        //input Tracking
        processInput(Window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
     //   trans = glm::mat4(1.0f); // Сбрасываем матрицу на единичную (без преобразований)
     //   trans2 = glm::mat4(1.0f);
        degree = deltaTime/2;

      
        trans = glm::rotate(trans, degree, glm::vec3(0.0f, 0.0f, 1.0f));
        shader.activate();
        shader.setMat4("transform", trans);

      

        //draw shapes 
        glBindVertexArray(VAO);
        shader.activate();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        trans2 = glm::rotate(trans2, -degree, glm::vec3(0.0f, 0.0f, 1.0f));
       // trans2 = glm::scale(trans, glm::vec3(degree));
        shader2.activate();
        shader2.setMat4("transform", trans2);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint)));
       // glUseProgram(shaderPrograms[1]);
       // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        glBindVertexArray(0);

       
        glfwSwapBuffers(Window);  // Отправка нового кадра на экран (обмен буферов)
        glfwPollEvents();    // Обработка событий (ввод с клавиатуры, мыши и т.д.)
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);

    // Завершение работы GLFW, освобождение ресурсов
    glfwTerminate();

    return 0;
}

// Определение функции обратного вызова для изменения размеров окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Обновление области отрисовки под новый размер окна
    glViewport(0, 0, width, height); // Параметры: x, y, новая ширина, новая высота
}
void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}


//
//glm::vec4 vec(1.0f, 1.0f, 0.0f, 1.0f); // Вектор, который будем трансформировать
//glm::mat4 trans = glm::mat4(1.0f);    // Единичная матрица 4x4    // cвойства это матрицы не изменяет вектор или другую матрицу
//
//float pi = 3.1415f / 2.0f;   // 90 градусов
//
//// Применяем трансляцию
//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 1.0f));     // добавили смещение в "пустую" матрицу 
//trans = glm::rotate(trans, pi, glm::vec3(0.0f, 0.0f, 1.0f));    // добавили вращение по y
//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));       // матрица умножения
//vec = trans * vec; // Применяем трансформацию к вектору    // проганил вектор через матрицу добавим то смщение 
//
//// Выводим результат
//std::cout << round(vec.x * 100) / 100 << ' ' << round(vec.y * 100) / 100 << ' ' << round(vec.z * 100) / 100 << std::endl;
