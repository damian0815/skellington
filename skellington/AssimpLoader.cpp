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
        Skeleton *BuildSkeletonFromAssimpMesh(const aiScene *aiScene, aiMesh *aiMesh);
        Mesh* BuildMeshFromAssimpMesh(const struct aiMesh* aiMesh);
        void CreateVertexGroups(const struct aiMesh *aiMesh, Mesh *mesh);



        void DumpSceneTree(aiNode* startNode, const string& prefex = "")
        {
            fmt::print("{0}{1}\n", prefex, startNode->mName);
            for (int i=0; i<startNode->mNumChildren; i++) {
                DumpSceneTree(startNode->mChildren[i], prefex + "  ");
            }
        }


        aiNode * FindNodeRecursive(aiNode *node, aiString nodeName)
        {
            if (node->mName == nodeName) {
                return node;
            }

            for (int i=0; i<node->mNumChildren; i++) {
                auto child = node->mChildren[i];
                auto foundNode = FindNodeRecursive(child, nodeName);
                if (foundNode != nullptr) {
                    return foundNode;
                }
            }

            return nullptr;
        }

        aiNode* FindNode(const aiScene* scene, aiString nodeName)
        {
            return FindNodeRecursive(scene->mRootNode, nodeName);
        }




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

            *meshOut = BuildMeshFromAssimpMesh(aiMesh);
            *skeletonOut = BuildSkeletonFromAssimpMesh(aiScene, aiMesh);
            CreateVertexGroups(aiMesh, *meshOut);

            return true;
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


        void AddBoneToSkeleton(Skeleton *skeleton, const aiBone *bone);

        Skeleton *BuildSkeletonFromAssimpMesh(const aiScene *aiScene, aiMesh *aiMesh)
        {
            auto skeleton = new Skeleton();
            for (int i = 0; i < aiMesh->mNumBones; i++) {
                auto bone = aiMesh->mBones[i];

                AddBoneToSkeleton(skeleton, bone);
            }

            for (int i=0; i<aiMesh->mNumBones; i++) {
                auto bone = aiMesh->mBones[i];
                auto node = FindNode(aiScene, bone->mName);
                auto parent = node->mParent;
                if (skeleton->HasJoint(parent->mName.C_Str())) {
                    fmt::print("node {0} has parent {1}\n", node->mName, node->mParent->mName);
                    skeleton->SetJointParent(node->mName.C_Str(), parent->mName.C_Str());
                }
            }

            return skeleton;
        }

        void AddBoneToSkeleton(Skeleton *skeleton, const aiBone *bone)
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
        }


    }
}