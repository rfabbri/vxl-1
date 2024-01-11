// This is bmcsd_odt_curve_stereo_e.h
#ifndef bmcsd_odt_curve_stereo_e_h
#define bmcsd_odt_curve_stereo_e_h
//:
//\file
//\brief Multiview curve stereo using distance transforms and orientation
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 06/22/2009 05:30:17 PM EDT
//

#include <bdifd/bdifd_frenet.h>
#include <sdet/sdet_edgemap_sptr.h>
#include <sdetd/pro/sdetd_sel_storage_sptr.h>
#include <bcsid/bcsid_curve_distance.h>
#include <bmcsd/bmcsd_curve_3d_attributes.h>
#include <bmcsd/algo/bmcsd_dt_curve_stereo.h>


//: Matches curve fragments using costs based on the distance transform and
// closest feature map (label map) of the edge map at each image, as well as
// orientation. This class assumes the user provides precomputed distance
// transform maps and label maps for each image, and that they are compatible
// with the provided edge map. The reason is that we want to run this class
// repeatedly for different pairs of views without having to recompute the DT.

class bmcsd_discrete_corresp;

class bmcsd_odt_curve_stereo_e : public bmcsd_odt_curve_stereo {
public:
  bmcsd_odt_curve_stereo_e();
  virtual ~bmcsd_odt_curve_stereo_e() {}

  // Setters and Getters ------------------------------------------------------

  //: Anil: get/set original curve sizes for each view
  void set_original_curve_sizes(const vcl_vector<vcl_vector<unsigned> > &original_curve_sizes);
  unsigned get_original_curve_size(unsigned v, unsigned c);

  //: Anil: get/set number of image curves for v0()
  void set_num_image_curves_v0(const unsigned &num_image_curves_v0);
  unsigned get_num_image_curves_v0();

  //Anil: get/set subsequence set stored in this class
  void set_sseq(const vcl_vector<dbbl_subsequence_set> &sseq);
  vcl_vector<dbbl_subsequence_set> get_sseq();

  //Anil: A version of the matching function to be used with mate curves
  bool match_mate_curves_using_orientation_dt_extras(unsigned long *votes_ptr, unsigned v, unsigned ic);

  //: Anil: Another version of the function that returns a list of supporting edgel IDs as well as inlier view IDs
  bool match_using_orientation_dt_extras(vcl_vector<unsigned long> *votes_ptr, vcl_vector<vcl_vector<vcl_set<int> > > &inlierEdgelsPerCandidate, 
                                         vcl_vector<vcl_vector<unsigned> > &inlierViewsPerCandidate,vcl_vector<vcl_vector<unsigned> > &edge_support_count_per_candidate,
					 vcl_vector<vcl_vector<vcl_vector<int> > > *edge_index_chain_per_candidate_ptr = NULL);

  //: Anil: Another version of the function that returns a list of supporting edgel IDs as well as inlier view IDs
  //: Anil: This version is for matching the selected curve in v0() with particular candidates
  bool match_using_orientation_dt_extras(vcl_vector<unsigned long> *votes_ptr, vcl_vector<vcl_vector<vcl_set<int> > > &inlierEdgelsPerCandidate, 
                                         vcl_vector<vcl_vector<unsigned> > &inlierViewsPerCandidate, vcl_set<int> curve_ids,
					 vcl_vector<vcl_vector<unsigned> > &edge_support_count_per_candidate);

  //: Anil: Another version of the function that returns uncertainty flags for samples, turn markFlag on to mark used samples using usedSamples_ member
  void reconstruct_candidate_1st_order_with_flags(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
						  const dbdif_rig &rig, dbdif_1st_order_curve_3d *crv_ptr, vcl_vector<bool> &flags, unsigned &recon_shift);

  //: Anil: A version of the flagged function that does reconstructions using confirmation views
  void reconstruct_candidate_1st_order_with_flags_temp(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
						  const dbdif_rig &rig, dbdif_1st_order_curve_3d *crv_ptr, vcl_vector<bool> &flags, unsigned v);

