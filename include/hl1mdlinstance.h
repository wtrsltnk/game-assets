#ifndef _HL1MDLINSTANCE_H_
#define _HL1MDLINSTANCE_H_

#include "hl1mdlasset.h"
#include "hl1mdlshader.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Hl1MdlInstance : public Hl1Instance
{
    friend class Hl1MdlAsset;
    Hl1MdlInstance(Hl1MdlAsset* asset);
public:
    virtual ~Hl1MdlInstance();

    virtual void Update(float dt);
    virtual void Render(const glm::mat4& proj, const glm::mat4& view);
    void Unload();

    void ExtractBbox(glm::vec3& mins, glm::vec3& maxs);
    int SetSequence(int iSequence, bool repeat);
    void GetSequenceInfo(float *pflFrameRate, float *pflGroundSpeed);
    float SetController(int iController, float flValue);
    float SetMouth(float flValue);
    float SetBlending(int iBlender, float flValue);
    int SetVisibleBodygroupModel(int bodygroup, int model);
    int SetSkin(int iValue);
    float SetSpeed(float speed);

    const Hl1MdlAsset* Asset() const { return this->_asset; }

private:
    void SetupBones();
    void CalcBoneAdj();
    void CalcRotations(glm::vec3 pos[], glm::quat q[], HL1::tMDLSequenceDescription *pseqdesc, HL1::tMDLAnimation *panim);
    void CalcBoneQuaternion(int frame, float s, HL1::tMDLBone *pbone, HL1::tMDLAnimation *panim, glm::quat& q);
    void CalcBonePosition(int frame, float s, HL1::tMDLBone *pbone, HL1::tMDLAnimation *panim, glm::vec3& pos);
    void SlerpBones(glm::quat q1[], glm::vec3 pos1[], glm::quat q2[], glm::vec3 pos2[], float s);

    // The bones are different for each instance of mdl so we need to manage
    // the data to the uniformbuffer in the instance, not the asset
    unsigned int _bonesBuffer;

    glm::mat4 _bonetransform[MAX_MDL_BONES];
    int _visibleModels[MAX_MDL_BODYPARTS];

    float _speed;			// speed of playing animation
    int _sequence;			// sequence index
    float _frame;			// frame
    bool _repeat;			// repeat after end of sequence
    int _skinnum;			// skin group selection
    byte _controller[4];	// bone controllers
    byte _blending[2];		// animation blending
    byte _mouth;			// mouth position
    glm::quat _adj;			// FIX: non persistant, make static

    Hl1MdlAsset* _asset;
    Hl1MdlShader* _shader;
};

#endif // _HL1MDLINSTANCE_H_
