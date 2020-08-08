// g++ main.cpp -I. -g -O0 -lX11

// $ sudo apt install libx11-dev/stable

#include <iostream>

#include "../install_dir/include/PDFWriter/PDFWriter.h"
#include "../install_dir/include/PDFWriter/PDFPage.h"
#include "../install_dir/include/PDFWriter/PageContentContext.h"
#include "../install_dir/include/PDFWriter/PDFFormXObject.h"

#include "../install_dir/include/LibPng/png.h"

#include "capture.h"


int savepng(std::vector<vpl::RGB>& rgbs, vpl::PageInfo& info) {


    int width = info.width;
    int height = info.height;

    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;

    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;

    fp = fopen (info.filename.c_str(), "wb");
    if (! fp) {
        throw ("open file failed");
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        throw ("png_create_write_struct_failed");
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        throw ("png_create_info_struct_failed");
    }

    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        throw ("png_failure");
    }

    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  width,
                  height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);


    row_pointers =  (png_byte**)png_malloc (png_ptr, height * sizeof (png_byte *));
    for (y = 0; y < height; y++) {
        png_byte *row = (png_byte*) png_malloc (png_ptr, sizeof (uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < width; x++) {
            size_t pixel_index = y * width + x;
            *row++ = (uint8_t) rgbs[pixel_index].r;
            *row++ = (uint8_t) rgbs[pixel_index].g;
            *row++ = (uint8_t) rgbs[pixel_index].b;
        }
    }
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    fclose(fp);
}


int savepdf(vpl::PageInfo& info){

    using namespace std;

    PDFWriter pdfWriter;
    EStatusCode status;

    do
    {
        status = pdfWriter.StartPDF( info.filename+"_.pdf", ePDFVersion14);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to start PDF\n";
            break;
        }

        PDFPage* page = new PDFPage();
        // set document to the size of the image (setting proportional makes things super small) 
        page->SetMediaBox(PDFRectangle(0, 0, info.width, info.height));

        PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
        if (NULL == pageContentContext)
        {
            status = PDFHummus::eFailure;
            cout << "failed to create content context for page\n";
        }

        // place a large red rectangle all over the page
        // AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill,
        //     AbstractContentContext::eRGB,
        //     0xFF0000);
        // pageContentContext->DrawRectangle(0, 0, 595, 842, pathFillOptions);

        // place the image on top...hopefully we can see soem transparency
        AbstractContentContext::ImageOptions imageOptions;
        // too small
        // imageOptions.transformationMethod = AbstractContentContext::EImageTransformation::eFit;
        // imageOptions.fitProportional = true;

        // imageOptions.transformationMethod = AbstractContentContext::eMatrix;
        // imageOptions.matrix[0] = imageOptions.matrix[3] = 0.5;   // scale image by half
        pageContentContext->DrawImage(0, 0, info.filename, imageOptions);  // problems reading the file

        status = pdfWriter.EndPageContentContext(pageContentContext);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to end page content context\n";
            break;
        }

        status = pdfWriter.WritePageAndRelease(page);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to write page\n";
            break;
        }


        status = pdfWriter.EndPDF();
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed in end PDF\n";
            break;
        }
    } while (false);
    return status;
}

void help(){
    using namespace std;

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
    using namespace std;

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
        // savepng("../FallFoliage.ppm");
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
        info.width = 270;
        info.height = 358;
        // http://cs.colby.edu/courses/F15/cs151-labs/labs/lab04/FallFoliage.ppm
        // info.filename = "../FallFoliage.ppm";
    }

    if ((capture_screen_) ||
        (capture_window_) ||
        (crop_only_) ||
        (ppm_)){
        // TODO: specify pixels to extract (currently white page RGB(255,255,255)
        savepng(rgbs, info);
        vpl::boundary_tracing(info, rgbs); // this updates rgbs 
        savepng(rgbs, info);
        savepdf(info);
    }


    // TODO: Testing RGB comparators
    // auto aaa = RGB(1,2,0) > RGB(1,1,0);
    // cout << aaa <<endl;


    /*
    - add loading
    - convert to single pdf using pdf write
    - enable different formats
    - tests

     */
}
