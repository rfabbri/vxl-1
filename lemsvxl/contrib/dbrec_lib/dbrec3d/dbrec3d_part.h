// This is contrib/dbrec_lib/dbrec3d/dbrec3d_part.h
#ifndef dbrec3d_part_h
#define dbrec3d_part_h

//:
// \file
// \brief A generic 3-dimensional part used for compositional hierarchy. Coordinates system of parts is the  same as the real scene
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  2-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>

#include <vnl/vnl_float_3.h>

#include <vsl/vsl_binary_io.h>

#include <bxml/bxml_document.h>

class dbrec3d_visitor;

//: A 3 dimensional part used for compositional hierarchy. 
//  This part contains only intrinsic characteristics; extrinsing characteristic such as location, response, are not stored here
class dbrec3d_part : public vbl_ref_count
{
  
protected:
  //: Default Constructor
  dbrec3d_part():type_id_(-1), symmetric_(false), rot_invar_(false), axis_(vnl_float_3(0.0f, 0.0f, 0.0f)),aux_axis_(vnl_float_3(0.0f, 0.0f, 0.0f)), angle_(0.0f), scale_(vnl_float_3(0.0f, 0.0f, 0.0f)),radius_(vnl_float_3(0.0f, 0.0f, 0.0f)){} 
  
  //: Constructor from type_id, and other geometric properties
  dbrec3d_part(int type_id, vnl_float_3 axis,vnl_float_3 aux_axis, float angle, vnl_float_3 scale, vnl_float_3 radius, bool symmetric, bool rot_invar) : 
  type_id_(type_id), symmetric_(symmetric), rot_invar_(rot_invar), axis_(axis), aux_axis_(aux_axis), angle_(angle), scale_(scale) , radius_(radius){}
  
  //: Copy Constructor
  dbrec3d_part(const dbrec3d_part& other):type_id_(other.type_id_),symmetric_(other.symmetric_), rot_invar_(other.rot_invar_),
  axis_(other.axis_), aux_axis_(other.aux_axis_), angle_(other.angle_), scale_(other.scale_), radius_(other.radius_){}
  
public:
  enum dbrec3d_part_types {PRIMITIVE, COMPOSITE};
  //: Destructor
  virtual ~dbrec3d_part(){}

  virtual dbrec3d_part_types part_type() { return PRIMITIVE; }
 
  //: Return the type id for this part
  int type_id() const { return type_id_; }
  
  //: Reset the part id for this part
  void reset_type_id(int type_id) { type_id_ = type_id; }
  
  //: Return global orientation axis of this part
  vnl_float_3 axis() { return axis_; }
  
  //: Return global auxiliary axis of this part - used to define a coordinate system
  vnl_float_3 aux_axis() { return aux_axis_; }
  
  //: Return scale of this part
  vnl_float_3 scale() { return scale_; }
  
  //: Retrun rotation angle about the axis of this part
  float angle() { return angle_; }
  
  //: Resturn the radius of the receptive field. The coordinates must be global i.e scene coordinates
  vnl_float_3 radius() {return radius_;}
  
  //: The global length of smallest resolution length
  virtual double min_cell_length(){ return 0.0;}
  
  //: Return the maximum values fo a porterior probability density - used for visualization normalization
  virtual double max_posterior() { return 1.0; }
 
  //: Accepts visitors - which are encapsulated algorithms
  virtual void accept(dbrec3d_visitor* v){}
  
  //: XML write
  virtual bxml_data_sptr xml_element() const =0;

    
protected:
  
  //: Unique identifier
  int type_id_;
  
  //: Global orientation - axis 
  vnl_float_3 axis_;
  
  //: Auxiliary axis - useful to define a coordinate system
  vnl_float_3 aux_axis_;

  float angle_;
  
  //: Scale
  vnl_float_3 scale_;
  
  //: Is this part symmetric about its axis?
  const bool symmetric_;
  
  //: Is this part rotationally invariant? e.i axis is irrevalevant
  const bool rot_invar_;
  
  //: The the readious of receptive field. This is used to perform local inhibition / non-maxima suppression
  vnl_float_3 radius_;
  
};


typedef vbl_smart_ptr<dbrec3d_part> dbrec3d_part_sptr;

void vsl_b_write(vsl_b_ostream & os, dbrec3d_part const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec3d_part &ph);
void vsl_b_read(vsl_b_istream& is, dbrec3d_part* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec3d_part* &ph);

#endif
