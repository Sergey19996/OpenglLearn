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

    GLuint vertexShader = compileShader(includeDefaultHeader,vertexShaderPath, GL_VERTEX_SHADER);    // ������ ������� ������
    GLuint fragmentShader = compileShader(includeDefaultHeader,fragShaderPath, GL_FRAGMENT_SHADER); // ������ ��������(�������) ������

    GLuint geoShader = 0; //placeholder
    if (geoShaderPath) { //�� nullptr
        geoShader = compileShader(includeDefaultHeader,geoShaderPath, GL_GEOMETRY_SHADER);
    }



    id = glCreateProgram();    // ������� ������ ���������  
    glAttachShader(id, vertexShader); // ����������� ��������� ������
    glAttachShader(id, fragmentShader); // ����������� ����������� ������

    if (geoShaderPath)
    glAttachShader(id, geoShader);
    
    glLinkProgram(id);  // ������� ��������� ���������

    // ���������, ������� �� ��������������� ����������� ������
    glGetShaderiv(id, GL_COMPILE_STATUS, &success); // ����������� ������ ����������
    if (!success) { // ���� ��������� ������
        glGetShaderInfoLog(id, 512, NULL, infoLog); // �������� ��� ������ (�������� 512 ��������)
        std::cout << "Error with frag shader comp.:" << std::endl << infoLog << std::endl; // ������� ��������� �� ������
    }

    glDeleteShader(vertexShader); // ������� ���������������� �������, ��� ��� ��� ��� ����������� � ��������� � ������ �� �����
    glDeleteShader(fragmentShader);
    if (geoShaderPath)
    glDeleteShader(geoShader);
     
}

void Shader::bind()
{
	glUseProgram(id);  //������������� ������ ������ opengl  
    //�� ���������� ��� ������� � ���� ���������, � �� ���� ��� Uniform ���������� 
}

void Shader::activate()
{
    glUseProgram(id);

}




GLuint Shader::compileShader(bool includeDefaultHeader,const char* filepath, GLenum type)
{
    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);  // c����� � ��������� ��� �������: �����������
    std::string shaderSrc = loadShadersSrc(includeDefaultHeader,filepath);  //������ ������
    const GLchar* shader = shaderSrc.c_str();  //  ����������� ������ � ������ C-style
    glShaderSource(ret, 1, &shader, NULL);  // �������� �������� ���  ������� � OpenGL
    glCompileShader(ret); // �����������  ������


    // ���������, ������� �� ���������������  ������
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success); // ����������� ������ ����������
    if (!success) { // ���� ��������� ������
        glGetShaderInfoLog(ret, 512, NULL, infoLog); // �������� ��� ������ (�������� 512 ��������)
        std::cout << "Error with frag shader comp." << filepath <<  ":" << std::endl << infoLog << std::endl; // ������� ��������� �� ������
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

void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

std::string Shader::loadShadersSrc(bool includeDefaultHeader,const char* filepath)
{
    // ������ ����� ��� ������ � ������
    std::ifstream file;

    // ������ ��������� ����� ��� ���������� �������� ����������� �����
    std::stringstream buf;

    // ���������� ��� ������������ ������ (���������� ������)
    std::string ret = "";

    //include default 
    if (includeDefaultHeader) {
        buf << Shader::defaultHeaders.str();

    }

    std::string fullPath = Shader::defaultDirectory + '/' + filepath;

    // ��������� ���� �� ���������� ����
    file.open(fullPath.c_str());

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
std::stringstream Shader::defaultHeaders;
void Shader::loadIntoDefault(const char* filepath){
    std::string fileContents = Shader::loadShadersSrc(false, filepath);

    Shader::defaultHeaders << fileContents; // ���������� � ������ ���������� 

}

void Shader::clearDefaults(){
    Shader::defaultHeaders.clear(); // ������� stringSteam

}
