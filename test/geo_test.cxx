#include "catch.hpp"

#include <iostream>

#include "Tracking/geo/point.h"

TEST_CASE( "Point class", "[Tracking][geo]" ) { 

    // Test basic constructor 
    tracking::geo::Point p1(1, 2, 3); 
    REQUIRE( p1.x() == 1 );
    REQUIRE( p1.y() == 2 );
    REQUIRE( p1.z() == 3 );

    // Test creation of a point from a vector
    std::vector<double> v = { 4, 5, 6 }; 
    tracking::geo::Point p2( v ); 
    REQUIRE( p2.x() == 4 );
    REQUIRE( p2.y() == 5 );
    REQUIRE( p2.z() == 6 );

    // Test addition
    tracking::geo::Point p3 = p1 + p2;
    REQUIRE( p3.x() == 5 );
    REQUIRE( p3.y() == 7 );
    REQUIRE( p3.z() == 9 );

    // Test subtraction
    p3 = p2 - p1;
    REQUIRE( p3.x() == 3 );
    REQUIRE( p3.y() == 3 );
    REQUIRE( p3.z() == 3 );

    // Test dot product
    auto scalar{p1.dot(p2)}; 
    REQUIRE(scalar == 32);

    // Test normalization
    scalar = p1.squaredNorm(); 
    REQUIRE(scalar == 14);
    scalar = p1.norm(); 
    REQUIRE(scalar == sqrt(14)); 
}
