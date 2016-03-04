#ifndef HL1MAPDOCUMENT_H
#define HL1MAPDOCUMENT_H

#include "hl1types.h"
#include "hl1wadasset.h"
#include "tokenizer.h"
#include "texture.h"
#include <vector>
#include <set>

class Hl1MapAsset : public Hl1Asset
{
public:
    typedef struct sBrushFaceTextureDefinition_v100
    {
        float x_off;     // Texture x-offset (must be multiple of 16)
        float y_off;     // Texture y-offset (must be multiple of 16)
        float rot_angle; // floating point value indicating texture rotation
        float x_scale;   // scales x-dimension of texture (negative value to flip)
        float y_scale;   // scales y-dimension of texture (negative value to flip)

    } tBrushFaceTextureDefinition_v100;

    typedef struct sBrushFaceTextureDefinition_v220
    {
        glm::vec3 uaxis;
        float ushift;
        glm::vec3 vaxis;
        float vshift;
        float rotate;
        float xscale;
        float yscale;

    } tBrushFaceTextureDefinition_v220;

    typedef struct sBrushFace
    {
        float distance;
        glm::vec3 normal;
        std::string texture;
        tBrushFaceTextureDefinition_v100 material_v100;
        tBrushFaceTextureDefinition_v220 material_v220;

    } tBrushFace;

    typedef struct sBrush
    {
        std::vector<tBrushFace> _brushFaces;

    } tBrush;

    typedef struct sEntity
    {
        std::string _className;
        KeyValueList _keyValues;
        std::vector<tBrush> _brushes;

    } tEntity;

public:
    Hl1MapAsset(DataFileLoader& loader);
    virtual ~Hl1MapAsset();

    virtual bool Load(const std::string& filename);
    virtual Hl1Instance* CreateInstance();

    tEntity* FindEntityByClassname(const std::string& classname);

    std::vector<tEntity> _entities;
    std::set<std::string> _textureNames;
    std::map<std::string, Texture*> _textures;

private:
    int _mapVersion;
    bool LoadEntity(Tokenizer& tok);
    bool LoadBrush(Tokenizer& tok, tEntity& entity);
    static tBrushFace CreateFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
    bool LoadTextures(const std::vector<Hl1WadAsset*>& wads);

};

#endif // HL1MAPDOCUMENT_H
