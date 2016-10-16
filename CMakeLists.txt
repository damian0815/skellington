cmake_minimum_required(VERSION 3.6)
project(skellington)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror-return-type -std=c++11")

add_custom_target(CopyData ditto "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_BINARY_DIR}/data")

find_library(OPENGL OpenGL)
find_library(GLFW glfw3)
set(TEST_LIBS ${OPENGL} ${GLFW} fmt assimp skellington)

set(LIB_SOURCE_FILES skellington/Skeleton.cpp skellington/Skeleton.h skellington/Joint.cpp skellington/Joint.h skellington/Mesh.cpp skellington/Mesh.h skellington/MeshSkeletonAnimator.cpp skellington/MeshSkeletonAnimator.h skellington/Transform.cpp skellington/Transform.h skellington/AssimpLoader.cpp skellington/AssimpLoader.h)
add_library(skellington ${LIB_SOURCE_FILES})


add_executable(testLoad test/testLoad.cpp test/OpenGLTestUtils.h test/SkellingtonTestUtils.h)
target_include_directories(testLoad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testLoad ${TEST_LIBS})
add_dependencies(testLoad CopyData)
