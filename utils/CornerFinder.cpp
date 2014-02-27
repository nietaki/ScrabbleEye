/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "opencv2/opencv.hpp"

#include "CornerFinder.hpp"
#include "constants.hpp"
#include "ImageOperations.hpp"
#include "Line.hpp"
#include "Utils.hpp"

using namespace cv;

namespace se {
  
CornerFinder::CornerFinder():popups(false),debug(true) { }

CornerFinder::~CornerFinder() { }

std::vector< Point2d > CornerFinder::getCorners(InputArray boardImage)
{
  CV_Assert(!boardImage.empty());
  Mat blurredBoardImage, triplesOneChannel, erodedTriplesOneChannel, dilatedTriplesOneChannel, onlyRed;
  GaussianBlur(boardImage, blurredBoardImage, Size(GAUSSIAN_SIZE, GAUSSIAN_SIZE), GAUSSIAN_SIGMA, GAUSSIAN_SIGMA);
  displayImage(blurredBoardImage, "gausser", popups);
  
  ImageOperations::extractTriples(blurredBoardImage, triplesOneChannel);
  
  bitwise_and(blurredBoardImage, blurredBoardImage, onlyRed, triplesOneChannel);
  displayImage(onlyRed, "onlyRed", popups);
  displayImage(triplesOneChannel, "extracted_triples", popups);
  
  //TODO: erode to get rid of tiny spots, afterwards dilate to get back to the original size (or a little further)
  erode(triplesOneChannel, erodedTriplesOneChannel, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
  dilate(erodedTriplesOneChannel, dilatedTriplesOneChannel, getStructuringElement(MORPH_ELLIPSE, Size(7,7)));
  //displayImage(erodedTriplesOneChannel, "extracted_triples_eroded", popups);//TODO
  
  Mat preprocessedOneChannel = dilatedTriplesOneChannel;
  
  Mat wipColorCoded, nonZeroPointCoordinates, clusterCenters;
  vector<int> nonZeroPointLabels;
  
  ImageOperations::clusterTriples(preprocessedOneChannel, nonZeroPointLabels, nonZeroPointCoordinates, clusterCenters);
  
  vector<Point2i> centerPoints = centerMatrixToPointVector(clusterCenters);
  
  
  //TODO: test if this works correctly - possibly create a function that goes through all this and throws exceptions when there 
  // are problems with interpretation at one point or another. 
  ImageOperations::fixclusterCentres(preprocessedOneChannel, centerPoints);
  
  /**
  *** visualising the clusters
  **/
  cvtColor(preprocessedOneChannel, wipColorCoded, CV_GRAY2BGR);
  vector<Scalar> colors = Utils::getColors(8);
  for(size_t i = 0; i < nonZeroPointLabels.size(); i++) {
    Point_<float> currentPoint(nonZeroPointCoordinates.at<float>(i, 0), nonZeroPointCoordinates.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    circle(wipColorCoded, currentIntPoint, 1, colors[nonZeroPointLabels[i]]); //this is actually just a dot
  }
  for(vector<Point>::iterator it = centerPoints.begin(); it != centerPoints.end(); it++)
  {
    circle(wipColorCoded, *it, 9, CV_RGB(200,200,200), 3);
  }

  /**
  *** flood-filling the clusters
  **/
  for(int i = 0; i < centerPoints.size() ; i++)
  {
    Point currentIntPoint = centerPoints[i];
    if(preprocessedOneChannel.at<unsigned char>(currentIntPoint) == 255)
    {
      //cout << "color: " << floodedMark + (unsigned char) i << endl;
      //different colors denote different clusters
      floodFill(preprocessedOneChannel, currentIntPoint, getFloodMark(i));
    }
    else
    {
      cout  << currentIntPoint.x << "," << currentIntPoint.y << ": " 
        << (int)preprocessedOneChannel.at<unsigned char>(currentIntPoint) << endl;
      //circle(src, currentIntPoint, 9, CV_RGB(255,150,150), 3);
    }
  }
  
  
  
  /**
  *** counting the points that are in the cluster but aren't in the flood-filled area
  **/
  //TODO think about the distance from cluster centre variance
  vector<int> misplacedPixelCounts(clusterCenters.rows);
  
  //flooding all the spots that contain a cluster center
  for(size_t i = 0; i < nonZeroPointLabels.size() ; i++ )
  {
    Point_<float> currentPoint(nonZeroPointCoordinates.at<float>(i, 0), nonZeroPointCoordinates.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    if(preprocessedOneChannel.at<unsigned char>(currentIntPoint) == 255)
    {
      misplacedPixelCounts[nonZeroPointLabels[i]]++;
      preprocessedOneChannel.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
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
  for(size_t i = 0; i < nonZeroPointLabels.size() ; i++ )
  {
    if(nonZeroPointLabels[i] == bottomLabel)
    {
      Point_<float> currentPoint(nonZeroPointCoordinates.at<float>(i, 0), nonZeroPointCoordinates.at<float>(i, 1));
      Point currentIntPoint = currentPoint;
      preprocessedOneChannel.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
  /**
  *** getting the edge points
  **/
  vector<Point> nonZeroPoints;
  nonZeroPoints = getNonZeroPoints(preprocessedOneChannel);
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
    
    untouchedClusterLabels.erase(getLabelByMark(preprocessedOneChannel.at<unsigned char>(curPoint)));
    //src.at<unsigned char>(edgePoint.y, edgePoint.y) = 255;
    //src.at<unsigned char>(edgePoint) = 255;
    circle(wipColorCoded, curPoint, 5, Scalar(255,255,255), 3);
    if(it == hullPointIndices.begin())
      continue;
    
    if(preprocessedOneChannel.at<unsigned char>(lastPoint) != preprocessedOneChannel.at<unsigned char>(curPoint)){
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
  
  displayImage(wipColorCoded, "clusters", popups);
  
  displayImage(preprocessedOneChannel,"flooded_edges", popups); 
  return corners;
}

}