  void reconstruct_subcurve_1st_order_with_flags(
      unsigned ini_id_sub,
      unsigned end_id_sub,
      const dbdif_rig &rig,
      dbdif_1st_order_curve_3d *curve_3d,
      vcl_vector<bool> &curveFlags
      );

  void reconstruct_subcurve_1st_order_with_flags(
      unsigned ini_id_sub,
      unsigned end_id_sub,
      const dbdif_rig &rig,
      dbdif_1st_order_curve_3d *curve_3d,
      vcl_vector<bool> &curveFlags,
      unsigned v
      );

  void reconstruct_curve_point_1st_order_with_flags(
      unsigned v,
      unsigned ini_id,
      unsigned di0,
      const dbdif_rig &rig,
      dbdif_1st_order_point_3d *pt_3D,
      bool &isConfident,
      unsigned &curve_v1
      );

  //: The reprojection curve in each view \p c obtained by corresponding
  // the subcurve in view[0] and the epipolar band candidate \p c in view[1].
  // \return the curve-tangent of the reprojection curve, indexed as
  // reprojection_crv(curve index, view index, point index)
  sdet_edgel reprojection_crv(unsigned c, unsigned v, unsigned p) const
  { return reprojection_crv_[c][v][p]; }

  //: Size of the reprojection curve(candidate index, view index). 
  // \see reprojection_crv
  unsigned reprojection_crv_size(unsigned c, unsigned v) const
  { return reprojection_crv_[c][v].size(); } 

  bool has_reprojection_crv() const { return !reprojection_crv_.empty(); }

  //: This version uses angle from set_min_epiangle to determine
  // keep only the episegs having all point-tangents within an angle of
  // the epipolar line. Such angle is set in set_min_epiangle().
  virtual void break_curves_into_episegs_pairwise(
      std::vector<std::vector< vsol_polyline_2d_sptr > > *broken_vsols,
      std::vector<bbld_subsequence_set> *ss_ptr
      ) const;
  //: Stand-alone episeg breaker using tangent angle information.
  // \see break_curves_into_episegs_pairwise
  static void break_curves_into_episegs_angle(
    const vcl_vector< vsol_polyline_2d_sptr >  &vsols,
    const vcl_vector<vcl_vector<double> > &tgts,
    double min_epiangle,
    vcl_vector<vsol_polyline_2d_sptr> *vsols2,
    const vgl_homg_point_2d<double> &e,
    dbbl_subsequence_set *ss_ptr, bool print, bool onlyMark,
    vcl_vector<vcl_vector<bool> > &uncertaintyFlags);

  bool dummyFlag;
  unsigned dummyID;
  vcl_vector<double> lineCoef;

  //Anil: For each view, we store the ID curve of that each edgel 
  //Size should be total number of confirmation views used in the stereo instance
  vcl_vector<vcl_vector<int> > edge_curve_index_;

  //Anil: Mate curve IDs for a given image curves in v0()
  //Size should be total number of confirmation views used in the stereo instance
  vcl_vector<vcl_set<int> > mate_curves_;

  //Anil: Key = mate curve ID, Value = weight, indicating level of support given in number of edges
  vcl_map<unsigned,unsigned> mate_curve_weights_;

  //Anil: Vector that stores the flags for curve samples used
  vcl_vector<vcl_vector<vcl_vector<bool> > > usedSamples_;

  //Anil: Buffer for storing the sample IDs on the image curve in v1 that are going to be used in this run
  vcl_vector<unsigned> curve_samples_v1_;

  //Anil: Curve IDs that are already used in a previous run
  vcl_vector<vcl_vector<unsigned> > usedCurves_;

  //Anil: Count for the number of curve sample matching operations
  unsigned matchCount_;

  //Anil: Count for the number of curve sample reconstruction operations
  unsigned reconCount_;

protected:
  bool has_edgemaps() { return !em_.empty(); }

private:
  //: edgemap for each view.
  std::vector<sdet_edgemap_sptr> em_;

