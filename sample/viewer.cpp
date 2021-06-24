#include <opencv2/opencv.hpp>
#include "cvPipeline.h"

using namespace cvPipeline;
using namespace cvPipeline::Filter;
#include <iostream>
int main(int argc, char* argv[])
{
    std::cout << "Entery of main." << std::endl;
    // opencv HighGUI
    cv::namedWindow(argv[0], 1);
    ImgProcSet processor;
    std::string xml_filename("viewer.xml");
    if (argc>1){
        xml_filename = argv[1];
    }
    if (!processor.loadXml(xml_filename)){
        std::cerr << "File read failure File name: " << xml_filename << std::endl;
        return -1;
    }
    //Sleep(2000);
    while(true){
        processor.execute();
        int ch = cv::waitKey();
        if (ch == 27){
            fprintf(stderr, "exit.\n");
            break;
        }
    }
    cv::destroyWindow(argv[0]);
    return 0;
}