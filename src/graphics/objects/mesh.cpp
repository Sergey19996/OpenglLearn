#include "mesh.h"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices)  
{
    std::vector<Vertex> ret(noVertices);    //Vertex хранит 5 float по 4 байта 
    int stride = 8;  //sizeof return in bytes not bit 8float and 8 int 
    
    for (int i = 0; i < noVertices; i++)
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

void averageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions) {
    if (!existingContributions) {
        baseVec = addition;
    }
    else{
        float f = 1 / ((float)existingContributions + 1);
        baseVec *= (float)(existingContributions)*f;
        
        baseVec += addition * f;



    }


}
//calculate tangent vectors for each face
void Vertex::calcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices){
    unsigned char* counts = (unsigned char*)malloc(list.size() * sizeof(unsigned char));
    for (unsigned int i = 0, len = list.size(); i < len; i++) {
        counts[i] = 0;

    }

    //iterate thorugh indices and calculate vectors for each face
    for (unsigned int i = 0, len = indices.size(); i < len; i += 3) {
        // 3 vertices  corresponding to the face
        Vertex& v1 = list[indices[i + 0]];
        Vertex& v2 = list[indices[i + 1]];
        Vertex& v3 = list[indices[i + 2]];

        //calculate edges
        glm::vec3 edge1 = v2.pos - v1.pos;
        glm::vec3 edge2 = v3.pos - v1.pos;

        //calculate dUV
        glm::vec2 deltaUV1 = v2.texCoord - v1.texCoord;
        glm::vec2 deltaUV2 = v3.texCoord - v1.texCoord;

        //use inverese of the UV matrix to determine tangent
        float det = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
        float f = (det == 0.0f) ? 1.0f : (1.0f / det);

        glm::vec3 tangent = {
            f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z),
        };

        //average of new tangent
        averageVectors(list[indices[i + 0]].tangent, tangent, counts[indices[i + 0]]++);
        averageVectors(list[indices[i + 1]].tangent, tangent, counts[indices[i + 1]]++);
        averageVectors(list[indices[i + 2]].tangent, tangent, counts[indices[i + 2]]++);

    }
}



// default constructor
Mesh::Mesh() : collision(NULL)
{}

Mesh::Mesh(BoundingRegion br) : br(br), collision(NULL){}

// initialize as textured object
Mesh::Mesh(BoundingRegion br, std::vector<texture> textures)
    : Mesh(br) {
    setupTextures(textures);
}

// initialize as material object
Mesh::Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec)
    :Mesh(br) {
    setupColors(diff, spec);
}

Mesh::Mesh(BoundingRegion br, Material m) : Mesh(br){
    setupMaterial(m);

}

// load vertex and index data
void Mesh::loadData(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, bool pad) {

    this->vertices = _vertices;
    this->indices = _indices;

    // bind VAO
    VAO.generate();
    VAO.bind();

    // generate/set EBO
    VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
    VAO["EBO"].generate();
    VAO["EBO"].bind();
    VAO["EBO"].setData<GLuint>(this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

    // load data into vertex buffers
    VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
    VAO["VBO"].generate();
    VAO["VBO"].bind();

   unsigned int size = this->vertices.size();
    if (pad && size) {
        size++;
    }

    VAO["VBO"].setData<Vertex>(size, &this->vertices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    VAO["VBO"].bind();
    // vertex Positions
    VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 11, 0); // 11 - колличество байт между началами двух атрибутов 0,1,2 -vertex; 3,4,5 - normal
    // normal ray
    VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 11, 3);
    // vertex texture coords
    VAO["VBO"].setAttPointer<GLfloat>(2, 3, GL_FLOAT, 11, 6);
    // tangent vector
    VAO["VBO"].setAttPointer<GLfloat>(3, 3, GL_FLOAT, 11, 8);


    VAO["VBO"].clear();

    ArrayObject::clear();
}

void Mesh::loadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices){
    this->collision = new CollisionMesh(noPoints, coordinates, noFaces, indices);
    this->br = this->collision->br;
}

void Mesh::setupTextures(std::vector<texture> textures){
    this->noTex = false;
    //                       внедряем в конец     с начала            по конец 
    this->textures.insert(this->textures.end(), textures.begin(), textures.end());
}

void Mesh::setupColors(aiColor4D diff, aiColor4D spec){
    this->noTex = true;
    this->diffuse = diff;
    this->specular = spec;
}

void Mesh::setupMaterial(Material mat){
    this->noTex = true;
    this->diffuse = { mat.diffuse.r, mat.diffuse.g, mat.diffuse.b, 1.0f };
    this->specular = { mat.specular.r, mat.specular.g, mat.specular.b, 1.0f };
}

void Mesh::render(Shader shader,unsigned int noInstances)
{
    shader.setBool("noNormalMap", true);

    if (noTex) {
        //materials
        shader.set4Float("material.diffuse", diffuse);
        shader.set4Float("material.specular", specular);
        shader.setBool("noTex", true); // true в фрагмент шейдер 

    }
    else
    {

        //textures
        unsigned int diffuseIdx = 0;
        unsigned int normalIdx = 0;
        unsigned int specularIdx = 0;
        for (unsigned int i = 0; i < textures.size(); i++)
        {  // activate texture
            glActiveTexture(GL_TEXTURE0 + i);

            //retrieve texture info
            std::string name;
            switch (textures[i].type) {
            case aiTextureType_DIFFUSE:
                name = "diffuse" + std::to_string(diffuseIdx++);
                break;
            case aiTextureType_SPECULAR:
                name = "specular" + std::to_string(specularIdx++);
                break;

            case aiTextureType_NORMALS :
                name = "normal" + std::to_string(normalIdx++);
                shader.setBool("noNormalMap", false);
                break;

            case aiTextureType_NONE:
                name = textures[i].name;
                break;
            
            }

            //set the shader value
            shader.setInt(name, i);
            //bind texture
            textures[i].bind();
        }

        shader.setBool("noTex", false); // false для ресета  
    }
    VAO.bind();
    VAO.draw(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, noInstances);
    ArrayObject::clear();

    glActiveTexture(GL_TEXTURE0);
    
}


void Mesh::setup()
{
 

  //  glBindVertexArray(0);

    //bind VAO
    VAO.generate();
    VAO.bind();

    //generate/set EBO
    VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
    VAO["EBO"].generate();
    VAO["EBO"].bind();
    VAO["EBO"].setData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);  // нужео компилятору сказать GLuint а не unsigned int 


    //generate /set VBO
    VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
    VAO["VBO"].generate();
    VAO["VBO"].bind();
    VAO["VBO"].setData<Vertex>(vertices.size(), &vertices[0], GL_STATIC_DRAW);

    //set vertex attrib pointer
    //vertex positions      3vert + 3norm+ 2tex = 8 stride
    VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 8,0);
    //normal 
    VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
    //textures coord                 |ind||siz||type|stride|offset|    
    VAO["VBO"].setAttPointer<GLfloat>(2, 2, GL_FLOAT, 8, 6);

    VAO["VBO"].clear();

    ArrayObject::clear();
}
void Mesh::cleanup()
{
    VAO.cleanUp();

    for (texture t : textures) {
        t.cleanup();
    }
}
