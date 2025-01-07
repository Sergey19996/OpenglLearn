#include "texture.h"
#include <iostream>

int texture::currId = 0;

texture::texture()
{
}

texture::texture(const char* path, const char* name, bool defaultParam) : path(path), name(name), id(currId++)
{
	generate();

	if (defaultParam) {
		setFilters(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
		setWrap(GL_REPEAT);


	}
}

void texture::generate()
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);


}

void texture::load(bool flip)
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);

	GLenum colorMode = GL_RGB;

	switch (nChannels) {
	case 1:
		colorMode = GL_RED; // Одноканальное изображение (чёрно-белое)
		break;
	case 4:
		colorMode = GL_RGBA; // Четырёхканальное изображение (RGB + альфа-канал)
		HasAlpha = true;  //удалить в скором времени 
		break;
	}
	if (data) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data); //Загружаем данные изображения в GPU.
		glGenerateMipmap(GL_TEXTURE_2D); 
		//Мипмапы — это уменьшенные копии текстуры для улучшения производительности при рендеринге объектов на расстоянии.
	}
	else
	{
		std::cout << "image not loaded at " << path << std::endl;
	}

	stbi_image_free(data);
}

void texture::setFilters(GLenum all)
{
	setFilters(all, all);

}

void texture::setFilters(GLenum mag, GLenum min)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag); //MAG_FILTER важен, когда текстура увеличивается, например, при приближении камеры к объекту.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min); //MIN_FILTER определяет, как текстура выглядит на расстоянии или при уменьшении её размера.
}

void texture::setWrap(GLenum all)
{
	setWrap(all, all);
}

void texture::setWrap(GLenum s, GLenum t)
//Варианты значений s и t :
//GL_REPEAT — повторение текстуры.
//
//Изображение бесконечно повторяется по краям.
//Подходит для плиточных текстур.
//GL_MIRRORED_REPEAT — зеркальное повторение.
//
//Текстура повторяется с чередованием зеркального отображения.
//Уменьшает видимость швов.
//GL_CLAMP_TO_EDGE — текстура "тянется" на крайний пиксель.
//
//Часто используется для скайбоксов и 2D - спрайтов, чтобы избежать артефактов по краям.
//GL_CLAMP_TO_BORDER — пиксели вне текстуры окрашиваются в цвет границы(GL_TEXTURE_BORDER_COLOR).
//
//Полезно для текстур с чёткими рамками или для изолирования объектов.
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}

void texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}
