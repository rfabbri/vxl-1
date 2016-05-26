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

void
dbdet_curve_fragment_cues(
    const dbdet_edgel_chain &c,
    const vil_image_view<rgbP >&hsv,
    vnl_vector<double> *features //< indexed by the enum
    )
{
  std::cout << "cues stub" << std::endl;
}

#endif // dbdet_curve_fragment_cues_h
