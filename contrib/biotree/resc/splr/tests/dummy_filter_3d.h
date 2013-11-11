#if !defined(DUMMY_FILTER_3D_H_)
#define DUMMY_FILTER_3D_H_

#include <bio_defs.h>
#include <splr/tests/dummy_camera.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <splr/tests/dummy_filter_2d.h>

struct dummy_filter_3d {
dummy_filter_2d splat(const dummy_camera & camera, const worldpt& pt){
   dummy_filter_2d filter_2d(pt.x());
   return filter_2d;
 }
dummy_filter_2d splat(const xmvg_perspective_camera<double> & camera, const worldpt& pt){
   dummy_filter_2d filter_2d(pt.x());
   return filter_2d;
 }
};

#endif
