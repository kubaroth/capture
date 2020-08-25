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
