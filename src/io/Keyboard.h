#ifndef KEYBOARD_H    //pragma once ���� ����� 
#define KEYBOARD_H

#include <glad/glad.h> // ��� �������� ������� OpenGL
#include <GLFW/glfw3.h> // ���������� ��� ������ � ������ � �����/������


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
