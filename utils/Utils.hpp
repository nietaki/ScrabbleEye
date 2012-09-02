#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;



namespace se {
  const unsigned char FLOOD_MARK = 150;
  
  typedef std::pair<Point, Point> Segment;
  
  class Utils {
  public:
    Utils();
    virtual ~Utils();
    static int stringToInt(std::string s);
    static std::string intToString(int s);
    static std::string readFile(std::string path);
  };
  
  template <class T>
  vector<T> rotate(const vector<T>& rotatedVector, typename vector<T>::const_iterator newBeginning)
  {
    vector<T> ret;
    ret.insert(ret.end(), newBeginning, rotatedVector.end());
    ret.insert(ret.end(), rotatedVector.begin(), newBeginning);
    return ret;
  }

  vector<Segment>::iterator findClosestSegment(vector<Segment>& segments, Point point);

  void concatenateSegment(vector<Segment>& segments, Segment firstSegment);

  unsigned char getFloodMark(int clusterNo);

  int getLabelByMark(unsigned char mark);

  vector<Point> centerMatrixToPointVector(Mat centers);

  vector<Point> getNonZeroPoints(InputArray matArray);

  void paintSegments(Mat canvas, vector<Segment> lines, Scalar color, int thickness);
  
  void displayImage(Mat image, const char* windowName);
}
#endif /* UTILS_HPP_ */
