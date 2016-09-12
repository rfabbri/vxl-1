#include <dbbgm/bbgm_image_of.hxx>
#ifdef DEVELINCLUDEPATH
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gaussian_indep.hxx>
#include <dbsta/io/bsta_io_attributes.h>
#include <dbsta/io/bsta_io_gaussian_indep.h>
#else
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_indep.hxx>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_gaussian_indep.h>
#endif
typedef bsta_num_obs<bsta_gaussian_indep<float,3> > gauss_type;
BBGM_IMAGE_INSTANTIATE(gauss_type);
