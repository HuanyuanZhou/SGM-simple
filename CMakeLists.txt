cmake_minimum_required (VERSION 2.6)
project (SGM)

# set(CMAKE_BUILD_TYPE debug)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()
MESSAGE("Build Type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
)

add_library(
    ${PROJECT_NAME} SHARED
    src/SemiGlobalMatching.cpp
    src/sgmUtils.cpp
)

add_executable (simpleTest example/simpleTest.cpp)
target_link_libraries(simpleTest
    ${OpenCV_LIBS}
    ${PROJECT_NAME}
)