/*
 *
 *  might be helpful: http://code.google.com/p/googlemock/wiki/ForDummies
 *  http://code.google.com/p/googlemock/wiki/CheatSheet
 */
#define CATCH_EXCEPTIONS( expr ) try{ expr } \
catch(const std::runtime_error& re) { \
  FAIL() << "runtime error: " << re.what(); \
} catch(...) { \
  FAIL() << "unknown exception"; \
}


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
  
  /**
   * reads the board from the directory, find the corners and compare to the expectedCorners (if passed)
   */
  void performStandardBoardTest(const char* boardName, vector<Point2d> expectedCorners = vector<Point2d>()) {
    Mat inputBoard = imread(boards_path + boardName + ".jpg");
    ASSERT_FALSE(inputBoard.empty());
    CornerFinder cf;
    cf.setDebug(false).setPopups(false).setDumpImages(true);
    cf.setDumpBasename(this->baseDebugFilePath + boardName);
    //vector<Point2d> desiredCorners;
    vector<Point2d> corners = cf.getCorners(inputBoard);
    if(!expectedCorners.empty()) {
      checkCornersSimilar(corners, expectedCorners); 
    }
  }
};

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
  CATCH_EXCEPTIONS({
    const char* boardNo = "1";
    Mat inputBoard = imread(boards_path + boardNo + ".jpg");
    ASSERT_FALSE(inputBoard.empty());
    CornerFinder cf;
    cf.setDebug(false).setPopups(false).setDumpImages(true);
    cf.setDumpBasename(this->baseDebugFilePath + boardNo);
    //FIXME
    //vector<Point2d> desiredCorners;
    //desiredCorners.push_back(Point2d(592, 246));
    //desiredCorners.push_back(Point2d(1393, 288));
    //desiredCorners.push_back(Point2d(1750, 1014));
    //desiredCorners.push_back(Point2d(403, 997));
    
    vector<Point2d> corners = cf.getCorners(inputBoard);
    //checkCornersSimilar(corners, desiredCorners); 
  });
}

TEST_F(CornerFinderTest, Board1Redux) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("1");
  });
}



TEST_F(CornerFinderTest, Board2) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("2");
  });
}

TEST_F(CornerFinderTest, Board3) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("3");
  });
}



TEST_F(CornerFinderTest, Board4) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("4");
  });
}



TEST_F(CornerFinderTest, Board5) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("5");
  });
}



TEST_F(CornerFinderTest, Board6) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("6");
  });
}



TEST_F(CornerFinderTest, Board7) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("7");
  });
}



TEST_F(CornerFinderTest, Board8) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("8");
  });
}

TEST_F(CornerFinderTest, Board9) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("9");
  });
}

TEST_F(CornerFinderTest, Board10) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("10");
  });
}

TEST_F(CornerFinderTest, Board11) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("11");
  });
}

TEST_F(CornerFinderTest, Board12) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("12");
  });
}

TEST_F(CornerFinderTest, Board13) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("13");
  });
}

TEST_F(CornerFinderTest, Board14) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("14");
  });
}

TEST_F(CornerFinderTest, Board15) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("15");
  });
}

TEST_F(CornerFinderTest, Board16) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("16");
  });
}

TEST_F(CornerFinderTest, Board17) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("17");
  });
}

TEST_F(CornerFinderTest, Board18) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("18");
  });
}

TEST_F(CornerFinderTest, Board19) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("19");
  });
}

//  20  //
TEST_F(CornerFinderTest, Board20) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("20");
  });
}

TEST_F(CornerFinderTest, Board21) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("21");
  });
}

TEST_F(CornerFinderTest, Board22) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("22");
  });
}

TEST_F(CornerFinderTest, Board23) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("23");
  });
}

TEST_F(CornerFinderTest, Board24) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("24");
  });
}

TEST_F(CornerFinderTest, Board25) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("25");
  });
}

TEST_F(CornerFinderTest, Board26) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("26");
  });
}

TEST_F(CornerFinderTest, Board27) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("27");
  });
}

TEST_F(CornerFinderTest, Board28) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("28");
  });
}

TEST_F(CornerFinderTest, Board29) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("29");
  });
}

//  30  //
TEST_F(CornerFinderTest, Board30) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("30");
  });
}

TEST_F(CornerFinderTest, Board31) {
  CATCH_EXCEPTIONS({
    performStandardBoardTest("31");
  });
}



