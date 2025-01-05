#include "Keyboard.h"


bool::keyboard::keys[GLFW_KEY_LAST] = { 0 };  //GLFW_KEY_LAST это константа, которая задаёт максимальное количество клавиш, распознаваемых GLFW.
bool keyboard::keysChanges[GLFW_KEY_LAST] = { 0 };


void keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
	if (action != GLFW_RELEASE)   //если кнопка не релиз	
	{
		if (!keys[key]) {    //если эта кнока false
			keys[key] = true; //ставим true
		}

	}
	else
	{
		keys[key] = false;
	}

	keysChanges[key] = action != GLFW_REPEAT; //Если событие не повторное (GLFW_PRESS или GLFW_RELEASE), то выражение вернёт true.
}

// Возвращает текущее состояние клавиши (нажата или нет).
bool keyboard::Key(int key)
{
    // Проверяет состояние клавиши в массиве `keys`.
    // Возвращает true, если клавиша нажата, иначе false.
    return keys[key];
}

// Проверяет, изменилось ли состояние клавиши.
bool keyboard::KeyChanged(int key)
{
    // Сохраняет текущий статус изменения клавиши (true, если было изменение).
    bool ret = keysChanges[key];

    // Сбрасывает флаг изменения клавиши (false, чтобы не повторять это событие).
    keysChanges[key] = false;

    // Возвращает, было ли изменение состояния клавиши.
    return ret;
}

// Проверяет, была ли клавиша отпущена.
bool keyboard::KeyWentUp(int key)
{
    // Клавиша была отпущена, если:
    // 1. Текущее состояние клавиши — не нажата (!keys[key]).
    // 2. Состояние клавиши изменилось (KeyChanged(key)).
    return !keys[key] && KeyChanged(key);
}

// Проверяет, была ли клавиша нажата.
bool keyboard::KeyWentDown(int key)
{
    // Клавиша была нажата, если:
    // 1. Текущее состояние клавиши — нажата (keys[key]).
    // 2. Состояние клавиши изменилось (KeyChanged(key)).
    
    return keys[key] && KeyChanged(key);
}