#include "bmcsd_odt_curve_stereo_e.h"
#include <bdifd/bdifd_rig.h>
#include <becld/becld_epipole.h>
#include <becld/becld_episeg.h>
#include <becld/becld_episeg_sptr.h>
#include <becld/becld_episeg_from_curve_converter.h>
#include <becld/becld_epiline_interceptor.h>
#include <bmcsd/bmcsd_discrete_corresp_e.h>
#include <bmcsd/algo/bmcsd_algo_util.h>
#include <bmcsd/algo/bmcsd_discrete_corresp_algo.h>
#include <sdet/sdet_edgemap.h>
#include <sdetd/pro/sdetd_sel_storage.h>

bmcsd_odt_curve_stereo_e::
bmcsd_odt_curve_stereo_e()
  :
    dummyFlag(false)
{
}

void bmcsd_odt_curve_stereo_e::
set_original_curve_sizes(const std::vector<std::vector<unsigned> > &original_curve_sizes)
{
  original_curve_sizes_ = original_curve_sizes;
}

unsigned bmcsd_odt_curve_stereo_e::
get_original_curve_size(unsigned v, unsigned c)
{
  return original_curve_sizes_[v][c];
}

void bmcsd_odt_curve_stereo_e::
set_num_image_curves_v0(const unsigned &num_image_curves_v0)
{
  num_image_curves_v0_ = num_image_curves_v0;
}
  
void bmcsd_odt_curve_stereo_e::
set_sseq(const std::vector<bbld_subsequence_set> &sseq)
{
  sseq_ = sseq;
}

std::vector<bbld_subsequence_set> bmcsd_odt_curve_stereo_e::
get_sseq()
{
  return sseq_;
}

bool bmcsd_odt_curve_stereo_e::
match_using_orientation_dt_extras(
    std::vector<unsigned long> *votes_ptr, 
    std::vector<std::vector<std::set<int> > > &inlierEdgelsPerCandidate,
    std::vector<std::vector<unsigned> > &inlierViews, 
    std::vector<std::vector<unsigned> > &edge_support_count_per_candidate,
		std::vector<std::vector<std::vector<int> > > *edge_index_chain_per_candidate_ptr)
{
#ifndef NDEBUG
  if (!ready_for_oriented_matching())
    return false;
#endif

  if (num_candidates() == 0) {
    votes_ptr->clear();
    return true;
  }



  if(edge_index_chain_per_candidate_ptr==NULL) {
    std::vector<std::vector<std::vector<int> > > *dummy_pointer;
    edge_index_chain_per_candidate_ptr = dummy_pointer;
  }

  std::vector<std::vector<std::vector<int> > > &edge_index_chain_per_candidate = *edge_index_chain_per_candidate_ptr;

  std::vector<unsigned long> &votes = *votes_ptr;
  votes.resize(num_candidates(), 0);

  unsigned ini_id, 
           end_id;

  get_increasing_endpoints(&ini_id, &end_id);

  bdifd_rig rig(cams(v0()).Pr_, cams(v1()).Pr_);

  // For each candidate curve
  reprojection_crv_.resize(num_candidates());

  std::vector<bbld_subsequence_set> sseq = this->get_sseq();

  for (unsigned ic=0; ic < num_candidates(); ++ic) {
    unsigned origID_u = sseq[this->v1()][crv_candidates(ic)].orig_id();
    if(std::find(usedCurves_[this->v1()].begin(), usedCurves_[this->v1()].end(), origID_u) == usedCurves_[this->v1()].end()) {

      // Anil: Adding a threshold for minimum number of inlier views Apr 18th, 2014
      // Anil: Also adding vector of inlier views per candidate, and inlier edgels per confirmation view for each candidate
      unsigned num_inlier_views = 0; 
      std::vector<std::set<int> > inlierEdgelsPerConfView;
      std::vector<unsigned> curInlierViews;
      std::vector<std::vector<int> > edge_index_chain_per_conf_view(nviews()-2);

      bdifd_1st_order_curve_3d curve_3d;
      reconstruct_candidate_1st_order(ini_id, end_id, ic, rig, &curve_3d);
      edge_support_count_per_candidate[ic].resize(curve_3d.size());
      std::fill(edge_support_count_per_candidate[ic].begin(),edge_support_count_per_candidate[ic].end(),0);

#ifdef BMCSD_VERBOSE_DEBUG
      std::cout << "Votes for curve[" << ic << "] ===========" << std::endl;
#endif
      reprojection_crv_[ic].resize(nviews());
      for (unsigned v=0; v < nviews(); ++v) {
        if (v == v0() || v == v1() )
          continue;

        // Compute reprojected_curve into view v
        bdifd_1st_order_curve_2d reprojected_curve;
        project_curve_1st_order(v, curve_3d, &reprojected_curve);
        assert (reprojected_curve.size() == curve_3d.size());

        // Anil: Clip to image bounds, but keep track of the original curve sample IDs
        std::vector<unsigned> orig_ids;
        bmcsd_util::clip_to_img_bounds(dt(v), &reprojected_curve, orig_ids);
        //bmcsd_util::clip_to_img_bounds(dt(v), &reprojected_curve);

        // translate reproj. curve into edgel sequence
        bcsid_edgel_seq reproj_edgels;
        bmcsd_algo_util::bdifd_to_sdet(reprojected_curve, &reproj_edgels);

        assert (reproj_edgels.size() == reprojected_curve.size());

        // For debug purposes:
        reprojection_crv_[ic][v] = reproj_edgels;

        // Compute match cost
        unsigned d_vote;
        std::set<int> cur_inliers;
        std::vector<int> edge_index_chain(curve_3d.size(),-1);
        if (sels_.empty()) {
          // Anil: Replacing the edge support counting code with my own May 21st, 2014
          //  d_vote = dbcsi_curve_distance::num_inliers_dt_oriented(
          //      reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), label(v), *em_[v]);
                d_vote = bcsid_curve_distance::inlier_edgels_dt_oriented(
                    reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), 
                    label(v), *em_[v], &cur_inliers, edge_support_count_per_candidate[ic], 
                    orig_ids, &edge_index_chain);          
        } else {
          d_vote = bcsid_curve_distance::num_inlier_curvelets_dt_oriented(reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), label(v), sels_[v]->CM(),
                          tau_min_num_inlier_edgels_per_curvelet_);
        }

        votes[ic] += (d_vote < tau_min_inliers_per_view_)? 0 : d_vote;

        //if(!cur_inliers.empty())
        inlierEdgelsPerConfView.push_back(cur_inliers);
        edge_index_chain_per_conf_view[v-2] = edge_index_chain;

        if(d_vote >= tau_min_inliers_per_view_)
          {
            curInlierViews.push_back(v);
            num_inlier_views++;
          }

#ifdef BMCSD_VERBOSE_DEBUG
        std::cout << "\t\tinliers on view[" << v << "] = " << d_vote << std::endl;
#endif
      }

      //inlierEdgelsPerCandidate.push_back(inlierEdgelsPerConfView);
      //inlierViews.push_back(curInlierViews);

      inlierEdgelsPerCandidate[ic] = inlierEdgelsPerConfView;
      inlierViews[ic] = curInlierViews;

      if (edge_index_chain_per_candidate_ptr!=NULL)
	      edge_index_chain_per_candidate[ic] = edge_index_chain_per_conf_view;

      if (votes[ic] < tau_min_total_inliers_)
	      votes[ic] = 0;
      if(num_inlier_views<6)
	      votes[ic] = 0;

#ifdef BMCSD_VERBOSE_DEBUG
      std::cout << "\t\tcurve[" << ic << "] has " << votes[ic] << " total inliers\n";
      std::cout << "\t\tend ===========" << std::endl;
#endif
    }
  }
  return true;
}

