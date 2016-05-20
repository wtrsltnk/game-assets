#include "hl1bspasset.h"
#include "hl1bspinstance.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <sstream>
#include <GL/glextl.h>
#include <glm/gtc/type_ptr.hpp>

Hl1BspAsset::Hl1BspAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Hl1Asset(locator, loader)
{ }

Hl1BspAsset::~Hl1BspAsset()
{ }

bool Hl1BspAsset::Load(const std::string &filename)
{
    Array<byte>& data = this->_loader(filename);
    this->_header = (HL1::tBSPHeader*)data.data;

    this->LoadLump(data, this->_planeData, HL1_BSP_PLANELUMP);
    this->LoadLump(data, this->_textureData, HL1_BSP_TEXTURELUMP);
    this->LoadLump(data, this->_verticesData, HL1_BSP_VERTEXLUMP);
    this->LoadLump(data, this->_nodeData, HL1_BSP_NODELUMP);
    this->LoadLump(data, this->_texinfoData, HL1_BSP_TEXINFOLUMP);
    this->LoadLump(data, this->_faceData, HL1_BSP_FACELUMP);
    this->LoadLump(data, this->_lightingData, HL1_BSP_LIGHTINGLUMP);
    this->LoadLump(data, this->_clipnodeData, HL1_BSP_CLIPNODELUMP);
    this->LoadLump(data, this->_leafData, HL1_BSP_LEAFLUMP);
    this->LoadLump(data, this->_marksurfaceData, HL1_BSP_MARKSURFACELUMP);
    this->LoadLump(data, this->_edgeData, HL1_BSP_EDGELUMP);
    this->LoadLump(data, this->_surfedgeData, HL1_BSP_SURFEDGELUMP);
    this->LoadLump(data, this->_modelData, HL1_BSP_MODELLUMP);

    Array<byte> entityData;
    if (this->LoadLump(data, entityData, HL1_BSP_ENTITYLUMP))
        this->_entities = Hl1BspAsset::LoadEntities(entityData);

    Array<byte> visibilityData;
    if (this->LoadLump(data, visibilityData, HL1_BSP_VISIBILITYLUMP))
        this->_visLeafs = Hl1BspAsset::LoadVisLeafs(visibilityData, this->_leafData, this->_modelData);

    std::vector<Hl1WadAsset*> wads;
    HL1::tBSPEntity* worldspawn = this->FindEntityByClassname("worldspawn");
    if (worldspawn != nullptr)
        wads = Hl1WadAsset::LoadWads(worldspawn->keyvalues["wad"], filename);
    this->LoadTextures(this->_va.Textures(), wads);
    Hl1WadAsset::UnloadWads(wads);

    std::vector<HL1::tVertex> vertices;
    this->LoadFacesWithLightmaps(this->_va.Faces(), this->_va.Lightmaps(), vertices);
    this->_va.LoadVertices(vertices);

    this->LoadModels();

    return true;
}

Hl1Instance* Hl1BspAsset::CreateInstance()
{
    return new Hl1BspInstance(this);
}

