#ifndef _HL1BSPINSTANCE_H_
#define	_HL1BSPINSTANCE_H_

#include "hl1bspasset.h"
#include "hl1shader.h"

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
    Hl1Shader* _shader;
    std::set<unsigned short> _visibleFaces;

    std::set<unsigned short> FindVisibleFaces(const glm::vec3& pos, int headNode);
    int TracePointInLeaf(const glm::vec3& point, int startNode);
};

#endif	/* _HL1BSPINSTANCE_H_ */

