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

#include "ImageOperations.hpp"

using namespace cv;
using namespace std;
using namespace se;

namespace se {

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
  //Mat marked(Size(2, marked_count), CV_32FC1);
  for(int i=0; i < marked.rows; i++)
  {
    for(int j=0; j < marked.cols; j++)
    {
      
      marked.at<float>(i,j) = markedVec[i][j];
    }
  }
  
  //marked.create(Size(marked_count, 2), CV_32FC1);
  //marked.getMat().assignTo();
  labels.create(Size(marked_count, 1), CV_32SC1);
  //centers.create(Size(cluster_count, 2), CV_32FC1);
  //TermCriteria criteria(TermCriteria.EPS + TermCriteria.COUNT, 100, 0.1);
  TermCriteria criteria;
  
  double compactness = kmeans(marked, cluster_count, labels, criteria, attempts, KMEANS_PP_CENTERS, centers);
  //exit( 0);
}  

}