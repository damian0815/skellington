cmake_minimum_required(VERSION 3.5)
project(skellington)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror-return-type -std=c++11")

add_custom_target(CopyData ditto "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_BINARY_DIR}/data")

find_library(OPENGL OpenGL)
find_library(GLFW glfw3)
set(TEST_LIBS ${OPENGL} ${GLFW} fmt assimp skellington)

set(LIB_SOURCE_FILES skellington/Skeleton.cpp skellington/Skeleton.h skellington/Joint.cpp skellington/Joint.h skellington/Mesh.cpp skellington/Mesh.h skellington/Pose.cpp skellington/Pose.h skellington/MeshSkeletonAnimator.cpp skellington/MeshSkeletonAnimator.h skellington/Transform.cpp skellington/Transform.h skellington/AssimpLoader.cpp skellington/AssimpLoader.h skellington/OptimizedCoRComputer.cpp skellington/OptimizedCoRComputer.h)
add_library(skellington ${LIB_SOURCE_FILES})

function(make_test_app NAME CPPFILE)
	add_executable(${NAME} ${CPPFILE} test/OpenGLTestUtils.h test/SkellingtonTestUtils.h test/simpletext.h)
	target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${NAME} ${TEST_LIBS})
	if(${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_BINARY_DIR})
		add_dependencies(${NAME} CopyData)
	endif(${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_BINARY_DIR})
endfunction()

make_test_app(testLoad test/testLoad.cpp)
make_test_app(testAnimateLinear test/testAnimateLinear.cpp)
make_test_app(testOptimizedCentersOfRotation test/testOptimizedCentersOfRotation.cpp)
make_test_app(testFlatSimple test/testFlatSimple.cpp)

file(GLOB DATA_FILES "data/*")
#make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
file(COPY ${DATA_FILES} DESTINATION ${CMAKE_BINARY_DIR}/data)
