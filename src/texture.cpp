/*
 * Copyright (C) 2009  Wouter Saaltink

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "texture.h"
#include <GL/glextl.h>

Texture::Texture()
    : _name(""), _width(0), _height(0), _bpp(0), _format(-1), _repeat(true), _data(0), _glIndex(0)
{ }

Texture::~Texture()
{
    if (this->_data != 0) delete []this->_data;
}

unsigned int Texture::UploadToGl()
{
    if (this->_glIndex > 0)
        return this->_glIndex;

    switch (this->_bpp)
    {
        case 3: this->_format = GL_RGB; break;
        case 4: this->_format = GL_RGBA; break;
    }

    glGenTextures(1, &this->_glIndex);
    glBindTexture(GL_TEXTURE_2D, this->_glIndex);

    if (this->_repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, this->_format, this->_width, this->_height, 0, this->_format, GL_UNSIGNED_BYTE, this->_data);

    return this->_glIndex;
}

void Texture::DeleteFromGl()
{
    glDeleteTextures(1, &this->_glIndex);
}

Texture* Texture::Copy() const
{
    Texture* result = new Texture();

    result->_name = std::string(this->_name);

    result->_width = this->_width;
    result->_height = this->_height;
    result->_bpp = this->_bpp;
    result->_format = this->_format;
    result->_repeat = this->_repeat;
    result->_glIndex = this->_glIndex;
    int dataSize = result->_width * result->_height * result->_bpp;
    if (dataSize > 0)
    {
        result->_data = new unsigned char[dataSize];
        memcpy(result->_data, this->_data, dataSize);
    }
    else
    {
        delete result;
        result = nullptr;
    }

    return result;
}

void Texture::CopyFrom(const Texture& from)
{
    if (this->_data != nullptr)
    {
        delete []this->_data;
        this->_data = nullptr;
    }

    this->_name = from._name;

    this->_width = from._width;
    this->_height = from._height;
    this->_bpp = from._bpp;
    this->_format = from._format;
    this->_repeat = from._repeat;
    this->_glIndex = from._glIndex;
    int dataSize = this->_width * this->_height * this->_bpp;
    if (dataSize > 0)
    {
        this->_data = new unsigned char[dataSize];
        memcpy(this->_data, from._data, dataSize);
    }
}

void Texture::DefaultTexture()
{
#define IMAGE_ROWS 64
#define IMAGE_COLS 64
    int value;
    GLubyte imageData[IMAGE_ROWS][IMAGE_COLS][3];
    for (int row = 0; row < IMAGE_ROWS; row++) {
       for (int col = 0; col < IMAGE_COLS; col++) {
          // Each cell is 8x8, value is 0 or 255 (black or white)
          value = (((row & 0x8) == 0) ^ ((col & 0x8) == 0)) * 255;
          imageData[row][col][0] = (GLubyte)value;
          imageData[row][col][1] = (GLubyte)value;
          imageData[row][col][2] = (GLubyte)value;
       }
    }
    this->SetData(IMAGE_ROWS, IMAGE_COLS, 3, (GLubyte*)&imageData[0][0][0]);
}

void Texture::SetData(int w, int h, int bpp, unsigned char* data, bool repeat)
{
    int dataSize = w * h * bpp;

    if (dataSize > 0)
    {
        this->_width = w;
        this->_height = h;
        this->_bpp = bpp;
        this->_repeat = repeat;

        if (this->_data != nullptr)
            delete []this->_data;
        this->_data = 0;

        this->_data = new unsigned char[dataSize];
        if (data != 0)
            memcpy(this->_data, data, sizeof(unsigned char) * dataSize);
        else
            memset(this->_data, 0, dataSize);
    }
}

void Texture::SetName(const std::string& name)
{
    this->_name = name;
}

void Texture::SetDimentions(int width, int height, int bpp, unsigned int format)
{
    if (this->_glIndex != 0)
        return;

    if (this->_data != nullptr)
    {
        delete []this->_data;
        this->_data = nullptr;
    }

    this->_width = width;
    this->_height = height;
    this->_bpp = bpp;
    this->_format = format;
    int dataSize = this->DataSize();
    if (dataSize > 0)
        this->_data = new unsigned char[dataSize];
}

void Texture::CorrectGamma(float gamma)
{
    // Only images with rgb colors
    if (this->_bpp < 3)
        return;

    for(int j = 0; j < (this->_width * this->_height); ++j)
    {
        float r, g, b;
        r = this->_data[j * this->_bpp + 0];
        g = this->_data[j * this->_bpp + 1];
        b = this->_data[j * this->_bpp + 2];

        r *= gamma/255.0f;
        g *= gamma/255.0f;
        b *= gamma/255.0f;

        // find the value to scale back up
        float scale = 1.0f;
        float temp;
        if(r > 1.0f && (temp = (1.0f/r)) < scale) scale = temp;
        if(g > 1.0f && (temp = (1.0f/g)) < scale) scale = temp;
        if(b > 1.0f && (temp = (1.0f/b)) < scale) scale = temp;

        // scale up color values
        scale *= 255.0f;
        r *= scale;
        g *= scale;
        b *= scale;

        //fill data back in
        this->_data[j * this->_bpp + 0] = (GLubyte)r;
        this->_data[j * this->_bpp + 1] = (GLubyte)g;
        this->_data[j * this->_bpp + 2] = (GLubyte)b;
    }
}

const std::string& Texture::Name() const
{
    return this->_name;
}

int Texture::Width() const
{
    return this->_width;
}

int Texture::Height() const
{
    return this->_height;
}

int Texture::Bpp() const
{
    return this->_bpp;
}

int Texture::DataSize() const
{
    return this->_width * this->_height * this->_bpp;
}

unsigned char* Texture::Data()
{
    return this->_data;
}

unsigned int Texture::GlIndex() const
{
    return this->_glIndex;
}
