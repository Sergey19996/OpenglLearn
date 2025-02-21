#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>

#include <string>
#include <map>

#include "vertexmemory.hpp"
#include "rendering/shader.h"

typedef struct {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph (1/64th pixrls)

} Character;

class TextRenderer {
public:
    TextRenderer();
    TextRenderer(int height);

    bool loadFont(FT_Library& ft, std::string path);
    void render(Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

    void cleanUp();

private:
    int height;
   
    std::map<char, Character> chars;

    ArrayObject VAO;

};





#endif // !TEXT_H

