#ifndef KEYBOARD_H    //pragma once Тоже самое 
#define KEYBOARD_H

#include <glad/glad.h> // Для загрузки функций OpenGL
#include <GLFW/glfw3.h> // Библиотека для работы с окнами и ввода/вывода


class keyboard {
public:
	//key state callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes);


	//accessors

	static bool Key(int key);
	static bool KeyChanged(int key);
	static bool KeyWentUp(int key);
	static bool KeyWentDown(int key);


private:

	static bool keys[];
	static bool keysChanges[];


};

#endif // !KEYBOARD_H
