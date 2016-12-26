// This is /algo/dbetl/dbetl_point_2d.cxx
//:
// \file

#include "dbetl_point_2d.h"
#include "dbetl_camera.h"
#include <dbecl/dbecl_episeg.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_vector_fixed.h>


//: Constructor
dbetl_point_2d::dbetl_point_2d( const dbecl_episeg_sptr& episeg, double index,
                              const dbetl_camera_sptr& camera )
 : dbecl_episeg_point(episeg, index),
   next_(NULL), prev_(NULL),
   camera_(camera)
{
  int i_prev = (int) vcl_floor(index);
  if(i_prev >= (int)_episeg->curve()->size()-1)
    i_prev = _episeg->curve()->size()-2;
  vgl_point_2d<double> p1 = _episeg->curve()->point(i_prev)->get_p();
  vgl_point_2d<double> p2 = _episeg->curve()->point(i_prev+1)->get_p();
  vgl_vector_2d<double> v = p2-p1;
  orientation_ = atan2(v.y(), v.x());
}


//: Destructor
dbetl_point_2d::~dbetl_point_2d()
{
  if(next_)
    next_->prev_ = NULL;
  if(prev_)
    prev_->next_ = NULL;
}


//: Return the distance to the epipole
double 
dbetl_point_2d::dist() const
{
  return _episeg->epipole()->distance(_episeg->curve()->interp(_index));
}


//: Return the angle with the epipole
double 
dbetl_point_2d::angle() const
{
  return _episeg->angle(_index);
}


//: Returns a 2 by 4 matrix used to calculate the x-y projection errors
vnl_matrix<double> 
dbetl_point_2d::proj_error_matrix() const
{
  assert(camera_);
  vnl_vector_fixed<double,4> P1((*camera_)[0]);
  vnl_vector_fixed<double,4> P2((*camera_)[1]);
  vnl_vector_fixed<double,4> P3((*camera_)[2]);

  vgl_point_2d<double> pt = this->pt();

  vnl_matrix<double> M(2,4);
  M.set_row(0, P1 - pt.x()*P3);
  M.set_row(1, P2 - pt.y()*P3);
  return M;
}

