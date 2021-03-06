#if !defined(_MSC_VER)
#include <unistd.h>
//#define _sleep(ms) usleep((ms) * 1000)
#endif
#include "VideoCapture.h"

namespace cvPipeline {
    namespace Filter {
        ///////////////////////////////////////////////////////////////////////
        // class VideoCapture
        
        IMPLEMENT_CVFILTER(VideoCapture);
        // cvPipeline::Filter::ImageProcessor* VideoCapture::createFilter(){
        //     return new VideoCapture;
        // }
        // void VideoCapture::entryFilter(){
        //     cvPipeline::Filter::ImageProcessor::entryFilter(VideoCapture, &VideoCapture::createFilter);
        // }
        // cvPipeline::Filter::InitialCallback<VideoCapture, VideoCapture::entryFilter> VideoCapture _entry_static_;
        
        VideoCapture::VideoCapture():
            deviceIndex("deviceIndex", -1),
            filename("filename", ""),
            startFrame("startFrame", 0),
            stopFrame("stopFrame", -1),
            width("width", 0.0),
            height("height", 0.0),
            captureStart("captureStart", false),
            captureEmpty("captureEmpty", false),
            frameNumber(0)
        {
            defParam(deviceIndex);
            defParam(filename);
            defParam(startFrame);
            defParam(stopFrame);
            defParam(width);
            defParam(height);
            defParam(captureStart);
            defParam(captureEmpty);
            setInputMatDesc("", "");
        }
        
        VideoCapture::~VideoCapture(){}
        
        bool VideoCapture::open(int deviceIndex)
        {
            bool open_state = videoCapture.open(deviceIndex);
            
            if(width != 0.0){
                videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
            }
            if(height != 0.0){
                videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
            }
            captureStart = false;
            frameNumber = 0;
            return open_state;
        }
        
        bool VideoCapture::open(const std::string& filename)
        {
            bool open_state = videoCapture.open(filename);
            
            if(width != 0.0){
                videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
            }
            if(height != 0.0){
                videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
            }
            captureStart = false;
            frameNumber = 0;
            return open_state;
        }
        
        bool VideoCapture::capture()
        {
            if(!videoCapture.isOpened()){
                if(deviceIndex >= 0){
                    if(!open(deviceIndex)){
                        return false;
                    }
                }else if((std::string)filename != ""){
                    if(!open((std::string)filename)){
                        return false;
                    }
                }else{
                    return false;
                }
            }
            cv::Mat& mat = refOutputMat();
            if(captureStart == false){
                do{
                    videoCapture >> mat;
                    frameNumber++;
                    //_sleep(10);
                    sleep(10*1000);
                }while(mat.empty());
                captureStart = true;
                captureEmpty = false;
            }else if(!captureEmpty){
                while(frameNumber < startFrame){
                    videoCapture >> mat;
                    frameNumber++;
                    if(mat.empty() || (stopFrame>=0 && frameNumber>=stopFrame)){
                        captureEmpty = true;
                        return true;
                    }
                }
                if(stopFrame<0 || frameNumber<stopFrame){
                    videoCapture >> mat;
                    frameNumber++;
                    if(mat.empty() || (stopFrame>=0 && frameNumber>=stopFrame)){
                        captureEmpty = true;
                        return true;
                    }
                }
            }
            return true;
        }
        
        void VideoCapture::execute()
        {
            capture();
        }
    }
}