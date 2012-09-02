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

#include "constants.hpp"
#include "ImageOperations.hpp"

using namespace cv;
using namespace std;
using namespace se;

namespace se {

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

  int h_lower_tresh, h_upper_tresh, sat_tresh, val_tresh;
  h_lower_tresh = 15;
  h_upper_tresh = 180 - h_lower_tresh;
  sat_tresh = 255 * 6 / 10;
  val_tresh = 255 * 5 / 10;
  Mat h_lower, h_upper, sat, val, output;
  
  compare(hsv_channels[0], h_lower_tresh, h_lower, CMP_LE); 
  compare(hsv_channels[0], h_upper_tresh, h_upper, CMP_GE); 
  
  compare(hsv_channels[1], sat_tresh, sat, CMP_GE); 
  compare(hsv_channels[2], val_tresh, val, CMP_GE);
  
  bitwise_or(h_lower, h_upper, output);
  bitwise_and(output, sat, output);
  bitwise_and(output, val, output);
  
  output.copyTo(triplesOneChannelArray);
}
  
void ImageOperations::clusterTriples(InputArray matArray, OutputArray labels, OutputArray markedArray, OutputArray centersArray ) 
{
  int cluster_count = 8;
  int attempts = 15;
  int marked_count = 0;
  
  Mat mat = matArray.getMat();
  
  vector<vector<float> > markedVec;
  for(int i = 0; i < mat.rows; i++)
  {
    const unsigned char* Mi = mat.ptr<unsigned char>(i);
    for(int j = 0; j < mat.cols; j++)
    {
      if(Mi[j] > 0)
      {
        marked_count++;
        
        vector<float> tmp;
        
        tmp.push_back((float)j);
        tmp.push_back((float)i);
        markedVec.push_back(tmp);
      }
    }
  }
  centersArray.create(Size(2, cluster_count), CV_32FC1);
  Mat centers = centersArray.getMat();
  
  markedArray.create(Size(2, marked_count), CV_32FC1);
  Mat marked = markedArray.getMat();

  for(int i=0; i < marked.rows; i++)
  {
    for(int j=0; j < marked.cols; j++)
    {
      marked.at<float>(i,j) = markedVec[i][j];
    }
  }
  

  labels.create(Size(marked_count, 1), CV_32SC1);
  TermCriteria criteria;
  double compactness = kmeans(marked, cluster_count, labels, criteria, attempts, KMEANS_PP_CENTERS, centers);
}  

}