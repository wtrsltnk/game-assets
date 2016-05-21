#include "hl1mdlinstance.h"
#include "hl1mdlasset.h"
#include "hl1shader.h"

#include <GL/glextl.h>
#include <glm/gtc/matrix_transform.hpp>

Hl1MdlInstance::Hl1MdlInstance(Hl1MdlAsset* asset)
    : _asset(asset), _shader(nullptr), _speed(1.0f)
{
    this->_shader = new Hl1Shader();
    this->_shader->BuildProgram();

    this->SetSequence(0, true);

    this->SetController(0, 0.0f);
    this->SetController(1, 0.0f);
    this->SetController(2, 0.0f);
    this->SetController(3, 0.0f);
    this->SetMouth(0);

    for (int bi = 0; bi < MAX_MDL_BODYPARTS; bi++)
        this->_visibleModels[bi] = 0;

    this->SetupBones();
}

Hl1MdlInstance::~Hl1MdlInstance()
{
    this->Unload();
}

void Hl1MdlInstance::Update(float dt)
{
    if (this->_asset != 0)
    {
        HL1::tMDLSequenceDescription& pseqdesc = this->_asset->_sequenceData[this->_sequence];

        if (dt > 0.1f)
            dt = 0.1f;

        if (this->_frame + (dt * pseqdesc.fps * this->_speed) < (pseqdesc.numframes - 1) || this->_repeat)
        {
            this->_frame += dt * pseqdesc.fps * this->_speed;

            if (pseqdesc.numframes <= 1)
                this->_frame = 0;
            else // wrap
                this->_frame -= (int)(this->_frame / (pseqdesc.numframes - 1)) * (pseqdesc.numframes - 1);
        }

        this->SetupBones();
    }
}

void Hl1MdlInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    if (this->_asset != 0)
    {
        if (_asset->_bodyparts == 0)
            return;

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        this->_shader->UseProgram();
        this->_shader->BindBones(this->_bonetransform, MAX_MDL_BONES);
        this->_shader->SetProjectionMatrix(proj);
        this->_shader->SetViewMatrix(view);

//        this->_shader->SetNormalMatrix(glm::transpose(glm::inverse(view)));

        std::set<unsigned short> indices;
        for (int bi = 0; bi < _asset->_bodyparts.count; bi++)
        {
            Hl1MdlAsset::tBodypart& b = _asset->_bodyparts[bi];
            for (int mi = 0; mi < b.models.count; mi++)
            {
                Hl1MdlAsset::tModel& m = b.models[_visibleModels[bi]];
                for (int e = 0; e < m.faces.count; e++)
                {
                    indices.insert(m.firstFace + e);
                }
            }
        }
        this->_asset->RenderFaces(indices);

        this->_shader->UnbindBones();
    }
}

void Hl1MdlInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}

void Hl1MdlInstance::CalcBoneAdj()
{
    if (this->_asset != 0)
    {
        float value;
        Array<HL1::tMDLBoneController> pbonecontroller = this->_asset->_boneControllerData;

        for (int i = 0; i < pbonecontroller.count; i++)
        {
            if (pbonecontroller[i].index <= 3)
            {
                // check for 360% wrapping
                if (pbonecontroller[i].type & HL1_MDL_RLOOP)
                    value = this->_controller[pbonecontroller[i].index] * (360.0f / 256.0f) + pbonecontroller[i].start;
                else
                {
                    value = this->_controller[pbonecontroller[i].index] / 255.0f;
                    if (value < 0) value = 0;
                    if (value > 1.0f) value = 1.0f;
                    value = (1.0f - value) * pbonecontroller[i].start + value * pbonecontroller[i].end;
                }
            }
            else
            {
                value = this->_mouth / 64.0f;
                if (value > 1.0f) value = 1.0f;
                value = (1.0f - value) * pbonecontroller[i].start + value * pbonecontroller[i].end;
            }
            switch(pbonecontroller[i].type & HL1_MDL_TYPES)
            {
            case HL1_MDL_XR:
            case HL1_MDL_YR:
            case HL1_MDL_ZR:
                this->_adj[i] = value * (glm::pi<float>() / 180.0f);
                break;
            case HL1_MDL_X:
            case HL1_MDL_Y:
            case HL1_MDL_Z:
                this->_adj[i] = value;
                break;
            }
        }
    }
}