bool bmcsd_odt_curve_stereo_e::
match_using_orientation_dt_extras(std::vector<unsigned long> *votes_ptr, std::vector<std::vector<std::set<int> > > &inlierEdgelsPerCandidate,
                                  std::vector<std::vector<unsigned> > &inlierViews, std::set<int> curve_ids, 
				  std::vector<std::vector<unsigned> > &edge_support_count_per_candidate)
{
#ifndef NDEBUG
  if (!ready_for_oriented_matching())
    return false;
#endif

  if (num_candidates() == 0) {
    votes_ptr->clear();
    return true;
  }

  std::vector<unsigned long> &votes = *votes_ptr;
  votes.resize(num_candidates(), 0);

  unsigned ini_id, end_id;

  get_increasing_endpoints(&ini_id, &end_id);

  bdifd_rig rig(cams(v0()).Pr_, cams(v1()).Pr_);

  // For each candidate curve
  reprojection_crv_.resize(num_candidates());
  std::vector<bbld_subsequence_set> sseq = this->get_sseq();

  for (unsigned ic=0; ic < num_candidates(); ++ic) {
    unsigned origID_u = sseq[this->v1()][crv_candidates(ic)].orig_id();
    int origID = static_cast<int>(origID_u);
    if((curve_ids.find(origID) != curve_ids.end()) && 
       (std::find(usedCurves_[this->v1()].begin(), usedCurves_[this->v1()].end(), origID_u) == usedCurves_[this->v1()].end())) {
      // Anil: Adding a threshold for minimum number of inlier views Apr 18th, 2014
      // Anil: Also adding vector of inlier views per candidate, and inlier edgels per confirmation view for each candidate
      unsigned num_inlier_views = 0; 
      std::vector<std::set<int> > inlierEdgelsPerConfView;
      std::vector<unsigned> curInlierViews;

      bdifd_1st_order_curve_3d curve_3d;
      reconstruct_candidate_1st_order(ini_id, end_id, ic, rig, &curve_3d);
      edge_support_count_per_candidate[ic].resize(curve_3d.size());
      std::fill(edge_support_count_per_candidate[ic].begin(),edge_support_count_per_candidate[ic].end(),0);

#ifdef BMCSD_VERBOSE_DEBUG
      std::cout << "Votes for curve[" << ic << "] ===========" << std::endl;
#endif
      reprojection_crv_[ic].resize(nviews());
      for (unsigned v=0; v < nviews(); ++v) {
        if (v == v0() || v == v1() )
          continue;

        // Compute reprojected_curve into view v
        bdifd_1st_order_curve_2d reprojected_curve;
        project_curve_1st_order(v, curve_3d, &reprojected_curve);
        assert (reprojected_curve.size() == curve_3d.size());

        // Anil: Clip to image bounds, but keep track of the original curve sample IDs
        std::vector<unsigned> orig_ids;
        bmcsd_util::clip_to_img_bounds(dt(v), &reprojected_curve, orig_ids);
        //bmcsd_util::clip_to_img_bounds(dt(v), &reprojected_curve);

        // translate reproj. curve into edgel sequence
        bcsid_edgel_seq reproj_edgels;
        bmcsd_algo_util::bdifd_to_sdet(reprojected_curve, &reproj_edgels);

        assert (reproj_edgels.size() == reprojected_curve.size());

        // For debug purposes:
        reprojection_crv_[ic][v] = reproj_edgels;

        // Compute match cost
        unsigned d_vote;
        std::set<int> cur_inliers;
        if (sels_.empty()) {
          // Anil: Replacing the edge support counting code with my own May 21st, 2014
          //  d_vote = dbcsi_curve_distance::num_inliers_dt_oriented(
          //      reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), label(v), *em_[v]);
                d_vote = bcsid_curve_distance::inlier_edgels_dt_oriented(
           reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), label(v), *em_[v], &cur_inliers, edge_support_count_per_candidate[ic], orig_ids);          
        } else {
          d_vote = bcsid_curve_distance::num_inlier_curvelets_dt_oriented(
           reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(v), label(v), sels_[v]->CM(),
           tau_min_num_inlier_edgels_per_curvelet_);
        }

        votes[ic] += (d_vote < tau_min_inliers_per_view_)? 0 : d_vote;

        //if(!cur_inliers.empty())
        inlierEdgelsPerConfView.push_back(cur_inliers);

        if(d_vote >= tau_min_inliers_per_view_) {
            curInlierViews.push_back(v);
            num_inlier_views++;
          }

#ifdef BMCSD_VERBOSE_DEBUG
        std::cout << "\t\tinliers on view[" << v << "] = " << d_vote << std::endl;
#endif
      }

      //inlierEdgelsPerCandidate.push_back(inlierEdgelsPerConfView);
      //inlierViews.push_back(curInlierViews);

      inlierEdgelsPerCandidate[ic] = inlierEdgelsPerConfView;
      inlierViews[ic] = curInlierViews;

      if (votes[ic] < tau_min_total_inliers_)
	      votes[ic] = 0;

      if(num_inlier_views<6)
	      votes[ic] = 0;

#ifdef BMCSD_VERBOSE_DEBUG      
      std::cout << "\t\tcurve[" << ic << "] has " << votes[ic] << " total inliers\n";
      std::cout << "\t\tend ===========" << std::endl;
#endif
    }
  }

  return true;
}

bool bmcsd_odt_curve_stereo_e::
match_mate_curves_using_orientation_dt_extras(unsigned long *votes_ptr, unsigned v, unsigned ic)
{
#ifndef NDEBUG
  if (!ready_for_oriented_matching())
    return false;
#endif

  // Anil: Not sure if the following snippet should be turned on
  //  if (num_candidates() == 0) {
  //    votes_ptr->clear();
  //    return true;
  //  }

  unsigned long &votes = *votes_ptr;

  unsigned ini_id, 
           end_id;

  get_increasing_endpoints(&ini_id, &end_id);

  bdifd_rig rig(cams(v0()).Pr_, cams(v).Pr_);

  // Anil: For the input candidate curve
  reprojection_crv_.resize(1);

  bdifd_1st_order_curve_3d curve_3d;
  reconstruct_candidate_1st_order(ini_id, end_id, ic, rig, &curve_3d);

#ifdef BMCSD_VERBOSE_DEBUG
  std::cout << "Votes for curve[" << ic << "] ===========" << std::endl;
#endif
  reprojection_crv_[0].resize(nviews());
  for (unsigned vv=0; vv < nviews(); ++vv) {
    if (vv == v0() || vv == v )
      continue;

    // Compute reprojected_curve into view vv
    bdifd_1st_order_curve_2d reprojected_curve;
    project_curve_1st_order(vv, curve_3d, &reprojected_curve);
    assert (reprojected_curve.size() == curve_3d.size());

    bmcsd_util::clip_to_img_bounds(dt(vv), &reprojected_curve);

    // translate reproj. curve into edgel sequence
    bcsid_edgel_seq reproj_edgels;
    bmcsd_algo_util::bdifd_to_sdet(reprojected_curve, &reproj_edgels);

    assert (reproj_edgels.size() == reprojected_curve.size());

    // For debug purposes:
    reprojection_crv_[0][vv] = reproj_edgels;

    // Compute match cost
    unsigned d_vote;
    if (sels_.empty()) {
      d_vote = bcsid_curve_distance::num_inliers_dt_oriented(reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(vv), label(vv), *em_[vv]);
    } else {
      d_vote = bcsid_curve_distance::num_inlier_curvelets_dt_oriented(reproj_edgels, tau_distance_squared(), tau_dtheta_, dt(vv), label(vv), sels_[vv]->CM(),
								      tau_min_num_inlier_edgels_per_curvelet_);
    }

    votes += (d_vote < tau_min_inliers_per_view_)? 0 : d_vote;

#ifdef BMCSD_VERBOSE_DEBUG
    std::cout << "\t\tinliers on view[" << vv << "] = " << d_vote << std::endl;
#endif
  }

  if (votes < tau_min_total_inliers_)
    votes = 0;

#ifdef BMCSD_VERBOSE_DEBUG
  std::cout << "\t\tcurve[" << ic << "] has " << votes[ic] << " total inliers\n";
  std::cout << "\t\tend ===========" << std::endl;
#endif

  return true;
}

