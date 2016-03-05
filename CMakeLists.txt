cmake_minimum_required(VERSION 2.6)

project(game-assets)

option(BUILD_EXAMPLES "Enable building examples" ON)
option(BUILD_TESTS "Enabled building tests" ON)

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
include_directories(${GLM_INCLUDE_DIR}/..)

include_directories("${CMAKE_SOURCE_DIR}/include")

set(SRC_APP
    include/hl1types.h
    include/hl1worldsize.h
    src/hl1shader.cpp
    include/hl1shader.h

    include/hl1bsptypes.h
    src/hl1bspasset.cpp
    include/hl1bspasset.h
    src/hl1bspinstance.cpp
    include/hl1bspinstance.h
    src/hl1bspshader.cpp
    include/hl1bspshader.h
    src/hl1wadasset.cpp
    include/hl1wadasset.h

    include/hl2bsptypes.h
    src/hl2bspasset.cpp
    include/hl2bspasset.h
    src/hl2bspinstance.cpp
    include/hl2bspinstance.h
    src/hl2bspshader.cpp
    include/hl2bspshader.h

    src/hl1mapasset.cpp
    include/hl1mapasset.h
    src/hl1mapinstance.cpp
    include/hl1mapinstance.h
    src/hl1mapshader.cpp
    include/hl1mapshader.h

    include/hl1mdltypes.h
    src/hl1mdlasset.cpp
    include/hl1mdlasset.h
    src/hl1mdlinstance.cpp
    include/hl1mdlinstance.h
    src/hl1mdlshader.cpp
    include/hl1mdlshader.h

    include/hl1sprtypes.h
    src/hl1sprasset.cpp
    include/hl1sprasset.h
    src/hl1sprinstance.cpp
    include/hl1sprinstance.h
    src/hl1sprshader.cpp
    include/hl1sprshader.h

    src/texture.cpp
    include/texture.h
    src/tokenizer.cpp
    include/tokenizer.h
)

add_library(game-assets STATIC ${SRC_APP})

if(BUILD_EXAMPLES)
    add_subdirectory(examples/viewer)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
Endif(BUILD_TESTS)
