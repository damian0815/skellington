//
// Created by Damian Stewart on 15/10/16.
//

#include "AssimpLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <fmt/ostream.h>
#include <iostream>

std::ostream& operator<<(std::ostream& o, const aiString& str)
{
    return o << str.C_Str();
}

namespace skellington
{
    namespace AssimpLoader
    {
        Skeleton *BuildSkeletonFromAssimpMesh(const aiNode *aiMeshNode, const aiMesh *aiMesh);
        Mesh* BuildMeshFromAssimpMesh(const struct aiMesh* aiMesh);
        void CreateVertexGroups(const struct aiMesh *aiMesh, Mesh *mesh);



        void DumpSceneTree(aiNode* startNode, const string& prefex = "")
        {
            fmt::print("{0}{1}\n", prefex, startNode->mName);
            for (int i=0; i<startNode->mNumChildren; i++) {
                DumpSceneTree(startNode->mChildren[i], prefex + "  ");
            }
        }

        struct aiNode *FindMeshNode(const aiScene *aiScene, aiString meshName);

        bool Load(string path, Mesh **meshOut, Skeleton **skeletonOut)
        {
            Assimp::Importer importer;

            *meshOut = nullptr;
            *skeletonOut = nullptr;

            auto options = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
            auto aiScene = importer.ReadFile(path, options);

            if (aiScene == nullptr) {
                fmt::print(stderr, "Can't read {0}:\n  {1}\n", path, importer.GetErrorString());
                return false;
            }

            fmt::print("Loaded scene from {0}:\n", path);
            DumpSceneTree(aiScene->mRootNode);

            if (aiScene->mNumMeshes != 1) {
                fmt::print(stderr, "Scene must have exactly 1 mesh\n");
                return false;
            }
            
            const auto aiMesh = aiScene->mMeshes[0];
            const auto meshNode = FindMeshNode(aiScene, aiMesh->mName);

            *meshOut = BuildMeshFromAssimpMesh(aiMesh);
            *skeletonOut = BuildSkeletonFromAssimpMesh(meshNode, aiMesh);
            CreateVertexGroups(aiMesh, *meshOut);

            return true;
        }

        struct aiNode *FindMeshNode(const aiScene *aiScene, aiString meshName)
        {
            for (int i=0; i<aiScene->mRootNode->mNumChildren; i++) {
                auto node = aiScene->mRootNode->mChildren[i];

            }

            return nullptr;
        }

        vector<Mesh::WeightedVectorIndex> GetWeightedVectorIndices(const aiBone *bone);
        void CreateVertexGroups(const struct aiMesh *aiMesh, Mesh *mesh)
        {
            for (int i = 0; i < aiMesh->mNumBones; i++) {
                auto bone = aiMesh->mBones[i];

                vector<Mesh::WeightedVectorIndex> vertexWeights = GetWeightedVectorIndices(bone);
                mesh->AddVertexGroup(bone->mName.C_Str(), vertexWeights);

            }
        }

        vector<Mesh::WeightedVectorIndex> GetWeightedVectorIndices(const aiBone *bone)
        {
            vector<Mesh::WeightedVectorIndex> vertexWeights;
            for (int j=0; j<bone->mNumWeights; j++) {
                    auto w = bone->mWeights[j];
                    vertexWeights.push_back({(int)w.mVertexId, w.mWeight});
                }
            return vertexWeights;
        }


        vector<vec3> GetVertices(const struct aiMesh *aiMesh);
        vector<int> GetTriangles(const struct aiMesh *aiMesh);

        Mesh *BuildMeshFromAssimpMesh(const struct aiMesh *aiMesh)
        {
            auto vertices = GetVertices(aiMesh);
            auto triangles = GetTriangles(aiMesh);

            return new Mesh(vertices, triangles);
        }

        vector<vec3> GetVertices(const struct aiMesh *aiMesh)
        {
            vector<vec3> vertices;
            for (int i=0; i < aiMesh->mNumVertices; i++) {
                auto v = aiMesh->mVertices[i];
                vertices.push_back(vec3(v.x, v.y, v.z));
            }
            return vertices;
        }

        vector<int> GetTriangles(const aiMesh *aiMesh)
        {
            vector<int> triangles;
            for (int i=0; i<aiMesh->mNumFaces; i++) {
                auto f = aiMesh->mFaces[i];
                assert(f.mNumIndices == 3);
                for (int j=0; j<3; j++) {
                    triangles.push_back(f.mIndices[j]);
                }
            }
            return triangles;
        }



        string AddBoneToSkeleton(Skeleton *skeleton, const aiBone *bone);

        Skeleton *BuildSkeletonFromAssimpMesh(const aiNode *aiMeshNode, const aiMesh *aiMesh)
        {
            auto skeleton = new Skeleton();
            for (int i = 0; i < aiMesh->mNumBones; i++) {
                auto bone = aiMesh->mBones[i];

                auto jointName = AddBoneToSkeleton(skeleton, bone);
                auto node = aiMeshNode->FindNode(jointName.c_str());
                fmt::print("node {0} has parent {1}", node->mName, (node->mParent ? node->mParent->mName.C_Str() : "<null>"));
                //skeleton.SetJointParent(jointName, parentName);

            }

            return skeleton;
        }

        string AddBoneToSkeleton(Skeleton *skeleton, const aiBone *bone)
        {
            auto aiFrameMatrix = bone->mOffsetMatrix;
            aiFrameMatrix.Transpose();

            glm::mat4 frameMatrix{
                    aiFrameMatrix.a1, aiFrameMatrix.a2, aiFrameMatrix.a3, aiFrameMatrix.a4,
                    aiFrameMatrix.b1, aiFrameMatrix.b2, aiFrameMatrix.b3, aiFrameMatrix.b4,
                    aiFrameMatrix.c1, aiFrameMatrix.c2, aiFrameMatrix.c3, aiFrameMatrix.c4,
                    aiFrameMatrix.d1, aiFrameMatrix.d2, aiFrameMatrix.d3, aiFrameMatrix.d4,
            };

            auto name = bone->mName.C_Str();
            skeleton->AddJoint(Joint(name, Transform(frameMatrix)));
            return name;
        }


    }
}