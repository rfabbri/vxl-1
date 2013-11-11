#include <psm/psm_apm_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_cell_id.h>

#include <hsds/hsds_fd_tree.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vpgl/vpgl_camera.h>

#include <vcl_vector.h>
#include <vcl_queue.h>
#include <vcl_set.h>

#include "psm_mfa_compute_factor_properties.h"
#include "psm_mfa_optimizer.h"

#include <psm/algo/psm_render_expected.h>


template<psm_apm_type APM>
psm_mfa_optimizer<APM>::psm_mfa_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_filenames, vcl_vector<vcl_string> const& camera_filenames, bool black_background)
: scene_(scene), black_background_(black_background)
{
  // read images and cameras
  vcl_vector<vcl_string>::const_iterator img_fname_it = image_filenames.begin();
  vcl_vector<vcl_string>::const_iterator cam_fname_it = camera_filenames.begin();
  for (; img_fname_it != image_filenames.end(); ++img_fname_it, ++cam_fname_it) {
    // load image
    vcl_cout << "loading image " << *img_fname_it << vcl_endl;
    vil_image_view_base_sptr img_base = vil_load(img_fname_it->c_str());
    vil_image_view_base_sptr img_base_conv = vil_convert_stretch_range(psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype(),img_base);

    vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype>*>(img_base_conv.ptr());
    
    //vil_image_view<psm_apm_traits<APM>::obs_datatype> *img = dynamic_cast<vil_image_view<psm_apm_traits<APM>::obs_datatype>*>(img_base.as_pointer());
    images_.push_back(*img);
    // load corresponding camera
    vcl_cout << "reading camera " << *cam_fname_it << vcl_endl;
    vcl_ifstream ifs(cam_fname_it->c_str());
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << *cam_fname_it << vcl_endl;
      return;
    }
    vpgl_perspective_camera<double> cam;
    ifs >> cam;
    cameras_.push_back(cam);

  }
}


template<psm_apm_type APM>
void psm_mfa_optimizer<APM>::optimize()
{
  vcl_set<psm_cell_id> to_process_set;
  vcl_queue<psm_cell_id> to_process_queue;

  // add each cell to list initially
  typename psm_scene<APM>::block_index_iterator bit = scene_.block_index_begin();
  for (; bit != scene_.block_index_end(); ++bit) {
    hsds_fd_tree<psm_sample<APM>,3> &block = scene_.get_block(*bit);
    typename hsds_fd_tree<psm_sample<APM>,3>::const_iterator cell_it = block.begin();
    for (; cell_it != block.end(); ++cell_it) {
      psm_cell_id id(*bit, cell_it->first);
      to_process_set.insert(id);
      to_process_queue.push(id);
    }
  }

  // while still some cells left to process
  unsigned int it_counter = 0;
  const unsigned int debug_out = 32*32;
  while(!to_process_queue.empty()) {
    ++it_counter;
    vcl_cout << "i:" << it_counter << " s:" << to_process_queue.size() << "    ";
    if (!(it_counter % debug_out)) {
      vcl_cout << vcl_endl << "writing debug files" << vcl_endl;
      // debug write scene
      char raw_fname_str[64];
      sprintf(raw_fname_str,"c:/research/psm/output/mfa_out_%03d.raw", it_counter / debug_out);
      scene_.save_alpha_raw(vcl_string(raw_fname_str), vgl_point_3d<int>(0,0,0), 6);

      char img_fname_str[64];
      sprintf(img_fname_str,"c:/research/psm/output/mfa_expected_%03d.tif", it_counter / debug_out);
      vil_image_view<float> expected_img(images_[0].ni(),images_[0].nj());
      vil_image_view<float> mask(expected_img.ni(), expected_img.nj());
      psm_render_expected(scene_, &cameras_[0], expected_img, mask, true);
      vil_image_view<vxl_byte> expected_img_byte(expected_img.ni(),expected_img.nj());
      vil_convert_stretch_range_limited(expected_img, expected_img_byte,0.0f,1.0f);

      vil_save(expected_img_byte,img_fname_str);

    }

    // retrive next cell for processesing
    psm_cell_id cell_id = to_process_queue.front();
    to_process_queue.pop();
    vcl_set<psm_cell_id>::iterator set_it = to_process_set.find(cell_id);
    if  (set_it == to_process_set.end()) {
      vcl_cerr << "error: cell_id at front of queue is not in set" << vcl_endl;
      break;
    }
    to_process_set.erase(set_it);

    // for each image
    vcl_vector<vil_image_view<psm_apm_traits<APM>::obs_datatype> >::const_iterator img_it = images_.begin();
    vcl_vector<vpgl_perspective_camera<double> >::const_iterator cam_it = cameras_.begin();

    vcl_vector<typename psm_apm_traits<APM>::obs_datatype> obs_vector;
    vcl_vector<float> vis_vector;
    vcl_vector<float> mfa_pre_vector;
    vcl_vector<float> mfa_post_vector;
    vcl_vector<float> seg_len_vector;

    vcl_set<psm_cell_id> markov_blanket;

    unsigned int n_valid_projections = 0;

    for (; img_it != images_.end(); ++img_it, ++cam_it) {
 
      bool projection_valid = psm_mfa_compute_factor_properties<APM>(scene_, *cam_it, *img_it, cell_id, obs_vector, vis_vector, mfa_pre_vector, mfa_post_vector, seg_len_vector, markov_blanket, black_background_);

      if (projection_valid) {
        ++n_valid_projections;
      }
    }

    bool converged = update_cell(cell_id, obs_vector, vis_vector, mfa_pre_vector, mfa_post_vector, seg_len_vector, n_valid_projections);
    if (!converged) {
      // add all members of markov blanket into queue for further processing
      vcl_set<psm_cell_id>::const_iterator add_it = markov_blanket.begin();
      for (; add_it != markov_blanket.end(); ++add_it) {
        // make sure cell is not already in queue
        if (to_process_set.find(*add_it) == to_process_set.end()) {
          to_process_set.insert(*add_it);
          to_process_queue.push(*add_it);
        }
      }
    }
  }
}

