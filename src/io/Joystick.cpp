#include "Joystick.h"
Joystick::Joystick(int i)
{
	id = getId(i);  // Получаем идентификатор джойстика на основе индекса
	update();       // Обновляем состояние джойстика
}
void Joystick::update() {
	present = glfwJoystickPresent(id);  // Проверяем, подключен ли джойстик

	if (present)
	{
		name = glfwGetJoystickName(id);             // Получаем имя джойстика
		axes = glfwGetJoystickAxes(id, &axesCount); // Получаем оси и их количество    массив значений осей (float от -1.0 до 1.0).
		buttons = glfwGetJoystickButtons(id, &buttonCount); // Получаем кнопки и их количество
	}
}

float Joystick::axesState(int axis)
{
	if (present)
	{
		return axes[axis]; // Возвращает значение указанной оси
	}
	return 0.0f;           // Если джойстик не подключен, возвращает 0.0
}

unsigned char Joystick::buttonState(int button)
{
	return present ? buttons[button] : GLFW_RELEASE; //Если джойстик подключен, возвращается текущее состояние (GLFW_PRESS или GLFW_RELEASE).
}

int Joystick::getAxesCount()
{
	return axesCount; // Возвращает количество осей
}

int Joystick::getButtonCount()
{
	return buttonCount; // Возвращает количество кнопок
}

bool Joystick::isPresent()
{
	return present; // Возвращает, подключен ли джойстик
}

const char* Joystick::getName()
{
	return name;
}

int Joystick::getId(int i)
{
	return GLFW_JOYSTICK_1 + i; // Возвращает ID джойстика на основе индекса
}
