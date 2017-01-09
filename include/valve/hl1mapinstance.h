#ifndef _HL1MAPINSTANCE_H_
#define _HL1MAPINSTANCE_H_

#include "hltypes.h"
#include "hl1mapasset.h"
#include "hl1mapshader.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace valve
{

namespace hl1
{

class MapInstance : public AssetInstance
{
public:
    typedef struct sVertex
    {
        glm::vec3 pos;
        glm::vec2 uv;

    } tVertex;

    typedef struct sFace
    {
        int firstVertex;
        int vertexCount;
        std::string texture;
        glm::vec3 normal;

    } tFace;

public:
    MapInstance(MapAsset* asset);
    virtual ~MapInstance();

    virtual void Update(float dt);
    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

private:
    MapAsset* _asset;
    MapShader* _shader;

    // OpenGL objects
    unsigned int _vbo;
    unsigned int _vao;

    std::vector<tFace> _faces;

    static std::vector<glm::vec3> ClipWinding(const std::vector<glm::vec3>& in, const glm::vec3& planeNormal, float planeDistance);
    static std::vector<glm::vec3> CreateWindingFromPlane(const glm::vec3& planeNormal, float planeDistance);

};

}

}

#endif // _HL1MAPINSTANCE_H_
