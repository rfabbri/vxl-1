#ifndef psm_opt_gen_synthetic_scene_h_
#define psm_opt_gen_synthetic_scene_h_

#include <psm/psm_scene.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_apm_traits.h>
#include <vpgl/vpgl_perspective_camera.h>

void gen_synthetic_scene(psm_scene_base_sptr &scene, vcl_vector<vpgl_perspective_camera<double> > &cameras, vcl_vector<vil_image_view<float> > &images);


#endif

