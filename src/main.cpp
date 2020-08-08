// g++ main.cpp -I. -g -O0 -lX11

// $ sudo apt install libx11-dev/stable

#include <iostream>

#include "../install_dir/include/PDFWriter/PDFWriter.h"
#include "../install_dir/include/PDFWriter/PDFPage.h"
#include "../install_dir/include/PDFWriter/PageContentContext.h"
#include "../install_dir/include/PDFWriter/PDFFormXObject.h"

#include "../install_dir/include/LibPng/png.h"

#include "capture.h"

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;
    
/* Given "bitmap", this returns the pixel of bitmap at the point 
   ("x", "y"). */

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

static int pix (int value, int max)
{
    if (value < 0) {
        return 0;
    }
    return (int) (256.0 *((double) (value)/(double) max));
}


int savepng(std::string filename, std::vector<vpl::RGB>& rgbs, vpl::PageInfo& info) {

    // auto image = vpl::loadPpm(filename);

    int width = info.width;
    int height = info.height;

    std::cout << "w: " << width << " h: "<<height<< std::endl;

    
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
    
    fp = fopen ("out_test.png", "wb");
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



    // png_set_packing(png_ptr);
    // png_set_swap_alpha(png_ptr);
    ///   // png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);
   // png_set_bgr(png_ptr);
       
    /* Initialize rows of PNG. */
    bitmap_t fruit;
    fruit.width = width;
    fruit.height = height;
    fruit.pixels = (pixel_t*)calloc (fruit.width * fruit.height, sizeof (pixel_t));
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // pixel_t * pixel = pixel_at (& fruit, x, y);
            // pixel_t * pixel = pixel_at (& fruit, x, y);
            // pixel->red = pix (x, width);
            // pixel->green = pix (y, height);
            
            // size_t pixel_index = y * width + x;
            // pixel->red = image[pixel_index];
            // pixel->green = image[pixel_index+1];
            // pixel->blue = image[pixel_index+2];
        }
    }

    row_pointers =  (png_byte**)png_malloc (png_ptr, height * sizeof (png_byte *));
    for (y = 0; y < height; y++) {
        png_byte *row = (png_byte*) png_malloc (png_ptr, sizeof (uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < width; x++) {
            // pixel_t * pixel = pixel_at (&fruit, x, y);
            // *row++ = pixel->red;
            // *row++ = pixel->green;
            // *row++ = pixel->blue;

              // ->pixels + bitmap->width * y + x;
            // size_t pixel_index = y * width + x;
            // *row++ = (uint8_t) image[pixel_index];
            // *row++ = (uint8_t) image[pixel_index+1];
            // *row++ = (uint8_t) image[pixel_index+2];

            size_t pixel_index = y * width + x;
            *row++ = (uint8_t) rgbs[pixel_index].r;
            *row++ = (uint8_t) rgbs[pixel_index].g;
            *row++ = (uint8_t) rgbs[pixel_index].b;

            
            // (row[x]) = (uint8_t) image[pixel_index];
            // (row[x+1]) = (uint8_t) image[pixel_index+1];
            // (row[x+2]) = (uint8_t) image[pixel_index+2];
            // std::cout << y << " " << x << " " <<image[pixel_index] << " " << image[pixel_index+1 ]<< " " << image[pixel_index+2] << std::endl;;
        }
        // std::cout << std::endl;
        // std::cout << y << std::endl;
    }

    // png_uint_32 k;
    // int bytes_per_pixel = 3;
    // png_byte iimage[height][width*bytes_per_pixel];
    // png_bytep row_pointers[height];
    // for (k = 0; k < height; k++){
    //     std::cout << k << " "<<k*width*bytes_per_pixel <<std::endl;
    //     row_pointers[k] = ( png_bytep )(iimage + k*width*bytes_per_pixel);
    // }
    // png_init_io(png_ptr, fp);
    // png_write_info(png_ptr, info_ptr);
    // png_write_image(png_ptr, row_pointers);
    // fclose(fp);
    
    // /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);


    fclose(fp);


    // The routine has successfully written the file, so we set
       // "status" to a value which indicates success.

    // status = 0;
    
    // for (y = 0; y < height; y++) {
    //     png_free (png_ptr, row_pointers[y]);
    // }
    // png_free (png_ptr, row_pointers);
    

}


int testpdf(){

    using namespace std;

    PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
        status = pdfWriter.StartPDF( "PNGTest_.pdf", ePDFVersion14, LogConfiguration(true, true, "PNGTest_.log"));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if (NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout << "failed to create content context for page\n";
		}

		// place a large red rectangle all over the page
		AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill,
			AbstractContentContext::eRGB,
			0xFF0000);
		pageContentContext->DrawRectangle(0, 0, 595, 842, pathFillOptions);

		// place the image on top...hopefully we can see soem transparency
		AbstractContentContext::ImageOptions imageOptions;
		imageOptions.transformationMethod = AbstractContentContext::eMatrix;
		imageOptions.matrix[0] = imageOptions.matrix[3] = 0.5;
        pageContentContext->DrawImage(10, 200, "/home/kuba/PRJ/capture/__build/out_test.png", imageOptions);  // problems reading the file
        // pageContentContext->DrawImage(10, 200, "/home/kuba/temp/fruit.png", imageOptions);  // problems reading the file
        // pageContentContext->DrawImage(10, 200, "/home/kuba/Downloads/800px-3D_Saturn.png", imageOptions);

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
        // testpdf();
        // aaa();
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
        info.filename = "../FallFoliage.ppm";
    }

    if ((capture_screen_) ||
        (capture_window_) ||
        (crop_only_) ||
        (ppm_)){
        // TODO: specify pixels to extract (currently white page RGB(255,255,255)
        // vpl::boundary_tracing(info, rgbs);
        cout << info.width <<endl;
        savepng("../FallFoliage.ppm", rgbs, info);
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
