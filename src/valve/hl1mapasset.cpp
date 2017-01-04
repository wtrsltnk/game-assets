#include "valve/hl1mapasset.h"
#include "valve/hl1mapinstance.h"

#include <iostream>

using namespace valve::hl1;

MapAsset::MapAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Asset(locator, loader)
{ }

MapAsset::~MapAsset()
{ }

bool MapAsset::Load(const std::string& filename)
{
    Array<byte>& data = this->_loader(filename);

    Tokenizer tok((char*)data.data, data.count);

    while (tok.nextToken())
    {
        if (strcmp(tok.getToken(), "{") == 0)
        {
            if (this->LoadEntity(tok) == false)
            {
                return false;
            }
        }
    }

    std::vector<WadAsset*> wads;
    tEntity* worldspawn = this->FindEntityByClassname("worldspawn");
    if (worldspawn != nullptr)
        wads = WadAsset::LoadWads(worldspawn->_keyValues["wad"], filename);
    this->LoadTextures(wads);
    WadAsset::UnloadWads(wads);

    return true;
}

bool MapAsset::LoadEntity(Tokenizer& tok)
{
    tEntity e;

    while (tok.nextToken() && strcmp(tok.getToken(), "}") != 0)
    {
        if (strcmp(tok.getToken(), "{") == 0)
        {
            if (this->LoadBrush(tok, e) == false)
                return false;
        }
        else
        {
            std::string key(tok.getToken());
            tok.nextToken();
            std::string value(tok.getToken());

            if (key == "mapversion")
                this->_mapVersion = atoi(value.c_str());
            else if (key == "classname")
                e._className = value;
            else
                e._keyValues.insert(std::make_pair(key, value));
        }
    }

    this->_entities.push_back(e);

    return strcmp(tok.getToken(), "}") == 0;
}

bool MapAsset::LoadBrush(Tokenizer& tok, tEntity& entity)
{
    tBrush b;

    while (tok.nextToken() && strcmp(tok.getToken(), "}") != 0)
    {
        glm::vec3 v1, v2, v3;
        if (tok.nextToken() == false) return false;	// Skip the "("
        v1[0] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v1[1] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v1[2] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        if (tok.nextToken() == false) return false;	// Skip the ")"

        if (tok.nextToken() == false) return false;	// Skip the "("
        v2[0] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v2[1] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v2[2] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        if (tok.nextToken() == false) return false;	// Skip the ")"

        if (tok.nextToken() == false) return false;	// Skip the "("
        v3[0] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v3[1] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        v3[2] = float(atoi(tok.getToken())); if (tok.nextToken() == false) return false;
        if (tok.nextToken() == false) return false;	// Skip the ")"

        tBrushFace face = MapAsset::CreateFace(v1, v2, v3);
        face.texture = tok.getToken(); if (tok.nextToken() == false) return false;	// Texture name

        if (this->_mapVersion == 100)      // Default Quake version
        {
            face.material_v100.x_off = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	// x_off     - Texture x-offset (must be multiple of 16)
            face.material_v100.y_off = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	// y_off     - Texture y-offset (must be multiple of 16)
            face.material_v100.rot_angle = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	// rot_angle - floating point value indicating texture rotation
            face.material_v100.x_scale = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	// x_scale   - scales x-dimension of texture (negative value to flip)
            face.material_v100.y_scale = float(atof(tok.getToken()));
        }
        else if (this->_mapVersion == 220) // Valve Hammer Version
        {
            if (tok.nextToken() == false) return false;	// Skip the "["
            face.material_v220.uaxis[0] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.uaxis[1] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.uaxis[2] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.ushift = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            if (tok.nextToken() == false) return false;	// Skip the "]"
            if (tok.nextToken() == false) return false;	// Skip the "["
            face.material_v220.vaxis[0] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.vaxis[1] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.vaxis[2] = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.vshift = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            if (tok.nextToken() == false) return false;	// Skip the "]"
            face.material_v220.rotate = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.xscale = float(atof(tok.getToken())); if (tok.nextToken() == false) return false;	//
            face.material_v220.yscale = float(atof(tok.getToken()));
        }
        b._brushFaces.push_back(face);
        this->_textureNames.insert(face.texture);
    }

    entity._brushes.push_back(b);

    return strcmp(tok.getToken(), "}") == 0;
}

MapAsset::tBrushFace MapAsset::CreateFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
    glm::vec3 normal = glm::normalize(glm::cross(v1-v2, v3-v2));

    tBrushFace face;
    face.distance = glm::dot(v2, normal);
    face.normal = normal;

    return face;
}

MapAsset::tEntity* MapAsset::FindEntityByClassname(const std::string& classname)
{
    for (std::vector<tEntity>::iterator i = this->_entities.begin(); i != this->_entities.end(); ++i)
    {
        tEntity* e = &(*i);
        if (e->_className == classname)
            return e;
    }

    return nullptr;
}

bool MapAsset::LoadTextures(const std::vector<WadAsset*>& wads)
{
    // Activate TEXTURE0 for the regular texture
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    for (std::set<std::string>::iterator n = this->_textureNames.begin(); n != this->_textureNames.end(); ++n)
    {
        Texture* texture = new Texture();
        texture->SetName(*n);
        this->_textures.insert(std::make_pair(*n, texture));

        const byte* textureData = nullptr;
        for (std::vector<WadAsset*>::const_iterator i = wads.cbegin(); i != wads.cend(); ++i)
        {
            WadAsset* wad = *i;
            textureData = wad->LumpData(wad->IndexOf(*n));
            if (textureData != nullptr)
                break;
        }

        if (textureData != nullptr)
        {
            hl1::tBSPMipTexHeader* miptex = (hl1::tBSPMipTexHeader*)textureData;
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
                if (texture->Name()[0] == '{' && source0[i] == 255)
                    r = g = b = a = 0;

                destination[i*4 + 0] = r;
                destination[i*4 + 1] = g;
                destination[i*4 + 2] = b;
                destination[i*4 + 3] = a;
            }

            texture->SetData(miptex->width, miptex->height, bpp, destination);

            delete []destination;
        }
        else
        {
            std::cout << "Texture \"" << (*n) << "\" not found" << std::endl;
            texture->DefaultTexture();
        }
        texture->UploadToGl();
    }

    glBindTexture(GL_TEXTURE_2D, 0);                // Unbind texture

    return true;
}
