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
        struct TriangleIndices
        {
            int alpha, beta, gamma;
        };

        struct TriangleNodes
        {
            vec3 alpha, beta, gamma;
        };

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<map<string, float>> &boneWeights, vec3 &corOut);


        map<int, vec3> ComputeOptimizedCoRs(skellington::Mesh *mesh, skellington::Skeleton *skeleton)
        {

            vector<TriangleIndices> triangles(mesh->GetTriangles().size());
            for (int triangleIndex=0; triangleIndex<mesh->GetTriangles().size()/3; triangleIndex++) {
                triangles[triangleIndex].alpha = mesh->GetTriangles()[triangleIndex*3+0];
                triangles[triangleIndex].beta  = mesh->GetTriangles()[triangleIndex*3+1];
                triangles[triangleIndex].gamma = mesh->GetTriangles()[triangleIndex*3+2];
            }

            vector<map<string, float>> boneWeights(mesh->GetVertices().size());
            for (const auto& j: skeleton->GetJoints()) {
                const auto& jointName = j.GetName();
                const auto& group = mesh->GetVertexGroups().at(jointName);
                for (const auto& wv: group) {
                    boneWeights[wv.mVertexIndex][jointName] = wv.mWeight;
                }
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


        float ComputeSimilarity(const map<string, float> &wP, const map<string, float> &wV)
        {
            // sigma = width of exponential kernel in eq.1
            const float sigma = 0.1f;
            float similarity = 0;
            const float sigmaSq = sigma*sigma;

            for (const auto& jt: wP) {
                for (const auto &kt: wV) {
                    const auto& j = jt.first;
                    const auto& k = kt.first;
                    if (j == k) {
                        continue;
                    }
                    if (wP.count(k) == 0) {
                        continue;
                    }
                    if (wV.count(j) == 0) {
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

        map<string, float> ComputeTriangleAverageWeights(const vector<map<string, float>>& boneWeights, const TriangleIndices &triangle)
        {
            map<string, float> result;
            for (const auto i: {triangle.alpha, triangle.beta, triangle.gamma}) {
                for (const auto& w: boneWeights[i]) {
                    result[w.first] += w.second;
                }
            }
            const float ONE_THIRD = 1.0f/3.0f;
            for (auto& w: result) {
                w.second *= ONE_THIRD;
            }
            return result;
        }

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<map<string, float>> &boneWeights, vec3 &corOut)
        {
            float totalWeight = 0;
            vec3 posAccumulator;

            for (const auto& t: triangles) {

                TriangleNodes tNodes{vertices[t.alpha], vertices[t.beta], vertices[t.gamma]};

                const auto& wI = boneWeights[vertexIndex];
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
