// This is contrib/mleotta/vidreg/vidreg_salient_group.h
#ifndef vidreg_salient_group_h_
#define vidreg_salient_group_h_
//:
// \file
// \brief A salient group of matched feature and related transformation
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/31/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_view_sptr.h>
#include <rgrl/rgrl_converge_status_sptr.h>
#include <vidreg/vidreg_feature_group.h>
#include <vidreg/vidreg_salient_group_sptr.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>
#include <vcl_map.h>

//: A salient group of matched feature and related transformation
class vidreg_salient_group: public rgrl_object
{
  public:
    //: Constructor
    vidreg_salient_group(const rgrl_view_sptr& view,
                         const vcl_vector<rgrl_match_set_sptr>& matches,
                         const vcl_vector<rgrl_scale_sptr>& scales,
                         const rgrl_converge_status_sptr& status);

    //: Constructor
    vidreg_salient_group(const vidreg_feature_group& init_group);

    //: Destructor
    ~vidreg_salient_group();

    // Defines type-related functions
    rgrl_type_macro( vidreg_salient_group, rgrl_object );

    rgrl_view_sptr view() const;

    const vcl_vector<rgrl_match_set_sptr>& matches() const;

    const vcl_vector<rgrl_scale_sptr>& scales() const;

    rgrl_converge_status_sptr status() const;

    double weight(const rgrl_feature_sptr& feature) const;

    const vnl_vector<double>& growth_center() const;

    vidreg_feature_group_sptr make_feature_group(double weight_thresh = 0.0) const;

    void set_growth_center(const vnl_vector<double>& center);

    void set_matches(unsigned index, const rgrl_match_set_sptr& matches);

    void assign_weight(const rgrl_feature_sptr& feature, double weight);

    const vcl_map<rgrl_feature_sptr, double>& weight_map() const;

    vidreg_salient_group_sptr previous_group() const { return prev_group_; }

    void set_previous_group(const vidreg_salient_group_sptr& g) { prev_group_ = g; }

  private:
    rgrl_view_sptr                      view_;
    vcl_vector<rgrl_match_set_sptr>     matches_;
    vcl_vector<rgrl_scale_sptr>         scales_;
    rgrl_converge_status_sptr           status_;
    vnl_vector<double>                  growth_center_;
    vcl_map<rgrl_feature_sptr, double>  weight_map_;
    vidreg_salient_group_sptr           prev_group_;
};

#endif // vidreg_salient_group_h_
