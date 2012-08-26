#include "Utils.hpp"

#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>
namespace se {
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
}






