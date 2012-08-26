/*
 * Utils.hpp
 *
 *  Created on: Feb 25, 2012
 *      Author: strop
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>

class Utils {
public:
  Utils();
  virtual ~Utils();
  static int stringToInt(std::string s);
  static std::string intToString(int s);
  static std::string readFile(std::string path);
};

#endif /* UTILS_HPP_ */
