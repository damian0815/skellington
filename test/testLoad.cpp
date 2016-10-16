#include <iostream>

#include "../skellington/Skeleton.h"
#include "../skellington/AssimpLoader.h"

#include "OpenGLTestUtils.h"
#include "SkellingtonTestUtils.h"

using namespace skellington;

int main()
{
    string path = "data/ArmatureStraight.dae";

    Mesh* mesh;
    Skeleton* skeleton;

    bool loaded = AssimpLoader::Load(path, &mesh, &skeleton);
    if (!loaded) {
        return 1;
    }

    const float CAM_DISTANCE = 5;
    const float ROTATE_SPEED = 0.5;
    OpenGLRotatingMainLoop(CAM_DISTANCE, ROTATE_SPEED, [skeleton]() {

        DrawSkeleton(skeleton);

    });

    return 0;
}