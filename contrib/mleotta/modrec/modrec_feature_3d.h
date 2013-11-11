// This is contrib/mleotta/modrec/modrec_feature_3d.h
#ifndef modrec_feature_3d_h_
#define modrec_feature_3d_h_

//:
// \file
// \brief A 3d feature point (projected from an image)
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/5/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

//: A simple 3d feature point with position, scale, and orientation
class modrec_feature_3d
{
public:
  //: Default Constructor
  modrec_feature_3d() : scale_(1.0) {}

  //: Constructor
  modrec_feature_3d(const vgl_point_3d<double>& p,
                    const vgl_rotation_3d<double>& o,
                    double s)
    : pos_(p), orient_(o), scale_(s) {}

  //: Accessor functions
  const vgl_point_3d<double>& position() const { return pos_; }
  const vgl_rotation_3d<double>& orientation() const { return orient_; }
  double scale() const { return scale_; }

  //: Modifier functions
  void set_position(const vgl_point_3d<double>& p) { pos_ = p; }
  void set_orientation(const vgl_rotation_3d<double>& o) { orient_ = o; }
  void set_scale(double s) { scale_ = s; }

private:
  vgl_point_3d<double> pos_; 
  vgl_rotation_3d<double> orient_;
  double scale_;
  
};


//: A 3d feature point with an n-dimensional descriptor vector
template<unsigned n>
class modrec_desc_feature_3d : public modrec_feature_3d
{
public:
  //: Default Constructor
  modrec_desc_feature_3d() {}
  
  //: Constructor
  modrec_desc_feature_3d(const vgl_point_3d<double>& p,
                         const vgl_rotation_3d<double>& o,
                         double s,
                         const vnl_vector_fixed<double,n>& d)
    : modrec_feature_3d(p,o,s), desc_(d) {}

  //: Accessor for the descriptor
  const vnl_vector_fixed<double,n>& descriptor() const { return desc_; }

  //: Modifier for the descriptor
  void set_descriptor(const vnl_vector_fixed<double,n>& d) { desc_ = d; }

 private:
  //: The descriptor vector
  vnl_vector_fixed<double,n> desc_;
};


#endif // modrec_feature_3d_h_
