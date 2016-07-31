cmake_minimum_required(VERSION 2.6)

project(viewer)

if(CMAKE_COMPILER_IS_GNUCXX)
   SET(ENABLE_CXX11 "-std=c++11")

   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
endif()

find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/common)

set(SRC_APP
    common/camera.cpp
    common/font.cpp
    common/main_sdl.cpp
    common/settings.cpp
    common/log.cpp
    filesystem.cpp
    program.cpp
	viewerhud.cpp
)

add_executable(viewer ${SRC_APP})

target_link_libraries(viewer
    game-assets
    ${SDL2_LIBRARY} Ole32 Oleaut32 Imm32 Winmm Version
    ${OPENGL_LIBRARIES}
)
