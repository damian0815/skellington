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
};