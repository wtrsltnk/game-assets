#include "hl2bspasset.h"
#include "hl2bspshader.h"
#include "hl2bspinstance.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <GL/glextl.h>
#include <glm/gtc/type_ptr.hpp>

Hl2BspAsset::Hl2BspAsset(DataFileLoader& loader)
    : Hl1Asset(loader), _vao(0), _vbo(0)
{ }

Hl2BspAsset::~Hl2BspAsset()
{
    this->_faces.Delete();
    this->_lightMaps.Delete();
    this->_textures.Delete();
}

bool Hl2BspAsset::Load(const std::string &filename)
{
    Array<byte>& data = this->_loader(filename);
    this->_header = (HL2::tBSPHeader*)data.data;

    if (this->_header->ident != HL2_BSP_SIGNATURE)
    {
        std::cout << "Wrong Signature" << std::endl;
        return false;
    }

    this->LoadLump(data, this->_planeData, HL2_BSP_PLANES);
    this->LoadLump(data, this->_textureData, HL2_BSP_TEXDATA);
    this->LoadLump(data, this->_verticesData, HL2_BSP_VERTEXES);
    this->LoadLump(data, this->_nodeData, HL2_BSP_NODES);
    this->LoadLump(data, this->_texinfoData, HL2_BSP_TEXINFO);
    this->LoadLump(data, this->_faceData, HL2_BSP_FACES);
    this->LoadLump(data, this->_lightingData, HL2_BSP_LIGHTING);
    this->LoadLump(data, this->_clipnodeData, HL2_BSP_OCCLUSION);
    this->LoadLump(data, this->_leafData, HL2_BSP_LEAFS);
    this->LoadLump(data, this->_marksurfaceData, HL2_BSP_FACEIDS);
    this->LoadLump(data, this->_edgeData, HL2_BSP_EDGES);
    this->LoadLump(data, this->_surfedgeData, HL2_BSP_SURFEDGES);
    this->LoadLump(data, this->_modelData, HL2_BSP_MODELS);

    Array<byte> entityData;
    if (this->LoadLump(data, entityData, HL2_BSP_ENTITIES))
        this->_entities = Hl2BspAsset::LoadEntities(entityData);

    //    std::vector<Hl1WadAsset*> wads;
    //    HL2::tBSPEntity* worldspawn = this->FindEntityByClassname("worldspawn");
    //    if (worldspawn != nullptr)
    //        wads = Hl1WadAsset::LoadWads(worldspawn->keyvalues["wad"], filename);
    //    this->LoadTextures(wads);
    //    Hl1WadAsset::UnloadWads(wads);

    this->LoadFacesWithLightmaps(this);

    glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.Count() * sizeof(Hl2BspAsset::tVertex), (const GLvoid *)this->_vertices, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer((GLuint)Hl2BspShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Hl2BspAsset::tVertex), 0);
    glEnableVertexAttribArray((GLuint)Hl2BspShaderAttributeLocations::Vertex);
    // Texcoords
    glVertexAttribPointer((GLuint)Hl2BspShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Hl2BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray((GLuint)Hl2BspShaderAttributeLocations::TexCoord);
    // Texcoords lightmap
    glVertexAttribPointer((GLuint)Hl2BspShaderAttributeLocations::LightCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Hl2BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*5));
    glEnableVertexAttribArray((GLuint)Hl2BspShaderAttributeLocations::LightCoord);
    // Normals
    glVertexAttribPointer((GLuint)Hl2BspShaderAttributeLocations::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Hl2BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*7));
    glEnableVertexAttribArray((GLuint)Hl2BspShaderAttributeLocations::Normal);

    glBindVertexArray(0);                           // Unbind vertex array

    return true;
}

Hl1Instance* Hl2BspAsset::CreateInstance()
{
    return new Hl2BspInstance(this);
}

