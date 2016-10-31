//
// Created by Damian Stewart on 29/10/2016.
//

#include "OptimizedCoRComputer.h"

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <glm/glm.hpp>
#include <fmt/ostream.h>

using std::string;
using std::vector;
using std::pair;
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
            int operator[](int which) const
            {
                switch(which) {
                    case 0: return alpha;
                    case 1: return beta;
                    case 2: return gamma;
                }
                assert(false);
                return 0;
            }

        };

        struct TriangleNodes
        {
            vec3 alpha, beta, gamma;
        };

        struct TriangleCentroidData
        {
            vec3 centroid;
            BoneWeightMap averageBoneWeights;
            float area;
        };

        struct SubdividedMesh
        {
            vector<vec3> vertices; // the first n vertices are identical to the source mesh, where n = source mesh vertex count
            vector<TriangleIndices> triangles;
            vector<BoneWeightMap> boneWeightsPerVertex;
        };

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<BoneWeightMap> &boneWeightsPerVertex, const vector<TriangleCentroidData> &triangleDataPerTriangle, float sigma, vec3 &corOut);



        SubdividedMesh
        SubdivideMesh(const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<BoneWeightMap> &boneWeightsPerVertex, float epsilon);



        vector<TriangleCentroidData> PrecalculateTriangleData(const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<BoneWeightMap> &weightsPerVertex);

        map<int, vec3>
        ComputeOptimizedCoRs(skellington::Mesh *mesh, skellington::Skeleton *skeleton, float subdivisionEpsilon, float similaritySigma)
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

            auto subdividedMesh = SubdivideMesh(mesh->GetVertices(), triangles, boneWeights, subdivisionEpsilon);
            auto triangleData = PrecalculateTriangleData(subdividedMesh.vertices, subdividedMesh.triangles, subdividedMesh.boneWeightsPerVertex);

            map<int, vec3> results;
            int numUnsubdividedVertices = mesh->GetVertices().size();
            fmt::print("Computing optimized centers of rotation for {0} vertices in {1} triangles, subdivided to {2} vertices in {3} triangles, over {4} bones...\n", numUnsubdividedVertices, triangles.size(), subdividedMesh.vertices.size(), subdividedMesh.triangles.size(), boneIndex);

            for (int i=0; i<numUnsubdividedVertices; i++) {
                if (i % (numUnsubdividedVertices / 40) == 0) {
                    float pct = float(i)/float(numUnsubdividedVertices);
                    fmt::print("{0}%\n", pct*100);
                }
                vec3 cor;
                bool hasCoR = ComputeOptimizedCoR(i, subdividedMesh.vertices, subdividedMesh.boneWeightsPerVertex, triangleData, similaritySigma, cor);
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

        // sigma = width of exponential kernel in eq.1
        float ComputeSimilarity(const BoneWeightMap &wP, const BoneWeightMap &wV, float sigma)
        {
            float similarity = 0;
            const float sigmaSq = sigma*sigma;

            for (int j=0; j<wP.size(); j++) {
                for (int k=j+1; k<wV.size(); k++) {
                    auto wPj = wP.at(j);
                    auto wPk = wP.at(k);
                    auto wVj = wV.at(j);
                    auto wVk = wV.at(k);

                    similarity += wPj*wPk*wVj*wVk * std::exp(-(std::pow(wPj*wVk - wPk*wVj, 2))/sigmaSq);
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

            for (auto& w: result) {
                w /= 3.0f;
            }
            return result;
        }

        vector<TriangleCentroidData> PrecalculateTriangleData(const vector<vec3> &vertices, const vector<TriangleIndices> &triangles, const vector<BoneWeightMap> &weightsPerVertex)
        {
            vector<TriangleCentroidData> result;
            result.reserve(triangles.size());

            for (const auto& t: triangles) {
                TriangleNodes tNodes{vertices[t.alpha], vertices[t.beta], vertices[t.gamma]};
                TriangleCentroidData data;
                data.area = ComputeTriangleArea(tNodes);
                data.averageBoneWeights = ComputeTriangleAverageWeights(weightsPerVertex, t);
                data.centroid = (tNodes.alpha + tNodes.beta + tNodes.gamma)/3.0f;
                result.push_back(data);
            }

            return result;
        }

        bool
        ComputeOptimizedCoR(int vertexIndex, const vector<vec3> &vertices, const vector<BoneWeightMap> &boneWeightsPerVertex, const vector<TriangleCentroidData> &triangleDataPerTriangle, float sigma, vec3 &corOut)
        {
            float totalWeight = 0;
            vec3 posAccumulator;

            for (const auto& t: triangleDataPerTriangle) {

                const auto& wI = boneWeightsPerVertex.at((size_t)vertexIndex);
                const auto& wTriangleAverage = t.averageBoneWeights;

                const auto similarity = ComputeSimilarity(wI, wTriangleAverage, sigma);
                float weight = similarity * t.area;
                totalWeight += weight;

                vec3 pos = t.centroid;
                posAccumulator += pos*weight;
            }

            if (totalWeight > 0) {
                corOut = posAccumulator / totalWeight;
                return true;
            }
            return false;
        }

        BoneWeightMap operator+(const BoneWeightMap& a, const BoneWeightMap& b)
        {
            assert(a.size() == b.size());
            BoneWeightMap result = a;
            for (int i=0; i<result.size(); i++) {
                result[i] += b[i];
            }
            return result;
        }

        BoneWeightMap operator*(const BoneWeightMap& x, float factor)
        {
            BoneWeightMap result = x;
            for (auto& it: result) {
                it *= factor;
            }
            return result;
        }

        void SubdivideEdge(vector<vec3>& verticesInOut, vector<TriangleIndices>& trianglesInOut, vector<BoneWeightMap>& boneWeightsInOut, int triangleIdx, int edgeStart)
        {

            auto triangle = trianglesInOut[triangleIdx];

            auto alpha = triangle[edgeStart];
            auto beta = triangle[(edgeStart+1)%3];
            auto gamma = triangle[(edgeStart+2)%3];

            auto newV = (verticesInOut[alpha] + verticesInOut[beta]) * 0.5f;
            auto newVIdx = int(verticesInOut.size());
            verticesInOut.push_back(newV);

            auto newBoneWeights = (boneWeightsInOut[alpha] + boneWeightsInOut[beta]) * 0.5f;
            boneWeightsInOut.push_back(newBoneWeights);

            trianglesInOut.push_back({alpha, newVIdx, gamma});
            trianglesInOut.push_back({newVIdx, beta, gamma});
        }

        SubdividedMesh
        SubdivideMesh(const vector<vec3> &verticesIn, const vector<TriangleIndices> &trianglesIn, const vector<BoneWeightMap> &boneWeightsPerVertexIn, float epsilon)
        {
            vector<vec3> vertices = verticesIn;
            vector<TriangleIndices> triangles = trianglesIn;
            vector<BoneWeightMap> boneWeightsPerVertex = boneWeightsPerVertexIn;

            for (int triangleIdx=0; triangleIdx<triangles.size(); triangleIdx++) {
                auto triangle = triangles[triangleIdx];
                for (int i=0; i<3; i++) {
                    const auto &w0 = boneWeightsPerVertex[triangle[i]];
                    const auto &w1 = boneWeightsPerVertex[triangle[(i + 1) % 3]];
                    float l2Distance = ComputeL2SkinningWeightDistance(w0, w1);
                    if (l2Distance > epsilon) {
                        SubdivideEdge(vertices, triangles, boneWeightsPerVertex, triangleIdx, i);
                        triangles.erase(triangles.begin() + triangleIdx);
                        --triangleIdx;
                        break;
                    }
                }
            }
            return {vertices, triangles, boneWeightsPerVertex};
        }

    }
}
