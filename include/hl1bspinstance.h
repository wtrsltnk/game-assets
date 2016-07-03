#ifndef _HL1BSPINSTANCE_H_
#define	_HL1BSPINSTANCE_H_

#include "hl1bspasset.h"
#include "hl1shader.h"

#include <set>
#include <string>
#include <glm/glm.hpp>

typedef struct
{
   bool      allsolid;   /* if true, plane is not valid */
   bool      startsolid; /* if true, the initial point was in a solid area */
   float     fraction;   /* time completed, 1.0 = didn't hit anything */
   glm::vec3 hitpos;     /* surface hit position (in solid) */
   glm::vec3 endpos;     /* final position (not in solid) */
   int       contents;   /* contents of endpos */
} botman_trace_t;

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
    void BotmanTraceLine (glm::vec3 start, glm::vec3 end, botman_trace_t *trace);
};

#endif	/* _HL1BSPINSTANCE_H_ */