void Hl1MdlInstance::CalcBoneQuaternion( int frame, float s, HL1::tMDLBone *pbone, HL1::tMDLAnimation *panim, glm::quat& q )
{
    int j, k;
    glm::vec3 angle1, angle2;
    HL1::tMDLAnimationValue* panimvalue;

    for (j = 0; j < 3; j++)
    {
        if (panim->offset[j+3] == 0)
        {
            angle2[j] = angle1[j] = pbone->value[j+3]; // default;
        }
        else
        {
            panimvalue = (HL1::tMDLAnimationValue *)((byte *)panim + panim->offset[j+3]);
            k = frame;
            while (panimvalue->num.total <= k)
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
            }
            // Bah, missing blend!
            if (panimvalue->num.valid > k)
            {
                angle1[j] = panimvalue[k+1].value;

                if (panimvalue->num.valid > k + 1)
                {
                    angle2[j] = panimvalue[k+2].value;
                }
                else
                {
                    if (panimvalue->num.total > k + 1)
                        angle2[j] = angle1[j];
                    else
                        angle2[j] = panimvalue[panimvalue->num.valid+2].value;
                }
            }
            else
            {
                angle1[j] = panimvalue[panimvalue->num.valid].value;
                if (panimvalue->num.total > k + 1)
                {
                    angle2[j] = angle1[j];
                }
                else
                {
                    angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
                }
            }
            angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
            angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
        }

        if (pbone->bonecontroller[j+3] != -1)
        {
            angle1[j] += this->_adj[pbone->bonecontroller[j+3]];
            angle2[j] += this->_adj[pbone->bonecontroller[j+3]];
        }
    }

    if (angle1 != angle2)
        q = glm::slerp(glm::quat(angle1), glm::quat(angle2), s);
    else
        q = glm::quat(angle1);
}

void Hl1MdlInstance::CalcBonePosition( int frame, float s, HL1::tMDLBone *pbone, HL1::tMDLAnimation *panim, glm::vec3& pos )
{
    int j, k;
    HL1::tMDLAnimationValue	*panimvalue;

    for (j = 0; j < 3; j++)
    {
        pos[j] = pbone->value[j]; // default;
        if (panim->offset[j] != 0)
        {
            panimvalue = (HL1::tMDLAnimationValue *)((byte *)panim + panim->offset[j]);

            k = frame;
            // find span of values that includes the frame we want
            while (panimvalue->num.total <= k)
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
            }
            // if we're inside the span
            if (panimvalue->num.valid > k)
            {
                // and there's more data in the span
                if (panimvalue->num.valid > k + 1)
                    pos[j] += (panimvalue[k+1].value * (1.0f - s) + s * panimvalue[k+2].value) * pbone->scale[j];
                else
                    pos[j] += panimvalue[k+1].value * pbone->scale[j];
            }
            else
            {
                // are we at the end of the repeating values section and there's another section with data?
                if (panimvalue->num.total <= k + 1)
                    pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0f - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
                else
                    pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
            }
        }
        if (pbone->bonecontroller[j] != -1)
            pos[j] += _adj[pbone->bonecontroller[j]];
    }
}

void Hl1MdlInstance::CalcRotations(glm::vec3 pos[], glm::quat q[], HL1::tMDLSequenceDescription *pseqdesc, HL1::tMDLAnimation *panim)
{
    int frame = (int)this->_frame;
    float s = (this->_frame - frame);

    // add in programatic controllers
    this->CalcBoneAdj();

    if (this->_asset != 0)
    {
        Array<HL1::tMDLBone> pbone = this->_asset->_boneData;
        for (int i = 0; i < pbone.count; i++, panim++)
        {
            this->CalcBoneQuaternion(frame, s, &pbone[i], panim, q[i]);
            this->CalcBonePosition(frame, s, &pbone[i], panim, pos[i]);
        }

        if (pseqdesc->motiontype & HL1_MDL_X)
            pos[pseqdesc->motionbone][0] = 0.0;
        if (pseqdesc->motiontype & HL1_MDL_Y)
            pos[pseqdesc->motionbone][1] = 0.0;
        if (pseqdesc->motiontype & HL1_MDL_Z)
            pos[pseqdesc->motionbone][2] = 0.0;
    }
}

void Hl1MdlInstance::SlerpBones(glm::quat q1[], glm::vec3 pos1[], glm::quat q2[], glm::vec3 pos2[], float s)
{
    if (this->_asset != 0)
    {
        int i;
        float s1;

        if (s < 0) s = 0;
        else if (s > 1.0f) s = 1.0f;

        s1 = 1.0f - s;

        for (i = 0; i < this->_asset->_boneData.count; i++)
        {
            q1[i] = glm::slerp(q1[i], q2[i], s);
            pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
            pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
            pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
        }
    }
}

