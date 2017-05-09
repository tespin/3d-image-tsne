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
    ofSetVerticalSync(true);
    
    string imageDir = "/Users/tespin/Documents/openFrameworks/apps/myApps/00_BatchFeatureEncoder/bin/data/image-set-a-scanner-darkly-2";
    
    string imageSavePath = "test-3d-tsne-scanner-darkly.png";
    
    // development
    //  nx = 15;
    //  ny = 10;
    //  nz = 10;
    
    // test
     nx = 8;
     ny = 8;
     nz = 8;
    
    // still not sure about these numbers?
    initPos = ofPoint(1850, 1850, 2100);
    gridSize = ofPoint(10000, 10000, 10000);
    
    marchingCubes.init(initPos, gridSize, 16, 16, 16);
    
    w = 256;
    h = 256;
    d = 256;
    
    perplexity = 50;
    theta = 0.4;
    
    scale = 2;
    
    sphere.setRadius(50);
    
    cam.setNearClip(0.1);
    cam.setFarClip(50000);
    
    // get images from directory
    ofLog() << "Gathering images...";
    ofDirectory dir = ofDirectory(imageDir);
    scan_dir_imgs(dir);
    std::cout << imageFiles.size() << endl;
    
    // check that there are enough images in directory
    if (imageFiles.size() < nx * ny * nz)
    {
        ofLog(OF_LOG_ERROR, "There are less images in the directory than the grid size requested (nx*ny*nz="+ofToString((nx*ny*nz))+"). Exiting to save you trouble...");
        ofExit(); // not enough images!
    }
    
    // load images
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
        std::vector<float> encoding = ccv.encode(images[i], ccv.numLayers()-1);
        encodings.push_back(encoding);
    }
    
    // run t-SNE and loage image points to imagePoints
    ofLog() << "Run t-SNE on images...";
    tsneVecs = tsne.run(encodings, 3, perplexity, theta, true);
    
    // solve 3D assignment grid
    for (auto t: tsneVecs) tsnePoints.push_back(ofVec3f(t[0], t[1], t[2]));
    gridPoints = makeGrid(nx, ny, nz);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);
    
    // add raw t-SNE data to k-means clusterer
    for (int i = 0; i < NUMIMAGES; i++)
    {
        instances[i].push_back(tsneVecs[i][0]);
        instances[i].push_back(tsneVecs[i][1]);
        instances[i].push_back(tsneVecs[i][2]);
        clusterer.addSample(instances[i]);
    }
    
    // set up clustering and train
    clusterer.setNumClusters(NUMCLUSTERS);
    clusterer.train();
    clusters = clusterer.getClusters();
    
    // assign each instance to a cluster
    for (int i = 0; i < clusters.size(); i++)
    {
        Instance instance;
        instance.assignClusterIndex(clusters[i]);
        
        instanceVector.push_back(instance);
        
    //    cout << "Instance " << ofToString(i) << " " << ofToString(instances[i]) << " assigned to cluster " << ofToString(clusters[i]) << endl;
    }
    
    // populate vector of meshes
    for (int i = 0; i < NUMCLUSTERS; i++)
    {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        meshVector.push_back(mesh);
        
        colors[i] = ofColor(ofRandom(255), ofRandom(255), ofRandom(255)) ;
    }
    
    // populate vector of verts
    for (int i = 0; i < solvedGrid.size(); i++)
    {
        float x = scale * (nx - 1) * w * solvedGrid[i].x;
        float y = scale * (ny - 1) * h * solvedGrid[i].y;
        float z = scale * (nz - 1) * d * solvedGrid[i].z;
        
        ofVec3f pos(x, y, z);
        posVector.push_back(pos);
    }
    
    // populate meshes with verts
    for (int i = 0; i < instanceVector.size(); i++)
    {
        instanceVector[i].setVertex(posVector[i]);
        
        for (int j = 0; j < NUMCLUSTERS; j++)
        {
            if (instanceVector[i].getClusterIndex() == j)
            {
                meshVector[j].addVertex(instanceVector[i].getVertex());
            }
        }
        
//        meshVector[instanceVector[i].getClusterIndex()].addVertex(instanceVector[i].getVertex());
        

        // check cluster
        // std::cout << "Instance " << ofToString(i) << "assigned to actual cluster " << ofToString(instanceVector[i].getClusterIndex()) << std::endl;
        
        // check vertices
        // std::cout << "Cluster:" << instanceVector[i].getClusterIndex() << ", Instance: " << ofToString(i) << ", Vertex: " << ofToString(instanceVector[i].getVertex()) << std::endl;
    }
    
    //now it would be nice to write that data out to a file of some sort if possible
    //for one reason: it takes a long time to test the code - this is worth 1-2 hours of work, because it will save 10 of testing
    
    //option 1: generate a 3d array of Instance Objects, so we have a 3d array of cluster IDs to convert into a voxel array
    
    // check mesh
    // for (int i = 0; i < meshVector.size(); i++)
    // {
    //    std::cout << "Mesh: " << ofToString(i) << ", Verts: " << ofToString(meshVector[i].getVertices());
    // }
    
    initGui();
    setupGui();
}

