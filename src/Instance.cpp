//
//  Instance.cpp
//  movie_stills_final
//
//  Created by Tristan Espinoza on 5/4/17.
//
//

#include "Instance.h"

void Instance::assignClusterIndex(int _clusterIndex)
{
    clusterIndex = _clusterIndex;
}

int Instance::getClusterIndex()
{
    return clusterIndex;
}

void Instance::setVertex(ofVec3f _vertex)
{
    vertex = _vertex;
}

ofVec3f Instance::getVertex()
{
    return vertex;
}