std::vector<HL1::sBSPEntity> Hl1BspAsset::LoadEntities(const Array<byte>& entityData)
{
    const byte* itr = entityData.data;
    const byte* end = entityData.data + entityData.count;

    std::string key, value;
    std::vector<HL1::tBSPEntity> entities;
    while (itr[0] == '{')
    {
        HL1::tBSPEntity entity;
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

HL1::tBSPEntity* Hl1BspAsset::FindEntityByClassname(const std::string& classname)
{
    for (std::vector<HL1::tBSPEntity>::iterator i = this->_entities.begin(); i != this->_entities.end(); ++i)
    {
        HL1::tBSPEntity* e = &(*i);
        if (e->classname == classname)
            return e;
    }

    return nullptr;
}

std::vector<HL1::tBSPVisLeaf> Hl1BspAsset::LoadVisLeafs(const Array<byte>& visdata, const Array<HL1::tBSPLeaf>& leafs, const Array<HL1::tBSPModel>& models)
{
    std::vector<HL1::tBSPVisLeaf> visLeafs = std::vector<HL1::tBSPVisLeaf>(leafs.count);

    for (int i = 0; i < leafs.count; i++)
    {
        visLeafs[i].leafs = 0;
        visLeafs[i].leafCount = 0;
        int visOffset = leafs[i].visofs;

        for (int j = 1; j < models[0].visLeafs; visOffset++)
        {
            if (visdata[visOffset] == 0)
            {
                visOffset++;
                j += (visdata[visOffset]<<3);
            }
            else
            {
                for (byte bit = 1; bit; bit<<=1, j++)
                {
                    if (visdata[visOffset] & bit)
                        visLeafs[i].leafCount++;
                }
            }
        }

        if (visLeafs[i].leafCount > 0)
        {
            visLeafs[i].leafs = new int[visLeafs[i].leafCount];
            if (visLeafs[i].leafs == 0)
                return visLeafs;
        }
    }

    for (int i = 0; i < leafs.count; i++)
    {
        int visOffset = leafs[i].visofs;
        int index = 0;
        for (int j = 1; j < models[0].visLeafs; visOffset++)
        {
            if (visdata[visOffset] == 0)
            {
                visOffset++;
                j += (visdata[visOffset]<<3);
            }
            else
            {
                for (byte bit = 1; bit; bit<<=1, j++)
                {
                    if (visdata[visOffset] & bit)
                    {
                        visLeafs[i].leafs[index++] = j;
                    }
                }
            }
        }
    }

    return visLeafs;
}

bool Hl1BspAsset::LoadFacesWithLightmaps(std::vector<HL1::tFace>& faces, std::vector<Texture*>& lightmaps, std::vector<HL1::tVertex>& vertices)
{
    // Temporary lightmap array for each face, these will be packed into an atlas later
    Array<Texture> lightMaps;

    // Allocate the arrays for faces and lightmaps
    lightMaps.Allocate(this->_faceData.count);

    std::vector<stbrp_rect> rects;
    for (int f = 0; f < this->_faceData.count; f++)
    {
        HL1::tBSPFace& in = this->_faceData[f];
        HL1::tBSPMipTexHeader* mip = this->GetMiptex(this->_texinfoData[in.texinfo].miptexIndex);
        HL1::tFace out;

        out.firstVertex = vertices.size();
        out.vertexCount = in.edgeCount;
        out.flags = this->_texinfoData[in.texinfo].flags;
        out.texture = this->_texinfoData[in.texinfo].miptexIndex;
        out.lightmap = f;
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
        float min[2], max[2];
        this->CalculateSurfaceExtents(in, min, max);

        // Skip the lightmaps for faces with special flags
        if (out.flags == 0)
        {
            if (this->LoadLightmap(in, lightMaps[f], min, max))
            {
                stbrp_rect rect;
                rect.id = f;
                rect.w = lightMaps[f].Width() + 2;
                rect.h = lightMaps[f].Height() + 2;
                rects.push_back(rect);
            }
        }

        float lw = float(lightMaps[f].Width());
        float lh = float(lightMaps[f].Height());
        float halfsizew = (min[0] + max[0]) / 2.0f;
        float halfsizeh = (min[1] + max[1]) / 2.0f;

        // Create a vertex list for this face
        for (int e = 0; e < in.edgeCount; e++)
        {
            HL1::tVertex v;

            // Get the edge index
            int ei = this->_surfedgeData[in.firstEdge + e];
            // Determine the vertex based on the edge index
            v.position = this->_verticesData[this->_edgeData[ei < 0 ? -ei : ei].vertex[ei < 0 ? 1 : 0]].point;

            // Copy the normal from the plane
            v.normal = glm::vec3(out.plane);

            // Reset the bone so its not used
            v.bone = -1;

            HL1::tBSPTexInfo& ti = this->_texinfoData[in.texinfo];
            float s = glm::dot(v.position, glm::vec3(ti.vecs[0][0], ti.vecs[0][1], ti.vecs[0][2])) + ti.vecs[0][3];
            float t = glm::dot(v.position, glm::vec3(ti.vecs[1][0], ti.vecs[1][1], ti.vecs[1][2])) + ti.vecs[1][3];

            // Calculate the texture texcoords
            v.texcoords[0] = glm::vec2(s / float(mip->width), t / float(mip->height));

            // Calculate the lightmap texcoords
            v.texcoords[1] = glm::vec2(((lw / 2.0f) + (s - halfsizew) / 16.0f) / lw, ((lh / 2.0f) + (t - halfsizeh) / 16.0f) / lh);

            vertices.push_back(v);
        }
        faces.push_back(out);
    }

    // Activate TEXTURE1 for lightmaps from each face
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);

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
            Texture& lm = lightMaps[faces[(*rect).id].lightmap];
            if ((*rect).was_packed)
            {
                // Copy the lightmap texture into the atlas
                atlas->FillAtPosition(lm, glm::vec2((*rect).x + 1, (*rect).y + 1), true);
                for (int vertexIndex = 0; vertexIndex < faces[(*rect).id].vertexCount; vertexIndex++)
                {
                    // Recalculate the lightmap texcoords for the atlas
                    glm::vec2& vec = vertices[faces[(*rect).id].firstVertex + vertexIndex].texcoords[1];
                    vec.s = float((*rect).x + 1 + (float(lm.Width()) * vec.s)) / atlas->Width();
                    vec.t = float((*rect).y + 1 + (float(lm.Height()) * vec.t)) / atlas->Height();
                }

                // Make sure we will use the (correct) atlas when we render
                faces[(*rect).id].lightmap = lightmaps.size();
            }
            else
            {
                // When the lightmap was not packed into the atlas, we try for a next atlas
                nextrects.push_back(*rect);
            }
        }

        // upload the atlas with all its lightmap textures
        atlas->UploadToGl();
        lightmaps.push_back(atlas);
        rects = nextrects;
    }

    // cleanup the temporary lightmap array
    lightMaps.Delete();

    return true;
}

