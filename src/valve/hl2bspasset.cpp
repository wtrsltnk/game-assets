#include "valve/hl2bspasset.h"
#include "valve/hl2bspinstance.h"

// Implementation is already don in hl1bsp
//#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace valve::hl2;

BspAsset::BspAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Asset(locator, loader), _vao(0), _vbo(0)
{ }

BspAsset::~BspAsset()
{ }

bool BspAsset::Load(const std::string &filename)
{
    Array<byte>& data = this->_loader(filename);
    this->_header = (tBSPHeader*)data.data;

    if (this->_header->ident != HL2_BSP_SIGNATURE)
    {
        std::cout << "Wrong Signature" << std::endl;
        return false;
    }

    this->LoadLump(data, this->_planeData, HL2_BSP_PLANES);
    this->LoadLump(data, this->_verticesData, HL2_BSP_VERTEXES);
    this->LoadLump(data, this->_texinfoData, HL2_BSP_TEXINFO);
    this->LoadLump(data, this->_faceData, HL2_BSP_FACES);
    this->LoadLump(data, this->_lightingData, HL2_BSP_LIGHTING);
    this->LoadLump(data, this->_marksurfaceData, HL2_BSP_FACEIDS);
    this->LoadLump(data, this->_edgeData, HL2_BSP_EDGES);
    this->LoadLump(data, this->_surfedgeData, HL2_BSP_SURFEDGES);
    this->LoadLump(data, this->_modelData, HL2_BSP_MODELS);
    this->LoadLump(data, this->_displacementInfoData, HL2_BSP_DISPINFO);
    this->LoadLump(data, this->_displacementTriangleData, HL2_BSP_DISP_TRIS);
    this->LoadLump(data, this->_displacementVertexData, HL2_BSP_DISP_VERTS);

    Array<byte> entityData;
    if (this->LoadLump(data, entityData, HL2_BSP_ENTITIES))
        this->_entities = BspAsset::LoadEntities(entityData);

    std::vector<tVertex> vertices;
    this->LoadFacesWithLightmaps(vertices);
    this->_va.LoadVertices(vertices);

    std::vector<hl1::WadAsset*> wads;
    this->LoadTextures(wads);

    return true;
}

bool BspAsset::LoadFacesWithLightmaps(std::vector<tVertex>& vertices)
{
    // Temporary lightmap array for each face, these will be packed into an atlas later
    Array<Texture> lightMaps;

    // Allocate the arrays for faces and lightmaps
    lightMaps.Allocate(this->_faceData.count);

    std::vector<stbrp_rect> rects;
    for (int f = 0; f < this->_faceData.count; f++)
    {
        tBSPFace& in = this->_faceData[f];
        tFace out;

        out.firstVertex = vertices.size();
        out.vertexCount = in.edgeCount;
        out.flags = this->_texinfoData[in.texinfo].flags;
        out.texture = 0;
        out.lightmap = 0;
        out.plane = glm::vec4(
                    this->_planeData[in.planeIndex].normal[0],
                    this->_planeData[in.planeIndex].normal[1],
                    this->_planeData[in.planeIndex].normal[2],
                    this->_planeData[in.planeIndex].distance
                );

        // Flip face normal when side == 1
        if (in.side == 1)
        {
            out.plane[0] = -out.plane[0];
            out.plane[1] = -out.plane[1];
            out.plane[2] = -out.plane[2];
            out.plane[3] = -out.plane[3];
        }

        // Calculate and grab the lightmap buffer
        if (this->LoadLightmap(in, lightMaps[f]))
        {
            stbrp_rect rect;
            rect.id = f;
            rect.w = lightMaps[f].Width() + 2;
            rect.h = lightMaps[f].Height() + 2;
            rects.push_back(rect);
        }

        // Create a vertex list for this face
        for (int e = 0; e < in.edgeCount; e++)
        {
            tVertex v;

            // Get the edge index
            int ei = this->_surfedgeData[in.firstEdge + e];
            // Determine the vertex based on the edge index
            v.position = this->_verticesData[this->_edgeData[ei < 0 ? -ei : ei].vertex[ei < 0 ? 1 : 0]].point;

            // Copy the normal from the plane
            v.normal = glm::vec3(out.plane);

            // Reset the bone so its not used
            v.bone = -1;

            auto pTexinfos = this->_texinfoData[in.texinfo];
            float tc3 = pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][0] * v.position[0] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][1] * v.position[1] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][2] * v.position[2] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][3] - in.LightmapTextureMinsInLuxels[0];
            float tc4 = pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][0] * v.position[0] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][1] * v.position[1] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][2] * v.position[2] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][3] - in.LightmapTextureMinsInLuxels[1];

            v.texcoords[0].x = v.texcoords[1].x = tc3 / lightMaps[f].Width();
            v.texcoords[0].y = v.texcoords[1].y = tc4 / lightMaps[f].Height();

            vertices.push_back(v);
        }
        this->_va.Faces().push_back(out);
    }

    // Pack the lightmaps into an atlas
    while (rects.size() > 0)
    {
        // Setup one atlas texture (for now)
        Texture* atlas = new Texture();
        atlas->SetDimentions(512, 512, 3);
        atlas->Fill(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

        // pack the lightmap rects into one atlas
        stbrp_context context = { 0 };
        Array<stbrp_node> nodes(rects.size());
        stbrp_init_target(&context, atlas->Width(), atlas->Height(), nodes, rects.size());
        stbrp_pack_rects(&context, (stbrp_rect*)&rects[0], rects.size());

        std::vector<stbrp_rect> nextrects;
        for (auto rect = rects.begin(); rect != rects.end(); rect++)
        {
            // a reference to the loaded lightmapfrom the rect
            Texture& lm = lightMaps[(*rect).id];
            if ((*rect).was_packed)
            {
                // Copy the lightmap texture into the atlas
                atlas->FillAtPosition(lm, glm::vec2((*rect).x + 1, (*rect).y + 1), true);
                for (int vertexIndex = 0; vertexIndex < this->_va.Faces()[(*rect).id].vertexCount; vertexIndex++)
                {
                    // Recalculate the lightmap texcoords for the atlas
                    glm::vec2& vec = vertices[this->_va.Faces()[(*rect).id].firstVertex + vertexIndex].texcoords[1];
                    vec.s = float((*rect).x + 1 + (float(lm.Width()) * vec.s)) / atlas->Width();
                    vec.t = float((*rect).y + 1 + (float(lm.Height()) * vec.t)) / atlas->Height();
                }

                // Make sure we will use the (correct) atlas when we render
                this->_va.Faces()[(*rect).id].lightmap = this->_va.Lightmaps().size();
            }
            else
            {
                // When the lightmap was not packed into the atlas, we try for a next atlas
                nextrects.push_back(*rect);
            }
        }

        // upload the atlas with all its lightmap textures
        atlas->UploadToGl();
        this->_va.Lightmaps().push_back(atlas);
        rects = nextrects;
    }

    lightMaps.Delete();

    return true;
}

