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

        const Skeleton* const GetSkeleton() const { return mSkeleton; }

        // in joint space
        void SetOffsetTransform(const string &jointName, const Transform &transformOffset);
        const Transform& GetOffsetTransform(const string &jointName) const;

        // in parent joint space
        Transform GetParentRelativeTransform(const string& jointName) const;
        // in 'world' space
        Transform GetAbsoluteTransform(const string &jointName) const;

    private:
        const Skeleton* mSkeleton;

        map<string, Transform> mJointOffsetTransforms;

    };

}


#endif //SKELLINGTON_POSE_H
