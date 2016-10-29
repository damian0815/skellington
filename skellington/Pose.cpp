//
// Created by Damian Stewart on 29/10/2016.
//

#include "Pose.h"

namespace skellington
{
    void Pose::SetParentRelativeTransformOffset(const string &jointName, const Transform &transformOffset)
    {
        mJointOffsetTransforms[jointName] = transformOffset;
    }

    Transform Pose::GetAbsoluteTransform(const string &jointName)const
    {
        Transform parentTransform;
        if (mSkeleton->JointHasParent(jointName)) {
            parentTransform = GetAbsoluteTransform(mSkeleton->GetParentJointName(jointName));
        }

        return parentTransform * mSkeleton->GetJoint(jointName).GetParentRelativeRestTransform() * GetJointOffsetTransform(jointName);
    }

    const Transform &Pose::GetJointOffsetTransform(const string &jointName) const
    {
        if (mJointOffsetTransforms.count(jointName)) {
            return mJointOffsetTransforms.at(jointName);
        }
        return Transform::IDENTITY;
    }


};