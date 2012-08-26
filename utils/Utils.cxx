#include "Utils.hpp"

#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>
namespace se {
  using namespace std;
  Utils::Utils() {

  }

  Utils::~Utils() {
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
    assert(target1 != segments.end());
    target2 = target1+1;
    assert(target2 != segments.end());
    
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
}






