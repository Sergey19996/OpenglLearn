#ifndef KEYBOARD_H    //pragma once ���� ����� 
#define KEYBOARD_H

#include <glad/glad.h> // ��� �������� ������� OpenGL
#include <GLFW/glfw3.h> // ���������� ��� ������ � ������ � �����/������
#include <vector>

class keyboard {
public:


	static std::vector<void(*)(GLFWwindow* windiw, int key, int scancode, int action, int mods)>keyCallbacks;

	//key state callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes);


	//accessors

	//get key state
	static bool Key(int key);

	//get if key recently changed 
	static bool KeyChanged(int key);
	static bool KeyWentUp(int key);
	static bool KeyWentDown(int key);


private:

	static bool keys[];
	static bool keysChanges[];


};

#endif // !KEYBOARD_H
