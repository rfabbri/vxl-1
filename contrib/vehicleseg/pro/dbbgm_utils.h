#ifndef _dbbgm_utils_h_
#define _dbbgm_utils_h_

#include<vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
class dbbgm_utils{
private:
    dbbgm_utils();

public:
 void static mask_nonoverlapping_region(vil_image_view<float> im,vgl_point_2d<double> p,
                                        vgl_vector_2d<double> u,vgl_vector_2d<double> v);

};

#endif
