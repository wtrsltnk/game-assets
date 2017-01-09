#ifndef _HL1MDLINSTANCE_H_
#define _HL1MDLINSTANCE_H_

#include "hl1mdlasset.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace valve
{

namespace hl1
{

class MdlInstance : public AssetInstance
{
public:
    MdlInstance(MdlAsset* asset);
    virtual ~MdlInstance();

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

    const MdlAsset* Asset() const { return this->_asset; }

private:
    void SetupBones();
    void CalcBoneAdj();
    void CalcRotations(glm::vec3 pos[], glm::quat q[], tMDLSequenceDescription *pseqdesc, tMDLAnimation *panim);
    void CalcBoneQuaternion(int frame, float s, tMDLBone *pbone, tMDLAnimation *panim, glm::quat& q);
    void CalcBonePosition(int frame, float s, tMDLBone *pbone, tMDLAnimation *panim, glm::vec3& pos);
    void SlerpBones(glm::quat q1[], glm::vec3 pos1[], glm::quat q2[], glm::vec3 pos2[], float s);

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

    MdlAsset* _asset;
    Shader* _shader;
};

}

}

#endif // _HL1MDLINSTANCE_H_
