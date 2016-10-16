//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_MESHSKELETONANIMATOR_H
#define SKELLINGTON_MESHSKELETONANIMATOR_H

#include "Mesh.h"
#include "Skeleton.h"

namespace skellington {

    namespace MeshSkeletonAnimator
    {
        void Animate(const Skeleton* skeleton, const Mesh* meshRest, Mesh* meshResult);

    };

};

#endif //SKELLINGTON_MESHSKELETONANIMATOR_H
