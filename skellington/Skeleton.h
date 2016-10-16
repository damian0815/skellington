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

        void AddJoint(Joint j) { mJoints.push_back(j); }

        const Joint& GetRootJoint() { return GetJoint(mRootJointName); }
        const Joint& GetJoint(string name) { return *FindJointWithName(name); }


        const vector<Joint>& GetJoints() const { return mJoints; }

        bool HasJoint(const string &name) const;
        void SetJointParent(const string &joint, const string &parent);

    private:
        inline vector<Joint>::const_iterator FindJointWithName(const string &name) const { return find_if(mJoints.begin(), mJoints.end(), [&name](const Joint& j) { return name == j.GetName(); }); }

        string mRootJointName;
        vector<Joint> mJoints;
        map<string,string> mJointParents;
    };

};

#endif //SKELLINGTON_SKELETON_H
