#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <utility>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "utils/constants.hpp"
#include "utils/Line.hpp"
#include "utils/Utils.hpp"

using namespace cv;
using namespace std;
using namespace se;

void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
          "Usage:\n"
          "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}



int main(int argc, char** argv)
{

  Mat src_bgr = imread(BOARD_FILENAME);
  Mat piecesImage = imread(PIECES_FILENAME);
  
  //TODO gaussian blur the image slightly to take care of the text on the red fields
 
  if(src_bgr.empty())
  {
    help();
    cout << "can not open " << BOARD_FILENAME << endl;
    return -1;
  }
  CV_Assert(src_bgr.depth() == CV_8U);
  CV_Assert(src_bgr.channels() == 3);
  Mat src_hsv;
  vector<Mat> bgr_channels, hsv_channels;
  cvtColor(src_bgr, src_hsv, CV_BGR2HSV);
  
  split(src_hsv, hsv_channels);
  split(src_bgr, bgr_channels);
  Mat src = bgr_channels[2]; //src is now the red channel
  int h_lower_tresh, h_upper_tresh, sat_tresh, val_tresh;
  h_lower_tresh = 10;
  h_upper_tresh = 180 - h_lower_tresh;
  sat_tresh = 255 * 8 / 10;
  val_tresh = 255 * 5 / 10;
  Mat h_lower, h_upper, sat, val, output;
  
  compare(hsv_channels[0], h_lower_tresh, h_lower, CMP_LE); 
  compare(hsv_channels[0], h_upper_tresh, h_upper, CMP_GE); 
  
  compare(hsv_channels[1], sat_tresh, sat, CMP_GE); 
  compare(hsv_channels[2], val_tresh, val, CMP_GE);
  
  bitwise_or(h_lower, h_upper, output);
  bitwise_and(output, sat, output);
  bitwise_and(output, val, output);
  
  Mat srcBgr;
  
  src = output; //src is the processed thing
  
  Mat marked;
  vector<int> labels;
  Mat centers;
  //clusterTriples(src, labels, marked, centres);
  clusterTriples(src, labels, marked, centers ) ;
  //Canny(src, dst, 50, 200, 3);
  
  /**
  *** visualising the clusters
  **/
  cvtColor(src, srcBgr, CV_GRAY2BGR);
  
  //HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
  vector<Scalar> colors;
  colors.push_back(Scalar(255,0,0));
  colors.push_back(Scalar(255,255,0));
  colors.push_back(Scalar(0,255,0));
  colors.push_back(Scalar(0,255,255));
  colors.push_back(Scalar(0,0,255));
  colors.push_back(Scalar(255,0,255));
  colors.push_back(Scalar(200,100,200));
  colors.push_back(Scalar(50,100,50));
  
  for(size_t i = 0; i < labels.size() ; i++ )
  {
    Point_<float> currentPoint(marked.at<float>(i, 0), marked.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    circle(srcBgr, currentIntPoint, 1, colors[labels[i]]);
  }
  
  vector<Point> centerPoints = centerMatrixToPointVector(centers);
  for(vector<Point>::iterator it = centerPoints.begin(); it != centerPoints.end(); it++)
  {
    circle(srcBgr, *it, 9, CV_RGB(200,200,200), 3);
  }
  /*
  const char* win1name = "clusters";
  namedWindow(win1name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  resizeWindow(win1name, default_window_width, default_window_height);
  
  imshow(win1name, srcBgr);
  */
  /**
  *** flood-filling the clusters
  **/
  //TODO: make sure it works correctly even if the center is on the black text
  for(int i = 0; i < centerPoints.size() ; i++)
  {
    Point currentIntPoint = centerPoints[i];
    if(src.at<unsigned char>(currentIntPoint) == 255)
    {
      //cout << "color: " << floodedMark + (unsigned char) i << endl;
      //different colors denote different clusters
      floodFill(src, currentIntPoint, getFloodMark(i));
    }
    else
    {
      cout	<< currentIntPoint.x << "," << currentIntPoint.y << ": " 
		<< (int)src.at<unsigned char>(currentIntPoint.x, currentIntPoint.y) << endl;
      //circle(src, currentIntPoint, 9, CV_RGB(255,150,150), 3);
    }
  }
  
  
  
  /**
  *** counting the points that are in the cluster but aren't in the flood-filled area
  **/
  //TODO think about the distance from cluster centre variance
  vector<int> misplacedPixelCounts(centers.rows);
  
  //flooding all the spots that contain a cluster center
  for(size_t i = 0; i < labels.size() ; i++ )
  {
    Point_<float> currentPoint(marked.at<float>(i, 0), marked.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    if(src.at<unsigned char>(currentIntPoint) == 255)
    {
      misplacedPixelCounts[labels[i]]++;
      src.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
  // << endl;
  //finding the label that doesn't stick closely to the cluster centre
  int bottomLabel = -1;
  int maxMisplacedPixels = 0;
  for(size_t i = 0; i < centers.rows; i++)
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
  //erasing the most sparse cluster
  for(size_t i = 0; i < labels.size() ; i++ )
  {
    if(labels[i] == bottomLabel)
    {
      Point_<float> currentPoint(marked.at<float>(i, 0), marked.at<float>(i, 1));
      Point currentIntPoint = currentPoint;
      src.at<unsigned char>(currentIntPoint) = 0;
    }
  }
  
  /**
  *** getting the edge points
  **/
  vector<Point> nonZeroPoints;
  nonZeroPoints = getNonZeroPoints(src);
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
  
  Point curPoint, lastPoint;
  int pointNo = 0;
  for(vector<int>::iterator it = hullPointIndices.begin(); it != hullPointIndices.end(); it++) {
    lastPoint = curPoint;
    curPoint = nonZeroPoints[*it];
    
    
    //putText(srcBgr, Utils::intToString(pointNo) , curPoint, 0, 5, CV_RGB(255,255,255));
    
    untouchedClusterLabels.erase(getLabelByMark(src.at<unsigned char>(curPoint)));
    //src.at<unsigned char>(edgePoint.y, edgePoint.y) = 255;
    //src.at<unsigned char>(edgePoint) = 255;
    circle(srcBgr, curPoint, 5, Scalar(255,255,255), 3);
    if(it == hullPointIndices.begin())
      continue;
    
    if(src.at<unsigned char>(lastPoint) != src.at<unsigned char>(curPoint)){
      segments.push_back(Segment(lastPoint, curPoint));
      line(srcBgr, lastPoint, curPoint, Scalar(255,255,255), 2);
    }
    
    pointNo++;
  }
  
  segments = rotate(segments, findClosestSegment(segments, centerPoints[bottomLabel]));
  
  line(srcBgr, segments.front().first, segments.front().second, CV_RGB(255, 0,0), 3);
 
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
  
  paintSegments(srcBgr, segments, CV_RGB(0,150,0), 3);
  vector<Point2d> corners = Line::getCorners(segments);
  
  for(vector<Point2d>::iterator it = corners.begin(); it != corners.end(); it++)
  {
    Point tmp = *it;
    circle(srcBgr, tmp, 12, CV_RGB(0,0,250), 6);
  }
  //concatenateSegment(vector<Segment>& segments, Segment firstSegment);
  
  FileStorage fs2("res/yml/lifecam_studio_1080.yml", FileStorage::READ);
  
  Mat camera_matrix, distortion_coefficients;
  fs2["camera_matrix"] >> camera_matrix;
  fs2["distortion_coefficients"] >> distortion_coefficients;
  double square_size = (double)fs2["square_size"];
  
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
  
  bool transformFound = solvePnP(objectPoints, corners, camera_matrix, distortion_coefficients, rvec, tvec);
  assert(transformFound);
  
  cout << "rvec:" << endl << rvec << endl;
  cout << "tvec:" << endl << tvec << endl;
  
  vector<Point2f> elevatedImagePoints;
  vector<Point> elevatedIntPoints;
  projectPoints(elevatedPoints, rvec, tvec, camera_matrix, distortion_coefficients, elevatedImagePoints);
  
  int i =0;
  for(vector<Point2f>::iterator it = elevatedImagePoints.begin(); it != elevatedImagePoints.end(); it++)
  {
    Point tmp = *it;
    elevatedIntPoints.push_back(tmp);
    circle(srcBgr, tmp, 12, CV_RGB(255,150,150), 6);
    putText(src,Utils::intToString(i) , tmp, 0, 5, CV_RGB(255,255,255));
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
  Mat dst(outputSize, srcBgr.type());
  warpPerspective(piecesImage, dst, homography, outputSize);
  
  Scalar gridColor = CV_RGB(100,50,0);
  int gridThickness = 2;
  for(int i = 1; i < BOARD_SIZE; i++)
  {
    int pos = i * TILE_PIXEL_WIDTH;
    line(dst, Point(0, pos), Point(BOARD_SIZE * TILE_PIXEL_WIDTH, pos), gridColor, gridThickness);
    line(dst, Point(pos, 0), Point(pos, BOARD_SIZE * TILE_PIXEL_WIDTH), gridColor, gridThickness);
  }
  
  const char* win1name = "clusters";
  namedWindow(win1name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  resizeWindow(win1name, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  imshow(win1name, srcBgr);
 
  const char* win2name = "flooded_edges";
  namedWindow(win2name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  imshow(win2name, src);
  resizeWindow(win2name,DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  
  const char* win3name = "output";
  namedWindow(win3name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  imshow(win3name, dst);
  resizeWindow(win3name,DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  
  
  
  waitKey();
 
  return 0;
}
