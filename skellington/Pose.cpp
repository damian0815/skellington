//
// Created by Damian Stewart on 29/10/2016.
//

#include "Pose.h"

namespace skellington
{
    void Pose::SetOffsetTransform(const string &jointName, const Transform &transformOffset)
    {
        mJointOffsetTransforms[jointName] = transformOffset;
    }

    Transform Pose::GetParentRelativeTransform(const string &jointName) const
    {
       return mSkeleton->GetJoint(jointName).GetParentRelativeRestTransform() * GetOffsetTransform(jointName);
    }

    Transform Pose::GetAbsoluteTransform(const string &jointName)const
    {
        Transform parentTransform;
        if (mSkeleton->JointHasParent(jointName)) {
            parentTransform = GetAbsoluteTransform(mSkeleton->GetParentJointName(jointName));
        }

        return parentTransform * GetParentRelativeTransform(jointName);
    }

    const Transform &Pose::GetOffsetTransform(const string &jointName) const
    {
        if (mJointOffsetTransforms.count(jointName)) {
            return mJointOffsetTransforms.at(jointName);
        }
        return Transform::IDENTITY;
    }


};