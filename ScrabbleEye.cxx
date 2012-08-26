#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <utility>
#include <vector>


using namespace cv;
using namespace std;
typedef std::pair<Point, Point> Segment;

/** a structure representing a line equation 
*** a*x + b*y = c
***/
struct Line {
  double a,b,c;
  
  public:
    
    Line():a(0),b(0),c(0){}
    Line(double a, double b, double c):a(a),b(b),c(c){}
    
    Point2d findIntersection(Line other){
      Point2d ret;
      return ret;
    }
    
    void print()
    {
      std::cout << a << "*x + " << b << "*y = " << c << endl;
    }
  
    static Line getBySegment(Segment segment)
    {
      double x1, x2, y1, y2;
      x1 = (double) segment.first.x;
      y1 = (double) segment.first.y;
      x2 = (double) segment.second.x;
      y2 = (double) segment.second.y;
      
      Line ret;
      double tmp;
      ret.a = (y2 - y1)/(x2 - x1);
      ret.b = -1.0;
      ret.c = ret.a * x1 - y1;
      
      return ret;  
    }
};





void help()
{
  cout << "\nThis program demonstrates line finding with the Hough transform.\n"
          "Usage:\n"
          "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

template <class T>
vector<T> rotate(const vector<T>& rotatedVector, typename vector<T>::const_iterator newBeginning)
{
  vector<T> ret;
  ret.insert(ret.end(), newBeginning, rotatedVector.end());
  ret.insert(ret.end(), rotatedVector.begin(), newBeginning);
  return ret;
}



vector<Segment>::iterator findClosestSegment(vector<Segment>& segments, Point point)
{
  vector<Segment>::iterator ret = segments.begin();
  double shortestDistance = std::numeric_limits<double>::max();
  
  for(vector<Segment>::iterator it = segments.begin(); it != segments.end(); it++)
  {
    Segment curSegment = *it;
    double curDistance = norm(((curSegment.first + curSegment.second) * 0.5) - point);
    if(curDistance < shortestDistance){
      shortestDistance = curDistance;
      ret = it;
    }
  }
  return ret;
}

void concatenateSegment(vector<Segment>& segments, Segment firstSegment)
{
  vector<Segment>::iterator target1, target2;
  target1 = find(segments.begin(), segments.end(), firstSegment);
  assert(target1 != segments.end());
  target2 = target1+1;
  assert(target2 != segments.end());
  
  target1->second = target2->second;
  segments.erase(target2);
}


unsigned char FLOOD_MARK = 150;

unsigned char getFloodMark(int clusterNo)
{
  return FLOOD_MARK + (unsigned char) clusterNo;
}

int getLabelByMark(unsigned char mark)
{
  return (int) (mark - FLOOD_MARK);
}

vector<Point> centerMatrixToPointVector(Mat centers)
{
  vector<Point> ret;
  for(size_t i=0; i< centers.rows; i++)
  {
    Point_<float> currentPoint(centers.at<float>(i, 0), centers.at<float>(i, 1));
    Point currentIntPoint = currentPoint;
    ret.push_back(currentIntPoint);
  }
  return ret;
}

vector<Point> getNonZeroPoints(InputArray matArray) {
  Mat mat = matArray.getMat();
  
  cout << norm(Point(-3, 4)) << endl;
  
  vector<Point> markedVec;
  for(int i = 0; i < mat.rows; i++)
  {
    const unsigned char* Mi = mat.ptr<unsigned char>(i);
    for(int j = 0; j < mat.cols; j++)
    {
      if(Mi[j] > 0)
      {
	Point tmp;
	tmp.x = j;
	tmp.y = i;
	markedVec.push_back(tmp);
      }
    }
  }
  return markedVec;
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

void paintSegments(Mat canvas, vector<Segment> lines, Scalar color, int thickness)
{
  for(vector<Segment>::iterator it = lines.begin(); it != lines.end(); it++)
  {
    line(canvas, it->first, it->second, color, thickness);
  }
}

int main(int argc, char** argv)
{
  Point p1(1,2);
  Point p2(3,-2);
  Segment seg(p1,p2);
  Line l1 = Line::getBySegment(seg);
  l1.print();
  exit(0);
  const char* filename = argc >= 2 ? argv[1] : "res/boards/1080/board.jpg";
 
  int default_window_width = 1024;
  int default_window_height = 768;
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
  
  convexHull(nonZeroPoints, hullPointIndices, true, false);
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
  for(vector<int>::iterator it = hullPointIndices.begin(); it != hullPointIndices.end(); it++) {
    lastPoint = curPoint;
    curPoint = nonZeroPoints[*it];
    
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
  
  
  //concatenateSegment(vector<Segment>& segments, Segment firstSegment);
  
  const char* win1name = "clusters";
  namedWindow(win1name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  resizeWindow(win1name, default_window_width, default_window_height);
  imshow(win1name, srcBgr);
 
  const char* win2name = "flooded_edges";
  namedWindow(win2name, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
  imshow(win2name, src);
  resizeWindow(win2name,default_window_width, default_window_height);
  
  waitKey();
 
  return 0;
}
