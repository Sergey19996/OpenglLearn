#include "mesh.h"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices)
{
    std::vector<Vertex> ret(noVertices);    //Vertex хранит 5 float по 4 байта 
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
    for (unsigned int i = 0; i < textures.size(); i++) //Цикл проходит по всем текстурам, привязанным к мешу.
    {
        shader.setInt(textures[i].name, textures[i].id); //устанавливает текстурный юнит
        glActiveTexture(GL_TEXTURE0 + i); //активирует текстурный юнит
        textures[i].bind(); // привязывает текстуру к активному текстурному юниту.

        if (textures[i].checkAlpha())   // пока не умеем груировать их в разные масивы делаем так с альфа каналом на текстурах 
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

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  //рисует меш с использованием индексов из EBO :
    glBindVertexArray(0); //отвязывает VAO, чтобы случайно не изменить его.

    glActiveTexture(GL_TEXTURE0); //Сброс активного текстурного юнита на GL_TEXTURE0 по завершении отрисовки.
}

void Mesh::cleanup()
{
    glDeleteVertexArrays(1, &VAO); // 1. удаление VAO (Vertex Array Object)
    glDeleteBuffers(1, &VBO);      // 2. удаление VBO (Vertex Buffer Object)
    glDeleteBuffers(1, &EBO);      // 3. удаление EBO (Element Buffer Object)
}

void Mesh::setup()
{
    glGenVertexArrays(1, &VAO); // 1. Создание VAO (Vertex Array Object) хранит ссылки на VBO и EBO
    glGenBuffers(1, &VBO);      // 2. Создание VBO (Vertex Buffer Object)  хранит сами вертексы (их коордитаны в мире и координаты текстуры)
    glGenBuffers(1, &EBO);      // 3. Создание EBO (Element Buffer Object) Хранит индексы, как эффективно отрисовывать треугольники

    glBindVertexArray(VAO);     // 4. Привязка VAO (делаем его активным)

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // 5. Привязка VBO как текущего буфера массива
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    // 6. Заполнение VBO данными о вершинах (копируем данные из вектора vertices в видеопамять)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);   // 7. Привязка EBO как текущего буфера элементов
    //После привязки, индексы из EBO будут использоваться при отрисовке (glDrawElements).

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // 8. Заполнение EBO данными о вершинах (копируем данные из вектора indices в видеопамять)


    //set vertex attribute pointers
    //vertex.position
    glEnableVertexAttribArray(0); //Включает атрибут вершины с индексом 0.

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
   /* 0 – индекс атрибута(layout(location = 0) в вершинном шейдере).
        3 – количество компонентов на вершину(x, y, z – трехмерные координаты).
        GL_FLOAT – тип данных(float).
        GL_FALSE – данные не нормализуются(передаются как есть).
        sizeof(Vertex) – шаг(stride) между вершинами(размер всей структуры Vertex).*/
    //(void*)0 – смещение начала данных (позиция вершины начинается с самого начала структуры Vertex).
    
    //vertex.normal;
    glEnableVertexAttribArray(1); //Включает атрибут вершины с индексом 1.

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //vertex.texcoord
    glEnableVertexAttribArray(2);  //Включает атрибут вершины с индексом 2.

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
  /*  1 – индекс атрибута(layout(location = 1) в шейдере).
        2 – количество компонентов(u, v – 2D текстурные координаты).
        GL_FLOAT – тип данных(float).
        GL_FALSE – без нормализации.
        sizeof(Vertex) – шаг между вершинами(размер структуры Vertex).
        offsetof(Vertex, texCoord) – смещение до текстурных координат внутри структуры Vertex.*/


    glBindVertexArray(0);

}