void Hl1MdlInstance::ExtractBbox(glm::vec3& mins, glm::vec3& maxs)
{
    if (this->_asset != 0)
    {
        HL1::tMDLSequenceDescription& pseqdesc = this->_asset->_sequenceData[this->_sequence];

        mins = pseqdesc.bbmin;
        maxs = pseqdesc.bbmax;
    }
}

int Hl1MdlInstance::SetSequence(int iSequence, bool repeat)
{
    if (this->_asset != 0)
    {
        if (iSequence > this->_asset->_sequenceData.count)
            iSequence = 0;
        if (iSequence < 0)
            iSequence = this->_asset->_sequenceData.count - 1;

        this->_sequence = iSequence;
        this->_frame = 0;
        this->_repeat = repeat;

        return this->_sequence;
    }
    return 0;
}

void Hl1MdlInstance::GetSequenceInfo(float *pflFrameRate, float *pflGroundSpeed)
{
    if (this->_asset != 0)
    {
        HL1::tMDLSequenceDescription& pseqdesc = this->_asset->_sequenceData[this->_sequence];

        if (pseqdesc.numframes > 1)
        {
            *pflFrameRate = 256 * pseqdesc.fps / (pseqdesc.numframes - 1);
            *pflGroundSpeed = sqrt(pseqdesc.linearmovement[0]*pseqdesc.linearmovement[0]+ pseqdesc.linearmovement[1]*pseqdesc.linearmovement[1]+ pseqdesc.linearmovement[2]*pseqdesc.linearmovement[2]);
            *pflGroundSpeed = *pflGroundSpeed * pseqdesc.fps / (pseqdesc.numframes - 1);
        }
        else
        {
            *pflFrameRate = 256.0;
            *pflGroundSpeed = 0.0;
        }
    }
}

float Hl1MdlInstance::SetController( int iController, float flValue )
{
    if (this->_asset != 0)
    {
        int i;
        Array<HL1::tMDLBoneController> bonecontrollers = this->_asset->_boneControllerData;
        HL1::tMDLBoneController* pbonecontroller = &bonecontrollers[0];

        // find first controller that matches the index
        for (i = 0; i < bonecontrollers.count; i++, pbonecontroller++)
        {
            if (pbonecontroller->index == iController)
                break;
        }
        if (i >= bonecontrollers.count)
            return flValue;

        // wrap 0..360 if it's a rotational controller
        if (pbonecontroller->type & (HL1_MDL_XR | HL1_MDL_YR | HL1_MDL_ZR))
        {
            // ugly hack, invert value if end < start
            if (pbonecontroller->end < pbonecontroller->start)
                flValue = -flValue;

            // does the controller not wrap?
            if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
            {
                if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0f) + 180)
                    flValue = flValue - 360;
                if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0f) - 180)
                    flValue = flValue + 360;
            }
            else
            {
                if (flValue > 360)
                    flValue = flValue - int(flValue / 360.0) * 360.0f;
                else if (flValue < 0)
                    flValue = flValue + int((flValue / -360.0) + 1) * 360.0f;
            }
        }

        int setting = int(255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start));

        if (setting < 0) setting = 0;
        if (setting > 255) setting = 255;
        this->_controller[iController] = setting;

        return setting * (1.0f / 255.0f) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
    }

    return 0.0f;
}

float Hl1MdlInstance::SetMouth( float flValue )
{
    if (this->_asset != 0)
    {
        Array<HL1::tMDLBoneController> bonecontrollers = this->_asset->_boneControllerData;
        HL1::tMDLBoneController* pbonecontroller = &bonecontrollers[0];

        // find first controller that matches the mouth
        for (int i = 0; i < bonecontrollers.count; i++, pbonecontroller++)
        {
            if (pbonecontroller->index == 4)
                break;
        }

        // wrap 0..360 if it's a rotational controller
        if (pbonecontroller->type & (HL1_MDL_XR | HL1_MDL_YR | HL1_MDL_ZR))
        {
            // ugly hack, invert value if end < start
            if (pbonecontroller->end < pbonecontroller->start)
                flValue = -flValue;

            // does the controller not wrap?
            if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
            {
                if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0f) + 180)
                    flValue = flValue - 360;
                if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0f) - 180)
                    flValue = flValue + 360;
            }
            else
            {
                if (flValue > 360)
                    flValue = flValue - (int)(flValue / 360.0) * 360.0f;
                else if (flValue < 0)
                    flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0f;
            }
        }

        int setting = int(64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start));

        if (setting < 0) setting = 0;
        if (setting > 64) setting = 64;
        this->_mouth = setting;

        return setting * (1.0f / 64.0f) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
    }

    return 0.0f;
}

