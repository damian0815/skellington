//
// Created by Damian Stewart on 15/10/16.
//

#include "Skeleton.h"

namespace skellington
{


    bool Skeleton::HasJoint(const string &name) const
    {
        auto it = FindJointWithName(name);
        return (it != mJoints.end());
    }

    void Skeleton::SetJointParent(const string &joint, const string &parent)
    {
        assert(HasJoint(joint) && HasJoint(parent));
        assert(mJointParents.count(joint) == 0);

        mJointParents[joint] = parent;
    }

    bool Skeleton::JointHasParent(const string &jointName)
    {
        return mJointParents.count(jointName) != 0;
    }

    void Skeleton::AddRootJoint(Joint joint)
    {
        assert(mRootJointName == "");
        assert(!HasJoint(joint.GetName()));
        mRootJointName = joint.GetName();
        mJoints.push_back(joint);
    }

    void Skeleton::AddJoint(Joint j, string parentJointName)
    {
        assert(!HasJoint(j.GetName()));
        mJoints.push_back(j);
        SetJointParent(j.GetName(), parentJointName);
    }

    Transform Skeleton::GetAbsoluteTransform(const Joint &joint)
    {
        vector<Transform> transformStack;

        string jointName = joint.GetName();
        while(true)
        {
            transformStack.push_back(GetJoint(jointName).GetParentRelativeRestTransform());
            if (!JointHasParent(jointName)) {
                break;
            }
            jointName = GetParentJoint(jointName).GetName();
        }

        mat4 absoluteTransform;
        for (const auto& t: transformStack) {
            absoluteTransform *= t.GetMatrix();
        }
        return Transform(absoluteTransform);

    }
};