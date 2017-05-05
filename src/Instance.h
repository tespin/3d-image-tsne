//
//  Instance.hpp
//  movie_stills_final
//
//  Created by Tristan Espinoza on 5/4/17.
//
//

#pragma once

#include "ofMain.h"

class Instance
{
    public:
    
        void assignClusterIndex(int _clusterIndex);
        int getClusterIndex();
        
        int clusterIndex;
};
