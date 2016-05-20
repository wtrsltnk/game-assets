#include "hl1types.h"
#include "hl1shader.h"

#include <glm/gtc/type_ptr.hpp>

Hl1VertexArray::Hl1VertexArray()
    : _vao(0), _vbo(0)
{ }

Hl1VertexArray::~Hl1VertexArray()
{
//    this->_faces.Delete();
//    this->_textures.Delete();
//    while (this->_lightmaps.empty() == false)
//    {
//        Texture* t = this->_lightmaps.back();
//        this->_lightmaps.pop_back();
//        delete t;
//    }
}

void Hl1VertexArray::LoadVertices(const std::vector<HL1::tVertex>& vertices)
{
    if (vertices.size() > 0)
    {
        if (this->_vao == 0)
            glGenVertexArrays(1, &this->_vao);
        glBindVertexArray(this->_vao);

        if (this->_vbo == 0)
            glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(HL1::tVertex), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(HL1::tVertex), (GLvoid*)&vertices[0]);

        glVertexAttribPointer((GLuint)Hl1ShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(HL1::tVertex), 0);
        glEnableVertexAttribArray((GLuint)Hl1ShaderAttributeLocations::Vertex);

        glVertexAttribPointer((GLuint)Hl1ShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(HL1::tVertex), (GLvoid*)(sizeof(float) * 3));
        glEnableVertexAttribArray((GLuint)Hl1ShaderAttributeLocations::TexCoord);

        glVertexAttribPointer((GLuint)Hl1ShaderAttributeLocations::LightCoord, 2, GL_FLOAT, GL_FALSE, sizeof(HL1::tVertex), (GLvoid*)(sizeof(float) * 5));
        glEnableVertexAttribArray((GLuint)Hl1ShaderAttributeLocations::LightCoord);

        glVertexAttribPointer((GLuint)Hl1ShaderAttributeLocations::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(HL1::tVertex), (GLvoid*)(sizeof(float) * 7));
        glEnableVertexAttribArray((GLuint)Hl1ShaderAttributeLocations::Normal);

        glVertexAttribPointer((GLuint)Hl1ShaderAttributeLocations::Bone, 1, GL_FLOAT, GL_FALSE, sizeof(HL1::tVertex), (GLvoid*)(sizeof(float) * 10));
        glEnableVertexAttribArray((GLuint)Hl1ShaderAttributeLocations::Bone);

        glBindVertexArray(0);
    }
}

void Hl1VertexArray::RenderFaces(const std::set<unsigned short>& visibleFaces, GLenum mode)
{
    Bind();

    for (std::set<unsigned short>::const_iterator i = visibleFaces.begin(); i != visibleFaces.end(); ++i)
    {
        short a = *i;

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_lightmaps[this->_faces[a].lightmap]->GlIndex());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_textures[this->_faces[a].texture]->GlIndex());

        glDrawArrays(mode, this->_faces[a].firstVertex, this->_faces[a].vertexCount);
    }

    Unbind();
}

void Hl1VertexArray::Bind()
{
    glBindVertexArray(this->_vao);
}

void Hl1VertexArray::Unbind()
{
    glBindVertexArray(0);
}

