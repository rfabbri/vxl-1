// This is /algo/dbetl/dbetl_camera.h
#ifndef dbetl_camera_h_
#define dbetl_camera_h_

//:
// \file
// \brief  A camera matrix with ref counting
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/22/2004
//
// This object is only needed because there is currently no
// photogrammetry library



#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_double_3x4.h>


//: A camera matrix with ref counting
class dbetl_camera : public vbl_ref_count, public vnl_double_3x4 {
public:
  //: Constructor
  dbetl_camera(){}

  //: Constructor
  dbetl_camera(const vnl_double_3x4& C) : vnl_double_3x4(C) {}

};



#endif // dbetl_camera_h_
