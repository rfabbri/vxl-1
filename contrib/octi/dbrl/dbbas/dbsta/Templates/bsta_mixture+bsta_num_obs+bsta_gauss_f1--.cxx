#include <dbsta/bsta_mixture.txx>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_attributes.h>
typedef bsta_num_obs<bsta_gauss_f1> gauss1;
BSTA_MIXTURE_INSTANTIATE(gauss1);