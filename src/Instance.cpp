//
//  Cluster.cpp
//  movie-stills
//
//  Created by Tristan Espinoza on 5/3/17.
//
//

#include "Instance.h"

void Instance::setVertex(ofVec3f _vec3f)
{
    vertex = _vec3f;
}

void Instance::setClusterIndex(int _cluster)
{
    clusterIndex = _cluster;
}

void Instance::setClusterImage(ofImage _image)
{
    image = _image;
}

ofVec3f Instance::getVertex()
{
    return vertex;
}

int Instance::getClusterIndex()
{
    return clusterIndex;
}
