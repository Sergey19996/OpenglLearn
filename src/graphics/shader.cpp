#include "shader.h"

Shader::Shader()
{
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{

    generate(vertexShaderPath, fragmentShaderPath);

}

void Shader::generate(const char* vertexShaderPath, const char* fragShaderPath)
{
    int success;
    char infoLog[512];

    GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);    // создаём вертекс шейдер
    GLuint fragmentShader = compileShader(fragShaderPath, GL_FRAGMENT_SHADER); // создаём фрагмент(пиксель) шейдер

    id = glCreateProgram();    // Создаем пустую программу  
    glAttachShader(id, vertexShader); // Прикрепляем вершинный шейдер
    glAttachShader(id, fragmentShader); // Прикрепляем фрагментный шейдер
    glLinkProgram(id);  // Линкуем шейдерную программу

    // Проверяем, успешно ли скомпилировался фрагментный шейдер
    glGetShaderiv(id, GL_COMPILE_STATUS, &success); // Запрашиваем статус компиляции
    if (!success) { // Если произошла ошибка
        glGetShaderInfoLog(id, 512, NULL, infoLog); // Получаем лог ошибки (максимум 512 символов)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // Выводим сообщение об ошибке
    }

    glDeleteShader(vertexShader); // Удаляем скомпилированные шейдеры, так как они уже прикреплены к программе и больше не нужны
    glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(id);  //устанавливает шейдер текщим opengl
}

std::string Shader::loadShadersSrc(const char* filepath)
{
    // Создаём поток для работы с файлом
    std::ifstream file;

    // Создаём строковый буфер для временного хранения содержимого файла
    std::stringstream buf;

    // Переменная для возвращаемой строки (изначально пустая)
    std::string ret = "";

    // Открываем файл по указанному пути
    file.open(filepath);

    // Проверяем, удалось ли открыть файл
    if (file.is_open()) {
        // Считываем содержимое файла в строковый буфер
        buf << file.rdbuf();

        // Преобразуем буфер в строку и сохраняем в переменную ret
        ret = buf.str();
    }
    else
    {
        // Если файл не открылся, выводим сообщение об ошибке в консоль
        std::cout << "could not open " << filepath << std::endl;
    }

    // Закрываем файл (все ресурсы освобождаются)
    file.close();

    // Возвращаем содержимое файла в виде строки
    return ret;


	//return std::string();
}

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);  // cоздаём и Указываем тип шейдера: фрагментный
    std::string shaderSrc = loadShadersSrc(filepath);  //читаем шейшер
    const GLchar* shader = shaderSrc.c_str();  //  Преобразуем строку в формат C-style
    glShaderSource(ret, 1, &shader, NULL);  // Передаем исходный код  шейдера в OpenGL
    glCompileShader(ret); // Компилируем  шейдер


    // Проверяем, успешно ли скомпилировался  шейдер
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success); // Запрашиваем статус компиляции
    if (!success) { // Если произошла ошибка
        glGetShaderInfoLog(ret, 512, NULL, infoLog); // Получаем лог ошибки (максимум 512 символов)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // Выводим сообщение об ошибке
    }


	return ret;
}

void Shader::setMat4(const std::string& name, glm::mat4 val)
{
                                                            
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
    //Количество матриц, которые вы передаёте. В данном случае одна матрица (4x4).
    //Если GL_TRUE, то строки и столбцы матрицы поменяются местами.
    //Функция glm::value_ptr возвращает указатель на данные матрицы в виде массива float.
    //для передачи 4x4 матрицы из вашего C++ кода в uniform-переменную шейдера
    //id: Идентификатор шейдерной программы.
    //name.c_str(): Имя uniform-переменной в шейдере (например, "model", "view", "projection").
    //glGetUniformLocation Функция возвращает location (индекс) переменной в шейдере.
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);

    //glUniform1i Устанавливает целочисленное значение (int) для uniform-переменной с заданным location.
    //glGetUniformLocation Функция получает location (местоположение) uniform-переменной в шейдерной программе.
    //id — идентификатор шейдерной программы.
    //name.c_str() — имя uniform-переменной в шейдере (например, "texture1" или "lightPosition").
}

void Shader::setValue(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
