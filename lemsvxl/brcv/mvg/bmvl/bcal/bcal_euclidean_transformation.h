//-----------------------------------------------------------------------------
//:
// \file
// \brief interface for the bcal_euclidean_transformation class.
//
// \verbatim
//  Modifications:
//    Sancar Adali  April 1, 2004  Added get_trans_matrix(i) for accessing
//                                 the ith transformation matrix
// \endverbatim
//-----------------------------------------------------------------------------


#ifndef AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_
#define AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <iosfwd>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

class bcal_euclidean_transformation : public vcsl_spatial_transformation
{
 private:
  std::vector<vgl_h_matrix_3d<double> > trans_;

 public: // constructor and destructor
  bcal_euclidean_transformation() {}
  virtual ~bcal_euclidean_transformation() {}

 public: // operators
  void set_transformations(std::vector<vgl_h_matrix_3d<double> > &t) {trans_=t;}

  vnl_vector<double> inverse(const vnl_vector<double> &v, double time) const;
  virtual vnl_vector<double> execute(const vnl_vector<double> &v, double tims) const;
  virtual bool is_invertible(double time) const; // for abstract interface
  virtual void set_beat(std::vector<double> const& new_beat);
  vnl_double_4x4 get_trans_matrix(int i);
  virtual bool is_valid() const;

  // for debugging
  // print information
  void print(std::ostream& os = std::cerr);

 protected:
  // clear trans_
  int remove();
};

#endif // AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_
