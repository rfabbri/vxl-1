// This is dbdet_curve_fragment_cues.h
#ifndef dbdet_curve_fragment_cues_h
#define dbdet_curve_fragment_cues_h
//:
//\file
//\brief Compute several geometric and appearance cues of a curve fragment
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/25/2016 21:57:40 BRT
//


// just to make it easier and more descriptive to index the feature vector
// rarely used since we just process the feature vec. generically
enum yuliang_features {
  Y_BG_GRAD, Y_SAT_GRAD, Y_HUE_GRAD, Y_ABS_K, Y_EDGE_SPARSITY, Y_WIGG, Y_LEN
};

#define Y_NUM_FEATURES 6

typedef vnl_vector_fixed<double, NUM_FEATURES> y_feature_vector;

void
dbdet_curve_fragment_cues(
    const dbdet_edgel_chain &c,
    const vil_image_view<rgbP >&hsv,
    y_feature_vector *features_ptr // indexed by the enum
    )
{
  static unsigned local_dist = 1; // distance used for local sampling
  y_feature_vector &features = *features_ptr;

  // curvature
  vnl_vector<double> k;
  dbgl_compute_curvature(c, &k);

  for (unsigned i=0; i < k.size(); ++i)
    if (vnl_math::isnan(k[i]))
      k[i] = 0;

  { // wiggliness is the number of times curvature changes sign
    features[Y_WIGG] = 0;
    for (unsigned i=0; i + 1 < k.size(); ++i) {
      features[Y_WIGG] += ( (k[i+1] >= 0) == (k[i] >= 0) );
    }
    features[Y_WIGG] /= k.size();
  }

  features[Y_ABS_K] = k.one_norm() / k.size();

  { // HSV gradients
    // examine local_dist neighborhood around curve along normals
    vcl_vector< vnl_vector_fixed<double, 2> > n;
    n.reserve(k.size());
    dbgl_compute_normals(c, &n);

    // get neighborhood points to be examined
    

    features[Y_HUE_GRAD] = 0;
    features[Y_SAT_GRAD] = 0;
    features[Y_BG_GRAD]  = 0;

    for (unsigned i=0; i < c.size(); ++i) {
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
    features[Y_HUE_GRAD] /= c.size();
    features[Y_SAT_GRAD] = c.size();
    features[Y_BG_GRAD]  = c.size();
  }
}

#endif // dbdet_curve_fragment_cues_h
