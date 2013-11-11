#include <dbbgm/bbgm_wavelet.txx>
#include <vil/vil_image_view.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_gauss_ff3.h>
#include <dbsta/io/bsta_io_attributes.h>
#include <dbsta/io/bsta_io_mixture.h>
#include <dbsta/io/bsta_io_gaussian_full.h>
#include <dbbgm/bbgm_io_wavelet.h>
#include <dbbgm/bbgm_image_of.h>
typedef bsta_gauss_ff3 bsta_gauss_f;
typedef bsta_num_obs<bsta_gauss_f> gauss_full3;
typedef bsta_mixture<gauss_full3> mix_gauss_full3;
typedef bsta_num_obs<mix_gauss_full3> obs_mix_gauss_full3;
typedef bbgm_image_of<obs_mix_gauss_full3> img_obs_gauss_full3;

DBBGM_WAVELET_INSTANTIATE(img_obs_gauss_full3);

