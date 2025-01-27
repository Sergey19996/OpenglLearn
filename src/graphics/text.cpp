#include "text.h"

TextRenderer::TextRenderer(){
}

TextRenderer::TextRenderer(int height) : height(height)
{
}

bool TextRenderer::loadFont(FT_Library & ft, std::string path)
{

		FT_Face fontFace; //variable that we use when we loading font FreeType
		if (FT_New_Face(ft, path.c_str(), 0, &fontFace)) {

			return false;
		}
		//set height and width
		FT_Set_Pixel_Sizes(fontFace, 0, height);  // 0 значит мы даем сам её определить отностиельно ширины  dynamicly resize


		//load texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //disable the byte-alignment restriction   1 байт на пиксель фонтовой текстуры 

		for (unsigned char c = 0; c < 128; c++) {
			//load glyph
			if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
				continue;
			}

			//generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RED,
				fontFace->glyph->bitmap.width,
				fontFace->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				fontFace->glyph->bitmap.buffer); // <-GL_RED для использования только одного канала в 1 байт
			//set texture parametres
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Для скейла
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // для скейла

			//store character texture for use
			chars[c] = {
			texture,
			glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
			glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
			(unsigned int)fontFace->glyph->advance.x

			};
		}

			FT_Done_Face(fontFace);

			//set VAO
			VAO.generate();
			VAO.bind();
			//set buffer
			VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["VBO"].generate();
			VAO["VBO"].bind();


			//2d quad req 6 vertices  of 4 float (2 f0r pos, 2 for texcoord)
			VAO["VBO"].setData<GLfloat>(6 * 4, nullptr, GL_DYNAMIC_DRAW);
			VAO["VBO"].setAttPointer<GLfloat>(0, 4, GL_FLOAT, 4, 0);

			VAO["VBO"].clear();

			ArrayObject::clear();



		return true;
}

	void TextRenderer::render(Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color){
		shader.activate();
		shader.set3Float("texColor", color);

		glActiveTexture(GL_TEXTURE0);
		VAO.bind();

		//go through all the characters of string
		for (int i = 0, len = text.size(); i < len; i++){
			Character c = chars[text[i]];

			float xPos = x + c.Bearing.x * scale.x;  // bearing смещениее от левого верхнего угла 
			float yPos = y - (c.Size.y - c.Bearing.y) * scale.y;   // для вычесление небольшого хвоста (разницы между текущим размером буквы и общим размером)


			float width = c.Size.x * scale.x;
			float height = c.Size.y * scale.y;

			//new VBO Data
			float vertices[6 * 4] = {
				//x			y				tex X			tex Y
				xPos,		yPos + height,	0.0f,           0.0f,
				xPos,		yPos,			0.0f,			1.0f,
				xPos+width,	yPos,			1.0f,			1.0f,

				xPos,       yPos+height,    0.0f,           0.0f,
				xPos+width, yPos,			1.0f,			1.0f,
				xPos+width, yPos+height,	1.0f,			0.0f
			};
			
			//setup texture
			glBindTexture(GL_TEXTURE_2D, c.TextureID);

			//update VBO data
			VAO["VBO"].bind();   // связали
			VAO["VBO"].updateData<GLfloat>(0, 6 * 4, vertices);
			//render quad
			VAO.draw(GL_TRIANGLES, 0, 6);
			
			//advance cursor
			x += (c.Advance >> 6) * scale.x; // multiply by 64
		}
		ArrayObject::clear();

		glBindTexture(GL_TEXTURE_2D, 0); // hfpdzpfkb 

}

void TextRenderer::cleanUp(){
	VAO.cleanUp();

}
