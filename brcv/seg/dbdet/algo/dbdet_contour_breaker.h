// This is dbdet_contour_breaker.h
#ifndef dbdet_contour_breaker_h
#define dbdet_contour_breaker_h

#include <edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_image_view.h>

#define y_params_1_size 2
#define y_params_1_vector vnl_vector_fixed<double, y_params_1_size>

#define y_params_0_size 9
#define y_params_0_vector vnl_vector_fixed<double, y_params_0_size>

#define y_hist_size 64

typedef vnl_vector_fixed<unsigned, y_hist_size> y_hist_vector;

//hackish solution to not use scoped enums and enforce c++11
namespace y_params_0 {

  enum my_y_params_0 {
    Y_ONE, Y_BG_GRAD, Y_SAT_GRAD, Y_HUE_GRAD, Y_ABS_K, Y_EDGE_SPARSITY, Y_WIGG, Y_GEOM, Y_TEXTURE
  };
}

class dbdet_contour_breaker {

public:
  dbdet_contour_breaker(
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const dbdet_edgemap &em,
    const vnl_matrix<unsigned> & tmap
    )
    :
    img_(img),
    em_(em),
    tmap_(tmap),
    ref_start_pts(img.ni(), img.nj()),
    ref_end_pts(img.ni(), img.nj())
  {
    assert(em.ncols() == img.ni() && em.nrows() == img.nj() && tmap.rows() == img.ni() && tmap.cols() == img.nj() && tmap.max_value() < y_hist_size);
    double diag = vcl_sqrt(ni() * ni() + nj() * nj());
    nbr_num_edges = vcl_max(static_cast<unsigned>((nbr_num_edges_ * diag / diag_of_train) + 0.5), nbr_len_th);
    diag_ratio = diag / diag_of_train;
  };

  void dbdet_contour_breaker_geom(dbdet_curve_fragment_graph & CFG, y_params_1_vector & beta, y_params_1_vector & fmean, dbdet_curve_fragment_graph newCFG);

  void dbdet_contour_breaker_semantic(dbdet_curve_fragment_graph & CFG, y_params_0_vector & beta, y_params_0_vector & fmean, dbdet_curve_fragment_graph newCFG);
  
  static double euclidean_length(const dbdet_edgel_chain &c) {
    double len=0;
    for (unsigned i=0; i+1 < c.edgels.size(); ++i)
      len += vgl_distance(c.edgels[i]->pt, c.edgels[i+1]->pt);
    return len;
  }

private:

  void compute_break_point(vcl_vector<dbdet_edgel_chain*> & frags, unsigned frag_id, vcl_vector<unsigned> & ids, vcl_set<unsigned> & unique_ids, bool front, vcl_vector<unsigned> & break_e_ids);

  void compute_merge_probability_geom(dbdet_edgel_chain & chain, unsigned nbr_range_th, y_params_1_vector & beta, y_params_1_vector & fmean, vcl_vector<double> & prob);

  void compute_merge_probability_semantic(dbdet_edgel_chain & chain, unsigned nbr_range_th, y_params_0_vector & beta, y_params_0_vector & fmean, vcl_vector<double> & prob);

  void compute_edge_sparsity_integral(dbdet_edgel_chain & chain, vcl_vector< vnl_vector_fixed<double, 2> > & n, unsigned nbr_width, vcl_vector<double> & edge_sparcity);

  void compute_texture_hist_integral(dbdet_edgel_chain & chain, vcl_vector< vnl_vector_fixed<double, 2> > & n, unsigned nbr_width, vcl_vector<y_hist_vector> & texton_hist_left, vcl_vector<y_hist_vector> & texton_hist_right);

  //Copy first level objects of CFG into newCFG
  void deep_copy_cfg(dbdet_curve_fragment_graph & CFG, dbdet_curve_fragment_graph & newCFG);

  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  const vnl_matrix<unsigned> tmap_;
  vbl_array_2d<bool> ref_start_pts;
  vbl_array_2d<bool> ref_end_pts;
  double nbr_num_edges;
  double diag_ratio;
  static double const diag_of_train; // ???
  static unsigned const nbr_num_edges_;  // # of edges close to connecting points
  static unsigned const max_it;
  static unsigned const nbr_len_th; // short curve under this length will be grouped due to geometry.
  static double const merge_th;
  static double const merge_th_geom;
  static double const epsilon;
};
#endif //dbdet_contour_breaker_h

