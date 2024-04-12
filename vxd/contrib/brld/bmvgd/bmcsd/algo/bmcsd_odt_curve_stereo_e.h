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
#include <bmcsd/bmcsd_curve_3d_attributes_e.h>
#include <bmcsd/bmcsd_discrete_corresp_e.h>
#include <bmcsd/algo/bmcsd_odt_curve_stereo.h>


//: Matches curve fragments using costs based on the distance transform and
// closest feature map (label map) of the edge map at each image, as well as
// orientation. This class assumes the user provides precomputed distance
// transform maps and label maps for each image, and that they are compatible
// with the provided edge map. The reason is that we want to run this class
// repeatedly for different pairs of views without having to recompute the DT.

class bmcsd_discrete_corresp_e;

class bmcsd_odt_curve_stereo_e : public bmcsd_odt_curve_stereo {
public:
  bmcsd_odt_curve_stereo_e();
  virtual ~bmcsd_odt_curve_stereo_e() {}

  // Setters and Getters ------------------------------------------------------

  //: Anil: get/set original curve sizes for each view
  void set_original_curve_sizes(const std::vector<std::vector<unsigned> > &original_curve_sizes);
  unsigned get_original_curve_size(unsigned v, unsigned c);

  //: Anil: get/set number of image curves for v0()
  void set_num_image_curves_v0(const unsigned &num_image_curves_v0);
  unsigned get_num_image_curves_v0();

  // Anil: Curve IDs that are already used in a previous run
  const std::vector<std::vector<unsigned> > &get_used_curves() const { return usedCurves_; }

  // Anil: get/set subsequence set stored in this class
  void set_sseq(const std::vector<bbld_subsequence_set> &sseq);
  std::vector<bbld_subsequence_set> get_sseq();

  virtual bool set_nviews(unsigned nviews) override;

  // ---------------------------------------------------------------------------

  //Anil: A version of the above function to compute epipolar candidates on any confirmation view
  void compute_epipolar_beam_candidates_on_conf_views(unsigned v);

  // Anil: A version of the matching function to be used with mate curves
  bool match_mate_curves_using_orientation_dt_extras(unsigned long *votes_ptr, unsigned v, unsigned ic);

  //: Anil: Another version of the function that returns a list of supporting edgel IDs as well as inlier view IDs
  bool match_using_orientation_dt_extras(std::vector<unsigned long> *votes_ptr, std::vector<std::vector<std::set<int> > > &inlierEdgelsPerCandidate, 
                                         std::vector<std::vector<unsigned> > &inlierViewsPerCandidate,std::vector<std::vector<unsigned> > &edge_support_count_per_candidate,
					 std::vector<std::vector<std::vector<int> > > *edge_index_chain_per_candidate_ptr = NULL);

  //: Anil: Another version of the function that returns a list of supporting edgel IDs as well as inlier view IDs
  //: Anil: This version is for matching the selected curve in v0() with particular candidates
  bool match_using_orientation_dt_extras(std::vector<unsigned long> *votes_ptr, std::vector<std::vector<std::set<int> > > &inlierEdgelsPerCandidate, 
                                         std::vector<std::vector<unsigned> > &inlierViewsPerCandidate, std::set<int> curve_ids,
					 std::vector<std::vector<unsigned> > &edge_support_count_per_candidate);


  //: Anil: This version is the same as the normal odt_curve_stereo, but will also increment matchCount_
  // 
  // Reconstructs given subcurve specified by index ini_id and end_id into
  // selected_crv_id(v0()), by assuming correspondence to candidate curve with index \p ic.
  // This version reconstructs the tangent information as well.
  // 
  void reconstruct_candidate_1st_order(unsigned ini_id, unsigned end_id, unsigned ic, 
      const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr);

  //: Anil: Another version of the function that returns uncertainty flags for samples, turn markFlag on to mark used samples using usedSamples_ member
  void reconstruct_candidate_1st_order_with_flags(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
						  const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr, std::vector<bool> &flags, unsigned &recon_shift);

  //: Anil: A version of the flagged function that does reconstructions using confirmation views
  void reconstruct_candidate_1st_order_with_flags_temp(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
						  const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr, std::vector<bool> &flags, unsigned v);

  //: Anil: This version is the same as the one in regular odt_curve_stereo, but
  // writes out info if dummyflag_ is true
  //
  //
  // Reconstructs subcurve given by curve selected_crv_id(v0()) and endpoints with
  // index ini_id_sub, end_id_sub, using view[v0()] and the corresponding
  // selected_crv_id(v1()) in view[v1()]. This version reconstructs the tangent
  // information as well.
  void reconstruct_subcurve_1st_order(
      unsigned ini_id_sub, 
      unsigned end_id_sub, 
      const bdifd_rig &rig,
      bdifd_1st_order_curve_3d *curve_3d
      );

  void reconstruct_subcurve_1st_order_with_flags(
      unsigned ini_id_sub,
      unsigned end_id_sub,
      const bdifd_rig &rig,
      bdifd_1st_order_curve_3d *curve_3d,
      std::vector<bool> &curveFlags
      );

  void reconstruct_subcurve_1st_order_with_flags(
      unsigned ini_id_sub,
      unsigned end_id_sub,
      const bdifd_rig &rig,
      bdifd_1st_order_curve_3d *curve_3d,
      std::vector<bool> &curveFlags,
      unsigned v
      );

  void reconstruct_curve_point_1st_order_with_flags(
      unsigned v,
      unsigned ini_id,
      unsigned di0,
      const bdifd_rig &rig,
      bdifd_1st_order_point_3d *pt_3D,
      bool &isConfident,
      unsigned &curve_v1
      );

