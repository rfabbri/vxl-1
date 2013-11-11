// This is//Projects/lemsvxl/src/contrib/dbrec_lib/dbrec3d/dbrec3d_primitive_part.h
#ifndef dbrec3d_primitive_kernel_part_h
#define dbrec3d_primitive_kernel_part_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for primitive parts (i.e leafs). 
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  2/24/10
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_part.h"
#include <bvpl/kernels/bvpl_kernel.h>
#include <vcl_iostream.h>
#include <bxml/bxml_document.h>

class dbrec3d_parts_manager;

//: A class that implements the dbrec3d_part for primitive parts (i.e leafs), where primitives are formed from bvpl_kernels
class dbrec3d_primitive_kernel_part: public dbrec3d_part
{
public:
  //Parts are only created by dbrec3d_parts_manager
  friend class dbrec3d_parts_manager;
  
private:
  //: Defaul contructor
  dbrec3d_primitive_kernel_part() : dbrec3d_part(),class_prior_(-1.0f) {}
  
  
  //: Constructor from: part id, greometry, children, neighborhood radious
  dbrec3d_primitive_kernel_part(int type_id, vnl_float_3 axis, vnl_float_3 aux_axis, float angle, vnl_float_3 scale, vnl_float_3 radius, bool symmetric, bool rot_invar, const vcl_vector<dbrec3d_part_sptr>& children) 
  : dbrec3d_part(type_id, axis, aux_axis, angle, scale,radius, symmetric, rot_invar),class_prior_(-1.0f), kernel_(NULL){}
  
 
  //: Construct from bvpl_kernel
  dbrec3d_primitive_kernel_part(int type_id, float class_prior, bvpl_kernel_sptr kernel, bool symmetric, bool rot_invar):
  dbrec3d_part(type_id, kernel->axis(), kernel->aux_axis(), kernel->angle(), kernel->scale(),vnl_float_3(kernel->dim().x()*kernel->voxel_length(),kernel->dim().y()*kernel->voxel_length(),kernel->dim().z()*kernel->voxel_length()),
               symmetric, rot_invar), class_prior_(class_prior), kernel_(kernel){}
  
  //: Copy constructor
  dbrec3d_primitive_kernel_part(const dbrec3d_primitive_kernel_part& other): dbrec3d_part(other.type_id_,other.axis_, other.aux_axis_, other.angle_,other.scale_, other.radius_, other.symmetric_, other.rot_invar_), 
  class_prior_(other.class_prior_){}
 
public:
  
   //: Destructor
  virtual ~dbrec3d_primitive_kernel_part(){}
  
  //: Allow for various visitors (encapsulated algos working with the meta-structure) visit the composition
  //virtual void accept(dbrec_visitor* v); 
  
  //: Return the class prior probability
  const float class_prior() { return class_prior_; }
  
  //: Set the class prior probability
  void set_class_prior(float p) { class_prior_ = p; }
  
  //: Get direction vector for this part
  //const vnl_float_3 direction_vector() { return direction_vector_ };
  
  //: XML write
  virtual bxml_data_sptr xml_element() const;
  
  //: XML read
  static dbrec3d_part_sptr parse_xml_element(bxml_data_sptr d);
  
  virtual void accept(dbrec3d_visitor* v);
  
  //: The global length of smallest resolution length
  virtual double min_cell_length(){ return kernel_->voxel_length();}
  
  bvpl_kernel_sptr kernel() {return kernel_;}
  
protected:
  
  //: Prior distribution of this composition
  float class_prior_;  
  
  //: The primitive kernel associated with this part - usuful for non-maxima suppression
  bvpl_kernel_sptr kernel_;
 
  // Primitive description from bvpl_kernel

  //: Dimension of bounding box
  //vgl_vector_3d<int> dim_;
  
};

//vcl_ostream & operator<<(vcl_ostream& out, const dbrec3d_primitive_part& p);


#endif
