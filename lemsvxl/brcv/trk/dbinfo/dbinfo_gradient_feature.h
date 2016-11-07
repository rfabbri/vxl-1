// This is brl/bseg/dbinfo/dbinfo_gradient_feature.h
#ifndef dbinfo_gradient_feature_h_
#define dbinfo_gradient_feature_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The gradient tracking feature
//
// \author
//  J.L. Mundy - March 20, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_feature_base.h>

class dbinfo_gradient_feature : public dbinfo_feature_base
{
 public:
  //: parameters for computing the feature
  //: Radius of the smoothing Gaussian filter used prior to 
  // computing the gradient components
  float sigma_;

  //: Constructors/destructor
  dbinfo_gradient_feature(); // default
  
  ~dbinfo_gradient_feature(){}

  dbinfo_gradient_feature(dbinfo_feature_data_base_sptr const& data);
    

  //: copy constructor
  dbinfo_gradient_feature(const dbinfo_gradient_feature& rhs){}


  //:scan the current frame and extract image data into cache
  virtual bool scan(const unsigned frame,
                    vcl_vector<vgl_point_2d<unsigned> > const& points,
                    vcl_vector<bool> const& valid,
                    vil_image_resource_sptr const& image);


  //Can't be sure that dynamic cast will work on all compilers
  //so we use this rather antiquated method
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an intensity feature, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const dbinfo_gradient_feature* 
    cast_to_gradient_feature() const { return this; }

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbinfo_gradient_feature";}

 protected:
  //Set the appropriate image processing margin
  void set_margin();
};
#include <dbinfo/dbinfo_gradient_feature_sptr.h>
#endif // dbinfo_gradient_feature_h_
