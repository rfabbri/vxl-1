// This is contrib/mleotta/vidreg/vidreg_feature_pt_desc.h
#ifndef vidreg_feature_pt_desc_h_
#define vidreg_feature_pt_desc_h_

//:
// \file
// \brief An rgrl feature point with invarient descriptor
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/25/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vnl/vnl_vector.h>
#include <rgrl/rgrl_feature_point.h>

//: An rgrl feature point with invarient descriptor
class vidreg_feature_pt_desc : public rgrl_feature_point
{
  public:
    //: Constructor
    vidreg_feature_pt_desc(const vnl_vector<double>& loc,
                           double mag,
                           const vnl_vector<double>& desc = vnl_vector<double>(0));
    //: Destructor
    virtual ~vidreg_feature_pt_desc();

    // Defines type-related functions
    rgrl_type_macro( vidreg_feature_pt_desc, rgrl_feature_point );

    double magnitude() const { return magnitude_; }

    const vnl_vector<double>& descriptor() const { return descriptor_; }

    void set_descriptor(const vnl_vector<double>& d) { descriptor_ = d; }

    //: Used for sorting by decreasing magnitude
    static bool dec_mag_order(const rgrl_feature_sptr& f1,
                              const rgrl_feature_sptr& f2);

    //: Used for sorting by increasing magnitude
    static bool inc_mag_order(const rgrl_feature_sptr& f1,
                              const rgrl_feature_sptr& f2);

  private:
    double magnitude_;
    vnl_vector<double> descriptor_;
};


#endif // vidreg_feature_pt_desc_h_
