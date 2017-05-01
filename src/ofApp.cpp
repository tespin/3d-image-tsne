#include "ofApp.h"

// how to define clusters?
// how to check for points inside a shape

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
    
    // test
    nx = 8;
    ny = 8;
    nz = 8;
    
    // development
//    nx = 10;
//    ny = 10;
//    nz = 10;
    
    w = 256;
    h = 256;
    d = 256;
    
    perplexity = 50;
    theta = 0.3;
    
//    ofEnableDepthTest();
    
    scale = 2;
    
    cam.setNearClip(0.1);
    cam.setFarClip(50000);
    
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
        if (i % 20 == 0) ofLog() << " - encoding image " << i << " / " << images.size();
        vector<float> encoding = ccv.encode(images[i], ccv.numLayers()-1);
        encodings.push_back(encoding);
    }
    
    // run t-SNE and load image points to imagePoints
    ofLog() << "Run t-SNE on images";
    tsneVecs = tsne.run(encodings, 3, perplexity, theta, true);
    
    // solve 3D assignment grid
    for (auto t: tsneVecs) tsnePoints.push_back(ofVec3f(t[0], t[1], t[2]));
    gridPoints = makeGrid(nx, ny, nz);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);
    
    sphere.setRadius(50);
    
    for (int i = 0; i < NUMIMAGES; i++)
    {
        instances[i].push_back(tsneVecs[i][0]);
        instances[i].push_back(tsneVecs[i][1]);
        instances[i].push_back(tsneVecs[i][2]);
        clusterer.addSample(instances[i]);
        
        
    }
    
    clusterer.setNumClusters(NUMCLUSTERS);
    clusterer.train();
    clusters = clusterer.getClusters();
    
    
    for (int i = 0; i < clusters.size(); i++) {
        Cluster cluster;
        
        cluster.clusterIndex = clusters[i];
        
        for (int j = 0; j < images.size(); j++)
        {
            cluster.image = images[j];
        }
        
        imageClusters.push_back(cluster);
        
        std::cout << "Image: " << i << " Assigned to: " << imageClusters[i].clusterIndex << " Actual cluster: " << clusters[i] << std::endl;
        
//        cout << "Instance " << ofToString(i) << " " << ofToString(instances[i]) << " assigned to cluster " << ofToString(clusters[i]) << endl;
    }
    
    for (int i = 0; i < NUMCLUSTERS; i++) {
        colors[i] = ofColor( ofRandom(255), ofRandom(255), ofRandom(255) );
    }
    
    // find cluster -> iterate through vertices -> check for verts inside mesh -> save out as obj
    // save points?
    setupGui();
}

void ofApp::update()
{

}

void ofApp::draw()
{
    cam.begin();
    ofEnableDepthTest();

    ofBackground(0);
    
    float t = ofMap(cos(ofGetElapsedTimef()), -1, 1, 0, 1);
    
    for (int i = 0; i < solvedGrid.size(); i++)
    {
        float x = scale * (nx - 1) * w * solvedGrid[i].x;
        float y = scale * (ny - 1) * h * solvedGrid[i].y;
        float z = scale * (nz - 1) * d * solvedGrid[i].z;
        
        if (imagesDraw)
        {
        ofSetColor(255, 255, 255);
            
            if (imageClusters[i].clusterIndex == 1)
            {
                imageClusters[i].image.draw(x, y, z, imageClusters[i].image.getWidth(), imageClusters[i].image.getHeight());
   
            }
            
//        images[i].draw(x, y, z, images[i].getWidth(), images[i].getHeight());
        }
        
        if (pointCloudsDraw)
        {
        ofSetColor(colors[clusters[i]]);
        sphere.setPosition(x + (images[i].getWidth() / 2) , y + (images[i].getHeight() / 2), z);
        sphere.draw();
        }

//        std::cout << "x: " << x << " , y: " << y << " , z: " << z << endl;
    }
    cam.end();
    ofDisableDepthTest();
    drawGui();
    
}

void ofApp::setupGui()
{
    guiImages.setup();
    guiImages.setPosition(0, 0);
    guiImages.add(imagesDraw.set("Draw Images", true));
    
    guiPointClouds.setup();
    guiPointClouds.setPosition(guiImages.getPosition().x, guiImages.getHeight());
    guiPointClouds.add(pointCloudsDraw.set("Draw Point Clouds", true));
}

void ofApp::drawGui()
{
    guiImages.draw();
    guiPointClouds.draw();
}
