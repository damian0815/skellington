//
// Created by Damian Stewart on 15/10/16.
//

#include "MeshSkeletonAnimator.h"
#include "Pose.h"

namespace skellington
{
    namespace MeshSkeletonAnimator
    {
        Mesh ApplyPose_Linear(const Pose &pose, const Mesh *meshRest)
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
            }

            return Mesh(posedVertices, meshRest->GetTriangles());

        }


        quat AntipodalityAwareAdd(const quat &q1, const quat &q2);

        Mesh ApplyPose_OptimizedCoR(const Pose &pose, const Mesh *meshRest, const map<int, vec3> optimizedCoMs)
        {
            const auto numVertices = meshRest->GetVertices().size();

            const auto& skeleton = *pose.GetSkeleton();
            map<string, glm::quat> unitRotationQuats;
            map<string, vec3> translations;
            map<string, Transform> inverseRestTransforms;
            for (const auto& j: skeleton.GetJoints()) {
                const auto poseTransform = pose.GetAbsoluteTransform(j.GetName());

                unitRotationQuats[j.GetName()] = poseTransform.GetRotation();
                translations[j.GetName()] = poseTransform.GetTranslation();

                inverseRestTransforms[j.GetName()] = skeleton.GetAbsoluteTransform(j.GetName()).GetInverse();
            }


            vector<quat> rotations;
            rotations.assign(numVertices, quat(0,0,0,0));
            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                const auto& qi = unitRotationQuats.at(groupName);
                for (const auto& wv: it.second) {
                    rotations[wv.mVertexIndex] = AntipodalityAwareAdd(rotations[wv.mVertexIndex], wv.mWeight * qi);
                }
            }


            vector<vec3> corT(numVertices);
            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                for (const auto& wv: it.second) {
                    if (!optimizedCoMs.count(wv.mVertexIndex)) {
                        continue;
                    }
                    auto pi = optimizedCoMs.at(wv.mVertexIndex);
                    corT[wv.mVertexIndex] += wv.mWeight * (unitRotationQuats[groupName] * pi + translations[groupName]);
                }
            }

            vector<vec3> posedVertices(numVertices);


            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                const auto& lbsR = unitRotationQuats[groupName];
                const auto& lbsT = translations[groupName];
                for (const auto& wv: it.second) {
                    auto restV = meshRest->GetVertices()[wv.mVertexIndex];
                    if (!optimizedCoMs.count(wv.mVertexIndex)) {
                        posedVertices[wv.mVertexIndex] += wv.mWeight * ((lbsR * (inverseRestTransforms[groupName] * restV)) + lbsT);
                    }
                }
            }
            for (int i=0; i<numVertices; i++) {
                if (optimizedCoMs.count(i)) {
                    auto restV4 = vec4(meshRest->GetVertices()[i], 1);
                    auto R = glm::normalize(rotations[i]);
                    auto pi4 = vec4(optimizedCoMs.at(i), 1);
                    auto corT4 = vec4(corT[i], 1);

                    auto t = corT4 - (R * pi4);

                    auto vDash = (R * restV4) + t;

                    posedVertices[i] = vec3(vDash.x, vDash.y, vDash.z)/vDash.w;
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