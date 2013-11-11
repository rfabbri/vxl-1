#ifndef psm_mfa_optimizer_h_
#define psm_mfa_optimizer_h_

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>

#include <psm/psm_cell_id.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>
#include <psm/psm_scene.h>

template<psm_apm_type APM>
class psm_mfa_optimizer
{
public:
  psm_mfa_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_filenames, vcl_vector<vcl_string> const& camera_filenames, bool use_black_background = false);

  psm_mfa_optimizer(psm_scene<APM> &scene, vcl_vector<vil_image_view<typename psm_apm_traits<APM>::obs_datatype> > const& images, vcl_vector<vpgl_perspective_camera<double> > const& cameras, bool black_background = false) 
    : scene_(scene), images_(images), cameras_(cameras), black_background_(black_background) {}


  void optimize();

private:
  //: updates the cell and returns true if the cell parameters have converged
  bool update_cell(psm_cell_id const& cell_id, vcl_vector<typename psm_apm_traits<APM>::obs_datatype> const& obs_vector, vcl_vector<float> const& vis_vector, vcl_vector<float> const& mfa_pre_vector, vcl_vector<float> const& mfa_post_vector, vcl_vector<float> const& seg_len_vector, unsigned int n_valid_projections);

  psm_scene<APM> &scene_;
  vcl_vector<vil_image_view<typename psm_apm_traits<APM>::obs_datatype> >  images_;
  vcl_vector<vpgl_perspective_camera<double> >  cameras_;

  bool black_background_;

};


#endif

