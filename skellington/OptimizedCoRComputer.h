//
// Created by Damian Stewart on 29/10/2016.
//

#ifndef SKELLINGTON_OPTIMIZEDCORCOMPUTER_H
#define SKELLINGTON_OPTIMIZEDCORCOMPUTER_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>
#include "Mesh.h"
#include "Skeleton.h"

using glm::vec3;
using std::vector;
using std::map;
using std::string;

namespace skellington
{
    namespace OptimizedCoRComputer
    {

        map<int, vec3>
        ComputeOptimizedCoRs(skellington::Mesh *mesh, skellington::Skeleton *skeleton, float subdivisionEpsilon=0.1f, float similaritySigma=0.1f);
    }
}

#endif //SKELLINGTON_OPTIMIZEDCORCOMPUTER_H
