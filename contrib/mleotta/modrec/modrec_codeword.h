// This is contrib/mleotta/modrec/modrec_codeword.h
#ifndef modrec_codeword_h_
#define modrec_codeword_h_

//:
// \file
// \brief A codeword for detecting feature_3d
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/25/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

//: A codeword
class modrec_codeword_base
{
public:
  //: Default Constructor
  modrec_codeword_base() : scale_(1.0), v_pos_(0.0), v_orient_(0.0), v_scale_(0.0) {}

  //: Constructor
  modrec_codeword_base(const vgl_point_3d<double>& p, double vp,
                       const vgl_rotation_3d<double>& o, double vo,
                       double s, double vs)
    : pos_(p), orient_(o), scale_(s), v_pos_(vp), v_orient_(vo), v_scale_(vs) {}

  //: Accessor functions
  const vgl_point_3d<double>& position() const { return pos_; }
  double var_position() const { return v_pos_; }
  const vgl_rotation_3d<double>& orientation() const { return orient_; }
  double var_orientation() const { return v_orient_; }
  double scale() const { return scale_; }
  double var_scale() const { return v_scale_; }

  //: Modifier functions
  void set_position(const vgl_point_3d<double>& p, double v=0.0) { pos_ = p; v_pos_ = v; }
  void set_orientation(const vgl_rotation_3d<double>& o, double v=0.0) { orient_ = o; v_orient_ = v; }
  void set_scale(double s, double v=0.0) { scale_ = s; v_scale_ = v; }

private:
  vgl_point_3d<double> pos_; 
  vgl_rotation_3d<double> orient_;
  double scale_;

  double v_pos_;
  double v_orient_;
  double v_scale_;

};


//: A 3d feature point with an n-dimensional descriptor vector
template<unsigned n>
class modrec_codeword : public modrec_codeword_base
{
public:
  //: Default Constructor
  modrec_codeword() : v_desc_(0.0) {}
  
  //: Constructor
  modrec_codeword(const vgl_point_3d<double>& p, double vp,
                  const vgl_rotation_3d<double>& o, double vo,
                  double s, double vs,
                  const vnl_vector_fixed<double,n>& d, double vd)
    : modrec_codeword_base(p,vp,o,vo,s,vs), desc_(d), v_desc_(vd) {}

  //: Accessor for the descriptor
  const vnl_vector_fixed<double,n>& descriptor() const { return desc_; }
  double var_descriptor() const { return v_desc_; }

  //: Modifier for the descriptor
  void set_descriptor(const vnl_vector_fixed<double,n>& d, double v=0.0) { desc_ = d; v_desc_ = v; }

  double compare_descriptor(const vnl_vector_fixed<double,n>& d)
  {
    return vnl_vector_ssd(d,desc_) + v_desc_;
  }

 private:
  //: The descriptor vector
  vnl_vector_fixed<double,n> desc_;
  double v_desc_;
};


#endif // modrec_codeword_h_
