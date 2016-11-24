// This is dbdet_graphical_model_contour_merge.h
#ifndef dbdet_graphical_model_contour_merge_h
#define dbdet_graphical_model_contour_merge_h

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
    tmap_(tmap)
  {
    assert(em.ncols() == img.ni() && em.nrows() == img.nj() && tmap.rows() == img.ni() && tmap.cols() == img.nj());
  }

  void dbdet_merge_contour(dbdet_curve_fragment_graph & CFG, y_params_1_vector & beta, y_params_1_vector & fmean, y_params_0_vector & beta, y_params_0_vector & fmean);
private:

  void dbdet_degree_2_node_cues(dbdet_edgel_chain & c1, dbdet_edgel_chain & c2/*, tmap, nbr_num_edges*/);

  void merge_at_degree_2_node(dbdet_factor_graph G, dbdet_curve_fragment_graph & newCFG, v, actual_edge, nbrs_fac, c1_ids, c2_ids);

  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  const vnl_matrix<unsigned> tmap_;
};
#endif //dbdet_graphical_model_contour_merge_h
