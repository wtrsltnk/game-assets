#include "hl1bspinstance.h"

#include <glm/glm.hpp>
#include <set>

using namespace std;

Hl1BspInstance::Hl1BspInstance(Hl1BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new Hl1Shader();
    this->_shader->BuildProgram();
}

Hl1BspInstance::~Hl1BspInstance()
{
    this->Unload();
}

void Hl1BspInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    glm::mat3 rotMat(view);
    glm::vec3 pos = -glm::vec3(view[3]) * rotMat;
    std::set<unsigned short> visibleFaces/*;visibleFaces.insert(320);//*/ = this->FindVisibleFaces(pos, this->_asset->_modelData[0].headnode[0]);
    if (visibleFaces.size() > 0)
        this->_visibleFaces = visibleFaces;

    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void Hl1BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}

std::set<unsigned short> Hl1BspInstance::FindVisibleFaces(const glm::vec3& pos, int headNode)
{
    std::set<unsigned short> leafFaces;

    int leaf = this->TracePointInLeaf(pos, headNode);
    if (leaf != 0)
    {
        // add all faces of current leaf
        for (int f = 0; f < this->_asset->_leafData[leaf].markSurfacesCount; f++)
        {
            unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[leaf].firstMarkSurface + f];
            if (this->_asset->FaceFlags(findex) == 0)
                leafFaces.insert(findex);
        }

        HL1::tBSPVisLeaf* visleaf = &this->_asset->_visLeafs[leaf];
        // add all faces of leafs through vis data
        for (int l = 0; l < visleaf->leafCount; l++)
        {
            for (int f = 0; f < this->_asset->_leafData[visleaf->leafs[l]].markSurfacesCount; f++)
            {
                unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[visleaf->leafs[l]].firstMarkSurface + f];
                if (this->_asset->FaceFlags(findex) == 0)
                    leafFaces.insert(findex);
            }
        }

        // Add all faces of non worldspawn model
        for (int m = 1; m < this->_asset->_modelData.count; m++)
        {
            for (unsigned short f = 0; f < this->_asset->_modelData[m].faceCount; f++)
            {
                if (this->_asset->FaceFlags(f + this->_asset->_modelData[m].firstFace) != 0)
                    continue;

                leafFaces.insert(f + this->_asset->_modelData[m].firstFace);
            }
        }
    }
    else
    {
        for (unsigned short i = 0; i < this->_asset->_faceData.count; i++)
        {
            if (this->_asset->FaceFlags(i) != 0)
                continue;

            this->_visibleFaces.insert(i);
        }
    }

    return leafFaces;
}

int Hl1BspInstance::TracePointInLeaf(const glm::vec3& point, int nodenum)
{
    float d;
    HL1::tBSPNode *node;
    HL1::tBSPPlane *plane;

    while (nodenum >= 0)
    {
        node = &this->_asset->_nodeData[nodenum];
        plane = &this->_asset->_planeData[node->planeIndex];
        d = glm::dot(point, plane->normal) - plane->distance;
        if (d > 0)
            nodenum = node->children[0];
        else
            nodenum = node->children[1];
    }

    return -nodenum - 1;
}

void Hl1BspInstance::BotmanTraceLine (glm::vec3 start, glm::vec3 end, botman_trace_t *tr)
{
    HL1::tBSPLeaf  *startleaf, *endleaf;
    int      numsteps, totalsteps;
    glm::vec3   move, step, position;
    float    dist, trace_dist;

    memset(tr, 0, sizeof(botman_trace_t));

    if ((start[0] < -4095) || (start[0] > 4095) ||
            (start[1] < -4095) || (start[1] > 4095) ||
            (start[2] < -4095) || (start[2] > 4095))
    {
        // start beyond edge of world is INVALID!!!
        fprintf(stderr,"TraceLine: start point beyond edge of world!\n");
    }

    if (end[0] > 4095.0f)
    {
        float percent = 4095.0f / end[0];
        end[1] = end[1] * percent;
        end[2] = end[2] * percent;
        end[0] = 4095.0f;
    }

    if (end[1] > 4095.0f)
    {
        float percent = 4095.0f / end[1];
        end[0] = end[0] * percent;
        end[2] = end[2] * percent;
        end[1] = 4095.0f;
    }

    if (end[2] > 4095.0f)
    {
        float percent = 4095.0f / end[2];
        end[0] = end[0] * percent;
        end[1] = end[1] * percent;
        end[2] = 4095.0f;
    }

    if (end[0] < -4095.0f)
    {
        float percent = 4095.0f / end[0];
        end[1] = end[1] * percent;
        end[2] = end[2] * percent;
        end[0] = -4095.0f;
    }

    if (end[1] < -4095.0f)
    {
        float percent = 4095.0f / end[1];
        end[0] = end[0] * percent;
        end[2] = end[2] * percent;
        end[1] = -4095.0f;
    }

    if (end[2] < -4095.0f)
    {
        float percent = 4095.0f / end[2];
        end[0] = end[0] * percent;
        end[1] = end[1] * percent;
        end[2] = -4095.0f;
    }

    // find the starting and ending leafs...
    startleaf = &this->_asset->_leafData[TracePointInLeaf(start, 0)];
    endleaf = &this->_asset->_leafData[TracePointInLeaf(end, 0)];

    // set endpos, fraction and contents to the default (trace completed)
    tr->endpos = end;
    tr->fraction = 1.0f;
    tr->contents = endleaf->contents;

    if (startleaf->contents == CONTENTS_SOLID)
        tr->startsolid = TRUE;

    // is start and end leaf the same (couldn't possibly hit the world)...
    if (startleaf == endleaf) {
        if (startleaf->contents == CONTENTS_SOLID)
            tr->allsolid = TRUE;
        return;
    }

    // get the length of each interation of the loop...
    move = end - start;
    dist = glm::length(move);

    // determine the number of steps from start to end...
    if (dist > 1.0f)
        numsteps = totalsteps = (int)dist + 1;
    else
        numsteps = totalsteps = 1;

    // calculate the length of the step vector...
    step = move * float(2/numsteps);

    position = start;

    while (numsteps)
    {
        position = position + step;

        endleaf = &this->_asset->_leafData[TracePointInLeaf(position, 0)];

        if ((endleaf->contents == CONTENTS_SOLID) ||  // we hit something solid...
                (endleaf->contents == CONTENTS_SKY))      // we hit the sky
        {
            glm::vec3 hitpos = position;

            // store the hit position
            tr->hitpos = position;

            // back off one step before solid
            position = position - step;

            // store the end position and end position contents
            tr->endpos = position;
            tr->contents = endleaf->contents;

            move = position - start;
            trace_dist = glm::length(move);
            tr->fraction = trace_dist / dist;

            break;  // break out of while loop
        }

        numsteps--;
    }
}
