#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	int success;
	char infoLog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);    // ������ ������� ������
	GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER); // ������ ��������(�������) ������

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
}
