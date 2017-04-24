#include "ofApp.h"

const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};

void ofApp::scan_dir_imgs(ofDirectory dir)
{
    ofDirectory new_dir;
    
    // size is equal to # of files in directory
    int size = dir.listDir();
    
    // for every file
    for (int i = 0; i < size; i++)
    {
        if (dir.getFile(i).isDirectory())
        {
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            scan_dir_imgs(new_dir);
        }
        else if (std::find(std::begin(allowed_ext), std::end(allowed_ext), dir.getFile(i).getExtension()) != std::end(allowed_ext))
        {
            imageFiles.push_back(dir.getFile(i));
        }
    }
}

void ofApp::setup()
{
    string imageDir = "/Users/tespin/Documents/openFrameworks/apps/myApps/00_BatchFeatureEncoder/bin/data/image-set-a-scanner-darkly-2";
    
    string imageSavePath = "test-3d-tsne-scanner-darkly.png";
    
//    nx = 40;
    nx = 10;
    ny = 10;
    nz = 10;
    
    w = 256;
    h = 256;
    
    displayW = 100;
    displayH = 100;
    
    perplexity = 50;
    theta = 0.4;
    
    scaleX = ofGetWidth() / 2;
    scaleY = ofGetHeight() / 2;
    scale = 2;
    
//    cam.setFarClip(5);
    
    // get images from directory
    ofLog() << "Gathering images...";
    ofDirectory dir = ofDirectory(imageDir);
    scan_dir_imgs(dir);
    std::cout << imageFiles.size() << endl;
    
    if (imageFiles.size() < nx * ny * nz)
    {
        ofLog(OF_LOG_ERROR, "There are less images in the directory than the grid size requested (nx*ny*nz="+ofToString((nx*ny*nz))+"). Exiting to save you trouble...");
        ofExit(); // not enough images!
    }
    
    // load all images
    for (int i = 0; i < nx * ny * nz; i++)
    {
        if (i % 20 == 0) ofLog() << " - loading image " << i << " / " << nx * ny * nz << " (" << dir.size() << " in dir)";
        images.push_back(ofImage());
        images.back().load(imageFiles[i].getAbsolutePath());
    }
    
    // resize images to w x h
    for (int i = 0; i < images.size(); i++)
    {
        if (images[i].getWidth() > images[i].getHeight())
        {
            images[i].crop((images[i].getWidth()-images[i].getHeight()) * 0.5, 0, images[i].getHeight(), images[i].getHeight());
        }
        else if (images[i].getHeight() > images[i].getWidth())
        {
            images[i].crop(0, (images[i].getHeight()-images[i].getWidth()) * 0.5, images[i].getWidth(), images[i].getWidth());
        }
        images[i].resize(w, h);
    }
    
    // setup ccv
    ccv.setup("image-net-2012.sqlite3");
    
    // encode images with ccv
    ofLog() << "Encoding images...";
    for (int i = 0; i < images.size(); i++)
    {
        if (i % 20 == 0) ofLog() << " - encoding image " << i << " / " <<images.size();
        vector<float> encoding = ccv.encode(images[i], ccv.numLayers()-1);
        encodings.push_back(encoding);
    }
    
    // run t-SNE and load image points to imagePoints
    ofLog() << "Run t-SNE on images";
    tsneVecs = tsne.run(encodings, 3, perplexity, theta, true);
    
    // solve 3D assignment grid
    vector<ofVec3f> tsnePoints;
    for (auto t: tsneVecs) tsnePoints.push_back(ofVec3f(t[0], t[1], t[2]));
    vector<ofVec3f> gridPoints = makeGrid(nx, ny, nz);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);
    
    // save points
    
    
}

void ofApp::update()
{
 

}

void ofApp::draw()
{
    cam.begin();
    
//    float scaleX = ofGetWidth() / 2;
//    float scaleY = ofGetHeight() / 2;
    
    ofBackground(0);
    
    ofSetColor(255);
    ofFill();
    ofDrawEllipse(50, 50, 50, 50);
    
//    ofPushMatrix();
//    ofTranslate(scaleX, scaleY, scaleX);
//    ofScale(scaleY, scaleY, scaleY);
    
    for (int i = 0; i < solvedGrid.size(); i++)
    {
        float x = scale * (nx - 1) * w * solvedGrid[i].x;
        float y = scale * (ny - 1) * h * solvedGrid[i].y;
        float z = scale * (nz - 1) * 256 * solvedGrid[i].z;
        
        images[i].draw(x, y, z, images[i].getWidth(), images[i].getHeight());
//        for (int x = 0; x < nx; x++)
//        {
//            for (int y = 0; y < ny; y++)
//            {
//                for (int z = 0; z < nz; z++)
//                {
//                    images[i].draw(x, y, z, scaleX * images[i].getWidth(), scaleY * images[i].getHeight());
//                }
//            }
//        }
        std::cout << "x: " << x << " , y: " << y << " , z: " << z << endl;
    }
    
//    ofPopMatrix();
    
//    for (int x = 0; x < nx; x++)
//    {
//        for (int y = 0; y < ny; y++)
//        {
//            for (int z = 0; z < nz; z++)
//            {
//                ofPushMatrix();
//                ofTranslate(x, y, z);
//                )
//            }
//        }
//    }
    
    cam.end();
    
}
