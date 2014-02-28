/*
 * 
 */

#ifndef ASSERTIONEXCEPTION_H
#define ASSERTIONEXCEPTION_H

/** 
 * this is what I would do if I wanted to rewrite CV_Exception
 */
class AssertionException
{
 const char* filename; 
 int line;
 const char* assertion;
public:
AssertionException(const char* filename, int line, const char* assertion);
~AssertionException();
virtual const char* what() const throw();

};

#endif // ASSERTIONEXCEPTION_H
