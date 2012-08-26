/*
*
*  class for 
*/
#include <cstdlib>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils/Utils.hpp"
#include "utils/Line.hpp"
using namespace se;
using namespace cv;
// To use a test fixture, derive a class from testing::Test.
class Lines : public testing::Test {
  protected:
    
    virtual void SetUp() {
      //    q1_.Enqueue(1);
      //    q2_.Enqueue(2);
      //    q2_.Enqueue(3);
    }
    
    // virtual void TearDown() will be called after each test is run.
    // You should define it if there is cleanup work to do.  Otherwise,
    // you don't have to provide it.
    //
    virtual void TearDown() {
    }
    
};

// When you have a test fixture, you define a test using TEST_F
// instead of TEST.

// Tests the default c'tor.
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



