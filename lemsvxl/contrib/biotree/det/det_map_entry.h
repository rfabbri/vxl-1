#ifndef det_map_entry_h_
#define det_map_entry_h_
//: 
// \file  det_map_entry.h
// \brief  a data structure for cylinder map entry
// \author    Kongbin Kang
// \date        2005-09-12
// 
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

struct det_map_entry
{
  //: direction of cylinder
  vgl_vector_3d<double> dir_;

  //: strength of the cylinder field
  double strength_;
 
  //: location within each voxel
  vgl_point_3d<double> location_;

  //: radius
  double radius_;
  
  //: default constructor
  det_map_entry() 
    : dir_(0., 0., 0.),location_(0., 0., 0.)
  {
    strength_ = 0;
    radius_ = 0;
  }
 
  //:
  det_map_entry(det_map_entry const & c)
    : dir_(c.dir_), strength_(c.strength_),location_(c.location_)
  {
  }
  
  bool operator==(const det_map_entry& a) const
  {
    return (dir_ == a.dir_) && (strength_ == a.strength_) && 
      (location_ == a.location_); 
  }

#if 0 // default one doing exactly the same thing
  det_map_entry & operator=(const det_cm_entry & e)
  {
    if(&e != this)
    {
      dir_ = e.dir_;
      strength_ = e.strength_;
      location_ = e.location_;
    }

    return *this;
      
  }
#endif 
};

#endif
