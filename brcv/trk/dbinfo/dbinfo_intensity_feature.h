// This is brl/bseg/dbinfo/dbinfo_intensity_feature.h
#ifndef dbinfo_intensity_feature_h_
#define dbinfo_intensity_feature_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The intensity tracking feature
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

class dbinfo_intensity_feature : public dbinfo_feature_base
{
 public:
  //: Constructors/destructor
  dbinfo_intensity_feature()
    {format_=DBINFO_INTENSITY_FEATURE;} // default
  
  ~dbinfo_intensity_feature(){}

  dbinfo_intensity_feature(dbinfo_feature_data_base_sptr const& data):
    dbinfo_feature_base(data){format_=DBINFO_INTENSITY_FEATURE;}

  //: copy constructor
  dbinfo_intensity_feature(const dbinfo_intensity_feature& rhs){}

  //:scan the current frame and extract image data into cache
  virtual bool scan(const unsigned frame,
                    vcl_vector<vgl_point_2d<unsigned> > const& points,
                    vcl_vector<bool> const& valid,
                    vil_image_resource_sptr const& image);


  //: Reconstruct an image snippet around the feature of size n_i x n_j. Offset the feature points by (i0, j0)
  virtual vil_image_resource_sptr
    image(vcl_vector<vgl_point_2d<float> > const& points,
          vcl_vector<bool> const& valid,
          unsigned n_i, unsigned n_j, float i0, float j0, bool background_noise = true);
  

  //Can't be sure that dynamic cast will work on all compilers
  //so we use this rather antiquated method
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an intensity feature, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const dbinfo_intensity_feature* 
    cast_to_intensity_feature() const { return this; }

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbinfo_intensity_feature";}

 protected:

};
#include <dbinfo/dbinfo_intensity_feature_sptr.h>

#endif // dbinfo_intensity_feature_h_
