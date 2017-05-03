//
//  Cluster.hpp
//  movie-stills
//
//  Created by Tristan Espinoza on 5/3/17.
//
//

#pragma once

#include "ofMain.h"

class Cluster
{
    
    public:
    
        void setVertex(ofVec3f _vec3f);
        void setClusterIndex(int _cluster);
        void setClusterImage(ofImage _image);
    
        ofVec3f getVertex();
        int getClusterIndex();
    
        ofImage image;
        int clusterIndex;
        ofVec3f vertex;
    
};
