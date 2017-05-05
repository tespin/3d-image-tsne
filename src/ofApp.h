#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxLearn.h"
#include "ofxGui.h"
#include "ofxMarchingCubes.h"

//#define NUMIMAGES 1500
//#define NUMCLUSTERS 11

#define NUMIMAGES 512
#define NUMCLUSTERS 4

struct ClusterGui
{
    ofxPanel gui;
    ofParameter<bool> drawImages;
    ofParameter<bool> drawPointCloud;
    ofParameter<bool> save;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    void initGui();
    void setupGui();
    void drawGui();
    
    void scan_dir_imgs(ofDirectory dir);
    
    std::vector<ClusterGui> clustersGui;
    
    ofxCcv ccv;
    
    ofxTSNE tsne;
    std::vector<ofVec3f> tsnePoints;
    
    ofxLearnKMeans clusterer;
    std::vector<double> instances[NUMIMAGES];
    std::vector<int> clusters;
    ofColor colors[NUMCLUSTERS];
    
    ofxMarchingCubes marchingCubes;
    
    ofEasyCam cam;
    
    ofxAssignment solver;
    
    std::vector<ofFile> imageFiles;
    std::vector<ofImage> images;
    std::vector<std::vector<float> > encodings;
    std::vector<std::vector<ofxCcv::Classification> > results;
    std::vector<std::vector<double> > tsneVecs;
    std::vector<ofVec3f> solvedGrid, gridPoints;
    
    std::vector<ofVec3f> initial, grid;
    
    ofSpherePrimitive sphere;
    
    int nx, ny, nz;
    int w, h, d;
    float perplexity, theta;
    float scale;

};
