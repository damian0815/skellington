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

    vec3 Pose::GetAbsoluteTranslation(const string &jointName) const
    {
        vec3 translation;
        if (mSkeleton->JointHasParent(jointName)) {
            translation = GetAbsoluteTranslation(mSkeleton->GetParentJointName(jointName));
        }
        auto parentRelativeTranslation = mSkeleton->GetJoint(jointName).GetParentRelativeRestTransform().GetTranslation();
        return translation + parentRelativeTranslation;
    }

    quat Pose::GetAbsoluteOffsetRotation(const string &jointName) const
    {
        quat parentRotation;
        if (mSkeleton->JointHasParent(jointName)) {
            parentRotation = GetAbsoluteOffsetRotation(mSkeleton->GetParentJointName(jointName));
        }
        return parentRotation * mJointOffsetTransforms.at(jointName).GetRotation();
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