bool Hl1BspAsset::LoadTextures(std::vector<Texture*>& textures, const std::vector<Hl1WadAsset*>& wads)
{
    Array<int> textureTable(int(*this->_textureData.data), (int*)(this->_textureData.data + sizeof(int)));

    // Activate TEXTURE0 for the regular texture
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    for (int t = 0; t < int(*this->_textureData.data); t++)
    {
        const unsigned char* textureData = this->_textureData.data + textureTable[t];
        HL1::tBSPMipTexHeader* miptex = (HL1::tBSPMipTexHeader*)textureData;
        Texture* tex = new Texture();
        tex->SetName(miptex->name);

        if (miptex->offsets[0] == 0)
        {
            for (std::vector<Hl1WadAsset*>::const_iterator i = wads.cbegin(); i != wads.cend(); ++i)
            {
                Hl1WadAsset* wad = *i;
                textureData = wad->LumpData(wad->IndexOf(miptex->name));
                if (textureData != nullptr)
                    break;
            }
        }

        if (textureData != nullptr)
        {
            miptex = (HL1::tBSPMipTexHeader*)textureData;
            int s = miptex->width * miptex->height;
            int bpp = 4;
            int paletteOffset = miptex->offsets[0] + s + (s/4) + (s/16) + (s/64) + sizeof(short);

            // Get the miptex data and palette
            const unsigned char* source0 = textureData + miptex->offsets[0];
            const unsigned char* palette = textureData + paletteOffset;

            unsigned char* destination = new unsigned char[s * bpp];

            unsigned char r, g, b, a;
            for (int i = 0; i < s; i++)
            {
                r = palette[source0[i]*3];
                g = palette[source0[i]*3+1];
                b = palette[source0[i]*3+2];
                a = 255;

                // Do we need a transparent pixel
                if (tex->Name()[0] == '{' && source0[i] == 255)
                    r = g = b = a = 0;

                destination[i*4 + 0] = r;
                destination[i*4 + 1] = g;
                destination[i*4 + 2] = b;
                destination[i*4 + 3] = a;
            }

            tex->SetData(miptex->width, miptex->height, bpp, destination);

            delete []destination;
        }
        else
        {
            std::cout << "Texture \"" << miptex->name << "\" not found" << std::endl;
            tex->DefaultTexture();
        }
        tex->UploadToGl();
        textures.push_back(tex);
    }

    glBindTexture(GL_TEXTURE_2D, 0);                // Unbind texture

    return true;
}

HL1::tBSPMipTexHeader* Hl1BspAsset::GetMiptex(int index)
{
    HL1::tBSPMipTexOffsetTable* bspMiptexTable = (HL1::tBSPMipTexOffsetTable*)this->_textureData.data;

    if (index >= 0 && bspMiptexTable->miptexCount > index)
        return (HL1::tBSPMipTexHeader*)(this->_textureData.data + bspMiptexTable->offsets[index]);

    return 0;
}

int Hl1BspAsset::FaceFlags(int index)
{
    if (index >= 0 && this->_va.Faces().size() > index)
        return this->_va.Faces()[index].flags;

    return -1;
}

//
// the following computations are based on:
// PolyEngine (c) Alexey Goloshubin and Quake I source by id Software
//

void Hl1BspAsset::CalculateSurfaceExtents(const HL1::tBSPFace& in, float min[2], float max[2]) const
{
    const HL1::tBSPTexInfo* t = &this->_texinfoData[in.texinfo];

    min[0] = min[1] =  999999;
    max[0] = max[1] = -999999;

    for (int i = 0; i < in.edgeCount; i++)
    {
        const HL1::tBSPVertex* v;
        int e = this->_surfedgeData[in.firstEdge + i];
        if (e >= 0)
            v = &this->_verticesData[this->_edgeData[e].vertex[0]];
        else
            v = &this->_verticesData[this->_edgeData[-e].vertex[1]];

        for (int j = 0; j < 2; j++)
        {
            int val = v->point[0] * t->vecs[j][0] + v->point[1] * t->vecs[j][1] + v->point[2] * t->vecs[j][2] +t->vecs[j][3];
            if (val < min[j]) min[j] = val;
            if (val > max[j]) max[j] = val;
        }
    }
}

bool Hl1BspAsset::LoadLightmap(const HL1::tBSPFace& in, Texture& out, float min[2], float max[2])
{
    // compute lightmap size
    int size[2];
    for (int c = 0; c < 2; c++)
    {
        float tmin = floorf(min[c] / 16.0f);
        float tmax = ceilf(max[c] / 16.0f);

        size[c] = (int) (tmax - tmin);
    }

    out.SetData(size[0] + 1, size[1] + 1, 3, this->_lightingData.data + in.lightOffset, false);

    return true;
}

bool Hl1BspAsset::LoadModels()
{
    this->_models.Allocate(this->_modelData.count);

    for (int m = 0; m < this->_modelData.count; m++)
    {
        this->_models[m].position = this->_modelData[m].origin;
        this->_models[m].firstFace = this->_modelData[m].firstFace;
        this->_models[m].faceCount = this->_modelData[m].faceCount;
    }

    return true;
}

void Hl1BspAsset::RenderFaces(const std::set<unsigned short>& visibleFaces)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    this->_va.RenderFaces(visibleFaces);
}

