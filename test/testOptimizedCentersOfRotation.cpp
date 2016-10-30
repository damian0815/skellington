#include <iostream>

#include <skellington/Pose.h>
#include <skellington/AssimpLoader.h>
#include <skellington/MeshSkeletonAnimator.h>
#include <skellington/OptimizedCoRComputer.h>

#include "OpenGLTestUtils.h"
#include "SkellingtonTestUtils.h"
#include "simpletext.h"

void DrawPosedSkeleton(Skeleton *skeleton, const Pose &pose);

using namespace skellington;

bool curlActive = true;
bool twistActive = true;

/*
 *  @param[in] window The window that received the event.
 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 *  @param[in] scancode The system-specific scancode of the key.
 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were held down.
*/
void KeyFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_T) {
        twistActive = !twistActive;
    }
    if (key == GLFW_KEY_C) {
        curlActive = !curlActive;
    }
}

int main()
{
    string path = "data/ArmatureStraight.dae";

    Mesh* mesh;
    Skeleton* skeleton;

    bool loaded = AssimpLoader::Load(path, &mesh, &skeleton);
    if (!loaded) {
        return 1;
    }

    auto optimizedCoRs = OptimizedCoRComputer::ComputeOptimizedCoRs(mesh, skeleton);


    Pose pose(skeleton);


    const vec2 WINDOW_SIZE(1200,600);
    const vec3 CAM_POS(0, -0.5, -4);
    const float ROTATE_SPEED = 0.0;
    float alpha = 0;
    const float ALPHA_SPEED = 0.005f;

    SimpleText* text = nullptr;

    OpenGLRotatingMainLoop(WINDOW_SIZE, CAM_POS, ROTATE_SPEED, [&]() {

        if (text == nullptr) {
            text = new SimpleText();
            text->SetTextSize(SimpleText::SIZE_32);
            //text->EnableBlending(true);
            text->SetColor(SimpleText::TEXT_COLOR, SimpleText::WHITE, SimpleText::NORMAL);
        }

        alpha += ALPHA_SPEED;
        const auto curlAngle = (curlActive ? (float(M_PI * 0.63f) * sinf(alpha)) : 0);
        auto curl = Transform::MakeRotation(curlAngle, vec3(0, 0, 1));

        auto twistAngle = (twistActive ? float(M_PI) * sinf(alpha / 1.5f) : 0);
        auto twist = Transform::MakeRotation(twistAngle, vec3(0, 1, 0));

        pose.SetOffsetTransform("A", curl);
        pose.SetOffsetTransform("B", curl*twist);
        pose.SetOffsetTransform("C", curl*twist);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(GL_NONE);
        glPushMatrix();

        glRotatef(-90, 1, 0, 0);
        glTranslatef(0, 1, 0);
        DrawPosedSkeleton(skeleton, pose);

        glColor4f(1,1,1,0.2f);
        vec4 optimizedCoRColor(1, 0.2f, 0.2f, 0.3f);
        //mesh->DrawWireframe();
        /*for (const auto it: optimizedCoRs) {
            auto v1 = mesh->GetVertices()[it.first];
            auto v2 = it.second;
            DrawLine(v1, v2, optimizedCoRColor);
        }*/

        auto posedMesh_Linear = MeshSkeletonAnimator::ApplyPose_Linear(pose, mesh);
        auto posedMesh_OptimizedCoR = MeshSkeletonAnimator::ApplyPose_OptimizedCoR(pose, mesh, optimizedCoRs);

        glColor4f(1,1,1,0.2f);
        glTranslatef(-1.5f, 0, 0);
        posedMesh_Linear.DrawWireframe();

        glTranslatef(3, 0, 0);
        posedMesh_OptimizedCoR.DrawWireframe();

        text->RenderLabel(fmt::format("Twist: {0:>4}  Curl: {1:>4}", int(twistAngle*180/M_PI), int(curlAngle*180/M_PI)).c_str(), 10, int(WINDOW_SIZE.y-40));

        text->RenderLabel("Linear Blend", int(WINDOW_SIZE.x/4)-(16*6), 10);
        text->RenderLabel("Optimized Center of Rotation", int(3*WINDOW_SIZE.x/4)-(16*14), 10);

        glPopMatrix();

    }, &KeyFunction);

    return 0;
}

