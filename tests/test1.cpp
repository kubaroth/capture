#include "../Catch2/single_include/catch2/catch.hpp"

TEST_CASE( "First test" ) {
    REQUIRE( 1 == 1 );
    REQUIRE( 1 == 0 );
}
