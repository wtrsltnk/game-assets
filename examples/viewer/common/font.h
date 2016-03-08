/*
 * uiFont.h
 *
 *  Created on: Mar 12, 2011
 *      Author: wouter
 */

#ifndef UIFONT_H_
#define UIFONT_H_

#include "stb_truetype.h"

#include <GL/glextl.h>
#include <glm/glm.hpp>

class FontShader
{
public:
    static void LoadShader();

    static unsigned int _program;
    static unsigned int _u_projection;
    static unsigned int _u_view;
    static unsigned int _u_global_color;

};

class Font
{
public:
    Font();
    virtual ~Font();

    float Size() const { return this->_fontSize; }

    bool InitializeFont(const char* fontpath, float Size = 15.0f);
    float GetTextLength(const char* text, int count = 0);
    float GetTextHeight(const char* text);
    void GetTextBoungingBox(const char* text, float bb[4]);
    void DrawText(const glm::mat4& proj, const glm::mat4& view, float x, float y, const char *text, const glm::vec4& global_color = glm::vec4(1, 1, 1, 1));
    void DrawTextColorLess(float x, float y, const char *text);
    void GetBakedQuad(int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q);

protected:
    stbtt_bakedchar _charData[96]; // ASCII 32..126 is 95 glyphs
    float _fontSize;
    GLuint _textureId;
    GLuint _vertexArrayObject;
    GLuint _vertexBufferObject;

};

#endif /* UIFONT_H_ */