void ofApp::update()
{
    // cycle through each gui
    for (int i = 0; i < instanceVector.size(); i++)
    {
        for (int j = 0; j < NUMCLUSTERS; j++)
        {
            // for each selected cluster
            if (instanceVector[i].getClusterIndex() == j)
            {
                // if selected cluster is show cubes and model hasn't been rendered
                if (clustersGui[j].showCubes && !clustersGui[j].modelRendered)
                {
                    marchingCubes.resetIsoValues();
                    auto vertices = meshVector[j].getVertices();
                    
                    // add meta ball at vertices of selected mesh
                    for (int k = 0; k < meshVector[j].getNumVertices(); k++)
                    {
                        ofVec3f vertex = vertices.at(k);
                        ofPoint p = ofPoint(vertex.x, vertex.y, vertex.z);
                        marchingCubes.addMetaBall(p, 0.05);
                        // std::cout << "Vertex: " << ofToString(vertex) << " added!" << std::endl;
                        
                    }
                    // std::cout << "Metaballs added!" << std::endl;
                    
                    // std::cout << "Before update: " << ofToString(marchingCubes.getVertices()) << std::endl;
                    
                    // update adds the meshes's verts and apply marching cubes
                    marchingCubes.update(0.0035, true);
                    
                    // std::cout << "After update: " << ofToString(marchingCubes.getVertices()) << std::endl;
                    clustersGui[j].modelRendered = true;
                    
                    
                }
                
                // if selected model is rendered and save is true
                if (clustersGui[j].modelRendered && clustersGui[j].save)
                {
                    meshVector[j].save(ofToDataPath("meshSave.ply"));
                    clustersGui[j].save = false;
                    marchingCubes.saveModel(ofToDataPath("cluster_" + ofToString(j+1) + ".stl"));
                    // std::cout << "Saving cluster " << ofToString(j+1) << "!" << std::endl;
                }
            }
        }
    }
    // std::cout << "Marching cubes verts: " << ofToString(marchingCubes.getVertices()) << std::endl;
    // std::cout << cam.getPosition() << std::endl;
}

void ofApp::draw()
{
    ofBackground(0);
    cam.begin();
    ofEnableDepthTest();
    
    for (int i = 0; i < solvedGrid.size(); i++)
    {
        for (int j = 0; j < NUMCLUSTERS; j++)
        {
            if (instanceVector[i].getClusterIndex() == j)
            {
                // if selected cluster's draw images parameter is true
                if (clustersGui[j].drawImages)
                {
                    ofSetColor(255, 255, 255);
                    images[i].draw(posVector[i], images[i].getWidth(), images[i].getHeight());
                }
                
                // if selected cluster's draw point cloud parameter is true
                if (clustersGui[j].drawPointCloud)
                {
                    ofSetColor(colors[clusters[i]]);
                    sphere.setPosition(posVector[i].x + (images[i].getWidth() / 2) , posVector[i].y + (images[i].getHeight() / 2), posVector[i].z);
                    sphere.draw();
                }
                
                // if selected cluster's draw mesh parameter is true
                if (clustersGui[j].drawMesh)
                {
                    ofSetColor(colors[clusters[i]]);
                    meshVector[j].draw();
                }
                
                // if selected cluster's show cubes parameter is true
                if (clustersGui[j].showCubes)
                {
                    ofSetColor(colors[clusters[i]]);
                    marchingCubes.drawFilled();
                }
                else
                {
                    clustersGui[j].modelRendered = false;
                }
            }
        }
    }
    ofDisableDepthTest();
    cam.end();
    drawGui();
}

void ofApp::initGui()
{
    // for every cluster, create a clustergui and push it back to vector of clusterguis
    for (int i = 0; i < NUMCLUSTERS; i++)
    {
        ClusterGui clusterGui;
        clustersGui.push_back(clusterGui);
    }
}

void ofApp::setupGui()
{
    // set up every gui
    for (int i = 0; i < NUMCLUSTERS; i++)
    {
        clustersGui[i].gui.setup();
        clustersGui[i].gui.setName("Cluster: " + ofToString(i+1));
        
        // if (NUMCLUSTERS > 5)
        // {
            // for (int j = 0; j < 6; j++) clustersGui[j].gui.setPosition(clustersGui[j].gui.getWidth(), clustersGui[j].gui.getHeight() * (j*6));
            // for (int k = 6; k < NUMCLUSTERS; k++) clustersGui[k].gui.setPosition(ofGetWidth() - clustersGui[k].gui.getWidth(), clustersGui[k].gui.getHeight() * (k*6));
        // }
        
        clustersGui[i].gui.setPosition(0, clustersGui[i].gui.getHeight() * (i*6));
        clustersGui[i].gui.add(clustersGui[i].drawImages.set("Draw Image Cluster: " + ofToString(i+1), true));
        clustersGui[i].gui.add(clustersGui[i].drawPointCloud.set("Draw Point Cloud Cluster: " + ofToString(i+1), true));
        clustersGui[i].gui.add(clustersGui[i].drawMesh.set("Draw Mesh", false));
        clustersGui[i].gui.add(clustersGui[i].showCubes.set("Show Cubes", false));
        clustersGui[i].gui.add(clustersGui[i].save.set("Save", false));
    }
}

void ofApp::drawGui()
{
    // draw each gui
    for (int i = 0; i < NUMCLUSTERS; i++) clustersGui[i].gui.draw();
}

void ofApp::keyReleased(int key)
{
    // for selected gui, if 's' key is pressed, save parameter is true
    for (int i = 0; i < instanceVector.size(); i++)
    {
        for (int j = 0; j < NUMCLUSTERS; j++)
        {
            if (instanceVector[i].getClusterIndex() == j)
            {
                if(clustersGui[j].modelRendered)
                {
                    if (key == 's') clustersGui[j].save = true;
                }
            }
        }
    }
}
