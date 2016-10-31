//
// Created by Damian Stewart on 15/10/16.
//

#include "MeshSkeletonAnimator.h"
#include "Pose.h"

namespace skellington
{
    namespace MeshSkeletonAnimator
    {
        vec3 GetTransformedPos(const vec3 &v, const vec3 &restCoR, const vec3 &posedCoR, const quat &offsetRotation)
        {
            return posedCoR + offsetRotation * (v - restCoR);
        }

        Mesh ApplyPose_Linear(const Pose &pose, const Mesh *meshRest)
        {
            vector<vec3> posedVertices(meshRest->GetVertices().size());
            const auto& skeleton = *pose.GetSkeleton();

            for (const auto& it: meshRest->GetVertexGroups()) {
                auto name = it.first;

                auto restTransform = skeleton.GetAbsoluteTransform(name);
                auto posedTransform = pose.GetAbsoluteTransform(name);

                auto restCoR = restTransform.GetTranslation();
                auto posedCoR = posedTransform.GetTranslation();
                auto offsetRotation = posedTransform.GetRotation() * glm::inverse(restTransform.GetRotation());

                for (const auto& wv: it.second) {
                    const auto& restPos = meshRest->GetVertices()[wv.mVertexIndex];

                    auto transformedPos = GetTransformedPos(restPos, restCoR, posedCoR, offsetRotation);
                    posedVertices[wv.mVertexIndex] += wv.mWeight * transformedPos;
                }
            }

            return Mesh(posedVertices, meshRest->GetTriangles());

        }

        quat AntipodalityAwareAdd(const quat &q1, const quat &q2);

        Mesh ApplyPose_OptimizedCoR(const Pose &pose, const Mesh *meshRest, const map<int, vec3> &optimizedCoRs, vector<vec3> &optimizedCoRsPosedOut)
        {
            const auto numVertices = meshRest->GetVertices().size();

            const auto& skeleton = *pose.GetSkeleton();


            map<string, vec3> restJointCoRs;
            map<string, vec3> posedJointCoRs;
            map<string, quat> offsetRotations;
            for (const auto& j: skeleton.GetJoints()) {
                const auto& name = j.GetName();
                auto restTransform = skeleton.GetAbsoluteTransform(name);
                auto posedTransform = pose.GetAbsoluteTransform(name);

                auto restCoR = restTransform.GetTranslation();
                auto posedCoR = posedTransform.GetTranslation();
                auto offsetRotation = posedTransform.GetRotation() * glm::inverse(restTransform.GetRotation());

                restJointCoRs[name] = restCoR;
                posedJointCoRs[name] = posedCoR;
                offsetRotations[name] = offsetRotation;
            }



            vector<vec3> posedVertices(numVertices);
            vector<vec3> optimizedCoRsPosed(numVertices);
            vector<quat> optimizedOffsetRotations;
            optimizedOffsetRotations.assign(numVertices, quat(0,0,0,0));
            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                const auto& restCoR = restJointCoRs[groupName];
                const auto& posedCoR = posedJointCoRs[groupName];
                const auto& offsetRotation = offsetRotations[groupName];
                for (const auto& wv: it.second) {
                    if (optimizedCoRs.count(wv.mVertexIndex)) {
                        const auto &optimizedCoRRest = optimizedCoRs.at(wv.mVertexIndex);
                        optimizedCoRsPosed[wv.mVertexIndex] += wv.mWeight * GetTransformedPos(optimizedCoRRest, restCoR, posedCoR, offsetRotation);

                        optimizedOffsetRotations[wv.mVertexIndex] = AntipodalityAwareAdd(optimizedOffsetRotations[wv.mVertexIndex], wv.mWeight * offsetRotation);
                        //optimizedOffsetRotations[wv.mVertexIndex] += wv.mWeight * offsetRotation;
                    } else {
                        const auto& restV = meshRest->GetVertices()[wv.mVertexIndex];
                        posedVertices[wv.mVertexIndex] += wv.mWeight * GetTransformedPos(restV, restCoR, posedCoR, offsetRotation);
                    }
                }
            }
            optimizedCoRsPosedOut = optimizedCoRsPosed;

            for (int i=0; i<numVertices; i++) {
                if (optimizedCoRs.count(i)) {
                    auto restV = meshRest->GetVertices()[i];

                    const auto& restCoR = optimizedCoRs.at(i);
                    const auto& posedCoR = optimizedCoRsPosed[i];
                    auto offsetRotation = glm::normalize(optimizedOffsetRotations[i]);

                    posedVertices[i] = GetTransformedPos(restV, restCoR, posedCoR, offsetRotation);
                }
            }

            return Mesh(posedVertices, meshRest->GetTriangles());
        }

        quat AntipodalityAwareAdd(const quat &q1, const quat &q2)
        {
            if (glm::dot(q1,q2) >= 0) {
                return q1 + q2;
            } else {
                return q1 + (-q2);
            }
        }

    };

};