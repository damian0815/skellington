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


        void AddBoneToSkeleton(Skeleton *skeleton, const aiScene *aiScene, const aiMesh *aiMesh, const aiBone *bone);

        const aiBone * FindParentBone(const aiScene *aiScene, const aiMesh *aiMesh, const aiString &boneName);
        const aiBone * FindBone(const aiMesh *aiMesh, const aiString &boneName);

        Skeleton *BuildSkeletonFromAssimpMesh(const aiScene *aiScene, aiMesh *aiMesh)
        {
            auto skeleton = new Skeleton();

            for (int i = 0; i < aiMesh->mNumBones; i++) {
                auto bone = aiMesh->mBones[i];

                AddBoneToSkeleton(skeleton, aiScene, aiMesh, bone);
            }

            return skeleton;
        }

        const aiBone * FindParentBone(const aiScene *aiScene, const aiMesh *aiMesh, const aiString &boneName)
        {
            auto node = FindNode(aiScene, boneName);
            auto parentNode = node->mParent;
            auto parentBone = FindBone(aiMesh, parentNode->mName);
            return parentBone;
        }


        void AddBoneToSkeleton(Skeleton *skeleton, const aiScene *aiScene, const aiMesh *aiMesh, const aiBone *bone)
        {
            auto node = FindNode(aiScene, bone->mName);
            auto parentBone = FindParentBone(aiScene, aiMesh, bone->mName);

            auto aiTransformMatrix = node->mTransformation;
            if (parentBone == nullptr) {
                auto armatureTransform = node->mParent->mTransformation;
                aiTransformMatrix = armatureTransform.Inverse() * aiTransformMatrix;
            }

            aiTransformMatrix.Transpose();
            glm::mat4 transformMatrix{
                    aiTransformMatrix.a1, aiTransformMatrix.a2, aiTransformMatrix.a3, aiTransformMatrix.a4,
                    aiTransformMatrix.b1, aiTransformMatrix.b2, aiTransformMatrix.b3, aiTransformMatrix.b4,
                    aiTransformMatrix.c1, aiTransformMatrix.c2, aiTransformMatrix.c3, aiTransformMatrix.c4,
                    aiTransformMatrix.d1, aiTransformMatrix.d2, aiTransformMatrix.d3, aiTransformMatrix.d4,
            };

            auto name = bone->mName.C_Str();
            auto joint = Joint(name, Transform(transformMatrix));

            if (parentBone == nullptr) {
                fmt::print("{0} (root)\n", joint.GetName());
                skeleton->AddRootJoint(joint);

            } else {
                fmt::print("{0} (parent {1})\n", joint.GetName(), parentBone->mName.C_Str());
                skeleton->AddJoint(joint, parentBone->mName.C_Str());
            }

        }

        const aiBone * FindBone(const aiMesh *aiMesh, const aiString &boneName)
        {
            for (int i=0; i<aiMesh->mNumBones; i++) {
                if (aiMesh->mBones[i]->mName == boneName) {
                    return aiMesh->mBones[i];
                }
            }
            return nullptr;
        }


    }
}