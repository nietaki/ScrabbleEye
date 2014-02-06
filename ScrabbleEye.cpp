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
    
  Mat blurredBoardImage;
  GaussianBlur(boardImage, blurredBoardImage, Size(GAUSSIAN_SIZE, GAUSSIAN_SIZE), GAUSSIAN_SIGMA, GAUSSIAN_SIGMA);
  displayImage(blurredBoardImage, "gausser");
  
  Mat triplesOneChannel;
  ImageOperations::extractTriples(blurredBoardImage, triplesOneChannel);
  Mat onlyRed;
  bitwise_and(blurredBoardImage, blurredBoardImage, onlyRed, triplesOneChannel);
  displayImage(onlyRed, "onlyRed");
  displayImage(triplesOneChannel, "extracted_triples");
  
  //erode(triplesOneChannel, triplesOneChannel, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
  //dilate(triplesOneChannel, triplesOneChannel, getStructuringElement(MORPH_ELLIPSE, Size(9,9)));
  //displayImage(triplesOneChannel, "extracted_triples_eroded");
  Mat wipColorCoded;
  
  Mat markedPoints;
  vector<int> pointLabels;
  Mat clusterCenters;
  
  ImageOperations::clusterTriples(triplesOneChannel, pointLabels, markedPoints, clusterCenters) ;
  /**
  *** visualising the clusters
  **/
  cvtColor(triplesOneChannel, wipColorCoded, CV_GRAY2BGR);
  
  //HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );

  
  vector<Scalar> colors = Utils::getColors(8);
  
  for(size_t i = 0; i < pointLabels.size() ; i++ )
  {
    Point_<float> currentPoint(markedPoints.at<float>(i, 0), markedPoints.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    circle(wipColorCoded, currentIntPoint, 1, colors[pointLabels[i]]);
  }
  
  vector<Point> centerPoints = centerMatrixToPointVector(clusterCenters);
  for(vector<Point>::iterator it = centerPoints.begin(); it != centerPoints.end(); it++)
  {
    circle(wipColorCoded, *it, 9, CV_RGB(200,200,200), 3);
  }

  /**
  *** flood-filling the clusters
  **/
  //TODO: make sure it works correctly even if the center is on the black text
  for(int i = 0; i < centerPoints.size() ; i++)
  {
    Point currentIntPoint = centerPoints[i];
    if(triplesOneChannel.at<unsigned char>(currentIntPoint) == 255)
    {
      //cout << "color: " << floodedMark + (unsigned char) i << endl;
      //different colors denote different clusters
      floodFill(triplesOneChannel, currentIntPoint, getFloodMark(i));
    }
    else
    {
      cout  << currentIntPoint.x << "," << currentIntPoint.y << ": " 
        << (int)triplesOneChannel.at<unsigned char>(currentIntPoint) << endl;
      //circle(src, currentIntPoint, 9, CV_RGB(255,150,150), 3);
    }
  }
  
  
  
  /**
  *** counting the points that are in the cluster but aren't in the flood-filled area
  **/
  //TODO think about the distance from cluster centre variance
  vector<int> misplacedPixelCounts(clusterCenters.rows);
  
  //flooding all the spots that contain a cluster center
  for(size_t i = 0; i < pointLabels.size() ; i++ )
  {
    Point_<float> currentPoint(markedPoints.at<float>(i, 0), markedPoints.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    if(triplesOneChannel.at<unsigned char>(currentIntPoint) == 255)
    {
      misplacedPixelCounts[pointLabels[i]]++;
      triplesOneChannel.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
  // << endl;
  //finding the label that doesn't stick closely to the cluster centre
  
  /**
   * bottomLabel is the number of the cluster that at the bottom of the board,
   * by the "scrabble" text
   */
  int bottomLabel = -1;
  int maxMisplacedPixels = 0;
  for(size_t i = 0; i < clusterCenters.rows; i++)
  {
    if(misplacedPixelCounts[i] > maxMisplacedPixels)
    {
      maxMisplacedPixels = misplacedPixelCounts[i];
      bottomLabel = i;
    }
  }
  //FIXME getting the closest centers won't work for creating the field "snake", have to think of something else
  //answer: all cluster centers that are close (enough) to the middlepoint between two other cluster centres qualify
  // the close enough can be calculated relatively to the dinstance between the two clusters
  
  //TODO: move this down and erase all the non-corner fields?
  /**
   * erasing the most sparse cluster
   */
  for(size_t i = 0; i < pointLabels.size() ; i++ )
  {
    if(pointLabels[i] == bottomLabel)
    {
      Point_<float> currentPoint(markedPoints.at<float>(i, 0), markedPoints.at<float>(i, 1));
      Point currentIntPoint = currentPoint;
      triplesOneChannel.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
  /**
  *** getting the edge points
  **/
  vector<Point> nonZeroPoints;
  nonZeroPoints = getNonZeroPoints(triplesOneChannel);
  vector<int> hullPointIndices;
  vector<Segment> segments;
  
  //TODO: check why clockwise parameter works reversly
  convexHull(nonZeroPoints, hullPointIndices, false, false);
  //let's wrap the indices
  hullPointIndices.push_back(hullPointIndices[0]);
  
  /**
  *** retrieving the segments
  **/
  
  set<int> untouchedClusterLabels;
  for(int i = 0; i<8; i++) {
    untouchedClusterLabels.insert(i);
  }
  untouchedClusterLabels.erase(bottomLabel);
  
  /**
   * the segments are connecting points belonging to different clusters
   */
  Point curPoint, lastPoint;
  int pointNo = 0;
  for(vector<int>::iterator it = hullPointIndices.begin(); it != hullPointIndices.end(); it++) {
    lastPoint = curPoint;
    curPoint = nonZeroPoints[*it];
    
    
    //putText(srcBgr, Utils::intToString(pointNo) , curPoint, 0, 5, CV_RGB(255,255,255));
    
    untouchedClusterLabels.erase(getLabelByMark(triplesOneChannel.at<unsigned char>(curPoint)));
    //src.at<unsigned char>(edgePoint.y, edgePoint.y) = 255;
    //src.at<unsigned char>(edgePoint) = 255;
    circle(wipColorCoded, curPoint, 5, Scalar(255,255,255), 3);
    if(it == hullPointIndices.begin())
      continue;
    
    if(triplesOneChannel.at<unsigned char>(lastPoint) != triplesOneChannel.at<unsigned char>(curPoint)){
      segments.push_back(Segment(lastPoint, curPoint));
      line(wipColorCoded, lastPoint, curPoint, Scalar(255,255,255), 2);
    }
    
    pointNo++;
  }
  
  segments = rotate(segments, findClosestSegment(segments, centerPoints[bottomLabel]));
  
  line(wipColorCoded, segments.front().first, segments.front().second, CV_RGB(255, 0,0), 3);
 
  cout << "number of clusters not touching the convex hull: " << untouchedClusterLabels.size() << endl;
  
  //get all but the first (bottom) segment
  vector<Segment> segmentsToManipulate(segments.begin()+1, segments.end());
  while(!untouchedClusterLabels.empty()){
    int curLabel = *(untouchedClusterLabels.begin());
    untouchedClusterLabels.erase(curLabel);
    segmentsToManipulate.erase(findClosestSegment(segmentsToManipulate, centerPoints[curLabel]));
  }
  cout << "number of segments left to manipulate (+-concatenate) " << segmentsToManipulate.size() << endl; 
  //TODO somehow replace segments to manipulate in the original segments "contcatinating" the left pairs

  for(vector<Segment>::iterator it = segmentsToManipulate.begin(); it != segmentsToManipulate.end(); it++)
  {
    concatenateSegment(segments, *it);
    //yes, we're skipping one segment
    assert(++it != segmentsToManipulate.end());
  }
  
  paintSegments(wipColorCoded, segments, CV_RGB(0,150,0), 3);
  vector<Point2d> corners = Line::getCorners(segments);
  
  for(vector<Point2d>::iterator it = corners.begin(); it != corners.end(); it++)
  {
    Point tmp = *it;
    circle(wipColorCoded, tmp, 12, CV_RGB(0,0,250), 6);
  }
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
  
  int i =0;
  for(vector<Point2f>::iterator it = elevatedImagePoints.begin(); it != elevatedImagePoints.end(); it++)
  {
    Point tmp = *it;
    elevatedIntPoints.push_back(tmp);
    circle(wipColorCoded, tmp, 12, CV_RGB(255,150,150), 6);
    putText(triplesOneChannel,Utils::intToString(i) , tmp, 0, 5, CV_RGB(255,255,255));
    i++;
  }
  
  vector<Point2f> dstPoints;
  
  int dstWidth = BOARD_SIZE * TILE_PIXEL_WIDTH;
  dstPoints.push_back(Point2f(0,         dstWidth));
  dstPoints.push_back(Point2f(0,         0));
  dstPoints.push_back(Point2f(dstWidth, 0));
  dstPoints.push_back(Point2f(dstWidth, dstWidth));
  
  
  
  Mat homography = findHomography(elevatedImagePoints, dstPoints);
  
  Size outputSize(dstWidth, dstWidth);
  Mat dst(outputSize, wipColorCoded.type());
  warpPerspective(piecesImage, dst, homography, outputSize);
  
  Scalar gridColor = CV_RGB(100,50,0);
  int gridThickness = 2;
  for(int i = 1; i < BOARD_SIZE; i++)
  {
    int pos = i * TILE_PIXEL_WIDTH;
    line(dst, Point(0, pos), Point(BOARD_SIZE * TILE_PIXEL_WIDTH, pos), gridColor, gridThickness);
    line(dst, Point(pos, 0), Point(pos, BOARD_SIZE * TILE_PIXEL_WIDTH), gridColor, gridThickness);
  }
  
  displayImage(wipColorCoded, "clusters");
  
  displayImage(triplesOneChannel,"flooded_edges");
  displayImage(dst,"output");
  
  waitKey();
 
  return 0;
}
