//
// Created by Damian Stewart on 15/10/16.
//

#include "Transform.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using glm::vec4;

namespace skellington {

    const Transform Transform::IDENTITY;

    static mat4 MakeFrameMatrix(const vec3 &translation, const quat &rotation, const vec3 &scale)
    {
        // translate then rotate then scale
        return glm::translate(translation) * glm::mat4_cast(rotation) * glm::scale(scale);
    }

    Transform::Transform(const vec3 &translation, const quat &rotation, const vec3& scale)
    : mTransform(MakeFrameMatrix(translation, rotation, scale))
    {

    }

    vec3 Transform::GetTranslationComponent() const
    {
        vec3 translate;
        quat rotate;
        vec3 scale;
        Decompose(mTransform, translate, rotate, scale);
        return translate;
    }

    void Transform::Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut)
    {
        vec3 scale;
        quat orientation;
        vec3 translation;
        vec3 skew;
        vec4 perspective;
        decompose(transform, scale, orientation, translation, skew, perspective);
        orientation = conjugate(orientation);
    }


};
