/*
 * uiFont.cpp
 *
 *  Created on: Mar 12, 2011
 *      Author: wouter
 */

#include "font.h"
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef struct sVertex
{
    float position[3];
    float texCoords[2];

} tVertex;

Font::Font()
    : _fontSize(12.0f), _textureId(0), _vertexArrayObject(0), _vertexBufferObject(0)
{ }

Font::~Font()
{ }

bool Font::InitializeFont(const char* fontpath, float fontSize)
{
    FontShader::LoadShader();

    this->_fontSize = fontSize;

    // Load font.
    FILE* fp = NULL;

    if (fopen_s(&fp, fontpath, "rb") != 0)
        return false;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* ttfBuffer = (unsigned char*)malloc(size);
    if (!ttfBuffer)
    {
        fclose(fp);
        return false;
    }

    fread(ttfBuffer, 1, size, fp);
    fclose(fp);
    fp = 0;

    unsigned char* bmap = (unsigned char*)malloc(512*512);
    if (!bmap)
    {
        free(ttfBuffer);
        return false;
    }

    stbtt_BakeFontBitmap(ttfBuffer,0, fontSize, bmap, 512, 512, 32, 96, this->_charData);
    free(ttfBuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glGenTextures(1, &this->_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(bmap);

    tVertex* vertices = new tVertex[96 * 6];

    int i = 0;
    for (unsigned char c = 32; c < 128; c++)
    {
        float x = 0, y = 0;
        stbtt_aligned_quad q;
        this->GetBakedQuad(512, 512, c-32, &x, &y, &q);

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;
    }

    if (this->_vertexArrayObject == 0)
        glGenVertexArrays(1, &this->_vertexArrayObject);
    glBindVertexArray(this->_vertexArrayObject);

    if (this->_vertexBufferObject == 0)
        glGenBuffers(1, &this->_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, i * sizeof(tVertex), (const GLvoid *)vertices, GL_STATIC_DRAW);

    GLint vertexAttrib = glGetAttribLocation(FontShader::_program, "vertex");
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
    glEnableVertexAttribArray(vertexAttrib);

    GLint texcoordAttrib = glGetAttribLocation(FontShader::_program, "texcoords");
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (const GLvoid *)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray(texcoordAttrib);

    glBindVertexArray(0);

    delete []vertices;

    return true;
}

static const float g_tabStops[4] = {150, 210, 270, 330};

float Font::GetTextLength(const char* text, int count)
{
    float xpos = 0;
    float len = 0;
    if (count <= 0) count = strlen(text);
    while (*text && count--)
    {
        int c = (unsigned char)*text;
        if (c == '\t')
        {
            for (int i = 0; i < 4; ++i)
            {
                if (xpos < g_tabStops[i])
                {
                    xpos = g_tabStops[i];
                    break;
                }
            }
        }
        else if (c >= 32 && c < 128)
        {
            stbtt_bakedchar *b = this->_charData + c-32;
            int round_x = STBTT_ifloor((xpos + b->xoff) + 0.5);
            len = round_x + b->x1 - b->x0 + 0.5f;
            xpos += b->xadvance;
        }
        ++text;
    }
    return len;
}

float Font::GetTextHeight(const char* text)
{
    float height0 = 0;
    while (*text)
    {
        int c = (unsigned char)*text;
        if (c >= 32 && c < 128)
        {
            stbtt_bakedchar *b = this->_charData + c-32;
            if ((b->y1-b->y0) > height0) height0 = float(b->y1-b->y0);
        }
        ++text;
    }
    return height0;
}

void Font::GetTextBoungingBox(const char* text, float bb[4])
{
    if (!text) return;

    float x = 0, y = 0;
    const float ox = x;

    while (*text)
    {
        int c = (unsigned char)*text;
        if (c == '\t')
        {
            for (int i = 0; i < 4; ++i)
            {
                if (x < g_tabStops[i]+ox)
                {
                    x = g_tabStops[i]+ox;
                    break;
                }
            }
        }
        else if (c >= 32 && c < 128)
        {
            stbtt_aligned_quad q;
            this->GetBakedQuad(512,512, c-32, &x,&y,&q);

            // min x
            if (bb[0] > q.x0) bb[0] = q.x0;
            if (bb[0] > q.x1) bb[0] = q.x1;

            // min y
            if (bb[1] > q.y0) bb[1] = q.y0;
            if (bb[1] > q.y1) bb[1] = q.y1;

            // max x
            if (bb[2] < q.x0) bb[2] = q.x0;
            if (bb[2] < q.x1) bb[2] = q.x1;

            // max y
            if (bb[3] < q.y0) bb[3] = q.y0;
            if (bb[3] < q.y1) bb[3] = q.y1;
        }
        ++text;
    }
}

void Font::DrawText(const glm::mat4& proj, const glm::mat4& view, float x, float y, const std::string& input, const glm::vec4& global_color)
{
    glm::mat4 local = glm::mat4(1.0f);
    if (this->_textureId == 0) return;
    const char* text = input.c_str();

    const float ox = x;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, this->_textureId);
    glUseProgram(FontShader::_program);
    glUniformMatrix4fv(FontShader::_u_projection, 1, false, glm::value_ptr(proj));
    glUniform4fv(FontShader::_u_global_color, 1, glm::value_ptr(global_color));

    glBindVertexArray(this->_vertexArrayObject);
    while (*text)
    {
        int c = (unsigned char)*text;
        if (c == '\t')
        {
            for (int i = 0; i < 4; ++i)
            {
                if (x < g_tabStops[i]+ox)
                {
                    x = g_tabStops[i]+ox;
                    break;
                }
            }
        }
        else if (c == '\n')
        {
            x = ox;
            y += this->_fontSize;
        }
        else if (c >= 32 && c < 128)
        {
            local = view * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
            glUniformMatrix4fv(FontShader::_u_view, 1, false, glm::value_ptr(local));
            glDrawArrays(GL_TRIANGLES, (c - 32) * 6, 6);

            stbtt_bakedchar *b = this->_charData + (c - 32);
            x += b->xadvance;
        }
        ++text;
    }
    glBindVertexArray(0);
}

void Font::GetBakedQuad(int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q)
{
    stbtt_bakedchar *b = this->_charData + char_index;
    int round_x = STBTT_ifloor(*xpos + b->xoff);
    int round_y = STBTT_ifloor(*ypos - b->yoff);

    q->x0 = (float)round_x;
    q->y0 = (float)round_y;
    q->x1 = (float)round_x + b->x1 - b->x0;
    q->y1 = (float)round_y - b->y1 + b->y0;

    q->s0 = b->x0 / (float)pw;
    q->t0 = b->y0 / (float)pw;
    q->s1 = b->x1 / (float)ph;
    q->t1 = b->y1 / (float)ph;

    *xpos += b->xadvance;
}

const std::string vertexShader(
        "#version 150\n"

        "in vec3 vertex;\n"
        "in vec2 texcoords;\n"

        "uniform mat4 u_projection;\n"
        "uniform mat4 u_view;\n"

        "out vec2 f_uv;\n"

        "void main()\n"
        "{"
        "    mat4 m = u_projection * u_view;\n"
        "    gl_Position = m * vec4(vertex.xyz, 1.0);\n"
        "    f_uv = texcoords.st;\n"
        "}"
    );

const std::string fragmentShader(
        "#version 150\n"

        "uniform sampler2D tex;\n"
        "uniform vec4 u_global_color;\n"

        "in vec2 f_uv;\n"

        "out vec4 color;"

        "void main()\n"
        "{"
        "   vec4 t = texture(tex, f_uv.st);"
        "   color = vec4(u_global_color.xyz, t.w * u_global_color.w);\n"
        "}"
    );

// TODO : The implementation can be found in hl1shader.cpp
GLuint LoadShaderProgram(const std::string& vertShaderStr, const std::string& fragShaderStr, const std::map<GLuint, std::string>& attrLoc);

GLuint LoadFontShaderProgram(const std::string& vertShaderStr, const std::string& fragShaderStr)
{
    if (glCreateShader == 0) std::cout << "glCreateShader not loaded" << std::endl;
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        std::cout << &vertShaderError[0] << std::endl;
    }

    // Compile fragment shader
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        std::cout << &fragShaderError[0] << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError( (logLength > 1) ? logLength : 1 );
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        std::cout << &programError[0] << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

void FontShader::LoadShader()
{
    if (FontShader::_program == 0)
    {
        FontShader::_program = LoadFontShaderProgram(vertexShader, fragmentShader);

        glUseProgram(FontShader::_program);

        FontShader::_u_projection = glGetUniformLocation(FontShader::_program, "u_projection");
        FontShader::_u_view = glGetUniformLocation(FontShader::_program, "u_view");
        FontShader::_u_global_color = glGetUniformLocation(FontShader::_program, "u_global_color");
    }
}

unsigned int FontShader::_program = 0;
unsigned int FontShader::_u_projection = 0;
unsigned int FontShader::_u_view = 0;
unsigned int FontShader::_u_global_color = 0;
