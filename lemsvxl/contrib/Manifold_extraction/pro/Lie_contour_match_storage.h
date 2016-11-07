/*************************************************************************
 *    NAME: pradeep yarlagadda
 *    FILE: Lie_contour_match_storage.h
 *************************************************************************/

#ifndef Lie_contour_match_storage_header
#define Lie_contour_match_storage_header

#include <bpro1/bpro1_storage.h>
#include "Lie_contour_match_storage_sptr.h"
#include <manifold_extraction/Lie_cv_cor_sptr.h>
#include <manifold_extraction/Lie_contour_match.h>
#include <manifold_extraction/Lie_contour_match_sptr.h>

class Lie_contour_match_storage : public bpro1_storage 
{
public:

  Lie_contour_match_storage() {}
  virtual ~Lie_contour_match_storage() {}
  virtual vcl_string type() const { return "Lie contour matching"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro1_Lie_contour_match_storage"; }

  Lie_contour_match_sptr get_curvematch() {
    return curvematch_;
  }
 
  void set_curvematch (Lie_contour_match_sptr new_curvematch) {
    curvematch_ = new_curvematch;
  }

  Lie_cv_cor_sptr get_curve_cor() {
    return cv_cor_;
  }
 
  void set_cv_cor (Lie_cv_cor_sptr new_curve_cor) {
    cv_cor_ = new_curve_cor;
  }

private:

  Lie_contour_match_sptr curvematch_;
  Lie_cv_cor_sptr cv_cor_;

};

//: Create a smart-pointer to a vidpro1_shock_storage.
struct Lie_contour_match_storage_new : public Lie_contour_match_storage_sptr
{
  typedef Lie_contour_match_storage_sptr base;

  //: Constructor - creates a default vidpro1_curve_storage_sptr.
  Lie_contour_match_storage_new() : base(new Lie_contour_match_storage()) { }
};

#endif

