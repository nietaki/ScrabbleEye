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

void clusterTriples(Mat mat, InputOutputArray labels, vector<vector<float> >& marked, vector<vector<float> > centres ) 
{   
  //todo: replace vectors of vectors with matrices
  for(int i = 0; i < mat.rows; i++)
  {
    const unsigned char* Mi = mat.ptr<unsigned char>(i);
    for(int j = 0; j < mat.cols; j++)
    {
      if(Mi[j] > 0)
      {
	vector<float> tmp;
	tmp.push_back((float)i);
	tmp.push_back((float)j);
	marked.push_back(tmp);
	labels.push_back(0);
      }
    }
  }

  //TermCriteria criteria(TermCriteria.EPS + TermCriteria.COUNT, 100, 0.1);
  TermCriteria criteria;
  double compactness = kmeans(marked, 8, labels, criteria, 7, KMEANS_RANDOM_CENTERS, centres);
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
  
  Mat dst, cdst;
  
  src = output; //src is the processed thing
  
  vector<vector<float> > marked;
  vector<int> labels;
  vector<vector<float> > centres;
  //clusterTriples(src, labels, marked, centres);
  Canny(src, dst, 50, 200, 3);
  cvtColor(dst, cdst, CV_GRAY2BGR);
 
  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
 
  imshow("source", src);
  imshow("detected lines", cdst);
 
  waitKey();
 
  return 0;
}
