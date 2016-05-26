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

void
dbdet_curve_fragment_ranker(
    const dbdet_edgel_chain_list &frags,
    const vil_image_view<rgb> &img
    beta,
    textons_params,
    vnl_vector<double> *rank
    )
{
}

#endif // dbdet_curve_fragment_ranker_h
