/*
    ScrabbleEye - live Scrabble game analysis app
    Copyright (C) 2012 nietaki@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <utility>
#include <vector>

#include "opencv2/opencv.hpp"

#include "utils/constants.hpp"
#include "utils/ImageOperations.hpp"
#include "utils/Line.hpp"
#include "utils/Utils.hpp"
#include "utils/CornerFinder.hpp"

using namespace cv;
using namespace std;
using namespace se;
  
const char* BOARD_FILENAME = "res/boards/1080/night/board.jpg";
const char* PIECES_FILENAME = "res/boards/1080/night/pieces.jpg";

void help()
{
  cout << "help message" << endl;
}

int main(int argc, char** argv)
{

  Mat boardImage = imread(BOARD_FILENAME);
  Mat piecesImage = imread(PIECES_FILENAME);
  
  // finding corners
  CornerFinder cf;
  cf.setDebug(true).setPopups(true).setDumpBasename("se").setDumpImages(true);
  vector<Point2d> corners = cf.getCorners(boardImage);
  
  //concatenateSegment(vector<Segment>& segments, Segment firstSegment);
  
  FileStorage fs2("res/yml/lifecam_studio_1080.yml", FileStorage::READ);
  
  Mat camera_matrix, distortion_coefficients;
  fs2["camera_matrix"] >> camera_matrix;
  fs2["distortion_coefficients"] >> distortion_coefficients;
  double square_size = (double)fs2["square_size"]; //TODO: WTF is that?
  
  Mat rvec, tvec;
  vector<Point3f> objectPoints;
  float boardWidth = TILE_WIDTH * BOARD_SIZE;
  objectPoints.push_back(Point3f(0,         0,          -TILE_THICKNESS));
  objectPoints.push_back(Point3f(0,         boardWidth,  -TILE_THICKNESS));
  objectPoints.push_back(Point3f(boardWidth, boardWidth,  -TILE_THICKNESS));
  objectPoints.push_back(Point3f(boardWidth, 0,          -TILE_THICKNESS));
  
  vector<Point3f> elevatedPoints;
  elevatedPoints.push_back(Point3f(0,         0,          0));
  elevatedPoints.push_back(Point3f(0,         boardWidth,  0));
  elevatedPoints.push_back(Point3f(boardWidth, boardWidth,  0));
  elevatedPoints.push_back(Point3f(boardWidth, 0,          0));
  
  //bool transformFound = solvePnP(objectPoints, corners, camera_matrix, distortion_coefficients, rvec, tvec);
  bool transformFound = solvePnP(objectPoints, corners, camera_matrix, noArray(), rvec, tvec);
  assert(transformFound);
  
  cout << "rvec:" << endl << rvec << endl;
  cout << "tvec:" << endl << tvec << endl;
  
  vector<Point2f> elevatedImagePoints;
  vector<Point> elevatedIntPoints;
  projectPoints(elevatedPoints, rvec, tvec, camera_matrix, distortion_coefficients, elevatedImagePoints);
  projectPoints(elevatedPoints, rvec, tvec, camera_matrix, noArray(), elevatedImagePoints);
  
  // FIXME
  // int i =0;
  // for(vector<Point2f>::iterator it = elevatedImagePoints.begin(); it != elevatedImagePoints.end(); it++)
  // {
  //   Point tmp = *it;
  //   elevatedIntPoints.push_back(tmp);
  //   circle(wipColorCoded, tmp, 12, CV_RGB(255,150,150), 6);
  //   putText(erodedTriplesOneChannel,Utils::intToString(i) , tmp, 0, 5, CV_RGB(255,255,255));
  //   i++;
  // }
  
  vector<Point2f> dstPoints;
  
  int dstWidth = BOARD_SIZE * TILE_PIXEL_WIDTH;
  dstPoints.push_back(Point2f(0,         dstWidth));
  dstPoints.push_back(Point2f(0,         0));
  dstPoints.push_back(Point2f(dstWidth, 0));
  dstPoints.push_back(Point2f(dstWidth, dstWidth));
  
  
  
  Mat homography = findHomography(elevatedImagePoints, dstPoints);
  
  Size outputSize(dstWidth, dstWidth);
  //Mat dst(outputSize, wipColorCoded.type());
  Mat dst(outputSize, boardImage.type());
  warpPerspective(piecesImage, dst, homography, outputSize);
  
  Scalar gridColor = CV_RGB(100,50,0);
  int gridThickness = 2;
  for(int i = 1; i < BOARD_SIZE; i++)
  {
    int pos = i * TILE_PIXEL_WIDTH;
    line(dst, Point(0, pos), Point(BOARD_SIZE * TILE_PIXEL_WIDTH, pos), gridColor, gridThickness);
    line(dst, Point(pos, 0), Point(pos, BOARD_SIZE * TILE_PIXEL_WIDTH), gridColor, gridThickness);
  }
  
  //displayImage(wipColorCoded, "clusters");
  //
  //displayImage(erodedTriplesOneChannel,"flooded_edges");
  displayImage(dst,"output");
  
  waitKey();
 
  return 0;
}