  //: Anil: this version is the same as the one in regular odt_curve_stereo, but also
  // writes to he lineCoef_ member. It is thus non-const
  //
  // Reconstructs a 3D point-tangent, given the 2D image point-tangent \c p_0
  // in the first view and the index \p v of the second view to use. The point in
  // the second view is found by intersecting the epipolar line of p_0 with the
  // selected curve of that view.
  //
  // \param[in] di0 is the index of the point p_0 in the selected sub-curve of view 0
  // \param[in] ini_id is how far the initial point of the subcurve is inside the
  // containing curve fragment container.
  //
  void reconstruct_curve_point_1st_order(
      unsigned v,
      unsigned ini_id,
      unsigned di0, 
      const bdifd_rig &rig,
      bdifd_1st_order_point_3d *pt_3D
      );


  //: This version uses angle from set_min_epiangle to determine
  // keep only the episegs having all point-tangents within an angle of
  // the epipolar line. Such angle is set in set_min_epiangle().
  virtual void break_curves_into_episegs_pairwise(
      std::vector<std::vector< vsol_polyline_2d_sptr > > *broken_vsols,
      std::vector<bbld_subsequence_set> *ss_ptr
      ) override;

  //: Stand-alone episeg breaker using tangent angle information.
  // \see break_curves_into_episegs_pairwise
  static void break_curves_into_episegs_angle(
    const std::vector< vsol_polyline_2d_sptr >  &vsols,
    const std::vector<std::vector<double> > &tgts,
    double min_epiangle,
    std::vector<vsol_polyline_2d_sptr> *vsols2,
    const vgl_homg_point_2d<double> &e,
    bbld_subsequence_set *ss_ptr, bool onlyMark,
    std::vector<std::vector<bool> > &uncertaintyFlags);

  friend bool 
  reconstruct_from_corresp_attr(
      bmcsd_odt_curve_stereo_e &s, 
      const bmcsd_discrete_corresp_e &corresp,
      std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
      unsigned seed_id);

  // Anil: Count for the number of curve sample matching operations
  unsigned matchCount_;

  // Anil: Count for the number of curve sample reconstruction operations
  unsigned reconCount_;

  // Anil: Curve IDs that are already used in a previous run
  std::vector<std::vector<unsigned> > usedCurves_;

  // Anil: For each view, we store the ID curve of that each edgel 
  //Size should be total number of confirmation views used in the stereo instance
  std::vector<std::vector<int> > edge_curve_index_;

  // Anil: Vector that stores the flags for curve samples used
  std::vector<std::vector<std::vector<bool> > > usedSamples_;

  // Anil: Storing the unreliable samples of each curve
  // Only used in enhanced curve sketch classes with _e suffix
  std::vector<std::vector<std::vector<bool> > > vsol_flags_;

protected:
  bool dummyFlag_;
  unsigned dummyID;
  std::vector<double> lineCoef_;


  // Anil: Mate curve IDs for a given image curves in v0()
  //Size should be total number of confirmation views used in the stereo instance
  std::vector<std::set<int> > mate_curves_;

  // Anil: Key = mate curve ID, Value = weight, indicating level of support given in number of edges
  std::map<unsigned,unsigned> mate_curve_weights_;

  // Anil: Buffer for storing the sample IDs on the image curve in v1 that are going to be used in this run
  std::vector<unsigned> curve_samples_v1_;

private:
  // Anil: Adding the subsequence set as a member here so we can do
  //      epipolar tangency stitching after reconstruction
  std::vector<bbld_subsequence_set> sseq_;

  // Anil: Adding the samples sizes of the original curves
  std::vector<std::vector<unsigned> > original_curve_sizes_;

  // Anil: Adding the total number of image curves on v0()
  unsigned num_image_curves_v0_;
};

//: version of bmcsd_match_and_reconstruct_all_curves which fills an attribute
// for each curve.
bool bmcsd_match_and_reconstruct_all_curves_attr_e(
    bmcsd_odt_curve_stereo_e &s, 
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    bmcsd_discrete_corresp_e *corresp_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    bool track_supporting_edges=false
    );

//: version of bmcsd_match_and_reconstruct_all_curves that makes use of mate curves
bool bmcsd_match_and_reconstruct_all_curves_attr_using_mates(
    bmcsd_odt_curve_stereo_e &s, 
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    bmcsd_discrete_corresp_e *corresp_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    std::vector<std::set<int> > mate_curves_v1,
    bool isFirstRun,
    bool track_supporting_edges=false
    );

//: Matches all curves bewtween two views.
// \param[in] s : must be all set and ready to match.
// \param[out] corresp_ptr : discrete correspondence between the first two
// views. Only the correspondences passing all thresholds are output.
bool bmcsd_match_all_curves(
  bmcsd_odt_curve_stereo_e &s, 
  bmcsd_discrete_corresp_e *corresp_ptr,
  bool track_supporting_edges=false);

// Anil: mcd version of the matching function that iterates the matches using mate curves
// Anil: can do the initial seed curve matching, as well as the elongation iterations
bool bmcsd_match_all_curves_using_mates(
  bmcsd_odt_curve_stereo_e &s, 
  bmcsd_discrete_corresp_e *corresp_ptr,
  unsigned seed_id,
  std::vector<std::set<int> > curve_ids,
  bool isFirstRun,
  bool track_supporting_edges=false);

//: variant of reconstruct_from_corresp which also outputs attributes.
bool 
reconstruct_from_corresp_attr(
    bmcsd_odt_curve_stereo_e &s, 
    const bmcsd_discrete_corresp_e &corresp,
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    unsigned seed_id = 0
    );

bool 
reconstruct_from_corresp_attr_using_mates(
    bmcsd_odt_curve_stereo_e &s, 
    const bmcsd_discrete_corresp_e &corresp,
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr
    );

#endif // bmcsd_odt_curve_stereo_e_h