float Hl1MdlInstance::SetBlending( int iBlender, float flValue )
{
    if (this->_asset != 0)
    {
        HL1::tMDLSequenceDescription& pseqdesc = this->_asset->_sequenceData[(int)this->_sequence];

        if (pseqdesc.blendtype[iBlender] == 0)
            return flValue;

        if (pseqdesc.blendtype[iBlender] & (HL1_MDL_XR | HL1_MDL_YR | HL1_MDL_ZR))
        {
            // ugly hack, invert value if end < start
            if (pseqdesc.blendend[iBlender] < pseqdesc.blendstart[iBlender])
                flValue = -flValue;

            // does the controller not wrap?
            if (pseqdesc.blendstart[iBlender] + 359.0f >= pseqdesc.blendend[iBlender])
            {
                if (flValue > ((pseqdesc.blendstart[iBlender] + pseqdesc.blendend[iBlender]) / 2.0f) + 180)
                    flValue = flValue - 360;
                if (flValue < ((pseqdesc.blendstart[iBlender] + pseqdesc.blendend[iBlender]) / 2.0f) - 180)
                    flValue = flValue + 360;
            }
        }

        int setting = int(255 * (flValue - pseqdesc.blendstart[iBlender]) / (pseqdesc.blendend[iBlender] - pseqdesc.blendstart[iBlender]));

        if (setting < 0) setting = 0;
        if (setting > 255) setting = 255;

        this->_blending[iBlender] = setting;

        return setting * (1.0f / 255.0f) * (pseqdesc.blendend[iBlender] - pseqdesc.blendstart[iBlender]) + pseqdesc.blendstart[iBlender];
    }

    return 0.0f;
}

int Hl1MdlInstance::SetVisibleBodygroupModel(int bodygroup, int model)
{
    if (this->_asset != 0)
    {
        if (bodygroup > this->_asset->_bodyparts.count)
            return -1;

        HL1::tMDLBodyParts& pbodypart = this->_asset->_bodyPartData[bodygroup];

        if (model > pbodypart.nummodels)
            return -1;

        this->_visibleModels[bodygroup] = model;

        return this->_visibleModels[bodygroup];
    }

    return -1;
}

int Hl1MdlInstance::SetSkin( int iValue )
{
    if (this->_asset != 0)
    {
        if (iValue < this->_asset->_skinFamilyData.count)
            return this->_skinnum;

        this->_skinnum = iValue;

        return iValue;
    }

    return 0;
}

float Hl1MdlInstance::SetSpeed(float speed)
{
    this->_speed = speed;

    return this->_speed;
}

void Hl1MdlInstance::SetupBones()
{
    static glm::vec3 pos[MAX_MDL_BONES];
    static glm::quat q[MAX_MDL_BONES];

    static glm::vec3 pos2[MAX_MDL_BONES];
    static glm::quat q2[MAX_MDL_BONES];
    static glm::vec3 pos3[MAX_MDL_BONES];
    static glm::quat q3[MAX_MDL_BONES];
    static glm::vec3 pos4[MAX_MDL_BONES];
    static glm::quat q4[MAX_MDL_BONES];

    if (this->_asset != 0)
    {
        if (this->_sequence >= this->_asset->_sequenceData.count)
            this->_sequence = 0;

        HL1::tMDLSequenceDescription* pseqdesc = &this->_asset->_sequenceData[this->_sequence];

        HL1::tMDLAnimation* panim = this->_asset->GetAnimation(pseqdesc);
        this->CalcRotations(pos, q, pseqdesc, panim);

        if (pseqdesc->numblends > 1)
        {
            panim += this->_asset->_boneData.count;
            this->CalcRotations(pos2, q2, pseqdesc, panim);
            float s = this->_blending[0] / 255.0f;

            this->SlerpBones(q, pos, q2, pos2, s);

            if (pseqdesc->numblends == 4)
            {
                panim += this->_asset->_boneData.count;
                this->CalcRotations(pos3, q3, pseqdesc, panim);

                panim += this->_asset->_boneData.count;
                this->CalcRotations(pos4, q4, pseqdesc, panim);

                s = this->_blending[0] / 255.0f;
                this->SlerpBones(q3, pos3, q4, pos4, s);

                s = this->_blending[1] / 255.0f;
                this->SlerpBones(q, pos, q3, pos3, s);
            }
        }

        for (int i = 0; i < this->_asset->_boneData.count; i++)
        {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), pos[i]) * glm::toMat4(q[i]);

            if (this->_asset->_boneData[i].parent == -1)
                this->_bonetransform[i] = m;
            else
                this->_bonetransform[i] = this->_bonetransform[this->_asset->_boneData[i].parent] * m;
        }
    }
}
