#ifndef VIDEOCAPTURE_H_
#define VIDEOCAPTURE_H_
#include "ImageProcessor.h"

namespace cvPipeline{
    namespace Filter{
        class SHARED VideoCapture: public ImageProcessor{
        public:
            //DECLARE_CVFILTER;
            static ImageProcessor* createFilter();
            static void entryFilter();
            TParam<int> deviceIndex;
            TParam<std::string> filename;
            TParam<double> width;
            TParam<double> height;
            TParam<int> startFrame;
			TParam<int> stopFrame;
            TParam<bool> captureStart;
            TParam<bool> captureEmpty;
        private:
            cv::VideoCapture videoCapture;
            int frameNumber;
        public:
            VideoCapture();
            ~VideoCapture();
            bool open(int deviceIndex);
            bool open(const std::string& filename);
            bool capture();
            void execute();
        };
    }
}
#endif