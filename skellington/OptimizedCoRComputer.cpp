//
// Created by Damian Stewart on 29/10/2016.
//

#include "OptimizedCoRComputer.h"

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <glm/glm.hpp>

using std::string;
using std::vector;
using std::map;
using glm::vec3;

namespace skellington
{
    // based on Le, Hodgins 2016 "Real-time Skeletal Skinning with Optimized Centers of Rotation"

    namespace OptimizedCoRComputer
    {
        typedef vector<float> BoneWeightMap;

        struct TriangleIndices
        {
            int alpha, beta, gamma;
        };

        struct TriangleNodes
        {
            vec3 alpha, beta, gamma;
        };

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<BoneWeightMap> &boneWeights, vec3 &corOut);


        map<int, vec3> ComputeOptimizedCoRs(skellington::Mesh *mesh, skellington::Skeleton *skeleton)
        {
            vector<TriangleIndices> triangles(mesh->GetTriangles().size());
            for (int triangleIndex=0; triangleIndex<mesh->GetTriangles().size()/3; triangleIndex++) {
                triangles[triangleIndex].alpha = mesh->GetTriangles()[triangleIndex*3+0];
                triangles[triangleIndex].beta  = mesh->GetTriangles()[triangleIndex*3+1];
                triangles[triangleIndex].gamma = mesh->GetTriangles()[triangleIndex*3+2];
            }

            vector<BoneWeightMap> boneWeights;
            boneWeights.assign(mesh->GetVertices().size(), BoneWeightMap(skeleton->GetJoints().size()));
            int boneIndex = 0;
            for (const auto& j: skeleton->GetJoints()) {
                const auto& jointName = j.GetName();
                const auto& group = mesh->GetVertexGroups().at(jointName);
                for (const auto& wv: group) {
                    boneWeights.at(wv.mVertexIndex).at(boneIndex) = wv.mWeight;
                }
                ++boneIndex;
            }

            map<int, vec3> results;
            for (int i=0; i<mesh->GetVertices().size(); i++) {
                vec3 cor;
                bool hasCoR = ComputeOptimizedCoR(i, mesh->GetVertices(), triangles, boneWeights, cor);
                if (hasCoR) {
                    results[i] = cor;
                }
            }

            return results;

        }


        float ComputeL2SkinningWeightDistance(const BoneWeightMap &wP, const BoneWeightMap & wV)
        {
            float distanceSq = 0;
            for (int i=0; i<wP.size(); i++) {
                distanceSq += std::pow(wP[i] - wV[i], 2);
            }
            return std::sqrt(distanceSq);
        }

        float ComputeSimilarity(const BoneWeightMap &wP, const BoneWeightMap &wV)
        {
            // sigma = width of exponential kernel in eq.1
            const float sigma = 0.1f;
            float similarity = 0;
            const float sigmaSq = sigma*sigma;

            for (int j=0; j<wP.size(); j++) {
                for (int k=0; k<wV.size(); k++) {
                    if (j==k) {
                        continue;
                    }
                    auto wPj = wP.at(j);
                    auto wPk = wP.at(k);
                    auto wVj = wV.at(j);
                    auto wVk = wV.at(k);

                    similarity += wPj*wPk*wVj*wVk * std::exp(-(std::pow(wPj*wVk - wPk-wVj, 2))/sigmaSq);
                }
            }

            return similarity;
        }




        float ComputeTriangleArea(const TriangleNodes &triangle)
        {
            auto u = triangle.beta - triangle.alpha;
            auto v = triangle.gamma - triangle.alpha;
            return 0.5f * glm::length(glm::cross(u, v));
        }

        BoneWeightMap ComputeTriangleAverageWeights(const vector<BoneWeightMap>& boneWeights, const TriangleIndices &triangle)
        {
            BoneWeightMap result(boneWeights.at(0).size());
            for (const auto vertexIndex: {triangle.alpha, triangle.beta, triangle.gamma}) {
                for (int j=0; j<boneWeights.at(vertexIndex).size(); j++) {
                    result.at(j) += boneWeights.at(vertexIndex).at(j);
                }
            }
            const float ONE_THIRD = 1.0f/3.0f;
            for (auto& w: result) {
                w *= ONE_THIRD;
            }
            return result;
        }

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<BoneWeightMap> &boneWeights, vec3 &corOut)
        {
            float totalWeight = 0;
            vec3 posAccumulator;

            for (const auto& t: triangles) {

                TriangleNodes tNodes{vertices[t.alpha], vertices[t.beta], vertices[t.gamma]};

                const auto& wI = boneWeights.at(vertexIndex);
                const auto& wTriangleAverage = ComputeTriangleAverageWeights(boneWeights, t);

                const auto similarity = ComputeSimilarity(wI, wTriangleAverage);
                float weight = similarity * ComputeTriangleArea(tNodes);
                totalWeight += weight;

                vec3 pos = (tNodes.alpha + tNodes.beta + tNodes.gamma)/3.0f;
                posAccumulator += pos*weight;
            }

            if (totalWeight > 0) {
                corOut = posAccumulator / totalWeight;
                return true;
            }
            return false;
        }

    }
}
