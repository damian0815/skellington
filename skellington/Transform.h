//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_FRAME_H
#define SKELLINGTON_FRAME_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::mat4;
using glm::mat3;

namespace skellington
{
    class Transform
    {
    public:
        static const Transform IDENTITY;

        static Transform MakeRotation(float angle, const vec3& axis) { return Transform(vec3(0,0,0), glm::angleAxis(angle, axis)); };

        Transform() {}
        Transform(const mat4& transform);
        Transform(const vec3& translation, const quat& rotation) : mRotation(rotation), mTranslation(translation) {};

        const mat4 GetMatrix() const;

        Transform GetInverse() const;

        vec3 operator*(const vec3& v) const { return mRotation * v + mTranslation; }

        Transform operator*(const Transform& other) const { return Transform(mTranslation + mRotation * other.mTranslation, mRotation * other.mRotation); }

        const quat &GetRotation() const { return mRotation; }
        const vec3 &GetTranslation() const { return mTranslation; }

    private:

        static void Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut);

        quat mRotation;
        vec3 mTranslation;

    };
};


#endif //SKELLINGTON_FRAME_H
