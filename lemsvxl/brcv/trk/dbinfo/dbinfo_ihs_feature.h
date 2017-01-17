// This is brl/bseg/dbinfo/dbinfo_ihs_feature.h
#ifndef dbinfo_ihs_feature_h_
#define dbinfo_ihs_feature_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The color (hue and saturation) feature.
//
// \author
//  J.L. Mundy - September 10, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//---------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_feature_base.h>

class dbinfo_ihs_feature : public dbinfo_feature_base
{
  //: Constructors/destructor
 public:
  dbinfo_ihs_feature()// default
    {format_ = DBINFO_IHS_FEATURE;} 
  
  ~dbinfo_ihs_feature(){}

  dbinfo_ihs_feature(dbinfo_feature_data_base_sptr const& data);
    

  //: copy constructor
  dbinfo_ihs_feature(const dbinfo_ihs_feature& rhs){}


  //:scan the current frame and extract image data into cache
  virtual bool scan(const unsigned frame,
                    vcl_vector<vgl_point_2d<unsigned> > const& points,
                    vcl_vector<bool> const& valid,
                    vil_image_resource_sptr const& image);


  //Can't be sure that dynamic cast will work on all compilers
  //so we use this rather antiquated method
  //------------------------------------------------------------------
  //: Return `this' if `this' is an intensity feature, 0 otherwise
  //-------------------------------------------------------------------
  virtual const dbinfo_ihs_feature* 
    cast_to_ihs_feature() const { return this; }

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbinfo_ihs_feature";}
 
};
#include <dbinfo/dbinfo_ihs_feature_sptr.h>
#endif // dbinfo_ihs_feature_h_