void bmcsd_odt_curve_stereo_e::
reconstruct_candidate_1st_order(unsigned ini_id, unsigned end_id, unsigned ic, 
    const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr)
{
  bdifd_1st_order_curve_3d &curve_3d = *crv_ptr;
  unsigned ini_id_sub, end_id_sub;
  get_matching_subcurve(ic, ini_id, end_id, &ini_id_sub, &end_id_sub);

  (this->matchCount_)++;

  set_selected_crv_by_id(v1(), crv_candidates(ic));

  curve_3d.reserve(end_id_sub - ini_id_sub + 1);
  reconstruct_subcurve_1st_order(ini_id_sub, end_id_sub, rig, &curve_3d);

  assert(curve_3d.size() == end_id_sub - ini_id_sub  + 1);
}

void bmcsd_odt_curve_stereo_e::
reconstruct_candidate_1st_order_with_flags(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
                                           const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr, std::vector<bool> &flags, unsigned &recon_shift)
{
  bdifd_1st_order_curve_3d &curve_3d = *crv_ptr;
  unsigned ini_id_sub, end_id_sub;
  get_matching_subcurve(ic, ini_id, end_id, &ini_id_sub, &end_id_sub);

  (this->reconCount_)++;

  set_selected_crv_by_id(v1(), crv_candidates(ic));

  curve_3d.reserve(end_id_sub - ini_id_sub + 1);

  std::vector<bool> secondAnchorFlags;

  reconstruct_subcurve_1st_order_with_flags(ini_id_sub, end_id_sub, rig, &curve_3d, secondAnchorFlags);

  flags.clear();
  for(unsigned i=ini_id_sub; i<=end_id_sub; ++i)
  {
      //std::cout << "FIRST: " << vsol_flags_[v0()][curve_id][i] << ", SECOND: " << secondAnchorFlags[i-ini_id_sub];
      //std::cout << ", JOINT: " << (vsol_flags_[v0()][curve_id][i] || secondAnchorFlags[i-ini_id_sub]) << std::endl;
      flags.push_back(vsol_flags_[v0()][curve_id][i] && secondAnchorFlags[i-ini_id_sub]);
  }

  assert(curve_3d.size() == end_id_sub - ini_id_sub  + 1);
  recon_shift = ini_id_sub;
}

void bmcsd_odt_curve_stereo_e::
reconstruct_candidate_1st_order_with_flags_temp(unsigned ini_id, unsigned end_id, unsigned ic, unsigned curve_id,
                                           const bdifd_rig &rig, bdifd_1st_order_curve_3d *crv_ptr, std::vector<bool> &flags, unsigned v)
{
  bdifd_1st_order_curve_3d &curve_3d = *crv_ptr;
  unsigned ini_id_sub, end_id_sub;
  get_matching_subcurve(ic, ini_id, end_id, &ini_id_sub, &end_id_sub);

  set_selected_crv_by_id(v, crv_candidates(ic));

  curve_3d.reserve(end_id_sub - ini_id_sub + 1);

  std::vector<bool> secondAnchorFlags;
  reconstruct_subcurve_1st_order_with_flags(ini_id_sub, end_id_sub, rig, &curve_3d, secondAnchorFlags,v);

  flags.clear();
  for(unsigned i=ini_id_sub; i<=end_id_sub; ++i)
  {
      //std::cout << "FIRST: " << vsol_flags_[v0()][curve_id][i] << ", SECOND: " << secondAnchorFlags[i-ini_id_sub];
      //std::cout << ", JOINT: " << (vsol_flags_[v0()][curve_id][i] || secondAnchorFlags[i-ini_id_sub]) << std::endl;
      flags.push_back(vsol_flags_[v0()][curve_id][i] && secondAnchorFlags[i-ini_id_sub]);
  }

  assert(curve_3d.size() == end_id_sub - ini_id_sub  + 1);
}


void bmcsd_odt_curve_stereo_e::
reconstruct_subcurve_1st_order(
    unsigned ini_id_sub, 
    unsigned end_id_sub, 
    const bdifd_rig &rig,
    bdifd_1st_order_curve_3d *curve_3d
    ) const
{
  static const unsigned second_view = 1;

  curve_3d->reserve(end_id_sub - ini_id_sub + 1);

  std::string out_fname = "alignment.txt";
  std::ofstream out_file;

  if(dummyFlag) {
      out_file.open(out_fname.c_str());
      unsigned numAlignmentSamples = end_id_sub - ini_id_sub + 1;
      out_file << numAlignmentSamples << std::endl;
  }
  

  for (unsigned di0=0; di0 + ini_id_sub <= end_id_sub; ++di0) {
    bdifd_1st_order_point_3d pt_3D;
    reconstruct_curve_point_1st_order(second_view, ini_id_sub, di0, rig, &pt_3D);
    curve_3d->push_back(pt_3D);

    if(dummyFlag) {
        unsigned firstSample = ini_id_sub + di0;
        out_file << firstSample << " " << dummyID << " " << lineCoef[0] << " " << lineCoef[1] << " " << lineCoef[2] << std::endl;
    }

  }

  if(dummyFlag)
      out_file.close();

}

