// This is brcv/seg/dbbgm/algo/dbbgm_hmm_algo.h
#ifndef dbbgm_hmm_algo_h_
#define dbbgm_hmm_algo_h_

//:
// \file
// \brief Algorithms for Hidden Markov BG Modeling
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 03/16/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_view.h>

//: Apply transistion probabilities to the image of probablities
void dbbgm_update_probs(const vil_image_view<float>& probs,
                        const vil_image_view<float>& trans,
                        vil_image_view<float>& next_probs);

//: Normalize a probability field image
void dbbgm_normalize_probs(vil_image_view<float>& probs);


//: Apply a mean field approximation to adding a MRF to the probabilities
void dbbgm_mean_field(vil_image_view<float>& probs,
                      unsigned int k_rad=3, float alpha = 1.0f);

#endif // dbbgm_hmm_algo_h_
