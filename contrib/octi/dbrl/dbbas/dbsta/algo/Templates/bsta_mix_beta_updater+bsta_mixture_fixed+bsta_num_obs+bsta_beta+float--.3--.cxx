#include <dbsta/algo/bsta_beta_updater.txx>
#include <dbsta/bsta_mixture_fixed.h>
#include <dbsta/bsta_beta.h>

typedef bsta_num_obs<bsta_beta<float> > beta;
typedef bsta_mixture_fixed<beta,3> mix_beta;

BSTA_MIX_BETA_UPDATER_INSTANTIATE(mix_beta);
