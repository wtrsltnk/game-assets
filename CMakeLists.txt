cmake_minimum_required(VERSION 2.8)

project(game-assets)

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
    include/hl1types.h
    include/hl1worldsize.h
    include/hl1shader.h

    include/hl1bsptypes.h
    include/hl1bspasset.h
    include/hl1bspinstance.h
    include/hl1wadasset.h

    include/hl2bsptypes.h
    include/hl2bspasset.h
    include/hl2bspinstance.h
    include/hl2bspshader.h

    include/hl1mapasset.h
    include/hl1mapinstance.h
    include/hl1mapshader.h

    include/hl1mdltypes.h
    include/hl1mdlasset.h
    include/hl1mdlinstance.h
    include/hl1mdlshader.h

    include/hl1sprtypes.h
    include/hl1sprasset.h
    include/hl1sprinstance.h
    include/hl1sprshader.h

    include/texture.h
    include/tokenizer.h
    include/stb_rect_pack.h
    )

set(SRC_APP
    src/hl1shader.cpp
    src/hl1types.cpp

    src/hl1bspasset.cpp
    src/hl1bspinstance.cpp
    src/hl1wadasset.cpp

    src/hl2bspasset.cpp
    src/hl2bspinstance.cpp
    src/hl2bspshader.cpp

    src/hl1mapasset.cpp
    src/hl1mapinstance.cpp
    src/hl1mapshader.cpp

    src/hl1mdlasset.cpp
    src/hl1mdlinstance.cpp
    src/hl1mdlshader.cpp

    src/hl1sprasset.cpp
    src/hl1sprinstance.cpp
    src/hl1sprshader.cpp

    src/texture.cpp
    src/tokenizer.cpp
)

add_library(game-assets STATIC ${SRC_APP} ${HDR_APP})

if(BUILD_EXAMPLES)
    add_subdirectory(examples/viewer)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
Endif(BUILD_TESTS)