void bmcsd_odt_curve_stereo_e::
reconstruct_subcurve_1st_order_with_flags(
    unsigned ini_id_sub,
    unsigned end_id_sub,
    const bdifd_rig &rig,
    bdifd_1st_order_curve_3d *curve_3d,
    std::vector<bool> &curveFlags
    )
{
  static const unsigned second_view = 1;
  curve_samples_v1_.clear();

  curve_3d->reserve(end_id_sub - ini_id_sub + 1);

  std::string out_fname = "alignment.txt";
  std::ofstream out_file;

  if(dummyFlag) {
      out_file.open(out_fname.c_str());
      unsigned numAlignmentSamples = end_id_sub - ini_id_sub + 1;
      out_file << numAlignmentSamples << std::endl;
  }

  for (unsigned di0=0; di0 + ini_id_sub <= end_id_sub; ++di0) {
    bdifd_1st_order_point_3d pt_3D;
    bool curFlag;
    unsigned curveID_v1;
    reconstruct_curve_point_1st_order_with_flags(second_view, ini_id_sub, di0, rig, &pt_3D, curFlag, curveID_v1);
    curveFlags.push_back(curFlag);
    curve_samples_v1_.push_back(curveID_v1);
    curve_3d->push_back(pt_3D);

    if(dummyFlag) {
        unsigned firstSample = ini_id_sub + di0;
        out_file << firstSample << " " << dummyID << " " << lineCoef[0] << " " << lineCoef[1] << " " << lineCoef[2] << std::endl;
        //std::cout << "SHIFT: " << ini_id_sub << std::endl;
        //std::cout << "END: " << end_id_sub << std::endl;
        //std::cout << "EP CONTAINER SIZE: " << ep(second_view-1).size() << std::endl;
        //std::cout << "SPANNED SIZE: " << end_id_sub - ini_id_sub + 1 << std::endl;
    }

  }

  if(dummyFlag)
      out_file.close();
    
}

void bmcsd_odt_curve_stereo_e::
reconstruct_subcurve_1st_order_with_flags(
    unsigned ini_id_sub,
    unsigned end_id_sub,
    const bdifd_rig &rig,
    bdifd_1st_order_curve_3d *curve_3d,
    std::vector<bool> &curveFlags,
    unsigned v
    )
{
  curve_3d->reserve(end_id_sub - ini_id_sub + 1);

  std::string out_fname = "alignment.txt";
  std::ofstream out_file;

  if(dummyFlag)
  {
      out_file.open(out_fname.c_str());
      unsigned numAlignmentSamples = end_id_sub - ini_id_sub + 1;
      out_file << numAlignmentSamples << std::endl;
  }

  for (unsigned di0=0; di0 + ini_id_sub <= end_id_sub; ++di0) {
    bdifd_1st_order_point_3d pt_3D;
    bool curFlag;
    unsigned curveID_v1;
    reconstruct_curve_point_1st_order_with_flags(v, ini_id_sub, di0, rig, &pt_3D, curFlag, curveID_v1);
    curveFlags.push_back(curFlag);
    curve_3d->push_back(pt_3D);

    if(dummyFlag)
    {
        unsigned firstSample = ini_id_sub + di0;
        out_file << firstSample << " " << dummyID << " " << lineCoef[0] << " " << lineCoef[1] << " " << lineCoef[2] << std::endl;
        //std::cout << "SHIFT: " << ini_id_sub << std::endl;
        //std::cout << "END: " << end_id_sub << std::endl;
        //std::cout << "EP CONTAINER SIZE: " << ep(second_view-1).size() << std::endl;
        //std::cout << "SPANNED SIZE: " << end_id_sub - ini_id_sub + 1 << std::endl;
    }

  }

  if(dummyFlag)
      out_file.close();
    
}

void bmcsd_odt_curve_stereo_e::
reconstruct_curve_point_1st_order(
    unsigned v,
    unsigned ini_id,
    unsigned di0, 
    const bdifd_rig &rig,
    bdifd_1st_order_point_3d *pt_3D
    )
{
  static const unsigned id_v0 = 0;

  bdifd_1st_order_point_2d p0_w
    = pt_tgts_[id_v0][selected_crv_id(id_v0)][ini_id + di0];

  // Corresponding points

  unsigned nearest_sample_id;
  lineCoef.clear();
  lineCoef.push_back(ep(v-1)[di0+ini_id].a());
  lineCoef.push_back(ep(v-1)[di0+ini_id].b());
  lineCoef.push_back(ep(v-1)[di0+ini_id].c());
  {
  vgl_point_2d<double> pt;
  becld_epiline_interceptor::curve_line_intersection_simple(
      *selected_crv(v), ep(v-1)[di0+ini_id], &pt, &nearest_sample_id);
  }

  dummyID = nearest_sample_id;

  bdifd_1st_order_point_2d 
    p1_w = pt_tgts_[v][selected_crv_id(v)][nearest_sample_id];

  if (!p0_w.valid || !p1_w.valid)
    std::cerr << "Warning: invalid points!!\n";

  // Reconstructions
  rig.reconstruct_1st_order(p0_w, p1_w, pt_3D);
}

void bmcsd_odt_curve_stereo_e::
reconstruct_curve_point_1st_order_with_flags(
    unsigned v,
    unsigned ini_id,
    unsigned di0,
    const bdifd_rig &rig,
    bdifd_1st_order_point_3d *pt_3D,
    bool &isConfident,
    unsigned &sample_v1
    )
{
  static const unsigned id_v0 = 0;

  bdifd_1st_order_point_2d p0_w
    = pt_tgts_[id_v0][selected_crv_id(id_v0)][ini_id + di0];

  // Corresponding points

  unsigned nearest_sample_id;
  lineCoef.clear();
  lineCoef.push_back(ep(v-1)[di0+ini_id].a());
  lineCoef.push_back(ep(v-1)[di0+ini_id].b());
  lineCoef.push_back(ep(v-1)[di0+ini_id].c());
  {
  vgl_point_2d<double> pt;
  becld_epiline_interceptor::curve_line_intersection_simple(
      *selected_crv(v), ep(v-1)[di0+ini_id], &pt, &nearest_sample_id);
  }

  dummyID = nearest_sample_id;
  isConfident = vsol_flags_[v][selected_crv_id(v)][nearest_sample_id];

  bdifd_1st_order_point_2d 
    p1_w = pt_tgts_[v][selected_crv_id(v)][nearest_sample_id];

  if (!p0_w.valid || !p1_w.valid)
    std::cerr << "Warning: invalid points!!\n";

  // Reconstructions
  rig.reconstruct_1st_order(p0_w, p1_w, pt_3D);
  sample_v1 = nearest_sample_id;
}

