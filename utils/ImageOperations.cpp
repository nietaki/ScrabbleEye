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

#include <vector>
#include <iostream>
#include <exception>

#include "constants.hpp"
#include "ImageOperations.hpp"

using namespace cv;
using namespace std;
using namespace se;

namespace se {

/**
 * @brief accepts an image as input, outputs a one channel output Matrix with the triple score fields marked as non-zero
 * 
 * @param boardImageArray the color input image Array
 * @param triplesOneChannelArray the output one channel array with only the triple score fields having non-zero values
 */
void ImageOperations::extractTriples(InputArray boardImageArray, OutputArray triplesOneChannelArray)
{
  Mat boardImage = boardImageArray.getMat();
  
  CV_Assert(!boardImage.empty());
  CV_Assert(boardImage.depth() == CV_8U);
  CV_Assert(boardImage.channels() == 3);
  
  Mat src_hsv;
  vector<Mat> bgr_channels, hsv_channels;
  cvtColor(boardImage, src_hsv, CV_BGR2HSV);
  
  split(src_hsv, hsv_channels);
  split(boardImage, bgr_channels);

  unsigned char h_lower_tresh, h_upper_tresh, sat_tresh, val_tresh;
  h_lower_tresh = 7; // x degrees
  h_lower_tresh = 5; // x degrees
  h_upper_tresh = 179 - h_lower_tresh;
  sat_tresh = 255 * 65 / 100; 
  sat_tresh = 255 * 6 / 10; 
  val_tresh = 255 * 4 / 10;
  Mat h_lower, h_upper, sat, val, output;
  
  int x = 1519;
  int y = 570;
  //std::cout << "the pixel " << (uint)hsv_channels[0].at<unsigned char>(y, x) << "," << (uint)hsv_channels[1].at<unsigned char>(y, x) << "," << (uint)hsv_channels[2].at<unsigned char>() << std::endl;
  compare(hsv_channels[0], h_lower_tresh, h_lower, CMP_LE); 
  compare(hsv_channels[0], h_upper_tresh, h_upper, CMP_GE); 
  
  compare(hsv_channels[1], sat_tresh, sat, CMP_GE); 
  compare(hsv_channels[2], val_tresh, val, CMP_GE);
  
  bitwise_or(h_lower, h_upper, output);
  bitwise_and(output, sat, output);
  bitwise_and(output, val, output);
  
  output.copyTo(triplesOneChannelArray);
}
  
/**
 * @brief performs the triple score fields clustering - clusters them into 8 clusters.
 * @param matArray input one channel array
 * @param labels
 * @param markedArray a collection of points clustered, used for input of the kmeans
 * @param centersArray a 2x8 float array of (x,y) cluster center coordinates
 */
void ImageOperations::clusterTriples(InputArray inputArray, OutputArray labels, OutputArray markedArray, OutputArray centersArray ) 
{
  //kmeans only accepts float data, that's what all the conversion is for
  int cluster_count = 8;
  int attempts = 5;
  
  //the count of non-zero/marked points
  int marked_count = 0;
  Mat input = inputArray.getMat();
  
  Mat marked(Size(2, 0), CV_32FC1);
  Mat tmpMat(Size(2, 1), CV_32FC1);
  for(int i = 0; i < input.rows; i++)
  {
    const unsigned char* Mi = input.ptr<unsigned char>(i);
    for(int j = 0; j < input.cols; j++)
    {
      if(Mi[j] > 0)
      {
        marked_count++;
        tmpMat.at<float>(0, 0) = (float)j;
        tmpMat.at<float>(0, 1) = (float)i;
        marked.push_back(tmpMat);
      }
    }
  }
  marked.copyTo(markedArray);
  
  centersArray.create(Size(2, cluster_count), CV_32FC1);
  Mat centers = centersArray.getMat();
  
  labels.create(Size(marked_count, 1), CV_32SC1);
  TermCriteria criteria(TermCriteria::EPS, 133700, 5.0);
  double compactness = kmeans(marked, cluster_count, labels, criteria, attempts, KMEANS_PP_CENTERS, centers);
}  

void ImageOperations::fixclusterCentres(InputArray image, vector<Point2i>& centers)
{
  CV_Assert(image.channels() == 1);
  CV_Assert(image.depth() == CV_8U);
  Mat imageMat = image.getMat();
  
  for(int idx = 0; idx < centers.size(); idx++) {
    cout << centers[idx] << endl;
    fixSingleClusterCenter(imageMat, centers[idx]);
  }
}

void ImageOperations::fixSingleClusterCenter(Mat image, Point2i& center) {
  Point2i copy = center;
  if(image.at<unsigned char>(center) != 0) {
    //already good
    //std::cout << "already good" << std::endl;
    return;
  }
  
  for(int stride = 2; stride < 100; stride += 2) {
    //layer after layer - first square size is 3, moving by 2
    center.x -= 1; center.y -= 1;
    Point2i dirs[4] = {Point2i(1, 0), Point2i(0, 1), Point2i(-1, 0), Point2i(0, -1)};
    for(Point2i& dir : dirs) {
      //each side
      for(int it = 0; it < stride; it++) {
        //each point on each side
        center.x += dir.x; center.y += dir.y;
        //std::cout << image.at<int>(center) << std::endl;
        if(image.at<unsigned char>(center) != 0){
          return;
        }
      }
    }
  }
  throw std::runtime_error("couldn't fix cluster center");
}

}