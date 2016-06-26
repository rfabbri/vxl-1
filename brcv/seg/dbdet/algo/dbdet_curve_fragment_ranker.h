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
#include <dbdet/algo/dbdet_curve_fragment_cues.h>
#include <vnl/vnl_matrix_fixed.h>

// TODO Possibly class it
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

#define Y_PARAMS_NCOLS 9
#define Y_PARAMS_NROWS 3

typedef vnl_matrix_fixed<double, Y_PARAMS_NROWS, Y_PARAMS_NCOLS> y_trained_parameters;

void
dbdet_curve_fragment_ranker(
    const dbdet_edgel_chain_list &frags,
    const dbdet_edgemap edgemap,
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const y_trained_parameters &beta,
//    const texton_data &txdata,
    vnl_vector<double> *rank_ptr
    )
{
  vnl_vector<double> &rank = *rank_ptr;
  y_feature_vector fv;

  dbdet_curve_fragment_cues cues(img, edgemap);

  int nfrags = frags.size(); // list: expensive
  rank.set_size(nfrags);
  unsigned i = 0;
  vnl_vector<double> fmean2 = beta.get_row(1);
  vnl_vector<double> beta_2 = beta.get_row(2);
  for (dbdet_edgel_chain_list_const_iter it=frags.begin(); it != frags.end(); it++, i++) {
    cues.compute_all_cues(*(*it), &fv);
    // rank[i] =  1 / (1 + exp(-([1, bg_grad, sat_grad, hue_grad, abs_k, edge_sparsity, wigg, len]-fmean_2)*beta_2'));
    rank[i] = 0;
    for (unsigned f=0; f < Y_NUM_FEATURES; ++f)
    {
      rank[i] += (fmean2[f] - fv[f]) * beta_2[f];
    }   
    rank[i] = 1.0 / (1.0 + exp(rank[i]));
  }
}

#endif // dbdet_curve_fragment_ranker_h
