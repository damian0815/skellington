//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_FRAME_H
#define SKELLINGTON_FRAME_H

#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::quat;
using glm::mat4;

namespace skellington
{
    class Transform
    {
    public:
        Transform(const mat4& transform): mTransform(transform) {};
        Transform(const vec3& translation, const quat& rotation, const vec3& scale=vec3(1,1,1));

        const mat4& GetMatrix() const { return mTransform; }
        vec3 GetTranslationComponent() const;

        static void Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut);

        vec3 operator*(const vec3& v) const { auto v4 = mTransform * vec4(v.x, v.y, v.z, 1); return vec3(v4.x/v4.w, v4.y/v4.w, v4.z/v4.w); }

    private:
        const mat4 mTransform;

    };
};


#endif //SKELLINGTON_FRAME_H
