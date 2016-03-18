#include <test_framework.h>
#include <hl2bspasset.h>
#include <../examples/viewer/filesystem.h>
#include <iostream>

using namespace test;

TEST(Hl2Test)
{
    Hl2BspAsset* asset = new Hl2BspAsset(FileSystem::LoadFileData);
    bool loaded =  asset->Load("C:\\Misc\\Programs\\de_cpl_fire.bsp");

    Assert::IsTrue(loaded,
                   "Loaded succeeded");

    for (int f = 0; f < asset->_faceData.count; f++)
    {
        int edgeIndex = asset->_faceData[f].firstEdge + asset->_faceData[f].edgeCount;
        std::cout << edgeIndex << " < " << asset->_edgeData.count << std::endl;
        Assert::IsTrue(edgeIndex < asset->_edgeData.count,
                    "Face edge indices points to non excisting edges");
    }

    for (int d = 0; d < asset->_displacementInfoData.count; d++)
    {
        unsigned int faceIndex = asset->_displacementInfoData[d].MapFace;

        Assert::IsTrue(faceIndex < asset->_faceData.count),
                    "Displacement info points to non excisting face";

        Assert::IsTrue(asset->_faceData[faceIndex].dispinfo == d,
                    "The face should point to the current displacement info");
    }
}

void AddTests()
{
    ADD_TEST(Hl2Test, "Test HL2 BSPfile loading");
}
