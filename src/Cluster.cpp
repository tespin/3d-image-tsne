//
//  Cluster.cpp
//  movie-stills
//
//  Created by Tristan Espinoza on 5/3/17.
//
//

#include "Cluster.h"

void Cluster::setClusterIndex(int _cluster)
{
    clusterIndex = _cluster;
}

void Cluster::setClusterImage(ofImage _image)
{
    image = _image;
}

std::vector<ofVec3f> Cluster::getVerts()
{
    return vertices;
}

int Cluster::getClusterIndex()
{
    return clusterIndex;
}
