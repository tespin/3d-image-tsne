//
//  Cluster.cpp
//  movie-stills
//
//  Created by Tristan Espinoza on 5/3/17.
//
//

#include "Cluster.h"

void Cluster::setVertex(ofVec3f _vec3f)
{
    vertex = _vec3f;
}

void Cluster::setClusterIndex(int _cluster)
{
    clusterIndex = _cluster;
}

void Cluster::setClusterImage(ofImage _image)
{
    image = _image;
}

ofVec3f Cluster::getVertex()
{
    return vertex;
}

int Cluster::getClusterIndex()
{
    return clusterIndex;
}
