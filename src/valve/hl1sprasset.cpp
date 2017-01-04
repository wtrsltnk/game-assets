#include "valve/hl1sprasset.h"
#include "valve/hl1sprinstance.h"

using namespace valve::hl1;

SprAsset::SprAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Asset(locator, loader)
{ }

SprAsset::~SprAsset()
{ }

bool SprAsset::Load(const std::string &filename)
{
    Texture* lm = new Texture();
    lm->SetDimentions(32, 32, 3);
    lm->Fill(glm::vec4(255, 255, 255, 255));
    lm->UploadToGl();
    this->_va.Lightmaps().push_back(lm);

    Array<byte>& data = this->_loader(filename);
    this->_header = (tSPRHeader*)data.data;

    int w = this->_header->width/2.0f;
    int h = this->_header->height/2.0f;

    tVertex verts[4];
    verts[0].position = glm::vec3(-w, 0.0f, -h); verts[0].texcoords[0] = glm::vec2(0.0f, 1.0f); verts[0].bone = -1;
    verts[1].position = glm::vec3( w, 0.0f, -h); verts[1].texcoords[0] = glm::vec2(1.0f, 1.0f); verts[1].bone = -1;
    verts[2].position = glm::vec3( w, 0.0f,  h); verts[2].texcoords[0] = glm::vec2(1.0f, 0.0f); verts[2].bone = -1;
    verts[3].position = glm::vec3(-w, 0.0f,  h); verts[3].texcoords[0] = glm::vec2(0.0f, 0.0f); verts[3].bone = -1;

    std::vector<tVertex> vertices;
    for (int i = 0; i < 4; i++)
    {
        vertices.push_back(verts[i]);
    }
    this->_va.LoadVertices(vertices);

    short paletteColorCount = *(short*)(data.data + sizeof(tSPRHeader));
    byte* palette = (byte*)(data.data + sizeof(tSPRHeader) + sizeof(short));
    byte* tmp = (byte*)(palette + (paletteColorCount * 3));

    this->_frames.Allocate(this->_header->numframes);

    for (int f = 0; f < this->_frames.count; f++)
    {
        eSpriteFrameType frames = *(eSpriteFrameType*)tmp;
        tmp += sizeof(eSpriteFrameType);
        if (frames == SPR_SINGLE)
        {
            tFace face;
            face.firstVertex = 0;
            face.vertexCount = 4;
            face.texture = this->_va.Textures().size();
            face.lightmap = 0;
            this->_va.Faces().push_back(face);

            tSPRFrame* frame = (tSPRFrame*)tmp;
            tmp += sizeof(tSPRFrame);
            unsigned char* textureData = new unsigned char[frame->width * frame->height * 3];
            for (int y = 0; y < frame->height; y++)
            {
                for (int x = 0; x < frame->width; x++)
                {
                    int item = x * frame->height + y;
                    unsigned char index = tmp[item];
                    textureData[item * 3] = palette[index * 3];
                    textureData[item * 3 + 1] = palette[index * 3 + 1];
                    textureData[item * 3 + 2] = palette[index * 3 + 2];
                }
            }
            auto tex = new Texture();
            tex->SetData(frame->width, frame->height, 3, textureData);
            this->_frames[f] = tex->UploadToGl();
            this->_va.Textures().push_back(tex);

            delete []textureData;

            tmp += frame->width * frame->height;
        }
    }

    return true;
}

void SprAsset::RenderSpriteFrame(int frame)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    std::set<unsigned short> indices;
    indices.insert(frame);

    this->_va.RenderFaces(indices);
}

int SprAsset::FrameCount() const
{
    return this->_frames.count;
}
