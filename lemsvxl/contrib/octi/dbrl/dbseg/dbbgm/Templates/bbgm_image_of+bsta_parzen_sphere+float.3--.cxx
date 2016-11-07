#include <dbbgm/bbgm_image_of.hxx>
#ifdef DEVELINCLUDEPATH
#include <dbsta/bsta_parzen_sphere.hxx>
#include <dbsta/io/bsta_io_parzen_sphere.h>
#else
#include <bsta/bsta_parzen_sphere.hxx>
#include <bsta/io/bsta_io_parzen_sphere.h>
#endif
typedef bsta_parzen_sphere<float,3> parzen;

BBGM_IMAGE_INSTANTIATE(parzen);
