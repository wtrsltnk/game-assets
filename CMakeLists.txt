cmake_minimum_required(VERSION 2.8)

project(game-assets)

set(WIN_CPP_PREFIX_PATH $ENV{WIN_CPP_PREFIX_PATH})
if (NOT WIN_CPP_PREFIX_PATH)
    message("Cannot find WIN_CPP_PREFIX_PATH")
endif()
if (NOT EXISTS "${WIN_CPP_PREFIX_PATH}")
    message("${WIN_CPP_PREFIX_PATH} does not exist")
else()
    set(CMAKE_PREFIX_PATH "${WIN_CPP_PREFIX_PATH}")
    set(CMAKE_MODULE_PATH "${WIN_CPP_PREFIX_PATH}/cmake-modules")
    set(ZLIB_ROOT ${WIN_CPP_PREFIX_PATH})
endif()

option(BUILD_EXAMPLES "Enable building examples" ON)
option(BUILD_TESTS "Enabled building tests" OFF)

if(CMAKE_COMPILER_IS_GNUCXX)
   SET(ENABLE_CXX11 "-std=c++11")

   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake-modules/"
)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/include")

set(HDR_APP
    include/valve/hl1types.h
    include/valve/hl1worldsize.h
    include/valve/hl1shader.h

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
    src/valve/hl1shader.cpp
    src/valve/hl1types.cpp

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

if(BUILD_EXAMPLES)
    add_subdirectory(examples/viewer)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
Endif(BUILD_TESTS)
