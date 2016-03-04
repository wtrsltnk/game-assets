cmake_minimum_required(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCXX)
   SET(ENABLE_CXX11 "-std=c++11")

   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_LESS 4.7)
      SET(ENABLE_CXX11 "-std=c++0x")
   endif()

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
endif()

include_directories("${CMAKE_SOURCE_DIR}/tests/include")

set(SRC_APP
    test_framework.cpp
    mdl_tests.cpp
)

add_executable(tests ${SRC_APP})

target_link_libraries(tests
    game-assets
)

add_test(DefaultTests tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
