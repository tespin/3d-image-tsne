#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    void scan_dir_imgs(ofDirectory dir);
    
    struct Cluster
    {
        // details on cluster ...
        // expose features?
        
    };
    
    
    ofxCcv ccv;
    
    ofxTSNE tsne;
    vector<ofVec3f> tsnePoints;
    
    ofEasyCam cam;
    ofxAssignment solver;
    
    vector<ofFile> imageFiles;
    vector<ofImage> images;
    vector<vector<float>> encodings;
    vector<vector<ofxCcv::Classification>> results;
    vector<ofxCcv::Classification> classification;
    vector<vector<double>> tsneVecs;
    vector<ofVec3f> solvedGrid, gridPoints;
    
    vector<ofVec3f> initial, grid;
    
    int nx, ny, nz;
    int w, h;
    int displayW, displayH;
    float perplexity, theta;
    float scaleX, scaleY, scale;
    
    bool runManually;
		
};
