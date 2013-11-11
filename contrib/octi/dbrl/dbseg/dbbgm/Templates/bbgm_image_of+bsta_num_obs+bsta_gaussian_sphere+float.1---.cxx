#include <dbbgm/bbgm_image_of.txx>
#ifdef DEVELINCLUDEPATH
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gaussian_sphere.txx>
#include <dbsta/io/bsta_io_attributes.h>
#include <dbsta/io/bsta_io_gaussian_sphere.h>
#else
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_sphere.txx>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#endif
typedef bsta_num_obs<bsta_gaussian_sphere<float,1> > gauss_type;
BBGM_IMAGE_INSTANTIATE(gauss_type);
