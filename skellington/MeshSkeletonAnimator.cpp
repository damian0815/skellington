//
// Created by Damian Stewart on 15/10/16.
//

#include "MeshSkeletonAnimator.h"
#include "Pose.h"

namespace skellington
{
    namespace MeshSkeletonAnimator
    {
        Mesh ApplyPose(const Pose &pose, const Mesh *meshRest)
        {
            vector<vec3> posedVertices(meshRest->GetVertices().size());
            const auto& skeleton = *pose.GetSkeleton();

            for (const auto& it: meshRest->GetVertexGroups()) {
                auto name = it.first;

                auto restInverseTransform = skeleton.GetAbsoluteTransform(name).GetInverse();
                auto poseTransform = pose.GetAbsoluteTransform(name);

                for (const auto& wv: it.second) {
                    auto transformedPos = poseTransform * (restInverseTransform * meshRest->GetVertices()[wv.mVertexIndex]);
                    posedVertices[wv.mVertexIndex] += wv.mWeight * transformedPos;
                }
                 /*

                auto transform =  pose.GetAbsoluteTransform(name);

                vec3 restCom = skeleton.GetAbsoluteTransform(name) * vec3(0,0,0);
                vec3 com = pose.GetAbsoluteTransform(name) * vec3(0,0,0);

                for (const auto& wv: it.second) {
                    auto transformedPos = transform * (meshRest->GetVertices()[wv.mVertexIndex] - restCom);
                    posedVertices[wv.mVertexIndex] += wv.mWeight * transformedPos;
                }*/
            }

            return Mesh(posedVertices, meshRest->GetTriangles());

        }

    };

};