  //: symbolic edge linker structure for each view.
  std::vector<sdetd_sel_storage_sptr> sels_;

  typedef std::vector<bdifd_1st_order_curve_2d> first_order_curve_map;

  //: curve map for each view with 1st order differential geometry
  // (point-tangents), already in world coordinates. Indexing goes
  // pt_tgts_[id_view][id_curve][id_sample]
  std::vector< first_order_curve_map > pt_tgts_;
  double tau_dtheta_;
  unsigned tau_min_inliers_per_view_;
  unsigned tau_min_total_inliers_;
  double tau_min_epiangle_;
  double tau_min_first_to_second_best_ratio_;
  unsigned tau_lonely_;
  unsigned tau_min_num_inlier_edgels_per_curvelet_;

  //: \see curve_tangents()
  std::vector<std::vector<std::vector<double> > > curve_tangents_;
  std::vector<std::vector<bcsid_edgel_seq> > reprojection_crv_;

  //Anil: Adding the subsequence set as a member here so we can do
  //      epipolar tangency stitching after reconstruction
  vcl_vector<dbbl_subsequence_set> sseq_;

  //Anil: Adding the samples sizes of the original curves
  vcl_vector<vcl_vector<unsigned> > original_curve_sizes_;

  //Anil: Adding the total number of image curves on v0()
  unsigned num_image_curves_v0_;
};

//: version of dbmcs_match_and_reconstruct_all_curves which fills an attribute
// for each curve.
bool dbmcs_match_and_reconstruct_all_curves_attr(
    mw_odt_curve_stereo &s, 
    vcl_vector<dbdif_1st_order_curve_3d> *crv3d_ptr,
    mw_discrete_corresp *corresp_ptr,
    vcl_vector< dbmcs_curve_3d_attributes > *attr_ptr,
    bool track_supporting_edges=false
    );

//: version of dbmcs_match_and_reconstruct_all_curves that makes use of mate curves
bool dbmcs_match_and_reconstruct_all_curves_attr_using_mates(
    mw_odt_curve_stereo &s, 
    vcl_vector<dbdif_1st_order_curve_3d> *crv3d_ptr,
    mw_discrete_corresp *corresp_ptr,
    vcl_vector< dbmcs_curve_3d_attributes > *attr_ptr,
    vcl_vector<vcl_set<int> > mate_curves_v1,
    bool isFirstRun,
    bool track_supporting_edges=false
    );

//: Matches all curves bewtween two views.
// \param[in] s : must be all set and ready to match.
// \param[out] corresp_ptr : discrete correspondence between the first two
// views. Only the correspondences passing all thresholds are output.
bool dbmcs_match_all_curves(
  mw_odt_curve_stereo &s, 
  mw_discrete_corresp *corresp_ptr,
  bool track_supporting_edges=false);

//Anil: mcd version of the matching function that iterates the matches using mate curves
//Anil: can do the initial seed curve matching, as well as the elongation iterations
bool dbmcs_match_all_curves_using_mates(
  mw_odt_curve_stereo &s, 
  mw_discrete_corresp *corresp_ptr,
  unsigned seed_id,
  vcl_vector<vcl_set<int> > curve_ids,
  bool isFirstRun,
  bool track_supporting_edges=false);


//: variant of reconstruct_from_corresp which also outputs attributes.
bool 
reconstruct_from_corresp_attr(
    mw_odt_curve_stereo &s, 
    const mw_discrete_corresp &corresp,
    vcl_vector<dbdif_1st_order_curve_3d> *crv3d_ptr,
    vcl_vector< dbmcs_curve_3d_attributes > *attr_ptr,
    unsigned seed_id = 0
    );

bool 
reconstruct_from_corresp_attr_using_mates(
    mw_odt_curve_stereo &s, 
    const mw_discrete_corresp &corresp,
    vcl_vector<dbdif_1st_order_curve_3d> *crv3d_ptr,
    vcl_vector< dbmcs_curve_3d_attributes > *attr_ptr
    );

#endif // bmcsd_odt_curve_stereo_e_h
