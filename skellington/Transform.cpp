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

    Transform::Transform(const mat4 &transform)
    {
        vec3 scale;
        Decompose(transform, mTranslation, mRotation, scale);
    }

    const mat4 Transform::GetMatrix() const
    {
        return glm::translate(mTranslation) * glm::mat4_cast(mRotation);
    }

    void Transform::Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut)
    {
        vec3 skew;
        vec4 perspective;
        decompose(transform, scaleOut, rotateOut, translateOut, skew, perspective);
        rotateOut = conjugate(rotateOut);
    }

    Transform Transform::GetInverse() const
    {
        auto rT = glm::transpose(glm::mat3_cast(mRotation));
        return Transform(-rT * mTranslation, glm::quat_cast(rT));
    }



};
