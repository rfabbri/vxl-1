// This is mw_sel_match.h
#ifndef sel_match_h
#define sel_match_h
//:
//\file
//\brief Matching small curve hypotheses from the symbolic edge linker
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/09/2008 01:22:59 AM EDT
//

// Old code for matching small curve hypotheses from the symbolic edge linker
class mw_sel_point_matcher : mw_point_matcher {
  enum sel_reason {
    // reasons for success:
    OK_UNDEFINED, 
    // reasons for failure:
    FAIL_NO_CURVS_P1,
    FAIL_NO_CURVS_P2,
    FAIL_NO_BUNDLE_P1,
    FAIL_NO_BUNDLE_P2,
    FAIL_UNDEFINED
  };
  void sel_geometry_costs (
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
    const vcl_vector<dbdet_sel_sptr> &sel_,
    mw_discrete_corresp *corr,
    unsigned iv1, unsigned iv2);
  
//  bool sel_geometry_match_cost( 
//    const dbdet_edgel *pt_img1, 
//    const dbdet_edgel *pt_img2, 
//    const dbdet_sel_sptr &sel_img1,
//    const dbdet_sel_sptr &sel_img2,
//    dbdif_rig &rig,
//    double *cost, 
//    sel_reason *reason) const;
};

#endif // sel_match_h

