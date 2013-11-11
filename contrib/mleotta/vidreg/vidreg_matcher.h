// This is contrib/mleotta/vidreg/vidreg_matcher.h
#ifndef vidreg_matcher_h_
#define vidreg_matcher_h_

//:
// \file
// \brief An object to match sets of feature points
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/20/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_view_sptr.h>
#include <rgrl/rgrl_data_manager_sptr.h>
#include <rgrl/rgrl_initializer_sptr.h>
#include <rgrl/rgrl_feature_set_sptr.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <vidreg/vidreg_feature_group.h>
#include <vidreg/vidreg_salient_group_sptr.h>

//: An object to match sets of feature points
class vidreg_matcher
{
  public:
    //: Constructor
    vidreg_matcher();
    //: Destructor
    ~vidreg_matcher();

    bool match(const vcl_vector<vidreg_salient_group_sptr>& groups,
               const vidreg_feature_group& last_features,
               const vidreg_feature_group& new_features);

    bool assisted_match(const vcl_vector<rgrl_transformation_sptr>& xforms,
                        const vidreg_feature_group& last_features,
                        const vidreg_feature_group& new_features);

    const vcl_vector<vidreg_salient_group_sptr>& salient_groups() const {return salient_groups_;}

  protected:
    void setup(const vidreg_feature_group& fixed,
               const vidreg_feature_group& moving,
               rgrl_data_manager_sptr& data,
               rgrl_view_sptr& prior_view) const;

    vidreg_salient_group_sptr run_registration(const rgrl_data_manager_sptr& data,
                                               const rgrl_initializer_sptr& initializer);

    //: remove features from the data manager if covered by this motion group
    rgrl_data_manager_sptr remove_covered_features(const vidreg_salient_group_sptr& group,
                                                   const rgrl_data_manager_sptr& data);

    //: assign the features to only one the the resulting salient groups
    void assign_features(const vcl_vector<rgrl_feature_sptr>& features,
                         const rgrl_feature_set& to_set,
                         unsigned f_idx);

    //: A metric on the space of 2D similarity xforms
    double xform_distance(const rgrl_trans_similarity& xform1,
                          const rgrl_trans_similarity& xform2) const;

    //: robustly crop the strongest features to get a bounding box
    void robust_crop(vidreg_salient_group& group) const;

    //: use edge connectivity to resolve match conflicts
    void group_edges(const vcl_vector<vidreg_edge>& edges);

  private:
    vcl_vector<vidreg_salient_group_sptr> salient_groups_;
};


#endif // vidreg_matcher_h_
