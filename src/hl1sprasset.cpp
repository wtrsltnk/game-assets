#include "hl1sprasset.h"
#include "hl1sprinstance.h"

Hl1SprAsset::Hl1SprAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Hl1Asset(locator, loader), _vao(0), _vbo(0)
{ }

Hl1SprAsset::~Hl1SprAsset()
{ }

bool Hl1SprAsset::Load(const std::string &filename)
{
    Array<byte>& data = this->_loader(filename);
    this->_header = (HL1::tSPRHeader*)data.data;

    int w = this->_header->width/2.0f;
    int h = this->_header->height/2.0f;

    tVertex verts[4];
    verts[0].position = glm::vec3(-w, 0.0f, -h); verts[0].texcoords = glm::vec2(0.0f, 1.0f);
    verts[1].position = glm::vec3( w, 0.0f, -h); verts[1].texcoords = glm::vec2(1.0f, 1.0f);
    verts[2].position = glm::vec3( w, 0.0f,  h); verts[2].texcoords = glm::vec2(1.0f, 0.0f);
    verts[3].position = glm::vec3(-w, 0.0f,  h); verts[3].texcoords = glm::vec2(0.0f, 0.0f);

    glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Hl1SprAsset::tVertex), (const GLvoid*)verts, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer((GLuint)Hl1SprShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Hl1SprAsset::tVertex), 0);
    glEnableVertexAttribArray((GLuint)Hl1SprShaderAttributeLocations::Vertex);
    // Texcoords
    glVertexAttribPointer((GLuint)Hl1SprShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Hl1SprAsset::tVertex), (const GLvoid*)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray((GLuint)Hl1SprShaderAttributeLocations::TexCoord);
    // Texcoords lightmap
    glBindVertexArray(0);                           // Unbind vertex array

    short paletteColorCount = *(short*)(data.data + sizeof(HL1::tSPRHeader));
    byte* palette = (byte*)(data.data + sizeof(HL1::tSPRHeader) + sizeof(short));
    byte* tmp = (byte*)(palette + (paletteColorCount * 3));

    this->_frames.Allocate(this->_header->numframes);
    glGenTextures(this->_frames.count, (GLuint*)this->_frames.data);

    for (int f = 0; f < this->_frames.count; f++)
    {
        HL1::eSpriteFrameType frames = *(HL1::eSpriteFrameType*)tmp;
        tmp += sizeof(HL1::eSpriteFrameType);
        if (frames == HL1::SPR_SINGLE)
        {
            HL1::tSPRFrame* frame = (HL1::tSPRFrame*)tmp;
            tmp += sizeof(HL1::tSPRFrame);
            unsigned char* textureData = new unsigned char[frame->width * frame->height * 3];
            for (int y = 0; y < frame->height; y++)
            {
                for (int x = 0; x < frame->width; x++)
                {
                    int item = x * frame->height + y;
                    unsigned char index = tmp[item];
                    textureData[item * 3] = palette[index * 3];
                    textureData[item * 3 + 1] = palette[index * 3 + 1];
                    textureData[item * 3 + 2] = palette[index * 3 + 2];
                }
            }
            glBindTexture(GL_TEXTURE_2D, this->_frames[f]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

            delete []textureData;

            tmp += frame->width * frame->height;
        }
    }

    return true;
}

Hl1Instance* Hl1SprAsset::CreateInstance()
{
    return new Hl1SprInstance(this);
}

void Hl1SprAsset::RenderSpriteFrame(int frame)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glBindVertexArray(this->_vao);

    glBindTexture(GL_TEXTURE_2D, this->_frames[frame]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
}

int Hl1SprAsset::FrameCount() const
{
    return this->_frames.count;
}
