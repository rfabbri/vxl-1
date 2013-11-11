#if !defined(BIOPROC_TESTS_MODIFIED_NO_NOISE_FILTER_3D_H_)
#define BIOPROC_TESTS_MODIFIED_NO_NOISE_FILTER_3D_H_

#include <xmvg/xmvg_no_noise_filter_3d.h>

/* This class is identifical to xmvg_no_noise_filter_3d except that the bounding box
    for splats should be independent of the filter orientation
*/


class bioproc_tests_modified_no_noise_filter_3d : public  xmvg_no_noise_filter_3d {
 public:
  bioproc_tests_modified_no_noise_filter_3d(xmvg_no_noise_filter_descriptor& descriptor);

 protected:
  vgl_box_2d<int> get_box_2d(xmvg_perspective_camera<double>  cam,
                                                       vgl_box_3d<double> const & box);
};
   

#endif
