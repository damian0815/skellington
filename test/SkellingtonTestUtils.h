//
// Created by Damian Stewart on 16/10/16.
//

#ifndef SKELLINGTON_SKELLINGTONTESTUTILS_H
#define SKELLINGTON_SKELLINGTONTESTUTILS_H

#include "OpenGLTestUtils.h"
#include <skellington/Skeleton.h>
#include <skellington/Pose.h>
#include <fmt/ostream.h>
#include <glm/gtc/quaternion.hpp>

using namespace skellington;

static const vec3 AXIS_X(1,0,0);
static const vec3 AXIS_Y(0,1,0);
static const vec3 AXIS_Z(0,0,1);

inline void DrawTransform(const Transform& transform, float length=0.5f)
{
    auto pos = transform*vec3(0,0,0);

    auto xAxis = transform*(AXIS_X*length);
    auto yAxis = transform*(AXIS_Y*length);
    auto zAxis = transform*(AXIS_Z*length);
    DrawLine(pos, xAxis, COLOR_RED);
    DrawLine(pos, yAxis, COLOR_GREEN);
    DrawLine(pos, zAxis, COLOR_BLUE);
}

inline void DrawSkeleton(Skeleton* skeleton)
{
    for (const auto& joint: skeleton->GetJoints()) {
        const Transform t = skeleton->GetAbsoluteTransform(joint.GetName());
        DrawTransform(t);

        if (skeleton->JointHasParent(joint.GetName())) {
            const auto& parent = skeleton->GetParentJoint(joint.GetName());
            const Transform parentT = skeleton->GetAbsoluteTransform(parent.GetName());

            auto v1 = t*vec3(0,0,0);
            auto v2 = parentT*vec3(0,0,0);
            DrawLine(v1, v2, vec4(1.0, 0.8, 0.8, 0.8));
        }
    }
}

void DrawPosedSkeleton(Skeleton *skeleton, const Pose &pose)
{
    for (const auto& joint: skeleton->GetJoints()) {
        const Transform t = pose.GetAbsoluteTransform(joint.GetName());
        DrawTransform(t);

        if (skeleton->JointHasParent(joint.GetName())) {
            auto t = pose.GetAbsoluteTransform(joint.GetName());
            auto parentT = pose.GetAbsoluteTransform(skeleton->GetParentJointName(joint.GetName()));

            auto v1 = t*vec3(0,0,0);
            auto v2 = parentT*vec3(0,0,0);
            DrawLine(v1, v2, vec4(1.0, 0.8, 0.8, 0.8));
        }
    }
}

#endif //SKELLINGTON_SKELLINGTONTESTUTILS_H
