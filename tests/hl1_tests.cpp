#include <test_framework.h>
#include <hl1bspasset.h>
#include <../examples/viewer/filesystem.h>
#include <iostream>

using namespace test;

TEST(Hl1Test)
{
    Hl1BspAsset* asset = new Hl1BspAsset(FileSystem::LoadFileData);
    bool loaded =  asset->Load("C:\\Misc\\Programs\\Counter-Strike1.1\\cstrike\\maps\\cs_italy.bsp");

    Assert::IsTrue(loaded,
                   "Loaded succeeded");

    Assert::IsTrue(asset->_header->signature == HL1_BSP_SIGNATURE,
                   "Header should contain the right signature");


    Assert::IsTrue(asset->_header->lumps[HL1_BSP_PLANELUMP].size % sizeof(HL1::tBSPPlane) == 0,
                   "Wrong size for PlaneLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_VERTEXLUMP].size % sizeof(HL1::tBSPVertex) == 0,
                   "Wrong size for VertexLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_NODELUMP].size % sizeof(HL1::tBSPNode) == 0,
                   "Wrong size for NodeLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_TEXINFOLUMP].size % sizeof(HL1::tBSPTexInfo) == 0,
                   "Wrong size for TexInfoLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_FACELUMP].size % sizeof(HL1::tBSPFace) == 0,
                   "Wrong size for FaceLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_CLIPNODELUMP].size % sizeof(HL1::tBSPClipNode) == 0,
                   "Wrong size for ClipNodeLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_LEAFLUMP].size % sizeof(HL1::tBSPLeaf) == 0,
                   "Wrong size for LeafLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_EDGELUMP].size % sizeof(HL1::tBSPEdge) == 0,
                   "Wrong size for EdgeLump");

    Assert::IsTrue(asset->_header->lumps[HL1_BSP_MODELLUMP].size % sizeof(HL1::tBSPModel) == 0,
                   "Wrong size for ModelLump");


    Assert::IsTrue(asset->_entities.size() > 0,
                  "At least one entity should be parsed from entity data");
    Assert::IsTrue(asset->_entities[0].classname == "worldspawn",
                  "The first entity should always be the worldspawn entity");


    for (int f = 0; f < asset->_faceData.count; f++)
    {
        HL1::tBSPFace& in = asset->_faceData[f];

        Assert::IsTrue(in.firstEdge + in.edgeCount <= asset->_surfedgeData.count,
                    "Face edge indices points to non excisting edges");

        Assert::IsTrue(in.planeIndex < asset->_planeData.count,
                    "Face plane indices points to non excisting plane");
    }
}

void AddTests()
{
    ADD_TEST(Hl1Test, "Test HL1 BSPfile loading");
}
