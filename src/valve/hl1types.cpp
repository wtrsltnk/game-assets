#include "valve/hl1types.h"
#include "valve/hl1shader.h"

#include <glm/gtc/type_ptr.hpp>

using namespace valve;

VertexArray::VertexArray()
    : _vao(0), _vbo(0)
{ }

VertexArray::~VertexArray()
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

void VertexArray::LoadVertices(const std::vector<tVertex>& vertices)
{
    if (vertices.size() > 0)
    {
        if (this->_vao == 0)
            glGenVertexArrays(1, &this->_vao);
        glBindVertexArray(this->_vao);

        if (this->_vbo == 0)
            glGenBuffers(1, &this->_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(tVertex), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(tVertex), (GLvoid*)&vertices[0]);

        glVertexAttribPointer((GLuint)hl1::ShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
        glEnableVertexAttribArray((GLuint)hl1::ShaderAttributeLocations::Vertex);

        glVertexAttribPointer((GLuint)hl1::ShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid*)(sizeof(float) * 3));
        glEnableVertexAttribArray((GLuint)hl1::ShaderAttributeLocations::TexCoord);

        glVertexAttribPointer((GLuint)hl1::ShaderAttributeLocations::LightCoord, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid*)(sizeof(float) * 5));
        glEnableVertexAttribArray((GLuint)hl1::ShaderAttributeLocations::LightCoord);

        glVertexAttribPointer((GLuint)hl1::ShaderAttributeLocations::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid*)(sizeof(float) * 7));
        glEnableVertexAttribArray((GLuint)hl1::ShaderAttributeLocations::Normal);

        glVertexAttribPointer((GLuint)hl1::ShaderAttributeLocations::Bone, 1, GL_FLOAT, GL_FALSE, sizeof(tVertex), (GLvoid*)(sizeof(float) * 10));
        glEnableVertexAttribArray((GLuint)hl1::ShaderAttributeLocations::Bone);

        glBindVertexArray(0);
    }
}

void VertexArray::RenderFaces(const std::set<unsigned short>& visibleFaces, GLenum mode)
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

void VertexArray::Bind()
{
    glBindVertexArray(this->_vao);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

