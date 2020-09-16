#include <iostream>
#include <vector>

#include "../Catch2/single_include/catch2/catch.hpp"
#include "../src/rgb.h"
#include "../src/png.h"

using namespace vpl;

TEST_CASE( "7x7 image" ) {
    using namespace std;

    vector<int> image_values{
        1,1,1,1,1,1,1,
        1,0,0,0,0,0,1,
        1,0,0,0,0,0,1,
        1,0,0,0,0,0,1,
        1,0,0,0,0,0,1,
        1,0,0,0,0,0,1,
        1,1,1,1,1,1,1
    };
    PageInfo info;
    info.width = 7;
    info.height = 7;
    info.filename = "gray_7x7.png";

    SECTION ( "save image"){
        vector<RGB> rgbs;
        transform(image_values.begin(), image_values.end(), back_inserter(rgbs), [index=0](int i) mutable {
            int y = index / 7;
            int x = index % 7;
            if (i == 1){
                return RGB(255, 255, 255, x, y, index);
            }
            else{
                return RGB(240, 240, 240, x, y, index);
            }
            index++;
         });
         savepng(rgbs, info);
    }
    SECTION ("load image"){
        REQUIRE (info.filename.compare("gray_7x7.png") == 0);
    }


}

