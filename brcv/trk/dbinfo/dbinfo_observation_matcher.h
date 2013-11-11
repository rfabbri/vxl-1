// This is brl/bseg/dbinfo/dbinfo_observation_matcher.h
#ifndef dbinfo_observation_matcher_h_
#define dbinfo_observation_matcher_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A class for static matching methods
//
//
// covariance ellipse etc.  Also 
// 
// \author
//  J.L. Mundy - April 21, 2005
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
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbinfo/dbinfo_feature_data_base_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vcl_utility.h>


class dbinfo_observation_matcher : public vbl_ref_count
{
 public:

  //:default destructor
  ~dbinfo_observation_matcher(){}

  //: compute the mutual information between two feature data base stores
  static float minfo(dbinfo_feature_data_base_sptr const& d0,
                     dbinfo_feature_data_base_sptr const& d1,
                     bool verbose = false);

  //: compute the mutual information between two observations
  static float minfo(dbinfo_observation_sptr const& obs0,
                     dbinfo_observation_sptr const& obsi,
                     bool verbose = false);

  //: compute the mutual information between two observations
  //selecting the information channels to use (if available)
  static float minfo(dbinfo_observation_sptr const& obs0,
                     dbinfo_observation_sptr const& obsi,
                     bool use_intensity,
                     bool use_gradient,
                     bool use_ihs,
                     bool verbose = false);


  //: compute the mutual information between two feature data base stores 
  //  using the explicit correspondence of region points
  static float minfo(dbinfo_feature_data_base_sptr const& d0,
                     dbinfo_feature_data_base_sptr const& d1,
                     vcl_vector<vcl_pair<unsigned, unsigned> > const& correspondences,
                     bool verbose,
                     float max_value = 255.0f);
  
  //: this version takes an explicit correspondence between points of the two regions
  //  e.g. we have correspondences.size() many correspondences
  //       correspondences[i].first is the id of the point (location/pixel) in region0 that 
  //       is mapped to the region1 point (location/pixel) with id correspondences[i].second 
  static float minfo(dbinfo_observation_sptr const& obs0,
                     dbinfo_observation_sptr const& obsi,
                     vcl_vector<vcl_pair<unsigned, unsigned> > const& correspondences,
                     bool verbose,
                     float max_value = 255.0f);

  //: cluster observation sets, determining the number of clusters
  static bool cluster(vcl_vector<dbinfo_observation_sptr> const& obvs,
                      vcl_vector<vcl_vector<dbinfo_observation_sptr> >& clusters,
                 bool verbose = false);

  //: compute mutual information between two image regions
  static float minfo(vil_image_resource_sptr const& imgr0,
                     vsol_polygon_2d_sptr const& region0,
                     vil_image_resource_sptr const& imgr1,
                     vsol_polygon_2d_sptr const& region1,
                     bool intensity_info = true,
                     bool gradient_info = true,
                     bool color_info = false);

  //: compute mutual information between an observation and image region
  static float minfo(dbinfo_observation_sptr const& obs,
                     vil_image_resource_sptr const& imgr,
                     vsol_polygon_2d_sptr const& region);


 protected:
  //: no default constructor, static methods only
  dbinfo_observation_matcher();
};

#endif // dbinfo_observation_matcher_h_
