#include "hl1bspasset.h"
#include "hl1bspshader.h"
#include "hl1bspinstance.h"

#include <iostream>
#include <GL/glextl.h>
#include <glm/gtc/type_ptr.hpp>

Hl1BspAsset::Hl1BspAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Hl1Asset(locator, loader), _vao(0), _vbo(0)
{ }

Hl1BspAsset::~Hl1BspAsset()
{
    this->_faces.Delete();
    this->_lightMaps.Delete();
    this->_textures.Delete();
}

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
    this->LoadTextures(wads);
    Hl1WadAsset::UnloadWads(wads);

    this->LoadFacesWithLightmaps();

    this->LoadModels();

    glBindTexture(GL_TEXTURE_2D, 0);                // Unbind texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);          // Reset UNPACK_ALIGNMENT
    glActiveTexture(GL_TEXTURE0);

    glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.Count() * sizeof(Hl1BspAsset::tVertex), (const GLvoid *)this->_vertices, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer((GLuint)Hl1BspShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Hl1BspAsset::tVertex), 0);
    glEnableVertexAttribArray((GLuint)Hl1BspShaderAttributeLocations::Vertex);
    // Texcoords
    glVertexAttribPointer((GLuint)Hl1BspShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Hl1BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray((GLuint)Hl1BspShaderAttributeLocations::TexCoord);
    // Texcoords lightmap
    glVertexAttribPointer((GLuint)Hl1BspShaderAttributeLocations::LightCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Hl1BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*5));
    glEnableVertexAttribArray((GLuint)Hl1BspShaderAttributeLocations::LightCoord);
    // Normals
    glVertexAttribPointer((GLuint)Hl1BspShaderAttributeLocations::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Hl1BspAsset::tVertex), (const GLvoid *)(sizeof(GLfloat)*7));
    glEnableVertexAttribArray((GLuint)Hl1BspShaderAttributeLocations::Normal);

    glBindVertexArray(0);                           // Unbind vertex array

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

bool Hl1BspAsset::LoadFacesWithLightmaps()
{
    this->_faces.Allocate(this->_faceData.count);
    this->_lightMaps.Allocate(this->_faceData.count);

    // Activate TEXTURE1 for lightmaps from each face
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);          // UNPACK_ALIGNMENT, lightmaps are byte per pixel

    for (int f = 0; f < this->_faces.count; f++)
    {
        HL1::tBSPFace& in = this->_faceData[f];
        HL1::tBSPMipTexHeader* mip = this->GetMiptex(this->_texinfoData[in.texinfo].miptexIndex);
        Hl1BspAsset::tFace& out = this->_faces[f];

        out.firstVertex = this->_vertices.Count();
        out.vertexCount = in.edgeCount;
        out.flags = this->_texinfoData[in.texinfo].flags;
        out.texture = this->_texinfoData[in.texinfo].miptexIndex;
        out.lightmap = f;
        out.plane = this->_planeData[in.planeIndex];

        // Flip face normal when side == 1
        if (in.side == 1)
        {
            out.plane.normal[0] = -out.plane.normal[0];
            out.plane.normal[1] = -out.plane.normal[1];
            out.plane.normal[2] = -out.plane.normal[2];
            out.plane.distance = -out.plane.distance;
        }

        // Calculate and grab the lightmap buffer
        float min[2], max[2];
        this->CalculateSurfaceExtents(in, min, max);
        if (this->LoadLightmap(in, this->_lightMaps[f], min, max))
            this->_lightMaps[f].UploadToGl();

        float lw = float(this->_lightMaps[f].Width());
        float lh = float(this->_lightMaps[f].Height());
        float halfsizew = (min[0] + max[0]) / 2.0f;
        float halfsizeh = (min[1] + max[1]) / 2.0f;

        // Create a vertex list for this face
        for (int e = 0; e < in.edgeCount; e++)
        {
            Hl1BspAsset::tVertex v;

            // Get the edge index
            int ei = this->_surfedgeData[in.firstEdge + e];
            // Determine the vertex based on the edge index
            v.position = this->_verticesData[this->_edgeData[ei < 0 ? -ei : ei].vertex[ei < 0 ? 1 : 0]].point;

            // Copy the normal from the plane
            v.normal = out.plane.normal;

            HL1::tBSPTexInfo& ti = this->_texinfoData[in.texinfo];
            float s = glm::dot(v.position, glm::vec3(ti.vecs[0][0], ti.vecs[0][1], ti.vecs[0][2])) + ti.vecs[0][3];
            float t = glm::dot(v.position, glm::vec3(ti.vecs[1][0], ti.vecs[1][1], ti.vecs[1][2])) + ti.vecs[1][3];

            // Calculate the texture texcoords
            v.texcoords[0] = glm::vec2(s / float(mip->width), t / float(mip->height));

            // Calculate the lightmap texcoords
            v.texcoords[1] = glm::vec2(((lw / 2.0f) + (s - halfsizew) / 16.0f) / lw, ((lh / 2.0f) + (t - halfsizeh) / 16.0f) / lh);

            this->_vertices.Add(v);
        }
    }

    return true;
}

bool Hl1BspAsset::LoadTextures(const std::vector<Hl1WadAsset*>& wads)
{
    this->_textures.count = int(*this->_textureData.data);
    this->_textures.data = new Texture[this->_textures.count];
    Array<int> textureTable(int(*((int*)this->_textureData.data)), (int*)(this->_textureData.data + sizeof(int)));

    // Activate TEXTURE0 for the regular texture
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    for (int t = 0; t < this->_textures.count; t++)
    {
        const unsigned char* textureData = this->_textureData.data + textureTable[t];
        HL1::tBSPMipTexHeader* miptex = (HL1::tBSPMipTexHeader*)textureData;
        this->_textures[t].SetName(miptex->name);

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
                if (this->_textures[t].Name()[0] == '{' && source0[i] == 255)
                    r = g = b = a = 0;

                destination[i*4 + 0] = r;
                destination[i*4 + 1] = g;
                destination[i*4 + 2] = b;
                destination[i*4 + 3] = a;
            }

            this->_textures[t].SetData(miptex->width, miptex->height, bpp, destination);

            delete []destination;
        }
        else
        {
            std::cout << "Texture \"" << miptex->name << "\" not found" << std::endl;
            this->_textures[t].DefaultTexture();
        }
        this->_textures[t].UploadToGl();
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

    float light_adjust = 1.0f;// - config.brightness;

    // scale lightmap value...
    for (int i = 0; i < out.DataSize(); i++)
    {
        float f = powf((out.Data()[i] + 1) / 256.0f, light_adjust);
        int inf = f * 255.0f + 0.5f;

        // clamp between 0 and 255
        if (inf < 0) inf = 0;
        if (inf > 255) inf = 255;

        out.Data()[i] = inf;
    }
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

void Hl1BspAsset::RenderFaces(const std::set<short>& visibleFaces)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(this->_vao);

    for (std::set<short>::const_iterator i = visibleFaces.begin(); i != visibleFaces.end(); ++i)
    {
        short a = *i;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_lightMaps[this->_faces[a].lightmap].GlIndex());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_textures[this->_faces[a].texture].GlIndex());

        glDrawArrays(GL_TRIANGLE_FAN, this->_faces[a].firstVertex, this->_faces[a].vertexCount);
    }

    glBindVertexArray(0);
}

