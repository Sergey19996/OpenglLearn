#include "mesh.h"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices)
{
    std::vector<Vertex> ret(noVertices);    //Vertex ������ 5 float �� 4 ����� 
    int stride = sizeof(Vertex) / sizeof(float);  //sizeof return in bytes not bit 8float and 8 int 
    
    for (int i = 0; i< noVertices; i++)
    {
        ret[i].pos = glm::vec3(
            vertices[i * stride + 0],
            vertices[i * stride + 1],
            vertices[i * stride + 2]
        );

        ret[i].normal = glm::vec3(
            vertices[i * stride + 3],
            vertices[i * stride + 4],
            vertices[i * stride + 5]
        );

        ret[i].texCoord = glm::vec2(
            vertices[i * stride + 6],
            vertices[i * stride + 7]


        );
    }

    return ret;
}

Mesh::Mesh()
{
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures)
    :vertices(vertices),indices(indices),textures(textures)
{
    setup();
}

void Mesh::render(Shader shader)
{//textures
    for (unsigned int i = 0; i < textures.size(); i++) //���� �������� �� ���� ���������, ����������� � ����.
    {
        shader.setInt(textures[i].name, textures[i].id); //������������� ���������� ����
        glActiveTexture(GL_TEXTURE0 + i); //���������� ���������� ����
        textures[i].bind(); // ����������� �������� � ��������� ����������� �����.

        if (textures[i].checkAlpha())   // ���� �� ����� ���������� �� � ������ ������ ������ ��� � ����� ������� �� ��������� 
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  //������ ��� � �������������� �������� �� EBO :
    glBindVertexArray(0); //���������� VAO, ����� �������� �� �������� ���.

    glActiveTexture(GL_TEXTURE0); //����� ��������� ����������� ����� �� GL_TEXTURE0 �� ���������� ���������.
}

void Mesh::cleanup()
{
    glDeleteVertexArrays(1, &VAO); // 1. �������� VAO (Vertex Array Object)
    glDeleteBuffers(1, &VBO);      // 2. �������� VBO (Vertex Buffer Object)
    glDeleteBuffers(1, &EBO);      // 3. �������� EBO (Element Buffer Object)
}

void Mesh::setup()
{
    glGenVertexArrays(1, &VAO); // 1. �������� VAO (Vertex Array Object) ������ ������ �� VBO � EBO
    glGenBuffers(1, &VBO);      // 2. �������� VBO (Vertex Buffer Object)  ������ ���� �������� (�� ���������� � ���� � ���������� ��������)
    glGenBuffers(1, &EBO);      // 3. �������� EBO (Element Buffer Object) ������ �������, ��� ���������� ������������ ������������

    glBindVertexArray(VAO);     // 4. �������� VAO (������ ��� ��������)

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // 5. �������� VBO ��� �������� ������ �������
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    // 6. ���������� VBO ������� � �������� (�������� ������ �� ������� vertices � �����������)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);   // 7. �������� EBO ��� �������� ������ ���������
    //����� ��������, ������� �� EBO ����� �������������� ��� ��������� (glDrawElements).

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // 8. ���������� EBO ������� � �������� (�������� ������ �� ������� indices � �����������)


    //set vertex attribute pointers
    //vertex.position
    glEnableVertexAttribArray(0); //�������� ������� ������� � �������� 0.

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
   /* 0 � ������ ��������(layout(location = 0) � ��������� �������).
        3 � ���������� ����������� �� �������(x, y, z � ���������� ����������).
        GL_FLOAT � ��� ������(float).
        GL_FALSE � ������ �� �������������(���������� ��� ����).
        sizeof(Vertex) � ���(stride) ����� ���������(������ ���� ��������� Vertex).*/
    //(void*)0 � �������� ������ ������ (������� ������� ���������� � ������ ������ ��������� Vertex).
    
    //vertex.normal;
    glEnableVertexAttribArray(1); //�������� ������� ������� � �������� 1.

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //vertex.texcoord
    glEnableVertexAttribArray(2);  //�������� ������� ������� � �������� 2.

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
  /*  1 � ������ ��������(layout(location = 1) � �������).
        2 � ���������� �����������(u, v � 2D ���������� ����������).
        GL_FLOAT � ��� ������(float).
        GL_FALSE � ��� ������������.
        sizeof(Vertex) � ��� ����� ���������(������ ��������� Vertex).
        offsetof(Vertex, texCoord) � �������� �� ���������� ��������� ������ ��������� Vertex.*/


    glBindVertexArray(0);

}
