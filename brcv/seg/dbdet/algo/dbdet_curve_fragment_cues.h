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
    );

static const vxl_uint_32 dbdet_curve_fragment_cues_unvisited vcl_numeric_limits<vxl_uint_32>::max()
//: Compute curve fragment cues for many curves.
// Holds state information such as distance transform and auxiliary buffers,
// so that computation os fast for many curve fragments in the same image.
class dbdet_curve_fragment_cues {
public:

  // Make sure the input parameters stay valid
  // while this class is in use.
  void dbdet_curve_fragment_cues(
    const vil_image_view<rgbP >&hsv,
    const vil_image_view<vxl_uint_32> &dt,
    const dbdet_edgemap &em
    )
    :
    visited_(hsv.ni(), hsv.nj(), 1)
    hsv_ = hsv,
    dt_ = dt
    em_ = em
  {
    visited_.fill(dbdet_curve_fragment_cues_unvisited);
  }

  void
  compute_cues(
      const dbdet_edgel_chain &c, 
      y_feature_vector *features_ptr // indexed by the enum
      );

private:
    const vil_image_view<rgbP >&hsv_;
    const vil_image_view<vxl_uint_32> &dt_;
    // visited(i,j) = c marks pixels(i,j) as visited by curve c's nhood tube
    // visited(i,j) = UIHNT_MAX marks pixels(i,j) as not visited
    vil_image_view<vxl_uint_32> visited_;
    const dbdet_edgemap &em_;
};


#endif // dbdet_curve_fragment_cues_h
