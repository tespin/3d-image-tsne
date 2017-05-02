#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxLearn.h"
#include "ofxGui.h"

//#define NUMIMAGES 1500
//#define NUMCLUSTERS 11

#define NUMIMAGES 512
#define NUMCLUSTERS 3

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void initGui();
        void setupGui();
        void drawGui();
        
        void scan_dir_imgs(ofDirectory dir);
    
        void saveButtonPressed();
    
        struct Cluster
        {
            ofImage image;
            int clusterIndex;
        };
        
        std::vector<Cluster> imageClusters;
        
        struct ClusterGui
        {
            ofxPanel gui;
            ofParameter<bool> drawImages;
            ofParameter<bool> drawPointCloud;
            ofxButton saveButton;
        };
        
        std::vector<ClusterGui> clustersGui;
        
        ofxCcv ccv;
        
        ofxTSNE tsne;
        vector<ofVec3f> tsnePoints;
        
        ofxLearnKMeans clusterer;
        vector<double> instances[NUMIMAGES];
        vector<int> clusters;
        ofColor colors[NUMCLUSTERS];
        
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
        
        ofxPanel guiImages;
        ofxPanel guiPointClouds;
        
        ofParameter<bool> imagesDraw;
        ofParameter<bool> pointCloudsDraw;
        ofParameter<float> testFloat;
        
        ofSpherePrimitive sphere;
        
        int nx, ny, nz;
        int w, h, d;
        float perplexity, theta;
        float scale;
    
        int buttonPress;
    
        bool runManually;
		
};
