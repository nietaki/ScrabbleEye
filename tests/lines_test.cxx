/*
*
*  class for 
*/
#include <cstdlib>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils/Utils.hpp"
#include "utils/Line.hpp"
using namespace std;
using namespace se;
using namespace cv;
// To use a test fixture, derive a class from testing::Test.
class Lines : public testing::Test {
  protected:
    
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
    
};

TEST_F(Lines, shouldBeExtrapolatedCorrectly) {
  Point p0(0,0);
  Point p1(1,2);
  Point p2(3,-2);
  Segment seg0(p0,p1);
  Segment seg1(p1,p2);
  Line l0 = Line::getBySegment(seg0);
  Line l1 = Line::getBySegment(seg1);
  //l0.print();
  EXPECT_FLOAT_EQ(0.0, l0.c) << "the line should go through the 0.0 point";
  EXPECT_FLOAT_EQ(2.0, abs(l0.a/l0.b));
  //l1.print();
  EXPECT_FLOAT_EQ(2.0, abs(l1.a/l1.b));
}

TEST_F(Lines, shouldEstimateIntersectionsCorrectly) {
  Point p0(0, 0);
  Point p1(1, 1);
  
  Point p2(1, 1);
  Point p3(2, 0);
  Segment seg0(p0,p1);
  Segment seg1(p2,p3);
  Line l0 = Line::getBySegment(seg0);
  Line l1 = Line::getBySegment(seg1);
  
  Point2d intersection = l0.intersect(l1);
  //cout << intersection.x << " " << intersection.y << endl;
  EXPECT_FLOAT_EQ(1.0, intersection.x); 
  EXPECT_FLOAT_EQ(1.0, intersection.y);
  //l0.print();
  //l1.print();
}

