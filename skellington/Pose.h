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

        // returns the center of rotation for the given joint in world space
        vec3 GetAbsoluteTranslation(const string& jointName) const;
        // returns the rotation to be applied around the center of rotation in world space
        quat GetAbsoluteOffsetRotation(const string &jointName) const;

    private:
        const Skeleton* mSkeleton;

        map<string, Transform> mJointOffsetTransforms;

        vec3 GetParentRelativeTranslation(const string &basic_string) const;
    };

}


#endif //SKELLINGTON_POSE_H
