cmake_minimum_required(VERSION 2.6)

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

include_directories(${CMAKE_SOURCE_DIR}/common)

include_directories("${CMAKE_SOURCE_DIR}/tests/include")



add_executable(mdl-tests
    test_framework.cpp
    mdl_tests.cpp
    )

target_link_libraries(mdl-tests
    game-assets
    ${SDL2_LIBRARY} Ole32 Oleaut32 Imm32 Winmm Version
    ${OPENGL_LIBRARIES}
)

add_test(DefaultTests mdl-tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )



add_executable(hl1-tests
    test_framework.cpp
    ../examples/viewer/common/glextl.cpp
    ../examples/viewer/filesystem.cpp
    hl1_tests.cpp
    )

target_link_libraries(hl1-tests
    game-assets
    ${SDL2_LIBRARY} Ole32 Oleaut32 Imm32 Winmm Version
    ${OPENGL_LIBRARIES}
)

add_test(DefaultTests hl1-tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )



add_executable(hl2-tests
    test_framework.cpp
    ../examples/viewer/common/glextl.cpp
    ../examples/viewer/filesystem.cpp
    hl2_tests.cpp
    )

target_link_libraries(hl2-tests
    game-assets
    ${SDL2_LIBRARY} Ole32 Oleaut32 Imm32 Winmm Version
    ${OPENGL_LIBRARIES}
)

add_test(DefaultTests hl2-tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
