#include "Keyboard.h"


bool::keyboard::keys[GLFW_KEY_LAST] = { 0 };  //GLFW_KEY_LAST ��� ���������, ������� ����� ������������ ���������� ������, �������������� GLFW.
bool keyboard::keysChanges[GLFW_KEY_LAST] = { 0 };


void keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
	if (action != GLFW_RELEASE)   //���� ������ �� �����	
	{
		if (!keys[key]) {    //���� ��� ����� false
			keys[key] = true; //������ true
		}

	}
	else
	{
		keys[key] = false;
	}

	keysChanges[key] = action != GLFW_REPEAT; //���� ������� �� ��������� (GLFW_PRESS ��� GLFW_RELEASE), �� ��������� ����� true.
}

// ���������� ������� ��������� ������� (������ ��� ���).
bool keyboard::Key(int key)
{
    // ��������� ��������� ������� � ������� `keys`.
    // ���������� true, ���� ������� ������, ����� false.
    return keys[key];
}

// ���������, ���������� �� ��������� �������.
bool keyboard::KeyChanged(int key)
{
    // ��������� ������� ������ ��������� ������� (true, ���� ���� ���������).
    bool ret = keysChanges[key];

    // ���������� ���� ��������� ������� (false, ����� �� ��������� ��� �������).
    keysChanges[key] = false;

    // ����������, ���� �� ��������� ��������� �������.
    return ret;
}

// ���������, ���� �� ������� ��������.
bool keyboard::KeyWentUp(int key)
{
    // ������� ���� ��������, ����:
    // 1. ������� ��������� ������� � �� ������ (!keys[key]).
    // 2. ��������� ������� ���������� (KeyChanged(key)).
    return !keys[key] && KeyChanged(key);
}

// ���������, ���� �� ������� ������.
bool keyboard::KeyWentDown(int key)
{
    // ������� ���� ������, ����:
    // 1. ������� ��������� ������� � ������ (keys[key]).
    // 2. ��������� ������� ���������� (KeyChanged(key)).
    
    return keys[key] && KeyChanged(key);
}