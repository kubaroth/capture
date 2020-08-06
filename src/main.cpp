// g++ main.cpp -I. -g -O0 -lX11

// $ sudo apt install libx11-dev/stable

#include <iostream>

#include "capture.h"

using namespace std;

void help(){    
    cout << "Screen/Window Capture program:"<< endl;
    cout << "\
\n\
-h/no args : Prints this message.\n\
-cs -screen: Captures current screen and saves it in the current directory as page_capture#.png\n\
             Subsequently the captured image is cropped base on the largest area of brightest pixels.\n\
             and saved with crop filename.\n\
-cw -window: Captures selected window followed by a crop to keep only the brightest pixels\n\
-ppm       : Runs test on page_012.ppm image\n\
-i -image  : Takes the input image and calculates crop\n\
\n\
Example:\n\
capture -cs \n\
capture -cw Chromium \n\
capture -ppm\n\
capture -i my_image.png\n\
"<<endl;
}

int main(int argc, char * argv[]){

    // Command arguments handling
    std::vector<string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);

    bool help_ = true;
    bool capture_screen_ = false;
    bool capture_window_ = false;
    bool ppm_ = false;
    bool crop_only_ = false;

    std::string crop_input_image_ = "";
    std::string window_name_ = "";

    if(vpl::cmdOptionExists(args, "-h")){
        help_ = true;
    }

    if ( (vpl::cmdOptionExists(args, "-cs")) || (vpl::cmdOptionExists(args, "-screen")) ) {
        help_ = false;
        capture_screen_ = true;
    }

    if ( (vpl::cmdOptionExists(args, "-cw")) || (vpl::cmdOptionExists(args, "-window")) ) {
        help_ = false;
        capture_window_ = true;

        if  (vpl::cmdOptionExists(args, "-cw")){
            auto it = vpl::getCmdOption(args, "-cw");
            window_name_ = *(++it);
        }
        else if  (vpl::cmdOptionExists(args, "-window")){
            auto it = vpl::getCmdOption(args, "-window");
            window_name_ = *(++it);
        }

        cout << "Search for window : " << window_name_ <<endl;
    }

    if (vpl::cmdOptionExists(args, "-i")){
        help_ = false;
        crop_only_ = true;
        auto it = vpl::getCmdOption(args, "-i");

        crop_input_image_ = *(++it);
        cout << "Loading input image: " << crop_input_image_ <<endl;
    }

    if (vpl::cmdOptionExists(args, "-ppm")){
        help_ = false;
        ppm_ = true;
    }

    // end of argument handling

    if (help_){
        help();
        return 0;
    }

    vpl::PageInfo info(0,0); // image dimensions to be populated by screen capture or test
    std::vector<vpl::RGB> rgbs;

    if(capture_screen_){
        rgbs = vpl::CaptureScreen(info, nullptr /*rootWindow*/);
    }

    if(capture_window_){
        rgbs = vpl::CaptureWindow(info, window_name_);
    }

    if(crop_only_){
        // TODO: load image from png file
        // rgbs = loadPng(crop_input_image_);
        cout << "crop_only_ - NOT implemented" <<endl;
        return 1;
    }

    if (ppm_){
        // testing boundary tracing
        info.test_ppm = true;
        info.width = 1920;
        info.height = 1200;
        info.filename = "page_012.ppm";
    }

    if ((capture_screen_) ||
        (capture_window_) ||
        (crop_only_) ||
        (ppm_)){
        // TODO: specify pixels to extract (currently white page RGB(255,255,255)
        vpl::boundary_tracing(info, rgbs);
    }


    // TODO: Testing RGB comparators
    // auto aaa = RGB(1,2,0) > RGB(1,1,0);
    // cout << aaa <<endl;


    /*
      setup repo
    - add saving png
    - add loading
    - convert to single pdf using pdf write
    - enable different formats
    - tests

     */
}
