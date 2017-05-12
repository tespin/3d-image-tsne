#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxLearn.h"
#include "ofxGui.h"
#include "ofxMarchingCubes.h"
#include "ofxIO.h"
#include "Element.h"
#include "Poco/BinaryWriter.h"
#include "Poco/BinaryReader.h"
#include "Poco/DeflatingStream.h"

//#define NUMIMAGES 1500
//#define NUMCLUSTERS 11

#define NUMIMAGES 512
#define NUMCLUSTERS 5

class CCVEncoder
{
    public:
        CCVEncoder(const std::string& network, std::vector<std::size_t> layersToEncode):
            _network(network),
            _isRunning(true),
            _layersToEncode(layersToEncode)
        {
        }
    
        ~CCVEncoder()
        {
            stop();
            
            if (_thread.joinable())
            {
                try
                {
                    _thread.join();
                }
                catch (const std::exception& exc)
                {
                    std::cout << exc.what() << std::endl;
                }
            }
        }
        
        void start()
        {
            _thread = std::thread(&CCVEncoder::run, this);
        }
        
        void stop()
        {
            _isRunning = false;
            _channel.close();
        }
        
        bool encode(const std::filesystem::path& file)
        {
            return _channel.send(file);
        }
        
        std::size_t size() const
        {
            return _channel.size();
        }
    
    private:
        void run()
        {
            _isRunning = true;
            
            while (_isRunning)
            {
                std::filesystem::path path;
                
                if (_channel.receive(path))
                {
                    for (auto& layer: _layersToEncode)
                    {
                        std::stringstream newExtension;
                        newExtension << "." << _network << "." << layer << ".dat";
                        
                        std::filesystem::path file = std::filesystem::path(path);
                        std::filesystem::path encodedFile = file;
                        encodedFile += newExtension.str();
                        
                        if (!_forceEncode && std::filesystem::exists(encodedFile))
                        {
                            continue;
                        }
                        
                        ofPixels pixels;
                        
                        if (!ofLoadImage(pixels, path))
                        {
                            ofLogError("CCVEncoder::run") << "Unable to load >>" << path << "<<";
                            continue;
                        }
                        
                        if (!_ccv.isLoaded())
                        {
                            if (!_attemptedSetup)
                            {
                                _ccv.setup(_network);
                                _attemptedSetup = true;
                            }
                            else
                            {
                                ofLogError("CCVEncoder::run") << "CCV could not be loaded with network: " << _network;
                                break;
                            }
                        }
                        
                        std::vector<float> encoding = _ccv.encode(pixels, layer);
                        
                        std::ofstream ostr(encodedFile.generic_string(), std::ios::binary);
                        Poco::DeflatingOutputStream deflater(ostr, Poco::DeflatingStreamBuf::STREAM_GZIP);
                        Poco::BinaryWriter writer(deflater);
                        writer.writeBOM();
                        writer << encoding.size();
                        for (const auto& f: encoding) writer << f;
                        writer.flush();
                    }
                }
            }
            
            _channel.close();
        }
        
        // network path
        std::string _network;
        
        // internal thread
        std::thread _thread;
        
        // true if attempted to load network
        bool _attemptedSetup = false;
        
        // true if thread is running
        std::atomic<bool> _isRunning;
        
        // queue of files to encode
        ofxIO::ThreadChannel<std::filesystem::path> _channel;
        
        // true if encoder should force a re-encode
        bool _forceEncode = false;
        
        // libccv instance
        ofxCcv _ccv;
        
        // layers to extract vectors for
        std::vector<std::size_t> _layersToEncode;
    
};

struct ClusterGui
{
    ofxPanel gui;
    ofParameter<bool> drawImages;
    ofParameter<bool> drawPointCloud;
    ofParameter<bool> drawMesh;
    ofParameter<bool> showCubes;
    ofParameter<bool> save;
    
    bool modelRendered;
};

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
    
        void initGui();
        void setupGui();
        void drawGui();
    
        void process(const std::filesystem::path& path);
        void encode(const std::filesystem::path& path);
        
        void keyReleased(int key);
        void dragEvent(ofDragInfo dragInfo) override;
    
        void scan_dir_imgs(ofDirectory dir);
    
        std::vector<std::unique_ptr<CCVEncoder>> encoders;
        std::size_t currentWorkerIndex = 0;
        std::unique_ptr<ofxIO::RegexPathFilter> fileFilter;
    
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
        
        std::vector<Element> elementVector;
        
        std::vector<ofVec3f> posVector;
        
        std::vector<ofMesh> meshVector;
        
        ofSpherePrimitive sphere;
        
        float xPos, yPos, zPos;
        
        int nx, ny, nz;
        int w, h, d;
        float perplexity, theta;
        float scale;
    

};
