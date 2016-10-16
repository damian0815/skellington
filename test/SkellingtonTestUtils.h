//
// Created by Damian Stewart on 16/10/16.
//

#ifndef SKELLINGTON_SKELLINGTONTESTUTILS_H
#define SKELLINGTON_SKELLINGTONTESTUTILS_H

#include "OpenGLTestUtils.h"
#include "skellington/Skeleton.h"
#include <fmt/ostream.h>

using namespace skellington;

inline void DrawSkeleton(Skeleton* skeleton)
{
    vec3 pos;



    for (const auto& joint: skeleton->GetJoints()) {

        const Transform t = skeleton->GetAbsoluteTransform(joint);

        auto origin = t*vec3(0,0,0);
        auto forward = t*(vec3(0,0,1)*0.5f);
        DrawLine(origin, forward, vec4(0.3, 1.0, 0.3, 0.8));

        if (skeleton->JointHasParent(joint.GetName())) {
            const auto& parent = skeleton->GetParentJoint(joint.GetName());
            const Transform parentT = skeleton->GetAbsoluteTransform(parent);

            auto v1 = t*vec3(0,0,0);
            auto v2 = parentT*vec3(0,0,0);
            DrawLine(v1, v2, vec4(1.0, 0.8, 0.8, 0.8));
        }




    }
}

#endif //SKELLINGTON_SKELLINGTONTESTUTILS_H
