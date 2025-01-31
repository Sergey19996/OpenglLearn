#include "texture.h"
#include <iostream>



texture::texture()
{
}

texture::texture(std::string name) :name(name), type(aiTextureType_NONE)
{
	generate();
}

texture::texture(std::string dir, std::string path, aiTextureType type):dir(dir),path(path),type(type)
{
	generate();  // генерирует текстуру с id
}



void texture::generate()
{
	glGenTextures(1, &id);



}

void texture::load(bool flip)
{
	stbi_set_flip_vertically_on_load(flip);


	int width, height, nChannels;

	unsigned char* data = stbi_load((dir + "/" + path).c_str(), & width, &height, &nChannels, 0);

	GLenum colorMode = GL_RGB;

	switch (nChannels) {
	case 1:
		colorMode = GL_RED; // Одноканальное изображение (чёрно-белое)
		break;
	case 4:
		colorMode = GL_RGBA; // Четырёхканальное изображение (RGB + альфа-канал)
	
		break;
	}
	if (data) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data); //Загружаем данные изображения в GPU.
		glGenerateMipmap(GL_TEXTURE_2D); 
		//Мипмапы — это уменьшенные копии текстуры для улучшения производительности при рендеринге объектов на расстоянии.

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  //установки на повторение текстур
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //MIN_FILTER определяет, как текстура выглядит на расстоянии или при уменьшении её размера.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //MAG_FILTER важен, когда текстура увеличивается, например, при приближении камеры к объекту.

	}
	else
	{
		std::cout << "image not loaded at " << path << std::endl;
	}

	stbi_image_free(data);
}

void texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}