//: compute 
template<psm_apm_type APM>
bool psm_mfa_optimizer<APM>::update_cell(psm_cell_id const& cell_id, vcl_vector<typename psm_apm_traits<APM>::obs_datatype> const& obs_vector, vcl_vector<float> const& vis_vector, vcl_vector<float> const& mfa_pre_vector, vcl_vector<float> const& mfa_post_vector, vcl_vector<float> const& seg_len_vector, unsigned int n_valid_projections)
{
  psm_sample<APM> &cell_value = scene_.get_cell(cell_id);

  if (n_valid_projections < 2) {
    cell_value.alpha = 0;
    return true;
  }

  // observation weights for appearance computation
  vcl_vector<float> obs_weight_vector;

  // compute factor value assuming all rays pass through cell
  double A_empty = 0, A_full = 0;
  double mean_seg_len = 0;
  vcl_vector<typename psm_apm_traits<APM>::obs_datatype>::const_iterator obs_it = obs_vector.begin();
  vcl_vector<float>::const_iterator vis_it = vis_vector.begin();
  vcl_vector<float>::const_iterator pre_it = mfa_pre_vector.begin();
  vcl_vector<float>::const_iterator post_it = mfa_post_vector.begin();
  vcl_vector<float>::const_iterator seg_len_it = seg_len_vector.begin();
  for (; obs_it != obs_vector.end(); ++obs_it, ++vis_it, ++pre_it, ++post_it, ++seg_len_it) {
    float PI = psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, *obs_it);
    double mfa_PI = vcl_log(PI + 0.001);
    A_empty += *pre_it + *vis_it * *post_it;
    A_full += *pre_it + *vis_it * mfa_PI;
    obs_weight_vector.push_back(*vis_it * (1.0f - (float)vcl_exp(-cell_value.alpha * *seg_len_it)));
    mean_seg_len += *seg_len_it;
  }
  mean_seg_len /= obs_vector.size();

  double Q_empty = vcl_exp(A_empty);
  double Q_full = vcl_exp(A_full);

  double Q_new = Q_full / (Q_empty + Q_full);

  float alpha_new = (float)(-vcl_log(1.0 - Q_new)/mean_seg_len);
  // sanity check on new alpha value
  const float max_alpha = 1e6;
  if (!(alpha_new < max_alpha) ) {
    vcl_cout << vcl_endl << "1)Q_full = " << Q_full << " Q_empty = " << Q_empty << " alpha_new = " << alpha_new << vcl_endl;
    alpha_new = max_alpha;
  }
  if (!(alpha_new > 0)){
    vcl_cout << vcl_endl << "2)Q_full = " << Q_full << " Q_empty = " << Q_empty << " alpha_new = " << alpha_new << vcl_endl;
    alpha_new = 0;
  }

  // compute new appearance
  const float min_sigma = 0.04f;
  //psm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, obs_weight_vector, cell_value.appearance, min_sigma);

  // check for convergence
  double Q_old = 1.0 - vcl_exp(-cell_value.alpha * mean_seg_len);

  // update to new alpha value
  cell_value.alpha = alpha_new;

  const double tol = 1e-3;
  if (vcl_abs(Q_new - Q_old) <= tol) {
    // value has not changed
    return true;
  }
  return false;
}





#define PSM_MFA_OPTIMIZER_INSTANTIATE(T) \
  template class psm_mfa_optimizer<T>