void bmcsd_odt_curve_stereo_e::
break_curves_into_episegs_pairwise(
    std::vector<std::vector< vsol_polyline_2d_sptr > > *broken_vsols,
    std::vector<bbld_subsequence_set> *ss_ptr
    )
{
  std::cout << "Called ODT episeg breaker with tau_min_epiangle = " 
    << tau_min_epiangle_*180.0/vnl_math::pi << " degrees" << std::endl;
  assert(has_curve_tangents());
  std::vector<bbld_subsequence_set> &ss = *ss_ptr;
  broken_vsols->resize(nviews());
  ss.resize(nviews());
  vgl_homg_point_2d<double> e, e_prime;
  fm_[v0()][v1()].get_epipoles(e, e_prime);

  {
  bbld_subsequence_set s_a;
  std::vector<vsol_polyline_2d_sptr> vsols_broken_at_turns;

  //Correct this!
  break_curves_into_episegs(vsols_[v0()], &vsols_broken_at_turns, e, &s_a, 0);
  //break_curves_into_episegs(vsols_[v0()], &(*broken_vsols)[v0()], e, &ss[v0()], 0);

  std::vector<vsol_spatial_object_2d_sptr> curvesIntermediate;

  for (unsigned i=0; i<vsols_broken_at_turns.size(); ++i)
      curvesIntermediate.push_back(dynamic_cast<vsol_spatial_object_2d*>(vsols_broken_at_turns[i].ptr()));
  
  bsold_save_cem(curvesIntermediate, std::string("intermediate_v0.cemv"));


  std::vector<std::vector<double> > tangents_a;
  consolidate_subsequences(curve_tangents_[v0()], s_a, &tangents_a);
  //consolidate_subsequences(curve_tangents_[v0()], ss[v0()], &tangents_a);

  vsol_flags_[v0()].resize(vsols_broken_at_turns.size());

  break_curves_into_episegs_angle(vsols_broken_at_turns, tangents_a,
                                  tau_min_epiangle_, &(*broken_vsols)[v0()], e, &ss[v0()], 0, 1, vsol_flags_[v0()]);
  compose_subsequences(s_a, &ss[v0()]);
  }
  
  {
  bbld_subsequence_set s_a;
  std::vector<vsol_polyline_2d_sptr> vsols_broken_at_turns;

  //Correct this!
  break_curves_into_episegs(vsols_[v1()], &vsols_broken_at_turns, e_prime, &s_a, 0);
  //break_curves_into_episegs(vsols_[v1()], &(*broken_vsols)[v1()], e_prime, &ss[v1()], 0);

  std::vector<std::vector<double> > tangents_a;
  consolidate_subsequences(curve_tangents_[v1()], s_a, &tangents_a); 
  //consolidate_subsequences(curve_tangents_[v1()], ss[v1()], &tangents_a);

  vsol_flags_[v1()].resize(vsols_broken_at_turns.size());

  break_curves_into_episegs_angle(vsols_broken_at_turns, tangents_a,
                                  tau_min_epiangle_, &(*broken_vsols)[v1()], e_prime, &ss[v1()], 0, 1, vsol_flags_[v1()]);
  compose_subsequences(s_a, &ss[v1()]);
  }

  for (unsigned v=0; v < nviews(); v++) {
    if (vsols_[v].empty() || v == v0() || v == v1() || 
        v >= curve_tangents_.size() || curve_tangents_[v].empty())
      continue;
    fm_[v0()][v].get_epipoles(e, e_prime);

    bbld_subsequence_set s_a;
    std::vector<vsol_polyline_2d_sptr> vsols_broken_at_turns;

    //Correct this!
    break_curves_into_episegs(vsols_[v], &vsols_broken_at_turns, e_prime, &s_a, 0);
    //break_curves_into_episegs(vsols_[v], &(*broken_vsols)[v], e_prime, &ss[v], 0);
    
    std::vector<std::vector<double> > tangents_a;
    consolidate_subsequences(curve_tangents_[v], s_a, &tangents_a);

    vsol_flags_[v].resize(vsols_broken_at_turns.size());

    break_curves_into_episegs_angle(vsols_broken_at_turns, tangents_a,
                                    tau_min_epiangle_, &(*broken_vsols)[v], e_prime, &ss[v], 0, 1, vsol_flags_[v]);
    compose_subsequences(s_a, &ss[v]);
  }
}

void bmcsd_odt_curve_stereo_e::
break_curves_into_episegs_angle(
  const std::vector< vsol_polyline_2d_sptr >  &vsols,
  const std::vector<std::vector<double> > &tgts,
  double min_epiangle,
  std::vector<vsol_polyline_2d_sptr> *vsols2,
  const vgl_homg_point_2d<double> &e,
  bbld_subsequence_set *ss_ptr,
  bool print, bool onlyMark,
  std::vector<std::vector<bool> > &uncertaintyFlags
  )
{
  bbld_subsequence_set &ss = *ss_ptr;
  // ----------------------------------------------------------------------
  // Break curve

  becld_epipole_sptr epipole = new becld_epipole(e.x()/e.w(), e.y()/e.w());
  becld_episeg_from_curve_converter factory(epipole);

  uncertaintyFlags.resize(vsols.size());

  if(onlyMark)
      factory.set_tangent_threshold(-1);
  else
      factory.set_tangent_threshold(min_epiangle);

  // A) For each vsol, do:
  
  std::vector<becld_episeg_sptr> all_episegs;
  all_episegs.reserve(2*vsols.size());
  ss.reserve(2*vsols.size());
  for (unsigned i=0; i < vsols.size(); ++i) {

    uncertaintyFlags[i].resize(vsols[i]->size());

    // A1 - convert to digital curve
    std::vector<vsol_point_2d_sptr>  samples;
    samples.reserve(vsols[i]->size());
    for (unsigned k=0; k < vsols[i]->size(); ++k)
      samples.push_back(vsols[i]->vertex(k));

    vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d(samples);
    // A2 - apply episeg
    bbld_subsequence_set ss_partition;
    std::vector<becld_episeg_sptr> eps = factory.convert_curve_using_tangents(dc, tgts[i], &ss_partition, print, i);

    assert(ss_partition.num_subsequences() == eps.size());
    assert(!(vsols[i]->size() && eps.empty()));

    double tau_epiangle;

    if(onlyMark)
        tau_epiangle = -1;
    else
        tau_epiangle = min_epiangle;

    //dbecl_delta_angle_predicate is_angle_acceptable(dc, tgts[i], epipole, min_epiangle);
    becld_delta_angle_predicate is_angle_acceptable(dc, tgts[i], epipole, tau_epiangle);

    // Keep only the episegs that are transversal to epilines.
    for(unsigned k=0; k < eps.size(); ++k ) {
        if ( ss_partition[k].size() && is_angle_acceptable(ss_partition[k].ini()) ) {
            all_episegs.push_back(eps[k]);
            ss.push_back(ss_partition[k]);
        }
    }

    // Anil: Construct another predicate using min_epiangle to mark segments we have high confidence in
    //This is redundant if onlyMark is not true
    becld_delta_angle_predicate is_angle_certain(dc, tgts[i], epipole, min_epiangle);

    if(onlyMark)
    {
        for(unsigned k=0; k<vsols[i]->size(); ++k)
            uncertaintyFlags[i][k] = is_angle_certain(k);
    }
  }

  // B) Extract vsol from episegs
  vsols2->reserve(all_episegs.size());

  for(unsigned k=0; k < all_episegs.size(); ++k) {
    //if ( all_episegs[k]->max_index() 
    //   - all_episegs[k]->min_index()  < threshold_number_of_points)
    //  continue;

    vsols2->push_back(new vsol_polyline_2d);
    // B1 - traverse episegs
    for (unsigned i = ss[k].ini(); i < ss[k].end(); ++i)
      vsols2->back()->add_vertex( all_episegs[k]->curve()->point(i) );
  }
}

