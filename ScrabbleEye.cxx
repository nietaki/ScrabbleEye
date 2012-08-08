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

void clusterTriples(InputArray matArray, OutputArray labels, OutputArray markedArray/*, OutputArray centers */) 
{
  int cluster_count = 8;
  int attempts = 7;
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
	tmp.push_back((float)i);
	tmp.push_back((float)j);
	markedVec.push_back(tmp);
      }
    }
  }
 
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
  Mat labels2(Size(1, marked_count), CV_32SC1);
  vector<int> labels3(marked_count);
  

  double compactness = kmeans(marked, cluster_count, labels, criteria, attempts, KMEANS_RANDOM_CENTERS);
  //exit( 0);
}  

int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "res/bed/bed_board.jpg";
 
  Mat src_bgr = imread(filename);
 
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
  clusterTriples(src, labels, marked /*,OutputArray marked, centers*/ ) ;
  //Canny(src, dst, 50, 200, 3);
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
    Point_<float> currentPoint(marked.at<float>(i, 1), marked.at<float>(i, 0));
    Point currentIntPoint = currentPoint;
    circle(srcBgr, currentIntPoint, 1, colors[labels[i]]);
    //line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
 
  imshow("source", srcBgr);

 
  waitKey();
 
  return 0;
}
