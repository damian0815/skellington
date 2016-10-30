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

                auto restTransform = skeleton.GetAbsoluteTransform(name);
                auto posedTransform = pose.GetAbsoluteTransform(name);

                auto restCoR = restTransform.GetTranslation();
                auto posedCoR = posedTransform.GetTranslation();
                auto offsetRotation = posedTransform.GetRotation() * glm::inverse(restTransform.GetRotation());

                for (const auto& wv: it.second) {
                    const auto& restPos = meshRest->GetVertices()[wv.mVertexIndex];

                    auto transformedPos = posedCoR + offsetRotation * (restPos - restCoR);
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

            /*
            map<string, vec3> restJointCoRs;
            map<string, quat> offsetRotations;
            for (const auto& j: skeleton.GetJoints()) {
                auto restTransform = skeleton.GetAbsoluteTransform(name);
                auto posedTransform = pose.GetAbsoluteTransform(name);

                auto restCoR = restTransform.GetTranslation();
                auto posedCoR = posedTransform.GetTranslation();
                auto offsetRotation = posedTransform.GetRotation() * glm::inverse(restTransform.GetRotation());*/

            map<string, glm::quat> unitRotationQuats;
            map<string, vec3> translations;
            map<string, Transform> inverseRestTransforms;
            for (const auto& j: skeleton.GetJoints()) {
                const auto poseTransform = pose.GetAbsoluteTransform(j.GetName());

                unitRotationQuats[j.GetName()] = glm::normalize(poseTransform.GetRotation());
                translations[j.GetName()] = poseTransform.GetTranslation();

                inverseRestTransforms[j.GetName()] = skeleton.GetAbsoluteTransform(j.GetName()).GetInverse();
            }


            vector<quat> rotations;
            rotations.assign(numVertices, quat(0,0,0,0));
            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                const auto& qi = unitRotationQuats.at(groupName);
                for (const auto& wv: it.second) {
                    //rotations[wv.mVertexIndex] = AntipodalityAwareAdd(rotations[wv.mVertexIndex], wv.mWeight * qi);
                    rotations[wv.mVertexIndex] += wv.mWeight * qi;

                }
            }


            vector<vec3> corT(numVertices);
            for (const auto& it: meshRest->GetVertexGroups()) {
                const auto& groupName = it.first;
                const auto& lbsR = unitRotationQuats[groupName];
                const auto& lbsT = translations[groupName];
                 for (const auto& wv: it.second) {
                    if (!optimizedCoMs.count(wv.mVertexIndex)) {
                        continue;
                    }
                    auto pi = optimizedCoMs.at(wv.mVertexIndex);
                    corT[wv.mVertexIndex] += wv.mWeight * ((lbsR * (inverseRestTransforms[groupName] * pi)) + lbsT);
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
                    auto restV = meshRest->GetVertices()[i];
                    auto R = glm::normalize(rotations[i]);
                    auto pi = optimizedCoMs.at(i);

                    auto t = corT[i] - (R * pi);

                    auto vDash = (R * restV) + t;

                    posedVertices[i] = vDash;
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