bool 
bmcsd_match_all_curves(
  bmcsd_odt_curve_stereo_e &s, 
  bmcsd_discrete_corresp_e *corresp_ptr,
  bool track_supporting_edges)
{
  bmcsd_discrete_corresp_e &corresp = *corresp_ptr;
  corresp.set_size(s.num_curves(s.v0()), s.num_curves(s.v1()));
  corresp.set_checksum(bmcsd_discrete_corresp_algo::compute_checksum(s));

  unsigned const ncurves = s.num_curves(s.v0());

  std::cout << "Started: stereo matching of " << ncurves 
    << " curves in view[" << s.v0() << "].\n";

  std::cout << "Match parameters:" << 
    " tau_dtheta_=" << s.dtheta_threshold() <<
    " tau_distance_squared_=" << s.tau_distance_squared() <<
    " tau_min_total_inliers_=" << s.min_total_inliers() << 
    " tau_min_inliers_per_view_=" << s.min_inliers_per_view() <<
    " use curvelets?" << ((s.has_sels()) ? "yes" : "no") <<
    " tau_min_num_inlier_edgels_per_curvelet_=" << s.min_num_inlier_edgels_per_curvelet() << 
    std::endl;

  //  for (unsigned c=0; c < ncurves; ++c) {
  for (unsigned c=0; c < ncurves; ++c) {
    if (c % 10 == 0)
      std::cout << "Matching curve[" << c << "-" << c+10 << "]\n";
    s.set_selected_crv_by_id(s.v0(), c);
    unsigned const ini_id = 0;
    unsigned const end_id = s.selected_crv(s.v0())->size()-1;
    s.set_subcurve(ini_id, end_id);
    s.compute_epipolar_beam_candidates();

    assert(s.ready_for_oriented_matching());

    std::vector<unsigned long> votes;
    std::vector<std::vector<std::set<int> > > inlierEdgelsPerCandidate;
    std::vector<std::vector<unsigned> > inlierViews;
    std::vector<std::vector<unsigned> > edge_support_count_per_candidate;

    edge_support_count_per_candidate.resize(s.num_candidates());

    if(track_supporting_edges) {
      if (!s.match_using_orientation_dt_extras(&votes,inlierEdgelsPerCandidate,inlierViews,edge_support_count_per_candidate)) {
            std::cerr << "Error: problem during matching.\n";
            return false;
        }
    }
    else {
        if (!s.match_using_orientation_dt_extras(&votes)) {
            std::cerr << "Error: problem during matching.\n";
            return false;
        }
    }

//    if(!inlierEdgelsPerCandidate.empty())
//        std::cout << "TOTAL NUMBER OF SUPPORT FOR FIRST CANDIDATE: " << inlierEdgelsPerCandidate[0].size() << std::endl;

    assert(votes.size() == s.num_candidates());
    for (unsigned i=0; i < s.num_candidates(); ++i) {
        if (votes[i] > 0)
            corresp[c].push_back(bmcsd_attributed_object(s.crv_candidates(i), inlierEdgelsPerCandidate[i], inlierViews[i], false, votes[i]));
    }

  }

  std::cout << "Ended: stereo matching " << ncurves << " curves in view[" << 
    s.v0() << "], with " << corresp.n0() - corresp.count_empty() << " having a corresp.\n";
  return true;
}

bool 
bmcsd_match_all_curves_using_mates(
  bmcsd_odt_curve_stereo_e &s, 
  bmcsd_discrete_corresp_e *corresp_ptr,
  unsigned seed_id,
  std::vector<std::set<int> > curve_ids,
  bool isFirstRun,
  bool track_supporting_edges)
{
  bmcsd_discrete_corresp_e &corresp = *corresp_ptr;
  corresp.set_size(s.num_curves(s.v0()), s.num_curves(s.v1()));
  corresp.set_checksum(bmcsd_discrete_corresp_algo::compute_checksum(s));

  unsigned const ncurves = s.num_curves(s.v0());

  std::cout << "Started: stereo matching of " << ncurves 
    << " curves in view[" << s.v0() << "].\n";

  std::cout << "Match parameters:" << 
    " tau_dtheta_=" << s.dtheta_threshold() <<
    " tau_distance_squared_=" << s.tau_distance_squared() <<
    " tau_min_total_inliers_=" << s.min_total_inliers() << 
    " tau_min_inliers_per_view_=" << s.min_inliers_per_view() <<
    " use curvelets?" << ((s.has_sels()) ? "yes" : "no") <<
    " tau_min_num_inlier_edgels_per_curvelet_=" << s.min_num_inlier_edgels_per_curvelet() << 
    std::endl;

  //s.edge_curve_index_.resize(s.nviews()-2);
  
  std::vector<bbld_subsequence_set> sseq = s.get_sseq();
  if(isFirstRun)
  {
    for (unsigned c=0; c < ncurves; ++c) {
      unsigned origID = sseq[s.v0()][c].orig_id();
      if(std::find(s.usedCurves_[s.v0()].begin(), s.usedCurves_[s.v0()].end(), origID) == s.usedCurves_[s.v0()].end()) {
      //if(sseq[s.v0()][c].orig_id()==seed_id) {
	if (c % 10 == 0)
	  std::cout << "Matching curve[" << c << "-" << c+10 << "]\n";
	s.set_selected_crv_by_id(s.v0(), c);
	unsigned const ini_id = 0;
	unsigned const end_id = s.selected_crv(s.v0())->size()-1;
	s.set_subcurve(ini_id, end_id);
	s.compute_epipolar_beam_candidates();

	assert(s.ready_for_oriented_matching());

	std::vector<unsigned long> votes;
	std::vector<std::vector<std::set<int> > > inlierEdgelsPerCandidate(s.num_candidates());
	std::vector<std::vector<unsigned> > inlierViews(s.num_candidates());
	std::vector<std::vector<unsigned> > edge_support_count_per_candidate(s.num_candidates());
	std::vector<std::vector<std::vector<int> > > edge_index_chain_per_candidate(s.num_candidates());

	//inlierEdgelsPerCandidate.resize(s.num_candidates());
	//inlierViews.resize(s.num_candidates());
	//edge_support_count_per_candidate.resize(s.num_candidates());
	

	if(track_supporting_edges) {
	  if (!s.match_using_orientation_dt_extras(&votes,inlierEdgelsPerCandidate,inlierViews,edge_support_count_per_candidate, &edge_index_chain_per_candidate)) {
	    std::cerr << "Error: problem during matching.\n";
	    return false;
	  }
	}
	else {
	  if (!s.match_using_orientation_dt_extras(&votes)) {
	    std::cerr << "Error: problem during matching.\n";
	    return false;
	  }
	}
  
	assert(votes.size() == s.num_candidates());
	for (unsigned i=0; i < s.num_candidates(); ++i) {
	  if (votes[i] > 0)
	    corresp[c].push_back(bmcsd_attributed_object(s.crv_candidates(i), s.v1(), inlierEdgelsPerCandidate[i], inlierViews[i], 
						      edge_support_count_per_candidate[i], false, votes[i], edge_index_chain_per_candidate[i]));
	}
      }
    }
  }
  else
  {
    for (unsigned c=0; c < ncurves; ++c) {
      unsigned origID = sseq[s.v0()][c].orig_id();
      if(std::find(s.usedCurves_[s.v0()].begin(), s.usedCurves_[s.v0()].end(), origID) == s.usedCurves_[s.v0()].end()) {
      //if(sseq[s.v0()][c].orig_id()==seed_id) {
	if (c % 10 == 0)
	  std::cout << "Matching curve[" << c << "-" << c+10 << "]\n";

	s.set_selected_crv_by_id(s.v0(), c);
	unsigned const ini_id = 0;
	unsigned const end_id = s.selected_crv(s.v0())->size()-1;
	s.set_subcurve(ini_id, end_id);
	s.compute_epipolar_beam_candidates();

	assert(s.ready_for_oriented_matching());

	std::vector<unsigned long> votes;
	std::vector<std::vector<std::set<int> > > inlierEdgelsPerCandidate;
	std::vector<std::vector<unsigned> > inlierViews;
	std::vector<std::vector<unsigned> > edge_support_count_per_candidate;

	inlierEdgelsPerCandidate.resize(s.num_candidates());
	inlierViews.resize(s.num_candidates());
	edge_support_count_per_candidate.resize(s.num_candidates());

	if(track_supporting_edges) {
	  if (!s.match_using_orientation_dt_extras(&votes,inlierEdgelsPerCandidate,inlierViews,curve_ids[sseq[s.v0()][c].orig_id()],edge_support_count_per_candidate)) {
	    std::cerr << "Error: problem during matching.\n";
	    return false;
	  }
	}
	else {
	  if (!s.match_using_orientation_dt_extras(&votes)) {
	    std::cerr << "Error: problem during matching.\n";
	    return false;
	  }
	}

	//    if(!inlierEdgelsPerCandidate.empty())
	//        std::cout << "TOTAL NUMBER OF SUPPORT FOR FIRST CANDIDATE: " << inlierEdgelsPerCandidate[0].size() << std::endl;

	
	assert(votes.size() == s.num_candidates());

	for (unsigned i=0; i < s.num_candidates(); ++i) {
	  if (votes[i] > 0)
	    corresp[c].push_back(bmcsd_attributed_object(s.crv_candidates(i), s.v1(), inlierEdgelsPerCandidate[i], inlierViews[i], edge_support_count_per_candidate[i], false, votes[i]));
	}

      }
    }
  }
  std::cout << "Ended: stereo matching " << ncurves << " curves in view[" << 
    s.v0() << "], with " << corresp.n0() - corresp.count_empty() << " having a corresp.\n";
  return true;
}

