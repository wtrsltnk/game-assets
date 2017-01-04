#include "valve/hl1mdlasset.h"
#include "valve/hl1mdlinstance.h"

#include <sstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>

using namespace valve::hl1;

MdlAsset::MdlAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Asset(locator, loader), _header(0)
{ }

MdlAsset::~MdlAsset()
{
    this->_bodyparts.Delete();
}

bool MdlAsset::Load(const std::string &filename)
{
    Array<byte> file = this->_loader(filename);
    this->_header = (tMDLHeader*)file.data;

    // preload textures
    if (this->_header->numtextures == 0)
    {
        Array<byte> textureFile = this->_loader(filename.substr(0, filename.size()-4) + "T.mdl");
        this->_textureHeader = (tMDLHeader*)textureFile.data;
    }
    else
        this->_textureHeader = this->_header;

    // preload animations
    if (this->_header->numseqgroups > 1)
    {
        for (int i = 1; i < this->_header->numseqgroups; i++)
        {
            std::stringstream seqgroupname;
            seqgroupname
                    << filename.substr(0, filename.size()-4)
                    << std::setw(2) << std::setfill('0') << i
                    << ".mdl";

            Array<unsigned char> buffer = this->_loader(seqgroupname.str());
            if (buffer.data != nullptr)
                this->_animationHeaders[i] = (tMDLSequenceHeader*)buffer.data;
        }
    }

    this->_textureData.Map(this->_textureHeader->numtextures, (tMDLTexture*)(file.data + this->_textureHeader->textureindex));
    this->_skinRefData.Map(this->_textureHeader->numskinref, (short*)((byte*)this->_textureHeader + this->_textureHeader->skinindex));
    this->_skinFamilyData.Map(this->_textureHeader->numskinref, (short*)((byte*)this->_textureHeader + this->_textureHeader->skinindex));
    this->_bodyPartData.Map(this->_header->numbodyparts, (tMDLBodyParts*)(file.data + this->_header->bodypartindex));
    this->_sequenceGroupData.Map(this->_header->numseqgroups, (tMDLSequenceGroup*)(file.data + this->_header->seqgroupindex));
    this->_sequenceData.Map(this->_header->numseq, (tMDLSequenceDescription*)(file.data + this->_header->seqindex));
    this->_boneControllerData.Map(this->_header->numbonecontrollers, (tMDLBoneController*)(file.data + this->_header->bonecontrollerindex));
    this->_boneData.Map(this->_header->numbones, (tMDLBone*)(file.data + this->_header->boneindex));

    std::vector<tVertex> vertices;
    this->LoadTextures(this->_va.Textures());
    this->LoadBodyParts(this->_va.Faces(), this->_va.Lightmaps(), vertices);

    this->_va.LoadVertices(vertices);

    return true;
}

void MdlAsset::LoadTextures(std::vector<Texture*>& _textures)
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < this->_textureHeader->numtextures; i++)
    {
        Texture* t = new Texture();
        tMDLTexture *ptexture= &this->_textureData[i];

        byte* data = ((byte*)this->_textureHeader) + ptexture->index;
        byte* pal = ((byte*)this->_textureHeader) + ptexture->width * ptexture->height + ptexture->index;

        std::stringstream ss;
        ss << ptexture->name << long(*(long*)ptexture);
        t->SetName(ss.str());

        // unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight;
        int outwidth, outheight;
        int row1[256], row2[256], col1[256], col2[256];
        byte *pix1, *pix2, *pix3, *pix4;
        byte *tex, *out;

        // convert texture to power of 2
        for (outwidth = 1; outwidth < ptexture->width; outwidth <<= 1)
            ;

        if (outwidth > 256)
            outwidth = 256;

        for (outheight = 1; outheight < ptexture->height; outheight <<= 1)
            ;

        if (outheight > 256)
            outheight = 256;

        tex = out = new byte[outwidth * outheight * 4];

        for (int k = 0; k < outwidth; k++)
        {
            col1[k] = int((k + 0.25f) * (float(ptexture->width) / float(outwidth)));
            col2[k] = int((k + 0.75f) * (float(ptexture->width) / float(outwidth)));
        }

        for (int k = 0; k < outheight; k++)
        {
            row1[k] = (int)((k + 0.25f) * (ptexture->height / (float)outheight)) * ptexture->width;
            row2[k] = (int)((k + 0.75f) * (ptexture->height / (float)outheight)) * ptexture->width;
        }

        // scale down and convert to 32bit RGB
        for (int k = 0 ; k < outheight ; k++)
        {
            for (int j = 0 ; j < outwidth ; j++, out += 4)
            {
                pix1 = &pal[data[row1[k] + col1[j]] * 3];
                pix2 = &pal[data[row1[k] + col2[j]] * 3];
                pix3 = &pal[data[row2[k] + col1[j]] * 3];
                pix4 = &pal[data[row2[k] + col2[j]] * 3];

                out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
                out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
                out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
                out[3] = 0xFF;
            }
        }

        t->SetData(outwidth, outheight, 4, tex);
        t->UploadToGl();
        delete []tex;

        this->_textureData[i].index = _textures.size();
        _textures.push_back(t);
    }
}

