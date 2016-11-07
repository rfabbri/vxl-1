#if !defined(DUMMY_SCAN_H_)
#define DUMMY_SCAN_H_

#include <bio_defs.h>
#include <splr/tests/dummy_camera.h>
struct dummy_scan {
  dummy_camera operator()(orbit_index t){
    dummy_camera camera(t);
    return camera;
  }
  dummy_scan(int scan_size) : scan_size_(scan_size) {}
  unsigned int scan_size() const {return scan_size_;}
  int scan_size_;
};

#endif