// Anil: Old version of the matching function, with no option to recover the
//supporting edgels
/*
bool 
bmcsd_match_and_reconstruct_all_curves_attr(
    bmcsd_odt_curve_stereo_e &s, 
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    bmcsd_discrete_corresp_e *corresp_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr
    )
{
  // TODO: this is were we should pass attr so that we can gent the inlier
  // views.
  bmcsd_match_all_curves(s, corresp_ptr);

  corresp_ptr->keep_only_unambiguous_max(
      s.min_first_to_second_best_ratio(), 
      static_cast<double>(s.lonely_threshold()));

  // The following can be simulated by setting tau_first_to_second_best_ratio_ to zero
  // 
  //   corresp_ptr->keep_only_extreme_cost(true);

  return reconstruct_from_corresp_attr(s, *corresp_ptr, crv3d_ptr, attr_ptr);
}*/

bool 
bmcsd_match_and_reconstruct_all_curves_attr(
    bmcsd_odt_curve_stereo_e &s, 
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    bmcsd_discrete_corresp_e *corresp_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    bool track_supporting_edges
    )
{
  // TODO: this is were we should pass attr so that we can gent the inlier
  // views.
  bmcsd_match_all_curves(s, corresp_ptr, track_supporting_edges);

  corresp_ptr->keep_only_unambiguous_max(
      s.min_first_to_second_best_ratio(), 
      static_cast<double>(s.lonely_threshold()));

  // The following can be simulated by setting tau_first_to_second_best_ratio_ to zero
  // 
  //   corresp_ptr->keep_only_extreme_cost(true);

  return reconstruct_from_corresp_attr(s, *corresp_ptr, crv3d_ptr, attr_ptr);
}

bool 
bmcsd_match_and_reconstruct_all_curves_attr_using_mates(
    bmcsd_odt_curve_stereo_e &s, 
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    bmcsd_discrete_corresp_e *corresp_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    std::vector<std::set<int> > mate_curves_v1,
    bool isFirstRun,
    bool track_supporting_edges
    )
{
  unsigned seed_id = 757;
  vul_timer matching;
  bmcsd_match_all_curves_using_mates(s, corresp_ptr, seed_id, mate_curves_v1, isFirstRun, track_supporting_edges);
  std::cout << "#3b MATCHING: " << matching.real() << std::endl;

  vul_timer unambiguating;
  corresp_ptr->keep_only_unambiguous_max(s.min_first_to_second_best_ratio(), 
					 static_cast<double>(s.lonely_threshold()));
  std::cout << "#3c UNAMBIGUATING: " << unambiguating.real() << std::endl;

  vul_timer reconstructing;
  bool retval = reconstruct_from_corresp_attr(s, *corresp_ptr, crv3d_ptr, attr_ptr, seed_id);
  std::cout << "#3d RECONSTRUCTING: " << reconstructing.real() << std::endl;
  return retval;

  //return reconstruct_from_corresp_attr_using_mates(s, *corresp_ptr, crv3d_ptr, attr_ptr);
}

