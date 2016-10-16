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
        Joint(string name, const Transform &restTransform) : mName(name), mRestTransform(restTransform) { }
        Joint(const Joint& other): Joint(other.mName, other.mRestTransform) { }

        const string& GetName() const { return mName; }
        const Transform& GetTransform() const { return mRestTransform; }

    private:

        const string mName;
        const Transform mRestTransform;
    };

}

#endif //SKELLINGTON_JOINT_H
