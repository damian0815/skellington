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

namespace skellington
{
    class Transform
    {
    public:
        static const Transform IDENTITY;

        static Transform MakeRotation(float angle, const vec3& axis) { return Transform(vec3(0,0,0), glm::angleAxis(angle, axis)); };
        static Transform MakeTranslation(const vec3 &translation) { return Transform(translation, glm::quat()); }

        Transform() {}
        Transform(const mat4& transform): mTransform(transform) {};
        Transform(const vec3& translation, const quat& rotation, const vec3& scale=vec3(1,1,1));

        const mat4& GetMatrix() const { return mTransform; }
        vec3 GetTranslationComponent() const;

        static void Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut);

        vec3 operator*(const vec3& v) const { auto v4 = mTransform * vec4(v.x, v.y, v.z, 1); return vec3(v4.x/v4.w, v4.y/v4.w, v4.z/v4.w); }

        Transform operator*(const Transform& other) const { return Transform(mTransform * other.mTransform); }



    private:
        mat4 mTransform;

    };
};


#endif //SKELLINGTON_FRAME_H
