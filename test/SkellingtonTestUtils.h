//
// Created by Damian Stewart on 16/10/16.
//

#ifndef SKELLINGTON_SKELLINGTONTESTUTILS_H
#define SKELLINGTON_SKELLINGTONTESTUTILS_H

#include "OpenGLTestUtils.h"
#include "skellington/Skeleton.h"

using namespace skellington;

inline void DrawSkeleton(Skeleton* skeleton)
{
    vec3 pos;
    for (const auto& joint: skeleton->GetJoints()) {
        const auto& t = joint.GetTransform();

    }
}

#endif //SKELLINGTON_SKELLINGTONTESTUTILS_H
