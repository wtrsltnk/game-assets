#include "hl1mdlasset.h"
#include "hl1mdlshader.h"
#include "hl1mdlinstance.h"

#include <sstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>

Hl1MdlAsset::Hl1MdlAsset(DataFileLocator& locator, DataFileLoader& loader)
    : Hl1Asset(locator, loader), _header(0)
{ }

Hl1MdlAsset::~Hl1MdlAsset()
{
    this->_textures.Delete();
    this->_bodyparts.Delete();
    this->_vertices.Clear();
}

bool Hl1MdlAsset::Load(const std::string &filename)
{
    Array<byte> file = this->_loader(filename);
    this->_header = (HL1::tMDLHeader*)file.data;

    // preload textures
    if (this->_header->numtextures == 0)
    {
        Array<byte> textureFile = this->_loader(filename.substr(0, filename.size()-4) + "T.mdl");
        this->_textureHeader = (HL1::tMDLHeader*)textureFile.data;
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
                this->_animationHeaders[i] = (HL1::tMDLSequenceHeader*)buffer.data;
        }
    }

    this->_textureData.Map(this->_textureHeader->numtextures, (HL1::tMDLTexture*)(file.data + this->_textureHeader->textureindex));
    this->_skinRefData.Map(this->_textureHeader->numskinref, (short*)((byte*)this->_textureHeader + this->_textureHeader->skinindex));
    this->_skinFamilyData.Map(this->_textureHeader->numskinref, (short*)((byte*)this->_textureHeader + this->_textureHeader->skinindex));
    this->_bodyPartData.Map(this->_header->numbodyparts, (HL1::tMDLBodyParts*)(file.data + this->_header->bodypartindex));
    this->_sequenceGroupData.Map(this->_header->numseqgroups, (HL1::tMDLSequenceGroup*)(file.data + this->_header->seqgroupindex));
    this->_sequenceData.Map(this->_header->numseq, (HL1::tMDLSequenceDescription*)(file.data + this->_header->seqindex));
    this->_boneControllerData.Map(this->_header->numbonecontrollers, (HL1::tMDLBoneController*)(file.data + this->_header->bonecontrollerindex));
    this->_boneData.Map(this->_header->numbones, (HL1::tMDLBone*)(file.data + this->_header->boneindex));

    this->LoadTextures();
    this->LoadBodyParts();

    this->_va.Load(_vertices);

    return true;
}

Hl1Instance* Hl1MdlAsset::CreateInstance()
{
    return new Hl1MdlInstance(this);
}

void Hl1MdlAsset::LoadTextures()
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    this->_textures.Allocate(this->_textureHeader->numtextures);
    for (int i = 0; i < this->_textureHeader->numtextures; i++)
    {
        Texture& t = this->_textures[i];
        HL1::tMDLTexture *ptexture= &this->_textureData[i];

        byte* data = ((byte*)this->_textureHeader) + ptexture->index;
        byte* pal = ((byte*)this->_textureHeader) + ptexture->width * ptexture->height + ptexture->index;

        std::stringstream ss;
        ss << ptexture->name << long(*(long*)ptexture);
        t.SetName(ss.str());

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

        t.SetData(outwidth, outheight, 4, tex);
        delete []tex;

        this->_textureData[i].index = this->_textures[i].UploadToGl();
    }
}

void Hl1MdlAsset::LoadBodyParts()
{
    float s, t;
    int vertnum;

    this->_bodyparts.Allocate(this->_header->numbodyparts);
    for (int i = 0; i < this->_header->numbodyparts; i++)
    {
        HL1::tMDLBodyParts& part = this->_bodyPartData[i];
        tBodypart& b = this->_bodyparts[i];
        b.models.Allocate(part.nummodels);

        Array<HL1::tMDLModel> models(part.nummodels, (HL1::tMDLModel*)((byte*)this->_header + part.modelindex));
        for (int j = 0; j < models.count; j++)
        {
            HL1::tMDLModel& model = models[j];
            tModel& m = b.models[j];
            m.meshes.Allocate(model.nummesh);

            Array<glm::vec3> vertices(model.numverts, (glm::vec3*)((byte*)this->_header + model.vertindex));
            Array<byte> vertexBones(model.numverts, (byte*)this->_header + model.vertinfoindex);
            Array<glm::vec3> normals(model.numnorms, (glm::vec3*)((byte*)this->_header + model.normindex));

            Array<HL1::tMDLMesh> meshes(model.nummesh, (HL1::tMDLMesh*)((byte*)this->_header + model.meshindex));
            for (int k = 0; k < meshes.count; k++)
            {
                HL1::tMDLMesh& mesh = meshes[k];
                tMesh& e = m.meshes[k];

                e.firstVertex = this->_vertices.Count();
                e.lightmap = 0;
                e.texture = this->_textureData[this->_skinRefData[mesh.skinref]].index;

                short* ptricmds = (short *)((byte*)this->_header + mesh.triindex);

                s = 1.0f / float(this->_textureData[this->_skinRefData[mesh.skinref]].width);
                t = 1.0f / float(this->_textureData[this->_skinRefData[mesh.skinref]].height);

                while (vertnum = *(ptricmds++))
                {
                    HL1::tVertex first, prev;
                    for(int l = 0; l < abs(vertnum); l++, ptricmds += 4)
                    {
                        HL1::tVertex v;

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
                                this->_vertices.Add(first);
                                this->_vertices.Add(prev);
                                this->_vertices.Add(v);

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
                                    this->_vertices.Add(first);
                                    this->_vertices.Add(v);
                                    this->_vertices.Add(prev);
                                }
                                else
                                {
                                    this->_vertices.Add(first);
                                    this->_vertices.Add(prev);
                                    _vertices.Add(v);
                                }

                                // laatste statement
                                first = prev;
                                prev = v;
                            }
                        }
                    }
                }
                e.vertexCount = this->_vertices.Count() - e.firstVertex;
            }
        }
    }
}

int Hl1MdlAsset::SequenceCount() const
{
    return this->_header->numseq;
}

int Hl1MdlAsset::BodypartCount() const
{
    return this->_header->numbodyparts;
}

HL1::tMDLAnimation* Hl1MdlAsset::GetAnimation(HL1::tMDLSequenceDescription *pseqdesc)
{
    HL1::tMDLSequenceGroup& pseqgroup = this->_sequenceGroupData[pseqdesc->seqgroup];

    if (pseqdesc->seqgroup == 0)
        return (HL1::tMDLAnimation*)((byte*)this->_header + pseqgroup.unused2 + pseqdesc->animindex);

    return (HL1::tMDLAnimation*)((byte*)this->_animationHeaders[pseqdesc->seqgroup] + pseqdesc->animindex);
}

void Hl1MdlAsset::RenderModels(int visibleModels[])
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    this->_va.Bind();

    for (int bi = 0; bi < this->_bodyparts.count; bi++)
    {
        tBodypart& b = this->_bodyparts[bi];
        for (int mi = 0; mi < b.models.count; mi++)
        {
            tModel& m = b.models[visibleModels[bi]];

            for (int e = 0; e < m.meshes.count; e++)
            {
                tMesh& mesh = m.meshes[e];

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mesh.lightmap);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.texture);

                glDrawArrays(GL_TRIANGLES, mesh.firstVertex, mesh.vertexCount);
            }
        }
    }

    this->_va.Unbind();
}
