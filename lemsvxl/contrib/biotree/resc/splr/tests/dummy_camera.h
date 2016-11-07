#if !defined(DUMMY_CAMERA_H_)
#define DUMMY_CAMERA_H_

#include <bio_defs.h>

struct dummy_camera {
 orbit_index t_;
 dummy_camera(orbit_index t) : t_(t) {}
};

#endif
