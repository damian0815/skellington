//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_SKELETON_H
#define SKELLINGTON_SKELETON_H

#include "Joint.h"
#include <vector>

using std::vector;

namespace skellington {

    class Skeleton
    {
    public:

        void AddJoint(Joint j) { mJoints.push_back(j); }

        const Joint& GetRootJoint() { return GetJoint(mRootJointName); }
        const Joint& GetJoint(string name) { return *std::find_if(mJoints.begin(), mJoints.end(), [&name](const Joint& j) { return name == j.GetName(); }); }
        const vector<Joint>& GetJoints() { return mJoints; }

    private:

        string mRootJointName;
        vector<Joint> mJoints;
    };

};

#endif //SKELLINGTON_SKELETON_H
