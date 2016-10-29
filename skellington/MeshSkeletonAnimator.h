//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_MESHSKELETONANIMATOR_H
#define SKELLINGTON_MESHSKELETONANIMATOR_H

#include "Mesh.h"
#include "Skeleton.h"
#include "Pose.h"

namespace skellington {

    namespace MeshSkeletonAnimator
    {
        Mesh ApplyPose_Linear(const Pose &pose, const Mesh *meshRest);

        Mesh ApplyPose_OptimizedCoR(const Pose &pose, const Mesh *meshRest, const map<int, vec3> optimizedCoMs);

    };

};

#endif //SKELLINGTON_MESHSKELETONANIMATOR_H
