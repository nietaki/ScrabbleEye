/*
 *
 *  might be helpful: http://code.google.com/p/googlemock/wiki/ForDummies
 *  http://code.google.com/p/googlemock/wiki/CheatSheet
 */

#include <string>
#include <vector>
#include <boost/concept_check.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils/CornerFinder.hpp"

using namespace cv;
using namespace se;

class CornerFinderTest : public testing::Test {
  protected:
    const std::string boards_path = "res/boards/1080/empty_boards/";
    const float maxDistance = 30;
    const std::string baseDebugFilePath = "debug_dumps/";
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  virtual void TearDown() {
  }
  
  void checkCornersSimilar(vector<Point2d> actualCorners, vector<Point2d> expectedCorners) { 
    ASSERT_EQ(4, actualCorners.size()); 
    ASSERT_EQ(4, expectedCorners.size()); 
    
    for(int i = 0; i < 4; i++) {
      Point2d actual = actualCorners[i];
      Point2d desired = expectedCorners[i];
      EXPECT_LE(norm(actual - desired), maxDistance) << "corner no " << i << " is too far away, is " << actual << " should be " << desired;
    }
  }
};

// When you have a test fixture, you define a test using TEST_F
// instead of TEST.


TEST_F(CornerFinderTest, NightBoard) {
  ASSERT_NO_THROW({
    Mat inputBoard = imread("res/boards/1080/night/board.jpg");
    ASSERT_FALSE(inputBoard.empty());
    CornerFinder cf;
    cf.setDebug(false).setPopups(false).setDumpImages(true);;
    cf.setDumpBasename(this->baseDebugFilePath + "night_board");
    
    vector<Point2d> desiredCorners;
    desiredCorners.push_back(Point2d(592, 246));
    desiredCorners.push_back(Point2d(1393, 288));
    desiredCorners.push_back(Point2d(1750, 1014));
    desiredCorners.push_back(Point2d(403, 997));
    
    vector<Point2d> corners = cf.getCorners(inputBoard);
    checkCornersSimilar(corners, desiredCorners);
  });
}



TEST_F(CornerFinderTest, Board1) {
  try{
    const char* boardNo = "1";
    Mat inputBoard = imread(boards_path + boardNo + ".jpg");
    ASSERT_FALSE(inputBoard.empty());
    CornerFinder cf;
    cf.setDebug(false).setPopups(false).setDumpImages(true);
    cf.setDumpBasename(this->baseDebugFilePath + boardNo);
    
    //vector<Point2d> desiredCorners;
    //desiredCorners.push_back(Point2d(592, 246));
    //desiredCorners.push_back(Point2d(1393, 288));
    //desiredCorners.push_back(Point2d(1750, 1014));
    //desiredCorners.push_back(Point2d(403, 997));
    
    vector<Point2d> corners = cf.getCorners(inputBoard);
    //checkCornersSimilar(corners, desiredCorners); 
  } catch(const std::runtime_error& re) {
    FAIL() << "runtime error: " << re.what();
  } catch(...) {
    FAIL() << "unknown exception";
  }
}