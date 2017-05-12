//
//  Element.cpp
//  movie_stills_final
//
//  Created by Tristan Espinoza on 5/4/17.
//
//

#include "Element.h"

void Element::assignClusterIndex(int _clusterIndex)
{
    clusterIndex = _clusterIndex;
}

int Element::getClusterIndex()
{
    return clusterIndex;
}

void Element::setVertex(ofVec3f _vertex)
{
    vertex = _vertex;
}

ofVec3f Element::getVertex()
{
    return vertex;
}
