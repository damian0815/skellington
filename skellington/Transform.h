//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_FRAME_H
#define SKELLINGTON_FRAME_H

#include <glm/glm.hpp>
using glm::vec3;
using glm::quat;
using glm::mat4;

namespace skellington
{
    class Transform
    {
    public:
        Transform(const mat4& transform): mTransform(transform) {};
        Transform(const vec3& translation, const quat& rotation, const vec3& scale=vec3(1,1,1));

        vec3 GetTranslationComponent() const;

        static void Decompose(const mat4 &transform, vec3 &translateOut, quat &rotateOut, vec3 &scaleOut);

    private:
        const mat4 mTransform;

    };
};


#endif //SKELLINGTON_FRAME_H
