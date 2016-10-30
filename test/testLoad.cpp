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

    for (const auto& j: skeleton->GetJoints()) {
        fmt::print("skeleton joint {0} has transform {1} (global {2})\n", j.GetName(), j.GetParentRelativeRestTransform().GetMatrix(), skeleton->GetAbsoluteTransform(j.GetName()).GetMatrix());
    }

    const vec2 WINDOW_SIZE(500,500);
    const vec3 CAM_POS(0, -1, -5);
    const float ROTATE_SPEED = 0.5;
    OpenGLRotatingMainLoop(WINDOW_SIZE, CAM_POS, ROTATE_SPEED, [skeleton]() {
        glRotatef(-90, 1, 0, 0);
        DrawSkeleton(skeleton);

    });

    return 0;
}