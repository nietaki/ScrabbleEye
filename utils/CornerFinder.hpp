/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef CORNERFINDER_H
#define CORNERFINDER_H

#include <vector>
#include <string>
#include "opencv2/opencv.hpp"
namespace se {

class CornerFinder
{
public:
  CornerFinder();
  ~CornerFinder();
  CornerFinder& setPopups(bool popups);
  CornerFinder& setDebug(bool debug);
  CornerFinder& dumpIntermediate(std::string targetFilename);
  
  std::vector<cv::Point2d> getCorners(cv::InputArray boardImage);
private:
  bool popups;
  bool debug;
  std::string dumpFilename;
};

inline CornerFinder& CornerFinder::setDebug(bool debug)
{
  this->debug = debug;
  return *this;
}

inline CornerFinder& CornerFinder::setPopups(bool popups)
{
  this->popups = popups;
  return *this;
}

inline CornerFinder& CornerFinder::dumpIntermediate(std::string targetFilename) {
  this->dumpFilename = targetFilename;
  return *this;
}
}
#endif // CORNERFINDER_H
