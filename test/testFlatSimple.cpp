//
// Created by Damian Stewart on 31/10/2016.
//

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
    string path = "data/ArmatureFlatSimple.dae";

    Mesh* dummyMesh;
    Skeleton* skeleton;

    bool loaded = AssimpLoader::Load(path, &dummyMesh, &skeleton);
    if (!loaded) {
        return 1;
    }

    Mesh* mesh;
    {
        std::vector<vec3> vertices;
        std::vector<int> triangles;
        const float y = 0;
        for (int i = 0; i < 6; i++) {
            float z = i * 0.4f;
            for (int j = 0; j < 2; j++) {
                float x = 0.6f * j - 0.3f;
                vertices.push_back(vec3(x, y, z));
            }
        }

        for (int i=0; i<5; i++) {
            int bl = i*2;
            int br = bl + 1;
            int tl = (i+1)*2;
            int tr = tl + 1;
            triangles.push_back(bl);
            triangles.push_back(tl);
            triangles.push_back(br);

            triangles.push_back(tl);
            triangles.push_back(tr);
            triangles.push_back(br);
        }

        mesh = new Mesh(vertices, triangles);

        vector<Mesh::WeightedVectorIndex> groupAWeights;
        vector<Mesh::WeightedVectorIndex> groupBWeights;
        groupAWeights.push_back({0, 1});
        groupAWeights.push_back({1, 1});
        groupAWeights.push_back({2, 1});
        groupAWeights.push_back({3, 1});
        groupAWeights.push_back({4, 0.667f});
        groupAWeights.push_back({5, 0.667f});
        groupAWeights.push_back({6, 0.333f});
        groupAWeights.push_back({7, 0.333f});
        for (const auto& wv: groupAWeights) {
            groupBWeights.push_back({11 - wv.mVertexIndex, wv.mWeight});
        }

        mesh->AddVertexGroup("A", groupAWeights);
        mesh->AddVertexGroup("B", groupBWeights);
    }

    auto optimizedCoRs = OptimizedCoRComputer::ComputeOptimizedCoRs(mesh, skeleton, 0, 0);


    Pose pose(skeleton);


    const vec2 WINDOW_SIZE(1200,600);
    const vec3 CAM_POS(0, -0.5, -4);
    const float ROTATE_SPEED = 0.0;
    float alpha = 0;
    const float ALPHA_SPEED = 0.005f;

    OpenGLRotatingMainLoop(WINDOW_SIZE, CAM_POS, ROTATE_SPEED, [&]() {

        alpha += ALPHA_SPEED;
        const auto curlAngle = (curlActive ? (float(M_PI * 0.63f) * sinf(alpha)) : 0);
        auto curlZ = Transform::MakeRotation(curlAngle, vec3(0, 0, 1));

        pose.SetOffsetTransform("B", curlZ);

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


        auto posedMesh_Linear = MeshSkeletonAnimator::ApplyPose_Linear(pose, mesh);

        vector<vec3> optimizedCoRsPosed;
        auto posedMesh_OptimizedCoR = MeshSkeletonAnimator::ApplyPose_OptimizedCoR(pose, mesh, optimizedCoRs, optimizedCoRsPosed);

        glColor4f(1,1,1,0.2f);
        glTranslatef(-1.5f, 0, 0);
        posedMesh_Linear.DrawWireframe();

        glTranslatef(3, 0, 0);
        posedMesh_OptimizedCoR.DrawWireframe();

        for (const auto it: optimizedCoRs) {
            auto v1 = posedMesh_OptimizedCoR.GetVertices()[it.first];
            auto v2 = optimizedCoRsPosed.at(it.first);
            DrawLine(v1, v2, optimizedCoRColor);
        }

        glPopMatrix();

    }, &KeyFunction);

    return 0;
}

