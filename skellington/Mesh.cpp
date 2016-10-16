//
// Created by Damian Stewart on 15/10/16.
//

#include "Mesh.h"

namespace skellington
{

    void Mesh::AddVertexGroup(string name, const vector<skellington::Mesh::WeightedVectorIndex> weightedVertices)
    {
        mVertexGroups[name] = weightedVertices;
    }

}