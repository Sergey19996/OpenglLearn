#include "screen.h"
#include "Keyboard.h"
#include "Mouse.h"

unsigned int Screen::SCR_WIDTH = 800;
unsigned int Screen::SCR_HEIGHT = 600;

void Screen::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Установка области отрисовки (всего окна по умолчанию)
	glViewport(0, 0, width, height);

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

Screen::Screen() : window(nullptr)
{
}

bool Screen::init()
{
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FatBoys", NULL, NULL);
	if (!window )
	{//window not created

	return false;

	}
	// Установка контекста OpenGL для созданного окна
	glfwMakeContextCurrent(window);

	return true;
}

void Screen::setParameters()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Регистрация функции обратного вызова для изменения размеров окна
	glfwSetFramebufferSizeCallback(window, Screen::framebufferSizeCallback);


	// Установка обработчика событий клавиатуры (callback-функции)
	glfwSetKeyCallback(window, keyboard::keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // скрыть курсор 
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);



}

void Screen::update()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame()
{
	glfwSwapBuffers(window); 
	glfwPollEvents();  // Метод glfwPollEvents() отвечает за обновление состояния клавиш и обработки событий окна.

}

bool Screen::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window,shouldClose);
}
