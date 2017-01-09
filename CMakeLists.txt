cmake_minimum_required(VERSION 2.8)

project(game-assets)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

option(BUILD_TESTS "Enabled building tests" OFF)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/include")

set(HDR_APP
    include/valve/hltypes.h
    include/valve/hl1worldsize.h

    include/valve/hl1bsptypes.h
    include/valve/hl1bspasset.h
    include/valve/hl1bspinstance.h
    include/valve/hl1wadasset.h

    include/valve/hl2bsptypes.h
    include/valve/hl2bspasset.h
    include/valve/hl2bspinstance.h

    include/valve/hl1mapasset.h
    include/valve/hl1mapinstance.h
    include/valve/hl1mapshader.h

    include/valve/hl1mdltypes.h
    include/valve/hl1mdlasset.h
    include/valve/hl1mdlinstance.h

    include/valve/hl1sprtypes.h
    include/valve/hl1sprasset.h
    include/valve/hl1sprinstance.h

    include/valve/hltexture.h
    include/valve/hltokenizer.h
    include/stb_rect_pack.h
    )

set(SRC_APP
    src/valve/hltypes.cpp

    src/valve/hl1bspasset.cpp
    src/valve/hl1bspinstance.cpp
    src/valve/hl1wadasset.cpp

    src/valve/hl2bspasset.cpp
    src/valve/hl2bspinstance.cpp

    src/valve/hl1mapasset.cpp
    src/valve/hl1mapinstance.cpp
    src/valve/hl1mapshader.cpp

    src/valve/hl1mdlasset.cpp
    src/valve/hl1mdlinstance.cpp

    src/valve/hl1sprasset.cpp
    src/valve/hl1sprinstance.cpp

    src/valve/hltexture.cpp
    src/valve/hltokenizer.cpp
)

add_library(game-assets STATIC ${SRC_APP} ${HDR_APP})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
Endif(BUILD_TESTS)
