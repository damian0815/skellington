//
// Created by Damian Stewart on 15/10/16.
//

#include <OpenGL/gl.h>
#include "Mesh.h"

namespace skellington
{
    static void glVertex3f(const vec3& v)
    {
        ::glVertex3f(v.x, v.y, v.z);
    }

    void Mesh::AddVertexGroup(string name, const vector<skellington::Mesh::WeightedVectorIndex> weightedVertices)
    {
        mVertexGroups[name] = weightedVertices;
    }

    void Mesh::DrawWireframe()
    {
        for (int triangleIndex=0; triangleIndex<mTriangles.size()/3; triangleIndex++) {
            glBegin(GL_LINE_LOOP);
            for (int i=0; i<3; i++) {
                glVertex3f(mVertices[mTriangles[triangleIndex*3 + i]]);
            }
            glEnd();
        }
    }

}