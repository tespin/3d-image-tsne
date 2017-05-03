#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxLearn.h"
#include "ofxGui.h"
#include "ofxMarchingCubes.h"
#include "Cluster.h"

//#define NUMIMAGES 1500
//#define NUMCLUSTERS 11

#define NUMIMAGES 512
#define NUMCLUSTERS 4

struct ClusterStruct
{
    ofImage image;
    int clusterIndex;
    
    std::vector<ofVec3f> vertices;
};

struct ClusterGui
{
    ofxPanel gui;
    ofParameter<bool> drawImages;
    ofParameter<bool> drawPointCloud;
    ofParameter<bool> save;
//    ofxButton saveButton;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void initGui();
        void setupGui();
        void drawGui();
    
//        void keyReleased(int key);
    
        void scan_dir_imgs(ofDirectory dir);
    
        void saveButtonPressed();
    
//    void saveToSTL(int _clusters);
//    void passToCluster(int _clusterIndex);
    
        std::vector<Cluster> clusterVector;
    
        std::vector<ClusterStruct> imageClusters;
        std::vector<ClusterGui> clustersGui;
        
        ofxCcv ccv;
        
        ofxTSNE tsne;
        vector<ofVec3f> tsnePoints;
        
        ofxLearnKMeans clusterer;
        vector<double> instances[NUMIMAGES];
        vector<int> clusters;
        ofColor colors[NUMCLUSTERS];
    
        ofxMarchingCubes marchingCubes;
        ofMesh mesh;
    
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
        
        ofSpherePrimitive sphere;
        
        int nx, ny, nz;
        int w, h, d;
        float perplexity, theta;
        float scale;
    
    int clusterIndex;
    
        int buttonPress;
    
        bool runManually;
    
    bool showCubes;
    bool modelRendered;
    bool saveModel;
		
};
