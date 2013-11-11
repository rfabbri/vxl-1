// This is dbrec3d_part_instance.h
#ifndef dbrec3d_part_instance_h
#define dbrec3d_part_instance_h

//:
// \file
// \brief Instance containing extrinsic/nons-shearable characteristics of a dbrec3d_part
// \author Isabel Restrepo mir@lems.brown.edu
// \date  2-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

class dbrec3d_part_instance
{

public:
  //: Default constructor
  dbrec3d_part_instance():type_id_(-1), posterior_(0.0f), location_(vgl_point_3d<double>(0.0,0.0,0.0)){}
  
  //:Constructor from id, posterior and location
  dbrec3d_part_instance(int type_id, float posterior, vgl_point_3d<double> location):
  type_id_(type_id),location_(location) , posterior_(posterior) {}
  
  
  // Accessors
  
  int type_id() const { return type_id_; } 
  vgl_point_3d<double> location() const {return location_; }
  float posterior() const {return posterior_;}
 
  //: Return global orientation axis of this part
  //  vnl_float_3 axis() { return axis_; }
  //: Retrun rotation angle about the axis of this part
  //  float angle() { return angle_; }
  
  // Setters
  void set_type_id(int type_id) { type_id_ = type_id; }
  void set_location(vgl_point_3d<double> location) { location_ = location; }

  // This should be the posterior?
  //void set_response(float response) { response_=response; }
  void set_posterior(float posterior) { posterior_=posterior; }

  //: I/O version
  short version_no() const {return 1; }
  
  
protected:
  
  //: Id to the intrinsic part
  int type_id_;
  
  //: 3d location of the part
  vgl_point_3d<double> location_;
  
 
  //: This should be changed to posterion, probably doesn't need to be templates
  //float response_;
  
  //: Posterior probability
  float posterior_;
  
  //: Global orientation - axis - should it be normalized?
  // vnl_float_3 axis_;
  
  //: Angle of rotation about its axis
  //float angle_;

};


class dbrec3d_pair_composite_instance : public dbrec3d_part_instance
{
public:
  //: Default constructor
  dbrec3d_pair_composite_instance():dbrec3d_part_instance(), location_child1_(vgl_point_3d<double>(0.0,0.0,0.0)),  location_child2_(vgl_point_3d<double>(0.0,0.0,0.0)){}
  
  //:Constructor from id, location, children
  dbrec3d_pair_composite_instance(int type_id, float posterior, vgl_point_3d<double> location_child1, vgl_point_3d<double> location_child2):
  dbrec3d_part_instance(type_id,posterior,midpoint(location_child1, location_child2)), location_child1_(location_child1), location_child2_(location_child2){}
  
  void set_location_child1(vgl_point_3d<double> location) { location_child1_ = location; }
  void set_location_child2(vgl_point_3d<double> location) { location_child2_ = location; }

  vgl_point_3d<double> location_child1() const {return location_child1_; }
  vgl_point_3d<double> location_child2() const {return location_child2_; }


protected:
  //: 3d location of the children - assume pairwise composition for now, this probably needs to change
  vgl_point_3d<double> location_child1_;
  vgl_point_3d<double> location_child2_;
  
  
};


//I/O
void vsl_b_write(vsl_b_ostream & os, dbrec3d_part_instance const &part);

void vsl_b_write(vsl_b_ostream & os, dbrec3d_part_instance const * &part);

void vsl_b_read(vsl_b_istream & is, dbrec3d_part_instance &part);

void vsl_b_read(vsl_b_istream & is, dbrec3d_part_instance *&part);

vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_part_instance &part);

void vsl_b_write(vsl_b_ostream & os, dbrec3d_pair_composite_instance const &part);

void vsl_b_write(vsl_b_ostream & os, dbrec3d_pair_composite_instance const * &part);

void vsl_b_read(vsl_b_istream & is, dbrec3d_pair_composite_instance &part);

void vsl_b_read(vsl_b_istream & is, dbrec3d_pair_composite_instance *&part);

vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_pair_composite_instance &part);


#endif
