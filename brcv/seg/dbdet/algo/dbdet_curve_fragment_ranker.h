// This is dbdet_curve_fragment_ranker.h
#ifndef dbdet_curve_fragment_ranker_h
#define dbdet_curve_fragment_ranker_h
//:
//\file
//\brief Ranking of edge curve fragments per tained distributions
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
// Original author: Yuliang Guo (matlab)
//\date 05/26/2016 10:51:10 BRT
//
#include <dbdet_curve_fragment_cues.h>

//class dbdet_curve_fragment_ranker {
//public:
//  dbdet_curve_fragment_ranker():
//    param_set(false)
//  {
//  }

//  set_beta
//  set_texton_params

//  void rank(
//      const dbdet_edgel_chain_list &frags,
//      const vil_image_view<rgb> &img
//      )
//  {
//    assert(param_set);
//  }
//private:
//  bool param_set;
//};

//
// Rank curve fragments using trained logistic regression classifier.
//
void
dbdet_curve_fragment_ranker(
    const dbdet_edgel_chain_list &frags,
    const vil_image_view<rgb> &img
    const y_trained_parameters &beta,
//    const texton_data &txdata,
    vnl_vector<double> *rank
    )
{

  // rgb2hsv

  // for each curve fragment i
  //    dbdet_curve_fragment_cues
  //        rank[i] = 1 / (1 + exp(-([1, bg_grad, sat_grad, hue_grad, abs_k, edge_sparsity, wigg, len]-fmean_2)*beta_2'));

}

#endif // dbdet_curve_fragment_ranker_h