bool BspAsset::LoadTextures(const std::vector<hl1::WadAsset*>& wads)
{
    // Activate TEXTURE0 for textures from each face
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    Texture* lm = new Texture();
    lm->SetDimentions(32, 32, 3);
    lm->Fill(glm::vec4(255, 255, 255, 255));
    lm->UploadToGl();
    this->_va.Textures().push_back(lm);

    return true;
}

std::vector<tBSPEntity> BspAsset::LoadEntities(const Array<byte>& entityData)
{
    const byte* itr = entityData.data;
    const byte* end = entityData.data + entityData.count;

    std::string key, value;
    std::vector<tBSPEntity> entities;
    while (itr[0] == '{')
    {
        tBSPEntity entity;
        itr++; // skip the bracket
        while (itr[0] <= ' ' && itr != end) itr++; // skip all space characters
        while (itr[0] != '}')
        {
            key = "";
            value = "";

            itr++; // skip the quote
            while (itr[0] != '\"' && itr != end) key += (*itr++);

            itr++; // skip the quote
            while (itr[0] <= ' ' && itr != end) itr++; // skip all space characters

            itr++; // skip the quote
            while (itr[0] != '\"' && itr != end) value += (*itr++);

            if (key == "classname") entity.classname = value;
            entity.keyvalues.insert(std::make_pair(key, value));

            itr++; // skip the quote
            while (itr[0] <= ' ' && itr != end) itr++; // skip all space characters
        }
        entities.push_back(entity);
        while (itr[0] != '{' && itr != end) itr++; // skip to the next entity
    }
    return entities;
}

tBSPEntity* BspAsset::FindEntityByClassname(const std::string& classname)
{
    for (std::vector<tBSPEntity>::iterator i = this->_entities.begin(); i != this->_entities.end(); ++i)
    {
        tBSPEntity* e = &(*i);
        if (e->classname == classname)
            return e;
    }

    return nullptr;
}

#define LIGHTMAP_GAMMA 2.2f

bool BspAsset::LoadLightmap(const tBSPFace& in, Texture& out)
{
    if (in.lightOffset < 0)
        return false;

    float gamma = 1.0f / 2.0f;
    int luxelCount = (in.LightmapTextureSizeInLuxels[0] + 1) * (in.LightmapTextureSizeInLuxels[1] + 1);
    tBSPColorRGBExp32* pLightmap = (tBSPColorRGBExp32*) (this->_lightingData.data + in.lightOffset);

    Array<byte> data(luxelCount * 3);
    for (int i = 0; i < luxelCount; i++)
    {
        float r = pLightmap[i].b * powf(2.0f, pLightmap[i].exponent);
        if (r < 0) r = 0.f; if (r > 255) r = 255.f;
        data[i * 3 + 0] = (byte)(powf(round(r) / 255.f, gamma) *255.f);

        float g = pLightmap[i].g * powf(2.0f, pLightmap[i].exponent);
        if (g < 0) g = 0.f; if (g > 255) g = 255.f;
        data[i * 3 + 1] = (byte)(powf(round(g) / 255.f, gamma) *255.f);;

        float b = pLightmap[i].b * powf(2.0f, pLightmap[i].exponent);
        if (b < 0) b = 0.f; if (b > 255) b = 255.f;
        data[i * 3 + 2] = (byte)(powf(round(b) / 255.f, gamma) *255.f);
    }
    out.SetData(in.LightmapTextureSizeInLuxels[0] + 1, in.LightmapTextureSizeInLuxels[1] + 1, 3, data.data, false);

    return true;
}

void BspAsset::RenderFaces(const std::set<unsigned short>& visibleFaces)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    this->_va.RenderFaces(visibleFaces, GL_TRIANGLE_FAN);
}

