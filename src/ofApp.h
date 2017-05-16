#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxIO.h"
#include "Poco/BinaryWriter.h"
#include "Poco/BinaryReader.h"
#include "Poco/DeflatingStream.h"

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
                    
                    // load image
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
                    
                    //write encoded image to disk
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
    
    // true if we attempted to load network
    bool _attemptedSetup  =false;
    
    // truth if thread is running
    std::atomic<bool> _isRunning;
    
    // queue of files to encode
    ofxIO::ThreadChannel<std::filesystem::path> _channel;
    
    // true if encoder should force re-encode
    bool _forceEncode = false;
    
    // libccv instance
    ofxCcv _ccv;
    
    // layers to extract vectors for
    std::vector<std::size_t> _layersToEncode;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyReleased(int key);
		void dragEvent(ofDragInfo dragInfo);
    
        void process(const std::filesystem::path& path);
        void encode(const std::filesystem::path& path);
        
        std::vector<std::unique_ptr<CCVEncoder>> encoders;
        std::size_t currentWorkerIndex = 0;
        std::unique_ptr<ofxIO::RegexPathFilter> fileFilter;
		
};
