// This is /contrib/biotree/proj/error_functions.h

#ifndef error_functions_h_
#define error_functions_h_

//: 
// \file    error_functions.h
// \brief   a class to hold error function estimations
// \author  H. Can Aras
// \date    2006-07-16
// 

class error_functions{
public:
  //: constructor
  error_functions();
  ~error_functions();

  double erfi(double x, int order);

};

#endif
