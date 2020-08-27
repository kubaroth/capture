#include <iostream>
#include <vector>

#include "../Catch2/single_include/catch2/catch.hpp"
#include "../src/rgb.h"

using namespace vpl;

TEST_CASE( "5x5" ) {
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

}

