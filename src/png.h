#pragma once

#include "rgb.h"

namespace vpl{

/// Minumal implementation of Png
struct Png {
    FILE*f; unsigned int tab[256], crc;
    ~Png() { fclose(f); }
    Png(const char* fn, int w, int h, const unsigned char* c) {
        crc=0x575e51f5;
        unsigned char d[]={137,80,78,71,13,
                           10,26,10,0,0,0,13,73,72,68,82,73,68,65,84,120,1,0,
                           0,0,73,69,78,68,174,66,96,130};/*chunk headers*/
        for (int i=0;i<256;i++)/*precompute crc tables*/
            for (unsigned int j=8,&v=tab[i]=i;j--;)
                v=(v>>1)^(0xedb88320u&(~(v&1)+1));
        fwrite(d,1,16,f=fopen(fn,"w")); /*write IHDR+IDAT*/
        *this>>w>>h<<8<<2<<0<<0<<0;int a=1,b=0;/*adler-32*/
        *this>>~crc>>(h*(w*3+6)+6);int len=w*3+1;/*nbytes*/
        fwrite(d+16,1,6,f);crc=0x13e5812d;
        for (;h--;) {/*DEFLATE raw block*/
            *this<<!h<<len<<(len>>8)<<~len<<(~len>>8);
            /*filter=0*/*this <<0;/*adler(0)*/b=(a+b)%65521;
            for (int x=w,v;x--;){ /*write & checksum*/
                v=*c++;*this<<v;a=(a+v)%65521;b=(a+b)%65521;
                v=*c++;*this<<v;a=(a+v)%65521;b=(a+b)%65521;
                v=*c++;*this<<v;a=(a+v)%65521;b=(a+b)%65521;
            }
        }
        *this>>((b<<16)|a);*this>>~crc;fwrite(d+22,1,12,f);
    }
    Png& operator<<(int b){crc=(crc>>8)^tab[(crc^fputc(b,f))&255];return *this;}
    Png& operator>>(unsigned int v) {return *this<<(v>>24)<<(v>>16)<<(v>>8)<<v;}
};

/// Image Operations

/// TODO: This Png is apparently not compatible with (older - verify) versions of ImageMagick
///       Move away and use Png library instead (probably static one shipped with PDFWriter)
void savePng(const std::vector<RGB>& rgbs, const std::string& name, const PageInfo& info){
    unsigned char* pngimage = new unsigned char[3 * info.width * info.height];

    int ly,lx;
    for( ly = 0; ly < info.height; ly++ ){
        for( lx = 0; lx < info.width; lx++ ) {
            size_t pixel_index = ly * info.width + lx;
            const RGB& px = rgbs[pixel_index];

            pngimage[pixel_index*3] = px.r;
            pngimage[pixel_index*3+1] = px.g;
            pngimage[pixel_index*3+2] = px.b;

        }
    }

    Png(name.c_str(), info.width, info.height, pngimage);

}

int savepng(std::vector<vpl::RGB>& rgbs, vpl::PageInfo& info) {


    int width = info.width;
    int height = info.height;

    assert (width > 0);
    assert (height > 0);

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
    return 0;
}


/// TODO: to implment but probably use the library instead (see comment above)
std::vector<RGB> loadPng(const std::string& input_path){
    std::vector<RGB> rgbs;
    return rgbs;
}

/// Used only to test segmentation without taking a screenshot each time.
std::vector<int>&& loadPpm(const std::string& path){
    using namespace std;

    std::ifstream ii(path, ifstream::binary);


    ii.seekg (0, ii.end);
    int length = ii.tellg();
    ii.seekg (0, ii.beg);

    char * buffer = new char [length];
    ii.read(buffer, length);
    int width = 270;
    int height = 358;


    auto image = new vector<int>(length-18);

    int i =0;
    while (i < length){
        // TODO:17 varies if there is a comment if the ppm file
        if (i >= 17)
            (*image)[i-17] = (int)buffer[i];

        i++;
    }

    delete[] buffer;
    return std::move(*image);
}

}// end namepace vpl
