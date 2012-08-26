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

#ifndef LINE_HPP
#define LINE_HPP

#include <vector>
#include "Utils.hpp"

namespace se {
  /** a structure representing a line equation 
  *** a*x + b*y = c
  ***/
  struct Line {
    double a,b,c;
    
    public:
      
      Line();
      Line(double a, double b, double c);
      
      Point2d intersect(Line other);
      
      void print();
      
      static Line getBySegment(Segment segment);
      
      static vector<Point2d> getCorners(vector<Segment> boundingSegments);
      static vector<Point2d> getCorners(vector<Line> boundingLines);
  };
}
#endif // LINE_HPP
