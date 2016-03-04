#ifndef HL1BSPINSTANCE_H
#define	HL1BSPINSTANCE_H

#include "hl1bspasset.h"
#include "hl1bspshader.h"

#include <set>
#include <string>
#include <glm/glm.hpp>

class Hl1BspInstance : public Hl1Instance
{
    friend class Hl1BspAsset;
    Hl1BspInstance(Hl1BspAsset* asset);
public:
    virtual ~Hl1BspInstance();

    virtual void Update(float dt) { }
    virtual void Render(const glm::mat4& proj, const glm::mat4& view);
    void Unload();

private:
    Hl1BspAsset* _asset;
    Hl1BspShader* _shader;
    std::set<short> _visibleFaces;

    std::set<short> FindVisibleFaces(const glm::vec3& pos);
    int TracePointInLeaf(const glm::vec3& point, int startNode);
};

#endif	/* HL1BSPINSTANCE_H */
