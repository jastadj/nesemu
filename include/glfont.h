#ifndef GLFONT_H
#define GLFONT_H

#include <map>
#include <string>

#include "gltools.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class GLFont
{
public:
    GLFont();
    ~GLFont();

    bool init(FT_Library& ft, const char* font_file);

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
    bool m_Initialized;
    unsigned int m_Shader;
    unsigned int m_VAO, m_VBO;

    // Font
    bool loadFont(FT_Library& ft, const char* fontPath, unsigned int pt_size = 24);
    
    // Character structure
    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };
    std::map<char, Character> Characters;  
};

#endif