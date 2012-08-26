/*
    <one line to give the program's name and a brief idea of what it does.>
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
#include <iostream>
#include "Line.hpp"

namespace se {

  Line::Line():a(0),b(0),c(0){}
  Line::Line(double a, double b, double c):a(a),b(b),c(c){}
  
  Point2d Line::intersect(Line other){
    //A*x = B
    //rows, cols, type
    Mat A(2, 2, CV_64FC1);
    Mat x(2, 1, CV_64FC1);
    Mat B(2, 1, CV_64FC1);
   
    A.at<double>(0, 0) = this->a;
    A.at<double>(0, 1) = this->b;
    B.at<double>(0, 0) = this->c;
    
    A.at<double>(1, 0) = other.a;
    A.at<double>(1, 1) = other.b;
    B.at<double>(1, 0) = other.c;
    
    solve(A, B, x, DECOMP_QR);
    
    Point2d ret(x.at<double>(0, 0), x.at<double>(1, 0));
    return ret;
  }
  
  void Line::print()
  {
    std::cout << a << "*x + " << b << "*y = " << c << std::endl;
  }
  
  Line Line::getBySegment(Segment segment)
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
  
  vector<Point2d> Line::getCorners(vector<Segment> boundingSegments)
  {
    vector<Line> lines;
    for(vector<Segment>::iterator it = boundingSegments.begin(); it != boundingSegments.end(); it++)
    {
      lines.push_back(Line::getBySegment(*it));
    }
    return getCorners(lines);
  }
  
  vector<Point2d> Line::getCorners(vector<Line> boundingLines)
  {
    vector<Point2d> ret;
    int cornerCount = boundingLines.size();
    for(int i = 0; i < cornerCount; i++)
    {
      int j = (i + 1) % cornerCount;
      ret.push_back(boundingLines[i].intersect(boundingLines[j]));
    }
    return ret;
  }
  
  
  
}

