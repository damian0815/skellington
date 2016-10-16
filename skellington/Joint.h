//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_JOINT_H
#define SKELLINGTON_JOINT_H

#include <string>
#include <glm/glm.hpp>
#include "Transform.h"

using glm::mat4;
using std::string;

namespace skellington
{

    class Joint
    {
    public:
        Joint(string name, const Transform &parentRelativeRestTransform) : mName(name), mParentRelativeRestTransform(parentRelativeRestTransform) { }
        Joint(const Joint& other): Joint(other.mName, other.mParentRelativeRestTransform) { }

        const string& GetName() const { return mName; }
        const Transform& GetParentRelativeRestTransform() const { return mParentRelativeRestTransform; }

    private:

        const string mName;
        const Transform mParentRelativeRestTransform;
    };

}

#endif //SKELLINGTON_JOINT_H
