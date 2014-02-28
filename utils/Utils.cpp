#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Utils.hpp"
#include "constants.hpp"

namespace se {
  using namespace std;
  Utils::Utils() {

  }

  Utils::~Utils() {
  }
  
  /**
   * @brief returns a vector of OpenCV colors of size "count"
   * 
   * this should be useful for coloring collections of stuff
   */
  vector<Scalar> Utils::getColors(int count)
  {
    CV_Assert(count <= 8);
    vector<Scalar> colors;
    colors.push_back(Scalar(255,0,0));
    colors.push_back(Scalar(255,255,0));
    colors.push_back(Scalar(0,255,0));
    colors.push_back(Scalar(0,255,255));
    colors.push_back(Scalar(0,0,255));
    colors.push_back(Scalar(255,0,255));
    colors.push_back(Scalar(200,100,200));
    colors.push_back(Scalar(50,100,50));
    colors.resize(count);
    return colors;
  }


  // TODO conversion errors 
  int Utils::stringToInt(std::string s) {
    std::stringstream converter;
    converter << s;
    int result;
    converter >> result;
    return result;
  }

  std::string Utils::intToString(int s) {
    std::stringstream converter;
    converter << s;
    return converter.str();
  }

  std::string Utils::readFile(std::string path) {
  // http://www.gamedev.net/topic/353162-reading-a-whole-file-into-a-string-with-ifstream/
    std::ifstream ifs(path.c_str());
    std::string f((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    //std::cout << f << std::endl;
    return f;
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
    CV_Assert(target1 != segments.end());
    target2 = target1+1;
    CV_Assert(target2 != segments.end());
    
    target1->second = target2->second;
    segments.erase(target2);
  }
  
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
  
  void paintSegments(Mat canvas, vector<Segment> lines, Scalar color, int thickness)
  {
    int i =0;
    for(vector<Segment>::iterator it = lines.begin(); it != lines.end(); it++)
    {
      
      putText(canvas ,Utils::intToString(i) , (it->first + it->second)*0.5, 0, 5, CV_RGB(255,255,255));
      line(canvas, it->first, it->second, color, thickness);
      i++;
    }
  }
  
  void displayImage(Mat image, const char* windowName)
  {
    namedWindow(windowName, CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL | CV_GUI_EXPANDED);
    resizeWindow(windowName, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    imshow(windowName, image);
  }
}