std::vector<HL2::tBSPEntity> Hl2BspAsset::LoadEntities(const Array<byte>& entityData)
{
    const byte* itr = entityData.data;
    const byte* end = entityData.data + entityData.count;

    std::string key, value;
    std::vector<HL2::tBSPEntity> entities;
    while (itr[0] == '{')
    {
        HL2::tBSPEntity entity;
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

HL2::tBSPEntity* Hl2BspAsset::FindEntityByClassname(const std::string& classname)
{
    for (std::vector<HL2::tBSPEntity>::iterator i = this->_entities.begin(); i != this->_entities.end(); ++i)
    {
        HL2::tBSPEntity* e = &(*i);
        if (e->classname == classname)
            return e;
    }

    return nullptr;
}

bool Hl2BspAsset::LoadFacesWithLightmaps(Hl2BspAsset* bspAsset)
{
    this->_faces.Allocate(bspAsset->_faceData.count);
    this->_lightMaps.Allocate(bspAsset->_faceData.count);

    glActiveTexture(GL_TEXTURE1);                   // Activate TEXTURE1 for lightmaps from each face
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);          // UNPACK_ALIGNMENT, lightmaps are byte per pixel

    for (int f = 0; f < this->_faces.count; f++)
    {
        HL2::tBSPFace& in = bspAsset->_faceData[f];
        //        HL2::tBSPMipTexHeader* mip = bspAsset->GetMiptex(bspAsset->_texinfoData[in.texinfo].miptexIndex);
        Hl2BspAsset::tFace& out = this->_faces[f];

        out.firstVertex = this->_vertices.Count();
        out.vertexCount = in.edgeCount;
        out.flags = bspAsset->_texinfoData[in.texinfo].flags;
        //        out.texture = bspAsset->_texinfoData[in.texinfo].miptexIndex;
        out.lightmap = f;
        out.plane = bspAsset->_planeData[in.planeIndex];

        // Flip face normal when side == 1
        if (in.side == 1)
        {
            out.plane.normal[0] = -out.plane.normal[0];
            out.plane.normal[1] = -out.plane.normal[1];
            out.plane.normal[2] = -out.plane.normal[2];
            out.plane.distance = -out.plane.distance;
        }

        // Calculate and grab the lightmap buffer
        if (bspAsset->LoadLightmap(in, this->_lightMaps[f]))
            this->_lightMaps[f].UploadToGl();

        // Create a vertex list for this face
        for (int e = 0; e < in.edgeCount; e++)
        {
            Hl2BspAsset::tVertex v;

            // Get the edge index
            int ei = bspAsset->_surfedgeData[in.firstEdge + e];
            // Determine the vertex based on the edge index
            v.position = bspAsset->_verticesData[bspAsset->_edgeData[ei < 0 ? -ei : ei].vertex[ei < 0 ? 1 : 0]].point;

            // Copy the normal from the plane
            v.normal = out.plane.normal;

            auto pTexinfos = bspAsset->_texinfoData[in.texinfo];
            float tc3 = pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][0] * v.position[0] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][1] * v.position[1] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][2] * v.position[2] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[0][3] - in.LightmapTextureMinsInLuxels[0];
            float tc4 = pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][0] * v.position[0] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][1] * v.position[1] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][2] * v.position[2] +
                    pTexinfos.lightmapVecsLuxelsPerWorldUnits[1][3] - in.LightmapTextureMinsInLuxels[1];

            v.texcoords[0].x = v.texcoords[1].x = tc3 / this->_lightMaps[f].Width();
            v.texcoords[0].y = v.texcoords[1].y = tc4 / this->_lightMaps[f].Height();

            this->_vertices.Add(v);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);                // Unbind texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);          // Reset UNPACK_ALIGNMENT
    glActiveTexture(GL_TEXTURE0);                   // Activate TEXTURE0 for regular textures

    return true;
}

bool Hl2BspAsset::LoadTextures(const std::vector<Hl1WadAsset*>& wads)
{
    return true;
}

HL2::tBSPMipTexHeader* Hl2BspAsset::GetMiptex(int index)
{
    HL2::tBSPMipTexOffsetTable* bspMiptexTable = (HL2::tBSPMipTexOffsetTable*)this->_textureData.data;

    if (index >= 0 && bspMiptexTable->miptexCount > index)
        return (HL2::tBSPMipTexHeader*)(this->_textureData.data + bspMiptexTable->offsets[index]);

    return 0;
}

#define LIGHTMAP_GAMMA 2.2f

bool Hl2BspAsset::LoadLightmap(const HL2::tBSPFace& in, Texture& out)
{
    if (in.lightOffset < 0)
        return false;

    float gamma = 1.0f / 2.0f;
    int luxelCount = (in.LightmapTextureSizeInLuxels[0] + 1) * (in.LightmapTextureSizeInLuxels[1] + 1);
    HL2::tBSPColorRGBExp32* pLightmap = (HL2::tBSPColorRGBExp32*) (this->_lightingData.data + in.lightOffset);

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

void Hl2BspAsset::RenderFaces(const std::set<int>& visibleFaces)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(this->_vao);
    for (auto i = visibleFaces.begin(); i != visibleFaces.end(); ++i)
    {
        int a = *i;
        if (this->_faces[a].flags &= SURF_SKY)
            continue;
        if (this->_faces[a].flags &= SURF_TRIGGER)
            continue;

        GLuint u = this->_lightMaps[a].GlIndex();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, u);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDrawArrays(GL_POLYGON, this->_faces[a].firstVertex, this->_faces[a].vertexCount);
    }
    glBindVertexArray(0);
}

