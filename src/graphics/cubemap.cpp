#include "cubemap.h"

Cubemap::Cubemap() :hasTextures(false)
{
}

void Cubemap::loadTextures(std::string dir, std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back){
	//dir = directory;
	directory = dir;
	hasTextures = true;
	faces = { right,left,top,bottom,front,back };


	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);


	int width, height, nChannels;
	for (unsigned int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load((dir + "/" + faces[i]).c_str(),    // считываем данные заполняем width, height, nchnnels, по пути
			&width, &height, &nChannels, 0);

		GLenum colorMode = GL_RED;  
		switch (nChannels)   // сколько коналов? 
		{
		case 3:           // если 3, то 
			colorMode = GL_RGB;
			break;
		case 4 :       // tckb 4
			colorMode = GL_RGBA;
			break; 
		
		}

		if (data) {   
			//создаем текстуру  GL_TEXTURE_CUBE_MAP_POSITIVE_X + i там енумиратор и мы по порядку создадим все стороны для   GL_TEXTURE_CUBE_MAP            
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
						colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);

		}
		else
		{
			std::cout << "failed to load textures" << faces[i] << std::endl;
		}

		stbi_image_free(data);

	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //настройки скейла  для GL_TEXTURE_CUBE_MAP
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // X = S   // clamp = обрезаны края 
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   // Y = T   // GL_CLAMP_TO_EDGE - Продлевает тексуры по краям,там значения от 0 до 1
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);   // Z = R

}

void Cubemap::init(){
	//set up the vertices for Cube
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//generate / setup VAO
	VAO.generate();
	VAO.bind();

	//set VBo
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER); // в массив под индекс VBO создем ARRAY_BUFFER для ввертексов
	VAO["VBO"].generate();
	VAO["VBO"].bind();
	VAO["VBO"].setData<float>(36 * 3, skyboxVertices, GL_STATIC_DRAW); // 36 идексов умножаем на 3 для записили xyz

	//set attribute pointers
	VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);
	VAO["VBO"].clear();

	ArrayObject::clear();

}

void Cubemap::render(Shader shader, Scene* scene){
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	//glDepthMask(GL_FALSE);  // говорим что мы для скайбокса не используем маски глубины, если мы смотрим на обьект и за ним ничего нет, то мы рисуем скайбокс
	shader.activate();


	// remove translation from view matrix ( что бы не двигать бокс, когда перемещается камера) нужно убрать с матрицы нижнюю строку и правую колонку
	glm::mat4 view = glm::mat4(glm::mat3(scene->getActiveCamera()->getViewMatrix())); // берём только первые 3 строки и 3 колонки
	shader.setMat4("view", view);
	shader.setMat4("projection", scene->projection);

	if (hasTextures) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	}
	VAO.bind();
	VAO.draw(GL_TRIANGLES, 0, 36);
	ArrayObject::clear();


	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS); // set depth function back to default
}

void Cubemap::cleanUp(){
	VAO.cleanUp();
}
