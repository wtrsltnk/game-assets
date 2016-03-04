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

    include/hl1bsptypes.h
    src/hl1bspasset.cpp
    src/hl1bspinstance.cpp
    src/hl1bspshader.cpp
    src/hl1wadasset.cpp

    include/hl2bsptypes.h
    src/hl2bspasset.cpp
    src/hl2bspinstance.cpp
    src/hl2bspshader.cpp

    src/hl1mapasset.cpp
    src/hl1mapinstance.cpp
    src/hl1mapshader.cpp

    include/hl1mdltypes.h
    src/hl1mdlasset.cpp
    src/hl1mdlinstance.cpp
    src/hl1mdlshader.cpp

    src/texture.cpp
    src/tokenizer.cpp
)

add_library(game-assets STATIC ${SRC_APP})

if(BUILD_EXAMPLES)
    add_subdirectory(examples/viewer)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
Endif(BUILD_TESTS)
