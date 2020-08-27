#include <iostream>
#include <vector>

#include "../Catch2/single_include/catch2/catch.hpp"
#include "../src/rgb.h"

using namespace vpl;

TEST_CASE( "First test" ) {
    REQUIRE( 1 == 1 );
    // REQUIRE( 1 == 0 );
}

TEST_CASE( "RGB ctors" ) {
    RGB rgb(1,2,3);
    REQUIRE( rgb.r  == 1 );
    REQUIRE( rgb.g  == 2 );
    REQUIRE( rgb.b  == 3 );
    REQUIRE( rgb.x  == -1 );
    REQUIRE( rgb.y  == -1 );
    REQUIRE( rgb.index  == 0 );
    rgb = RGB(1,2,3,4,5,6);
    REQUIRE( rgb.r  == 1 );
    REQUIRE( rgb.g  == 2 );
    REQUIRE( rgb.b  == 3 );
    REQUIRE( rgb.x  == 4 );
    REQUIRE( rgb.y  == 5 );
    REQUIRE( rgb.index  == 6 );
}

TEST_CASE( "RGB comparators" ) {
    REQUIRE (RGB(1,1,1) == RGB(1,1,1));
    REQUIRE (RGB(1,1,1) >= RGB(1,1,1));
    REQUIRE (RGB(1,1,1) <= RGB(1,1,1));

    REQUIRE (RGB(1,1,1) >= RGB(1,0,1));
    REQUIRE (RGB(1,1,0) <= RGB(1,1,1));

    REQUIRE (RGB(1,1,1) > RGB(0,1,1));
    REQUIRE (RGB(1,0,1) < RGB(1,1,1));

    REQUIRE (RGB(1,0,1) != RGB(1,1,1));
}
TEST_CASE( "RGB subtractions" ) {
    RGB rgb(1,1,1);
    REQUIRE (rgb - 1 == RGB(0,0,0));
    REQUIRE (rgb - RGB(1,0,1) == RGB(0,1,0));
}

TEST_CASE( "RGB compare ptrs" ) {
    RGB rgb(0,1,1);
    RGB rgb2(1,0,1);
    RGB rgb3(1,1,1);
    auto rgb_ptr = &rgb;
    auto rgb_ptr2 = &rgb2;
    auto rgb_ptr3 = &rgb3;
    REQUIRE ( compareRGB_Ptrs(rgb_ptr, rgb_ptr2) == false );
    REQUIRE ( compareRGB_Ptrs(rgb_ptr, rgb_ptr3) == true );
}

TEST_CASE( "RGB get pixel" ) {
    using namespace std;

    std::vector<RGB> rgbs;
    PageInfo info;
    info.width = 3;
    info.height = 3;
    for (int y=0; y < info.height; y++){
        for (int x=0; x < info.width; x++){
            rgbs.emplace_back(RGB(x,y,0,x,y, rgbs.size()) );
        }
    }
    // for( auto px : rgbs) cout << px << endl; // output each pixel value

    SECTION( "top left pixel" ) {
        // NOTE: here we encode index 0 - this is unfortuantely essential to determin
        //       place of the pxiel in the rgbs array
        RGB left_top_pixel(1,2,3, 0 /*x*/, 0 /*z*/, 0 /*index*/);
        auto px_ptr_up = getPixel(rgbs, left_top_pixel, Direction::up, info);
        REQUIRE (px_ptr_up == nullptr);
        auto px_ptr_left = getPixel(rgbs, left_top_pixel, Direction::left, info);
        // cout << *px_ptr_left <<endl;
        REQUIRE (px_ptr_left == nullptr);
        auto px_ptr_right = getPixel(rgbs, left_top_pixel, Direction::right, info);
        REQUIRE (px_ptr_right != nullptr);
        auto px_ptr_down = getPixel(rgbs, left_top_pixel, Direction::down, info);
        REQUIRE (px_ptr_down != nullptr);
    }

    SECTION( "down right pixel" ) {
        RGB down_right_pixel(1,2,3,info.width-1, info.height-1, info.width * info.height - 1 /*index*/ );
        auto px_ptr_up = getPixel(rgbs, down_right_pixel, Direction::up, info);
        REQUIRE (px_ptr_up != nullptr);
        auto px_ptr_left = getPixel(rgbs, down_right_pixel, Direction::left, info);
        REQUIRE (px_ptr_left != nullptr);
        auto px_ptr_right = getPixel(rgbs, down_right_pixel, Direction::right, info);
        REQUIRE (px_ptr_right == nullptr);
        auto px_ptr_down = getPixel(rgbs, down_right_pixel, Direction::down, info);
        REQUIRE (px_ptr_down == nullptr);
    }
}
