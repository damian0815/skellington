#include <iostream>

#include <skellington/Pose.h>
#include <skellington/AssimpLoader.h>
#include <skellington/MeshSkeletonAnimator.h>

#include "OpenGLTestUtils.h"
#include "SkellingtonTestUtils.h"

void DrawPosedSkeleton(Skeleton *skeleton, const Pose &pose);

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

    Pose pose(skeleton);

    const vec3 CAM_POS(0, -1, -5);
    const float ROTATE_SPEED = 0.1;
    float alpha = 0;
    const float ALPHA_SPEED = 0.01f;

    OpenGLRotatingMainLoop(CAM_POS, ROTATE_SPEED, [&]() {

        alpha += ALPHA_SPEED;
        Transform curl = Transform::MakeRotation(0.3f*(1.0f + sinf(alpha)), vec3(1,0,0));
        pose.SetOffsetTransform("A", curl);
        pose.SetOffsetTransform("B", curl);
        pose.SetOffsetTransform("C", curl);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPushMatrix();

        glRotatef(-90, 1, 0, 0);
        DrawSkeleton(skeleton);
        DrawPosedSkeleton(skeleton, pose);

        glColor4f(1,1,1,0.2f);
        mesh->DrawWireframe();

        auto posedMesh = MeshSkeletonAnimator::ApplyPose(pose, mesh);
        posedMesh.DrawWireframe();

        glPopMatrix();

    });

    return 0;
}


