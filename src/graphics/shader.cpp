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

    GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);    // ������ ������� ������
    GLuint fragmentShader = compileShader(fragShaderPath, GL_FRAGMENT_SHADER); // ������ ��������(�������) ������

    id = glCreateProgram();    // ������� ������ ���������  
    glAttachShader(id, vertexShader); // ����������� ��������� ������
    glAttachShader(id, fragmentShader); // ����������� ����������� ������
    glLinkProgram(id);  // ������� ��������� ���������

    // ���������, ������� �� ��������������� ����������� ������
    glGetShaderiv(id, GL_COMPILE_STATUS, &success); // ����������� ������ ����������
    if (!success) { // ���� ��������� ������
        glGetShaderInfoLog(id, 512, NULL, infoLog); // �������� ��� ������ (�������� 512 ��������)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // ������� ��������� �� ������
    }

    glDeleteShader(vertexShader); // ������� ���������������� �������, ��� ��� ��� ��� ����������� � ��������� � ������ �� �����
    glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(id);  //������������� ������ ������ opengl
}

std::string Shader::loadShadersSrc(const char* filepath)
{
    // ������ ����� ��� ������ � ������
    std::ifstream file;

    // ������ ��������� ����� ��� ���������� �������� ����������� �����
    std::stringstream buf;

    // ���������� ��� ������������ ������ (���������� ������)
    std::string ret = "";

    // ��������� ���� �� ���������� ����
    file.open(filepath);

    // ���������, ������� �� ������� ����
    if (file.is_open()) {
        // ��������� ���������� ����� � ��������� �����
        buf << file.rdbuf();

        // ����������� ����� � ������ � ��������� � ���������� ret
        ret = buf.str();
    }
    else
    {
        // ���� ���� �� ��������, ������� ��������� �� ������ � �������
        std::cout << "could not open " << filepath << std::endl;
    }

    // ��������� ���� (��� ������� �������������)
    file.close();

    // ���������� ���������� ����� � ���� ������
    return ret;


	//return std::string();
}

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);  // c����� � ��������� ��� �������: �����������
    std::string shaderSrc = loadShadersSrc(filepath);  //������ ������
    const GLchar* shader = shaderSrc.c_str();  //  ����������� ������ � ������ C-style
    glShaderSource(ret, 1, &shader, NULL);  // �������� �������� ���  ������� � OpenGL
    glCompileShader(ret); // �����������  ������


    // ���������, ������� �� ���������������  ������
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success); // ����������� ������ ����������
    if (!success) { // ���� ��������� ������
        glGetShaderInfoLog(ret, 512, NULL, infoLog); // �������� ��� ������ (�������� 512 ��������)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // ������� ��������� �� ������
    }


	return ret;
}

void Shader::setMat4(const std::string& name, glm::mat4 val)
{
                                                            
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
    //���������� ������, ������� �� ��������. � ������ ������ ���� ������� (4x4).
    //���� GL_TRUE, �� ������ � ������� ������� ���������� �������.
    //������� glm::value_ptr ���������� ��������� �� ������ ������� � ���� ������� float.
    //��� �������� 4x4 ������� �� ������ C++ ���� � uniform-���������� �������
    //id: ������������� ��������� ���������.
    //name.c_str(): ��� uniform-���������� � ������� (��������, "model", "view", "projection").
    //glGetUniformLocation ������� ���������� location (������) ���������� � �������.
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);

    //glUniform1i ������������� ������������� �������� (int) ��� uniform-���������� � �������� location.
    //glGetUniformLocation ������� �������� location (��������������) uniform-���������� � ��������� ���������.
    //id � ������������� ��������� ���������.
    //name.c_str() � ��� uniform-���������� � ������� (��������, "texture1" ��� "lightPosition").
}

void Shader::setValue(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
