//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_MESH_H
#define SKELLINGTON_MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <map>
#include <string>

using glm::vec3;
using std::vector;
using std::string;
using std::map;

namespace skellington {

    class Mesh
    {
    public:

        Mesh(const vector<vec3> &vertices, const vector<int> &triangles)
                : mVertices(vertices), mTriangles(triangles) {}

        struct WeightedVectorIndex
        {
            int mVertexIndex;
            float mWeight;
        };
        void AddVertexGroup(string name, const vector<WeightedVectorIndex> weightedVertices);

    private:
        vector<vec3> mVertices;
        vector<int> mTriangles;

        map<string, vector<WeightedVectorIndex>> mVertexGroups;
    };

}

#endif //SKELLINGTON_MESH_H
