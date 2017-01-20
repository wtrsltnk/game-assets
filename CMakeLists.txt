cmake_minimum_required(VERSION 2.8)

project(game-assets)

option(BUILD_TESTS "Enabled building tests" OFF)

# The only dependancy: GLM
find_package(GLM REQUIRED)

set(HDR_LIBRARY
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

    include/stb/stb_rect_pack.h
    )

set(SRC_LIBRARY
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

add_library(game-assets STATIC ${SRC_LIBRARY} ${HDR_LIBRARY})

target_include_directories(game-assets
	PUBLIC "${CMAKE_SOURCE_DIR}/include"
	PRIVATE ${GLM_INCLUDE_DIR}
	)

target_compile_features(game-assets
	PRIVATE cxx_auto_type
	PRIVATE cxx_nullptr
	PRIVATE cxx_range_for
	)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_TESTS)
