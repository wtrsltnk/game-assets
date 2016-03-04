#include "hl2bspinstance.h"
#include "hl2bspshader.h"

#include <iostream>
#include <fstream>
#include <set>
#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory.h>

using namespace std;

Hl2BspInstance::Hl2BspInstance(Hl2BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new Hl2BspShader();
    this->_shader->BuildProgram();

    for (int i = 0; i < this->_asset->_faces.count; i++)
    {
        if (this->_asset->_faces[i].flags != 0)
            continue;

        this->_visibleFaces.insert(i);
    }

}

Hl2BspInstance::~Hl2BspInstance()
{
    this->Unload();
}

void Hl2BspInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    static glm::vec4 global_color(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat3 rotMat(view);
    glm::vec3 pos = -glm::vec3(view[3]) * rotMat;
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);
    this->_shader->SetGlobalColorVec(global_color);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void Hl2BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}

std::set<short> Hl2BspInstance::FindVisibleFaces(const glm::vec3& pos)
{
    std::set<short> leafFaces;

    int leaf = this->TracePointInLeaf(pos, this->_asset->_modelData[0].headnode[0]);
    if (leaf != 0)
    {
        // add all faces of current leaf
        for (int f = 0; f < this->_asset->_leafData[leaf].markSurfacesCount; f++)
        {
            unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[leaf].firstMarkSurface + f];
            if (this->_asset->_faces[findex].flags == 0)
                leafFaces.insert(findex);
        }

        HL2::tBSPVisLeaf* visleaf = &this->_asset->_visLeafs[leaf];
        // add all faces of leafs through vis data
        for (int l = 0; l < visleaf->leafCount; l++)
        {
            for (int f = 0; f < this->_asset->_leafData[visleaf->leafs[l]].markSurfacesCount; f++)
            {
                unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[visleaf->leafs[l]].firstMarkSurface + f];
                if (this->_asset->_faces[findex].flags == 0)
                    leafFaces.insert(findex);
            }
        }
    }
    else
    {
        for (int i = 0; i < this->_asset->_faces.count; i++)
        {
            if (this->_asset->_faces[i].flags != 0)
                continue;

            this->_visibleFaces.insert(i);
        }
    }
    for (int m = 1; m < this->_asset->_modelData.count; m++)
    {
        // add all faces of current leaf
        for (int f = this->_asset->_modelData[m].firstFace; f < this->_asset->_modelData[m].faceCount; f++)
        {
            if (this->_asset->_faces[f].flags == 0)
                leafFaces.insert(f);
        }
    }

    return leafFaces;
}

int Hl2BspInstance::TracePointInLeaf(const glm::vec3& point, int nodenum)
{
    float d;
    HL2::tBSPNode *node;
    HL2::tBSPPlane *plane;

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
