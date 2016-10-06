#include <edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include "dbdet_curve_fragment_cues.h"

class dbdet_contour_breaker {

public:
  dbdet_contour_breaker(
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const dbdet_edgemap &em
    )
    :
    img_(img),
    em_(em) {};
dbdet_curve_fragment_graph dbdet_contour_breaker_geom(dbdet_curve_fragment_graph & CFG, y_feature_vector beta1, y_feature_vector fmean);

dbdet_curve_fragment_graph dbdet_contour_breaker_semantic(dbdet_curve_fragment_graph & CFG, y_feature_vector beta1, y_feature_vector fmean);

private:
  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  bool use_dt() const { return dt_ != NULL; }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  static double const diag_of_train = 578.275; // ???
  static unsigned const nbr_num_edges = 15;  // # of edges close to connecting points
  static unsigned const max_it = 2;
  static unsigned const nbr_len_th = 5; // short curve under this length will be grouped due to geometry.
  static double const merge_th = 0.2;
  static double const merge_th_geom = 0.5;
}


