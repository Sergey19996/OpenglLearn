#include "shader.h"

Shader::Shader()
{
  
}

Shader::Shader(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geoShaderPath)
{

    generate(includeDefaultHeader,vertexShaderPath, fragmentShaderPath,geoShaderPath);

}

void Shader::generate(bool includeDefaultHeader,const char* vertexShaderPath, const char* fragShaderPath, const char* geoShaderPath)
{
    int success;
    char infoLog[512];

    GLuint vertexShader = compileShader(includeDefaultHeader,vertexShaderPath, GL_VERTEX_SHADER);    // создаём вертекс шейдер
    GLuint fragmentShader = compileShader(includeDefaultHeader,fragShaderPath, GL_FRAGMENT_SHADER); // создаём фрагмент(пиксель) шейдер

    GLuint geoShader = 0; //placeholder
    if (geoShaderPath) { //не nullptr
        geoShader = compileShader(includeDefaultHeader,geoShaderPath, GL_GEOMETRY_SHADER);
    }



    id = glCreateProgram();    // Создаем пустую программу  
    glAttachShader(id, vertexShader); // Прикрепляем вершинный шейдер
    glAttachShader(id, fragmentShader); // Прикрепляем фрагментный шейдер

    if (geoShaderPath)
    glAttachShader(id, geoShader);
    
    glLinkProgram(id);  // Линкуем шейдерную программу

    // Проверяем, успешно ли скомпилировался фрагментный шейдер
    glGetShaderiv(id, GL_COMPILE_STATUS, &success); // Запрашиваем статус компиляции
    if (!success) { // Если произошла ошибка
        glGetShaderInfoLog(id, 512, NULL, infoLog); // Получаем лог ошибки (максимум 512 символов)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // Выводим сообщение об ошибке
    }

    glDeleteShader(vertexShader); // Удаляем скомпилированные шейдеры, так как они уже прикреплены к программе и больше не нужны
    glDeleteShader(fragmentShader);
    if (geoShaderPath)
    glDeleteShader(geoShader);
     
}

void Shader::bind()
{
	glUseProgram(id);  //устанавливает шейдер текщим opengl  
    //мы объедигяем все шейдеры в одну программу, и та ищем уже Uniform переменные 
}

void Shader::activate()
{
    glUseProgram(id);

}




GLuint Shader::compileShader(bool includeDefaultHeader,const char* filepath, GLenum type)
{
    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);  // cоздаём и Указываем тип шейдера: фрагментный
    std::string shaderSrc = loadShadersSrc(includeDefaultHeader,filepath);  //читаем шейшер
    const GLchar* shader = shaderSrc.c_str();  //  Преобразуем строку в формат C-style
    glShaderSource(ret, 1, &shader, NULL);  // Передаем исходный код  шейдера в OpenGL
    glCompileShader(ret); // Компилируем  шейдер


    // Проверяем, успешно ли скомпилировался  шейдер
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success); // Запрашиваем статус компиляции
    if (!success) { // Если произошла ошибка
        glGetShaderInfoLog(ret, 512, NULL, infoLog); // Получаем лог ошибки (максимум 512 символов)
        std::cout << "Error with frag shader comp." << filepath <<  ":" << std::endl << infoLog << std::endl; // Выводим сообщение об ошибке
    }


	return ret;
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, glm::vec3 v)
{
    set3Float(name,v.x, v.y, v.z);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3)
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4)
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3,v4);
}

void Shader::set4Float(const std::string& name, aiColor4D color)
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);

}

void Shader::set4Float(const std::string& name, glm::vec4 v)
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);

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

void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

std::string Shader::loadShadersSrc(bool includeDefaultHeader,const char* filepath)
{
    // Создаём поток для работы с файлом
    std::ifstream file;

    // Создаём строковый буфер для временного хранения содержимого файла
    std::stringstream buf;

    // Переменная для возвращаемой строки (изначально пустая)
    std::string ret = "";

    //include default 
    if (includeDefaultHeader) {
        buf << Shader::defaultHeaders.str();

    }

    std::string fullPath = Shader::defaultDirectory + '/' + filepath;

    // Открываем файл по указанному пути
    file.open(fullPath.c_str());

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
std::stringstream Shader::defaultHeaders;
void Shader::loadIntoDefault(const char* filepath){
    std::string fileContents = Shader::loadShadersSrc(false, filepath);

    Shader::defaultHeaders << fileContents; // записываем в статик переменную 

}

void Shader::clearDefaults(){
    Shader::defaultHeaders.clear(); // функция stringSteam

}
