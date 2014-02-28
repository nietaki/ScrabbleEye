/*
 * 
 */
#include <sstream>
#include "AssertionException.hpp"

AssertionException::AssertionException(const char* filename, int line, const char* assertion):filename(filename),line(line), assertion(assertion)
{

}

AssertionException::~AssertionException()
{

}


const char* AssertionException::what() const throw()
{
  std::stringstream ret;
  ret << "assertion failed in " << filename << " in line " << line << ": " << assertion; 
  return ret.str().c_str(); 
}
