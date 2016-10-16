//
// Created by Damian Stewart on 15/10/16.
//

#ifndef SKELLINGTON_ASSIMPLOADER_H
#define SKELLINGTON_ASSIMPLOADER_H

#include <assimp/mesh.h>
#include "Mesh.h"
#include "Skeleton.h"

namespace skellington
{

    namespace AssimpLoader
    {
        bool Load(string path, Mesh **meshOut, Skeleton **skeletonOut);
    };

};


#endif //SKELLINGTON_ASSIMPLOADER_H
