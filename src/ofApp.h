#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxLearn.h"
#include "ofxGui.h"
#include "ofxMarchingCubes.h"
#include "Instance.h"

//#define NUMIMAGES 1500
//#define NUMCLUSTERS 11

#define NUMIMAGES 512
#define NUMCLUSTERS 4

struct ClusterGui
{
    ofxPanel gui;
    ofParameter<bool> drawImages;
    ofParameter<bool> drawPointCloud;
    ofParameter<bool> drawMesh;
    ofParameter<bool> showCubes;
    ofParameter<bool> modelRendered;
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
        
        void keyReleased(int key);
        
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
        
        ofPoint initPos, gridSize;
        
        ofEasyCam cam;
    
        ofxAssignment solver;
        
        std::vector<ofFile> imageFiles;
        std::vector<ofImage> images;
        std::vector<std::vector<float> > encodings;
        std::vector<std::vector<ofxCcv::Classification> > results;
        std::vector<std::vector<double> > tsneVecs;
        std::vector<ofVec3f> solvedGrid, gridPoints;
        std::vector<ofVec3f> initial, grid;
        
        std::vector<Instance> instanceVector;
        
        std::vector<ofVec3f> posVector;
        
        std::vector<ofMesh> meshVector;
        
        ofSpherePrimitive sphere;
        
        float xPos, yPos, zPos;
        
        int nx, ny, nz;
        int w, h, d;
        float perplexity, theta;
        float scale;
    

};
