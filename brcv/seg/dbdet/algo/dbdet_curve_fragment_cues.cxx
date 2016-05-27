#include <dbdet_curve_fragment_cues.h>

void
dbdet_curve_fragment_cues(
    const dbdet_edgel_chain &c,
    const vil_image_view<rgbP >&hsv,
    const vil_image_view<vxl_uint_32> &dt,
    const dbdet_edgemap &em,
    y_feature_vector *features_ptr // indexed by the enum
    )
{
  static unsigned const local_dist = 1; // distance used for local sampling
  static unsigned const nbr_width = 3;  // distance used for lateral edge sparsity
  unsigned const npts = c.edgels.size();
  const dbdet_edgel_list e = &c.edgels;

  y_feature_vector &features = *features_ptr;

  // curvature
  vnl_vector<double> k;
  dbgl_compute_curvature(c, &k);
  assert(k.size() == npts);

  for (unsigned i=0; i < npts; ++i)
    if (vnl_math::isnan(k[i]))
      k[i] = 0;

  { // wiggliness is the number of times curvature changes sign
    features[Y_WIGG] = 0;
    for (unsigned i=0; i + 1 < npts; ++i) {
      features[Y_WIGG] += ( (k[i+1] >= 0) == (k[i] >= 0) );
    }
    features[Y_WIGG] /= npts;
  }

  features[Y_ABS_K] = k.one_norm() / npts;

  { // HSV gradients
    // examine local_dist neighborhood around curve along normals
    vcl_vector< vnl_vector_fixed<double, 2> > n;
    n.reserve(npts);
    dbgl_compute_normals(c, &n);

    // get neighborhood points to be examined
    

    features[Y_HUE_GRAD] = 0;
    features[Y_SAT_GRAD] = 0;
    features[Y_BG_GRAD]  = 0;

    for (unsigned i=0; i < size; ++i) {
      left_x  = c[i].x() - local_dist * n[i][0];
      left_y  = c[i].y() - local_dist * n[i][1];
      right_x = c[i].x() + local_dist * n[i][0];
      right_y = c[i].y() + local_dist * n[i][1];

      // make sure image clamps to within bounds
      vil_border_accessor<vil_image_view<rgbP> >
        hsv_clamped = vil_border_create_accessor(hsv,vil_border_create_geodesic(hsv));

      // TODO test if imae indexing is (x,y) or (y,x)
      features[Y_HUE_GRAD] += vnl::abs(hsv_clamped(left_x,left_y) - hsv_clamped(right_x,right_y));
      features[Y_SAT_GRAD] += vnl::abs(hsv_clamped(left_x,left_y) - hsv_clamped(right_x,right_y));
      features[Y_BG_GRAD] += vnl::abs(hsv_clamped(left_x,left_y) - hsv_clamped(right_x,right_y));
    }
    features[Y_HUE_GRAD] /= npts;
    features[Y_SAT_GRAD] /= npts;
    features[Y_BG_GRAD]  /= npts;
  }

  { // lateral edge sparsity
    unsigned total_edges = 0;

    for (unsigned i=0; i < npts; ++i) {
      // locate bucket
      unsigned p_i = static_cast<unsigned>(e[i]->pt.x()+0.5);
      unsigned p_j = static_cast<unsigned>(e[i]->pt.y()+0.5);
      
      if (dt(p_i, p_j) > nbr_width)
        continue;

      // visit all nearby edgels and count the number within a distance
      // mark already visited edgels

      // outside indices return false (visited)
      vil_border_accessor<vil_image_view<bool> >
        unvisited = vil_border_create_accessor(unvisited_img,vil_border_create_constant(unvisited_img, false));

      // TODO:optimize access to be row-first
      for (int d_i = -nbr_width; di < nbr_width; ++d_i) {
        for (int d_j = -nbr_width; dj < nbr_width; ++d_j) {
          if (unvisited(p_i + d_i, p_j + d_j)) {
            unsigned nh_x = static_cast<unsigned>(p_i + d_i);
            unsigned nh_y = static_cast<unsigned>(p_j + d_j);
//            total_edges += em.edge_cells.begin()[l].size();
            total_edges += em.cell(nh_x,nh_y).size();
            mark_visited(nh_x,nh_y)
          }
        }
      }
    }
  }
}
