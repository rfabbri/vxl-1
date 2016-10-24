// This is dbdet_contour_breaker.h
#ifndef dbdet_contour_breaker_h
#define dbdet_contour_breaker_h

#include <edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <vcl_cassert.h>
#include "dbdet_curve_fragment_cues.h"

class dbdet_contour_breaker {

public:
  dbdet_contour_breaker(
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const dbdet_edgemap &em
    )
    :
    img_(img),
    em_(em),
    ref_start_pts(img.ni(), img.nj()),
    ref_end_pts(img.ni(), img.nj()),
  {
    assert(em.ncols() == img.ni() && em.nrows() == img.nj());
    double diag = vcl_sqrt(ni() * ni() + nj() * nj());
    nbr_num_edges = vcl_max(static_cast<unsigned>((nbr_num_edges_ * diag / diag_of_train) + 0.5), nbr_len_th);
    diag_ratio = diag / diag_of_train;
  };
  dbdet_curve_fragment_graph dbdet_contour_breaker_geom(dbdet_curve_fragment_graph & CFG, double[2] beta1, double[2] fmean);

  dbdet_curve_fragment_graph dbdet_contour_breaker_semantic(dbdet_curve_fragment_graph & CFG, double[2] beta1, double[2] fmean);

private:

  void compute_break_point(dbdet_edgel_chain & chain, unsigned frag_id, vcl_vector<unsigned> ids, vcl_set<unsigned> unique_ids, bool front, vcl_vector<unisgned> break_ids);

  void compute_merge_probability_geom(dbdet_edgel_chain & chain, unsigned nbr_range_th, double[2] beta1, double[2] fmean, vcl_vector<double> & prob);

  void compute_merge_probability_semantic(dbdet_edgel_chain & chain,/*hsv_img, edge_map, tmap,*/ nbr_range_th unsigned nbr_range_th, yuliang_features beta1, yuliang_features fmean, vcl_vector<double> & prob);

  void compute_edge_sparcity_integral(dbdet_edgel_chain & chain, vcl_vector< vnl_vector_fixed<double, 2> > n, unsigned nbr_width, vcl_vector<double> & edge_sparcity);

  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  bool use_dt() const { return dt_ != NULL; }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  vbl_array_2d<bool> ref_start_pts;
  vbl_array_2d<bool> ref_end_pts;
  double nbr_num_edges;
  double diag_ratio;
  static double const diag_of_train = 578.275; // ???
  static unsigned const nbr_num_edges_ = 15;  // # of edges close to connecting points
  static unsigned const max_it = 2;
  static unsigned const nbr_len_th = 5; // short curve under this length will be grouped due to geometry.
  static double const merge_th = 0.2;
  static double const merge_th_geom = 0.5;
  static double const epsilon = 1e-10;
}
#endif dbdet_contour_breaker_h

