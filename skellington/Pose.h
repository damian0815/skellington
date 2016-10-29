//
// Created by Damian Stewart on 29/10/2016.
//

#ifndef SKELLINGTON_POSE_H
#define SKELLINGTON_POSE_H

#include <string>
#include <map>
#include "Transform.h"
#include "Skeleton.h"

using std::string;
using std::map;

namespace skellington
{

    class Pose
    {
    public:
        Pose(const Skeleton* skeleton) : mSkeleton(skeleton) {}

        void SetParentRelativeTransformOffset(const string& jointName, const Transform& transformOffset);

        Transform GetAbsoluteTransform(const string &jointName)const;

    private:
        const Skeleton* mSkeleton;

        map<string, Transform> mJointOffsetTransforms;

        const Transform& GetJointOffsetTransform(const string &jointName) const;
    };

}


#endif //SKELLINGTON_POSE_H