bool 
reconstruct_from_corresp_attr(
    bmcsd_odt_curve_stereo_e &s, 
    const bmcsd_discrete_corresp_e &corresp,
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr,
    unsigned seed_id
    )
{
  std::cout << "Reconstructing curves\n";

  std::vector<bdifd_1st_order_curve_3d> &crv3d = *crv3d_ptr;
  std::vector< bmcsd_curve_3d_attributes_e > &attr = *attr_ptr;
  unsigned const ncurves = s.num_curves(s.v0());
  assert (ncurves == corresp.size());
  crv3d.reserve(ncurves);
  attr.reserve(ncurves);

  bdifd_rig rig(s.cams(s.v0()).Pr_, s.cams(s.v1()).Pr_);
  for (unsigned c=0; c < ncurves; ++c) {
    if (corresp[c].empty())
      continue;
    
    if(c==195)
        s.dummyFlag = true;
    else
        s.dummyFlag = false;
    
    s.set_selected_crv_by_id(s.v0(), c);
    unsigned const ini_id = 0;
    unsigned const end_id = s.selected_crv(s.v0())->size()-1;
    s.set_subcurve(ini_id, end_id);
    s.compute_epipolar_beam_candidates();

    unsigned ic;
    bool found = s.get_index_of_candidate_curve(s.curves(s.v1(), corresp[c].front().id()), &ic);
    assert (found);
    crv3d.resize(crv3d.size()+1);
    
    // Anil: Create the vector of flags that will mark the samples in the 3D curve that are reliable
    std::vector<bool> certaintyFlags;
    unsigned ini_shift;

    std::vector<bbld_subsequence_set> sseq = s.get_sseq();

    s.reconstruct_candidate_1st_order_with_flags(ini_id, end_id, ic, c, rig, &crv3d.back(), certaintyFlags, ini_shift);
    //s.reconstruct_candidate_1st_order(ini_id, end_id, ic, rig, &crv3d.back());

    attr.resize(attr.size()+1);
    attr.back().set_i0_i1(c, corresp[c].front().id());
    attr.back().total_support_ = static_cast<unsigned>(corresp[c].front().cost());

    // Anil: Propagating the edgel support data from bmcsd_discrete_corresp_e to bmcsd_curve_3d_attributes_e
    attr.back().supportingEdgelsPerConfView_ = corresp[c].front().supportingStructures_;
    // Anil: Computing mate curves using the supporting edgel information
    s.mate_curves_.clear();
    s.mate_curves_.resize(s.nviews()-2);

    // Anil: Key = mate curve ID, Value = weight, indicating level of support given in number of edges
    //Vector elements are confirmation views
    std::vector<std::map<unsigned,unsigned> > mate_curve_weights(s.nviews()-2);

    for (unsigned v=0; v+2<s.nviews(); ++v) {
      std::set<int> curViewSupport = corresp[c].front().supportingStructures_[v];
      unsigned numInlierEdgels = curViewSupport.size();

      for (std::set<int>::iterator edgeit=curViewSupport.begin(); edgeit != curViewSupport.end(); ++edgeit) {
        int curEdgel = *edgeit;
        int curCurve = s.edge_curve_index_[v][curEdgel];
        if(curCurve > -1){
          s.mate_curves_[v].insert(curCurve);
          
          unsigned u_curCurve = static_cast<unsigned>(curCurve);
          if(mate_curve_weights[v].find(u_curCurve) == mate_curve_weights[v].end())
            mate_curve_weights[v].insert(std::pair<unsigned,unsigned>(u_curCurve,1));
          else
            mate_curve_weights[v][u_curCurve]++;
          
        }
      }
    }

    attr.back().mate_curves_ = s.mate_curves_;
    attr.back().mate_curve_weights_ = mate_curve_weights;

    // Anil: Propagating the inlier view data from bmcsd_discrete_corresp_e to bmcsd_curve_3d_attributes_e
    attr.back().inlier_views_ = corresp[c].front().inliers_;

    // Anil: Propagating the edge support data for each reconstructed curve sample
    attr.back().edge_index_chain_ = corresp[c].front().index_chain_;

    // Anil: Propagating the edge support count vector from bmcsd_discrete_corresp_e to bmcsd_curve_3d_attributes_e
    //Also propagating the offset to convert hypothesis indices to image curve indices using subseqence set
    //std::cout << "SHIFT: " << sseq[s.v0()][c].ini() << "+" << ini_shift << std::endl;
    attr.back().imageCurveOffset_ = sseq[s.v0()][c].ini() + ini_shift;
    attr.back().imageCurveOffset_v1_ = sseq[s.v1()][corresp[c].front().id()].ini();
    attr.back().edgeSupportCount_ = corresp[c].front().support_count_;

    // Anil: Propagating the original image curve size in v0() and v1() that gave rise to this 3D curve
    attr.back().origCurveSize_ = s.get_original_curve_size(s.v0(),sseq[s.v0()][c].orig_id());
    attr.back().origCurveSize_v1_ = s.get_original_curve_size(s.v1(),sseq[s.v1()][corresp[c].front().id()].orig_id());

    // Anil: Propagating the original ID of the unbroken image curve from bmcsd_odt_curve_stereo_e to 
    //      bmcsd_curve_3d_attributes_e using bbld_subsequence_set
    attr.back().orig_id_v0_ = sseq[s.v0()][c].orig_id();
    attr.back().orig_id_v1_ = sseq[s.v1()][corresp[c].front().id()].orig_id();

    // Anil: Propagating the ID of the broken image curve (before pruning) from bmcsd_odt_curve_stereo_e to 
    //      bmcsd_curve_3d_attributes_e using bbld_subsequence_set
    attr.back().int_id_v0_ = sseq[s.v0()][c].int_id();
    attr.back().int_id_v1_ = sseq[s.v1()][corresp[c].front().id()].int_id();

    // Anil: Adding the certainty flags to the attributes_e
    attr.back().certaintyFlags_ = certaintyFlags;

    // Anil: Adding the vector of used curve IDs in v1
    attr.back().used_samples_v1_ = s.curve_samples_v1_;

    // XXX set inlier views here .
  }
  std::cout << "Done reconstructing curves\n";
  return true;
}

bool 
reconstruct_from_corresp_attr_using_mates(
    bmcsd_odt_curve_stereo_e &s, 
    const bmcsd_discrete_corresp_e &corresp,
    std::vector<bdifd_1st_order_curve_3d> *crv3d_ptr,
    std::vector< bmcsd_curve_3d_attributes_e > *attr_ptr
    )
{
  std::cout << "Reconstructing curves\n";

  std::vector<bdifd_1st_order_curve_3d> &crv3d = *crv3d_ptr;
  std::vector< bmcsd_curve_3d_attributes_e > &attr = *attr_ptr;
  unsigned const ncurves = s.num_curves(s.v0());
  assert (ncurves == corresp.size());
  crv3d.reserve(ncurves);
  attr.reserve(ncurves);

  bdifd_rig rig(s.cams(s.v0()).Pr_, s.cams(s.v1()).Pr_);
  unsigned c=0;

  for (std::list<bmcsd_attributed_object>::const_iterator m=corresp[c].begin(); m != corresp[c].end(); ++m) {  

    unsigned curView = m->container_id_;

    s.set_selected_crv_by_id(s.v0(), c);
    unsigned const ini_id = 0;
    unsigned const end_id = s.selected_crv(s.v0())->size()-1;
    s.set_subcurve(ini_id, end_id);
    s.compute_epipolar_beam_candidates_on_conf_views(curView);

    unsigned ic;
    bool found = s.get_index_of_candidate_curve(s.curves(curView, m->id()), &ic);
    //assert (found);
    if(!found)
      std::cout << "ERROR: Candidate index could not be located during reconstruction!" << std::endl;

    crv3d.resize(crv3d.size()+1);
    
    // Anil: Create the vector of flags that will mark the samples in the 3D curve that are reliable
    std::vector<bool> certaintyFlags;
    s.reconstruct_candidate_1st_order_with_flags_temp(ini_id, end_id, ic, c, rig, &crv3d.back(), certaintyFlags, curView);
    //s.reconstruct_candidate_1st_order(ini_id, end_id, ic, rig, &crv3d.back());

    attr.resize(attr.size()+1);
    attr.back().set_i0_i1(c, m->id());
    attr.back().total_support_ = static_cast<unsigned>(m->cost());

    // Anil: Propagating the edgel support data from bmcsd_discrete_corresp_e to bmcsd_curve_3d_attributes_e
    attr.back().supportingEdgelsPerConfView_ = m->supportingStructures_;

    // Anil: Propagating the inlier view data from bmcsd_discrete_corresp_e to bmcsd_curve_3d_attributes_e
    attr.back().inlier_views_ = m->inliers_;

    // Anil: Propagating the original ID of the unbroken image curve from bmcsd_odt_curve_stereo_e to 
    //      bmcsd_curve_3d_attributes_e using bbld_subsequence_set
    std::vector<bbld_subsequence_set> sseq = s.get_sseq();
    attr.back().orig_id_v0_ = sseq[s.v0()][c].orig_id();
    attr.back().orig_id_v1_ = sseq[s.v1()][corresp[c].front().id()].orig_id();

    // Anil: Propagating the ID of the broken image curve (before pruning) from bmcsd_odt_curve_stereo_e to 
    //      bmcsd_curve_3d_attributes_e using bbld_subsequence_set
    attr.back().int_id_v0_ = sseq[s.v0()][c].int_id();
    attr.back().int_id_v1_ = sseq[s.v1()][corresp[c].front().id()].int_id();

    // Anil: Adding the certainty flags to the attributes_e
    attr.back().certaintyFlags_ = certaintyFlags;

    // XXX set inlier views here .
  }
  std::cout << "Done reconstructing curves\n";
  return true;
}
