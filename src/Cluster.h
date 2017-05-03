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
    
        void setClusterIndex(int _cluster);
        void setClusterImage(ofImage _image);
    
        std::vector<ofVec3f> getVerts();
        int getClusterIndex();
    
        ofImage image;
        int clusterIndex;
        std::vector <ofVec3f> vertices;
    
};
