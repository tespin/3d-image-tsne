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
    
    struct TestPoint
    {
        int class_;
        ofColor color;
        vector<float> point;
        ofPoint tsnePoint;
    };
    
    
    ofxCcv ccv;
    
    ofxTSNE tsne;
    vector<TestPoint> testPoints;
    vector<vector<double>> tsnePoints;
    
    ofEasyCam cam;
    ofxAssignment solver;
    
    vector<ofFile> imageFiles;
    vector<ofImage> images;
    vector<vector<float>> encodings;
    vector<vector<double>> tsneVecs;
    vector<ofVec3f> solvedGrid;
    
    vector<ofVec3f> initial, grid;
    
    int nx, ny, nz;
    int w, h;
    int displayW, displayH;
    float perplexity, theta;
    float scaleX, scaleY, scale;
    
    bool runManually;
		
};