void MdlAsset::LoadBodyParts(std::vector<tFace>& faces, std::vector<Texture*>& lightmaps, std::vector<tVertex>& _vertices)
{
    float s, t;
    int vertnum;

    Texture* lm = new Texture();
    lm->SetDimentions(32, 32, 3);
    lm->Fill(glm::vec4(255, 255, 255, 255));
    lm->UploadToGl();
    lightmaps.push_back(lm);

    this->_bodyparts.Allocate(this->_header->numbodyparts);
    for (int i = 0; i < this->_header->numbodyparts; i++)
    {
        tMDLBodyParts& part = this->_bodyPartData[i];
        tBodypart& b = this->_bodyparts[i];
        b.models.Allocate(part.nummodels);

        Array<tMDLModel> models(part.nummodels, (tMDLModel*)((byte*)this->_header + part.modelindex));
        for (int j = 0; j < models.count; j++)
        {
            tMDLModel& model = models[j];
            tModel& m = b.models[j];
            m.firstFace = faces.size();
            m.faceCount = model.nummesh;
            m.faces.Allocate(model.nummesh);

            Array<glm::vec3> vertices(model.numverts, (glm::vec3*)((byte*)this->_header + model.vertindex));
            Array<byte> vertexBones(model.numverts, (byte*)this->_header + model.vertinfoindex);
            Array<glm::vec3> normals(model.numnorms, (glm::vec3*)((byte*)this->_header + model.normindex));

            Array<tMDLMesh> meshes(model.nummesh, (tMDLMesh*)((byte*)this->_header + model.meshindex));
            for (int k = 0; k < model.nummesh; k++)
            {
                tMDLMesh& mesh = meshes[k];
                tFace& e = m.faces[k];

                e.firstVertex = _vertices.size();
                e.lightmap = 0;
                e.texture = this->_textureData[this->_skinRefData[mesh.skinref]].index;

                short* ptricmds = (short *)((byte*)this->_header + mesh.triindex);

                s = 1.0f / float(this->_textureData[this->_skinRefData[mesh.skinref]].width);
                t = 1.0f / float(this->_textureData[this->_skinRefData[mesh.skinref]].height);

                while (vertnum = *(ptricmds++))
                {
                    tVertex first, prev;
                    for(int l = 0; l < abs(vertnum); l++, ptricmds += 4)
                    {
                        tVertex v;

                        v.position = vertices[ptricmds[0]];
                        v.normal = normals[ptricmds[0]];
                        v.texcoords[0] = v.texcoords[1] = glm::vec2(ptricmds[2] * s, ptricmds[3] * t);
                        v.bone = int(vertexBones[ptricmds[0]]);

                        if (vertnum < 0)    // TRIANGLE_FAN
                        {
                            if (l == 0)
                                first = v;
                            else if (l == 1)
                                prev = v;
                            else
                            {
                                _vertices.push_back(first);
                                _vertices.push_back(prev);
                                _vertices.push_back(v);

                                // laatste statement
                                prev = v;
                            }
                        }
                        else                // TRIANGLE_STRIP
                        {
                            if (l == 0)
                                first = v;
                            else if (l == 1)
                                prev = v;
                            else
                            {
                                if (l & 1)
                                {
                                    _vertices.push_back(first);
                                    _vertices.push_back(v);
                                    _vertices.push_back(prev);
                                }
                                else
                                {
                                    _vertices.push_back(first);
                                    _vertices.push_back(prev);
                                    _vertices.push_back(v);
                                }

                                // laatste statement
                                first = prev;
                                prev = v;
                            }
                        }
                    }
                }
                e.vertexCount = _vertices.size() - e.firstVertex;
                faces.push_back(e);
            }
        }
    }
}

int MdlAsset::SequenceCount() const
{
    return this->_header->numseq;
}

int MdlAsset::BodypartCount() const
{
    return this->_header->numbodyparts;
}

tMDLAnimation* MdlAsset::GetAnimation(tMDLSequenceDescription *pseqdesc)
{
    if (pseqdesc->seqgroup == 0)
    {
        tMDLSequenceGroup& pseqgroup = this->_sequenceGroupData[pseqdesc->seqgroup];
        return (tMDLAnimation*)((byte*)this->_header + pseqgroup.unused2 + pseqdesc->animindex);
    }

    return (tMDLAnimation*)((byte*)this->_animationHeaders[pseqdesc->seqgroup] + pseqdesc->animindex);
}

void MdlAsset::RenderFaces(const std::set<unsigned short>& visibleFaces)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    this->_va.RenderFaces(visibleFaces, GL_TRIANGLES);
}
