// This is dbdet_graphical_model_contour_merge.h
#ifndef dbdet_graphical_model_contour_merge_h
#define dbdet_graphical_model_contour_merge_h

#include "dbdet_yuliang_features.h"
#include <vnl/vnl_vector.h>

class dbdet_graphical_model_contour_merge {

public:

  dbdet_graphical_model_contour_merge(
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const dbdet_edgemap &em,
    const vnl_matrix<unsigned> & tmap
    )
    :
    img_(img),
    em_(em),
    tmap_(tmap),
    cues_computer(img, em)
  {
    assert(em.ncols() == img.ni() && em.nrows() == img.nj() && tmap.rows() == img.ni() && tmap.cols() == img.nj());
    double diag = vcl_sqrt(ni() * ni() + nj() * nj());
    nbr_num_edges = vcl_max(static_cast<unsigned>((dbdet_yuliang_const::nbr_num_edges * diag / dbdet_yuliang_const::diag_of_train) + 0.5), dbdet_yuliang_const::nbr_len_th);
    temp_n.reserve(nbr_num_edges);
  }

  void dbdet_merge_contour(dbdet_curve_fragment_graph & CFG, y_params_1_vector & beta1, y_params_1_vector & fmean1, y_params_0_vector & beta0, y_params_0_vector & fmean0);
private:

  void dbdet_degree_2_node_cues(dbdet_edgel_chain & c1, dbdet_edgel_chain & c2, double & geom_diff, double & tex_diff);

  void dbdet_merge_at_degree_2_node(dbdet_factor_graph G, dbdet_curve_fragment_graph & CFG, v, actual_edge, nbrs_fac, c1_ids, c2_ids);

  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  const vnl_matrix<unsigned> tmap_;
  unsigned nbr_num_edges;
  dbdet_curve_fragment_cues cues_computer;
  vcl_vector<vnl_vector_fixed<double, 2> > temp_n;
};
#endif //dbdet_graphical_model_contour_merge_h
