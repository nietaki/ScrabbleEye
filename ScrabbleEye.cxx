#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
          "Usage:\n"
          "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

void clusterTriples(InputArray matArray, OutputArray labels, OutputArray markedArray, OutputArray centersArray ) 
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

int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "res/bed/bed_board.jpg";
 
  Mat src_bgr = imread(filename);
  
  //TODO gaussian blur the image slightly to take care of the text on the red fields
 
  if(src_bgr.empty())
  {
    help();
    cout << "can not open " << filename << endl;
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
  
  for(size_t i=0; i< centers.rows; i++)
  {
    Point_<float> currentPoint(centers.at<float>(i, 0), centers.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    circle(srcBgr, currentIntPoint, 9, CV_RGB(200,200,200), 3);
  }
 
  imshow("source", srcBgr);
  
  /**
  *** flood-filling the clusters
  **/
  unsigned char floodedMark = 150;
  for(int i = 0; i< centers.rows ; i++)
  {
    Point_<float> currentPoint(centers.at<float>(i, 0), centers.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    if(src.at<unsigned char>(currentIntPoint) == 255)
    {
      floodFill(src, currentIntPoint, floodedMark);
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
    if(src.at<unsigned char>(currentIntPoint) != floodedMark)
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
  
  imshow("floodfill", src);
  
 
  waitKey();
 
  return 0;
}
