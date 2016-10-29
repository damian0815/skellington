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
#include <deque>

std::ostream& operator<<(std::ostream& o, const aiString& str)
{
    return o << str.C_Str();
}

namespace skellington
{
    namespace AssimpLoader
    {
        Skeleton *BuildSkeletonFromAssimpNodeTree(aiNode *skeletonRootNode);
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

            const string SKELETON_NODE_NAME = "Armature";
            *meshOut = BuildMeshFromAssimpMesh(aiMesh);
            *skeletonOut = BuildSkeletonFromAssimpNodeTree(FindNode(aiScene, aiString(SKELETON_NODE_NAME)));
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


        void AddBoneToSkeleton(Skeleton *skeleton, aiString parentBoneName, aiNode *node);
        glm::mat4 GetGLMTransformFromAITransform(aiMatrix4x4 aiTransform);

        Skeleton *BuildSkeletonFromAssimpNodeTree(aiNode *skeletonRootNode)
        {
            auto skeleton = new Skeleton();

            std::deque<aiNode*> nodes;
            nodes.push_back(skeletonRootNode);
            skeleton->SetRootJointName(skeletonRootNode->mName.C_Str());

            while (!nodes.empty()) {
                auto parent = nodes.front();
                nodes.pop_front();
                for (int i=0; i<parent->mNumChildren; i++) {
                    auto child = parent->mChildren[i];
                    AddBoneToSkeleton(skeleton, parent->mName, child);
                    nodes.push_back(child);
                }
            }

            return skeleton;
        }

        void AddBoneToSkeleton(Skeleton *skeleton, aiString parentBoneName, aiNode *node)
        {
            auto transform = GetGLMTransformFromAITransform(node->mTransformation);
            Joint j(node->mName.C_Str(), transform);
            skeleton->AddJoint(j, parentBoneName.C_Str());
        }

        glm::mat4 GetGLMTransformFromAITransform(aiMatrix4x4 aiTransform)
        {
            aiTransform.Transpose();
            mat4 glmTransform{
                    aiTransform.a1, aiTransform.a2, aiTransform.a3, aiTransform.a4,
                    aiTransform.b1, aiTransform.b2, aiTransform.b3, aiTransform.b4,
                    aiTransform.c1, aiTransform.c2, aiTransform.c3, aiTransform.c4,
                    aiTransform.d1, aiTransform.d2, aiTransform.d3, aiTransform.d4,
            };
            return glmTransform;
        }


    }
}