// This is contrib/mleotta/vidreg/vidreg_initializer.h
#ifndef vidreg_initializer_h_
#define vidreg_initializer_h_

//:
// \file
// \brief An rgrl initializer using matched SIFT features
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/26/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_deque.h>
#include <vnl/vnl_vector.h>
#include <rgrl/rgrl_initializer.h>
#include <rgrl/rgrl_view_sptr.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_transformation_sptr.h>
#include <dbnl/algo/dbnl_bbf_tree.h>

//: An rgrl initializer using matched SIFT features
class vidreg_initializer : public rgrl_initializer
{
  public:
    //: Constructor
    vidreg_initializer(const vcl_vector<rgrl_feature_sptr>& fixed,
                       const vcl_vector<rgrl_feature_sptr>& moving,
                       rgrl_view_sptr view);

    //: Destructor
    virtual ~vidreg_initializer();

    //: Get next initial view.
    bool next_initial(rgrl_view_sptr &view, rgrl_scale_sptr &prior_scale);

    //: scan the queue of initial matches and remove those accounted for in the match_set 
    void remove_covered_matches(const rgrl_match_set_sptr& covered_set);

    //: scan the queue of initial matches and remove those accounted for by xform
    void remove_covered_matches(const rgrl_transformation& xform, double thresh);

    //: return number of initializations
    //  -1 stands for unknown
    int size() const { return -1; }

    // Defines type-related functions
    rgrl_type_macro( vidreg_initializer, rgrl_initializer );

    struct match
    {
      match(const rgrl_feature_sptr& f, const rgrl_feature_sptr& t,
            double d, double r) : from(f), to(t), distance(d), dist_ratio(r) {}
      rgrl_feature_sptr from;
      rgrl_feature_sptr to;
      double distance;
      double dist_ratio;
    };

    static bool match_dist_less(const match* m0, const match* m1);


  private:
    vcl_deque<match*> matches_;
    rgrl_view_sptr prior_view_;
};


#endif // vidreg_initializer_h_
