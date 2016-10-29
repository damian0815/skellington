//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_SKELETON_H
#define SKELLINGTON_SKELETON_H

#include "Joint.h"
#include <vector>
#include <map>

using std::vector;
using std::map;

namespace skellington {

    class Skeleton
    {
    public:
        void AddJoint(Joint j, string parentJointName);

        const Joint& GetJoint(string name) const { return *FindJointWithName(name); }

        const vector<Joint>& GetJoints() const { return mJoints; }

        bool HasJoint(const string &name) const;
        const string& GetParentJointName(const string &joint) const
            { return mJointParents.at(joint); }
        const Joint &GetParentJoint(const string &childJointName) const
            { return GetJoint(GetParentJointName(childJointName)); }

        bool JointHasParent(const string &jointName) const;
        Transform GetAbsoluteTransform(const string &jointName)const;

        void SetRootJointName(const string &name);
        const string& GetRootJointName() { return mRootJointName; }


    private:
        inline vector<Joint>::const_iterator FindJointWithName(const string &name) const { return find_if(mJoints.begin(), mJoints.end(), [&name](const Joint& j) { return name == j.GetName(); }); }
        void SetJointParent(const string &joint, const string &parent);

        string mRootJointName;
        vector<Joint> mJoints;
        map<string,string> mJointParents;
    };

};

#endif //SKELLINGTON_SKELETON_H
