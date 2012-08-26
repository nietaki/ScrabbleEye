#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;



namespace se {
  typedef std::pair<Point, Point> Segment;
  
  class Utils {
  public:
    Utils();
    virtual ~Utils();
    static int stringToInt(std::string s);
    static std::string intToString(int s);
    static std::string readFile(std::string path);
  };
}
#endif /* UTILS_HPP_ */
