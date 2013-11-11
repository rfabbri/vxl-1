#ifndef psm_opt_rt_bayesian_optimizer_txx_
#define psm_opt_rt_bayesian_optimizer_txx_

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_set.h>
#include <vcl_map.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_incremental_reader.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene.h>

#include "psm_opt_rt_sample.h"
#include "psm_opt_rt_bayesian_optimizer.h"
#include "psm_opt_appearance_estimator.h"

template <psm_apm_type APM, psm_aux_type AUX>
psm_opt_rt_bayesian_optimizer<APM,AUX>::psm_opt_rt_bayesian_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_ids)
: scene_(scene), image_ids_(image_ids), min_cell_P_(0.0001f), max_cell_P_(0.995f)
{}


template<psm_apm_type APM, psm_aux_type AUX>
bool psm_opt_rt_bayesian_optimizer<APM,AUX>::optimize_cells(double damping_factor)
{
  unsigned int c = 0;
  const unsigned int debug_c = 1509732;
  // get auxillary scenes associated with each imput image
  vcl_vector<psm_aux_scene_base_sptr> aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    psm_aux_scene_base_sptr aux_scene_base = scene_.template get_aux_scene<AUX>(image_ids_[i]);
    aux_scenes.push_back(aux_scene_base);
  }

  vcl_vector<psm_opt_rt_sample<typename psm_apm_traits<APM>::obs_datatype> > aux_samples;

  // for each block
  typename psm_scene<APM>::block_index_iterator block_it = scene_.block_index_begin();
  for (; block_it != scene_.block_index_end(); ++block_it) {
    hsds_fd_tree<psm_sample<APM>,3> &block = scene_.get_block(*block_it);
    // get a vector of incremental readers for each aux scene.
    vcl_vector<hsds_fd_tree_incremental_reader<typename psm_aux_traits<AUX>::sample_datatype,3>*> aux_readers(aux_scenes.size());
    for(unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = new hsds_fd_tree_incremental_reader<typename psm_aux_traits<AUX>::sample_datatype,3>();
      psm_aux_scene<AUX> *aux_scene_ptr = static_cast<psm_aux_scene<AUX>*>(aux_scenes[i].ptr());
      aux_scene_ptr->get_block_incremental(*block_it, *aux_readers[i]);
    }

    typename hsds_fd_tree<psm_sample<APM>,3>::iterator cell_it = block.begin();
    // for each cell
    for (; cell_it != block.end(); ++cell_it, ++c) {
      // get auxillary cells
      aux_samples.clear();
      for (unsigned int i=0; i<aux_readers.size(); ++i) {
        psm_opt_rt_sample<typename psm_apm_traits<APM>::obs_datatype> aux_cell;
        hsds_fd_tree_node_index<3> aux_idx;
        if (!aux_readers[i]->next(aux_idx, aux_cell)) {
          vcl_cerr << "error: incremental reader returned false." << vcl_endl;
          return false;
        }
        if (aux_idx != cell_it->first) {
          vcl_cerr << "error: aux_cell idx does not match cell idx." << vcl_endl;
          return false;
        }
        if (aux_cell.seg_len_ > 0.0f) {
          aux_samples.push_back(aux_cell);
        }
      }

      psm_sample<APM> &cell = cell_it->second;
      vcl_vector<float> pre_vector(aux_samples.size());
      vcl_vector<float> vis_vector(aux_samples.size());
      //vcl_vector<float> post_prob_vector(aux_samples.size());
      vcl_vector<typename psm_apm_traits<APM>::obs_datatype> obs_vector(aux_samples.size());
      //#define OLD_ALPHA_UPDATE
#ifdef OLD_ALPHA_UPDATE
      double alpha_numerator = 0.0;
      double alpha_denominator = 0.0;
#else
      unsigned int n_valid_obs = 0;
      double log_alpha_sum = 0.0;
#endif
      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        float seg_len = aux_samples[s].seg_len_;
        if (seg_len > 1e-5) {
#ifdef OLD_ALPHA_UPDATE
          alpha_numerator += aux_samples[s].updated_alpha_sum_;
          alpha_denominator += aux_samples[s].weighted_seg_len_;
#else
          if (aux_samples[s].weighted_seg_len_ > 0) {
            ++n_valid_obs;
            double alpha_est = -aux_samples[s].updated_alpha_sum_ / aux_samples[s].weighted_seg_len_;
            if (!(alpha_est > 1e-8)) {
              alpha_est = 1e-8;
            }
            log_alpha_sum +=  vcl_log(alpha_est); 
          }
#endif
          obs_vector[s] = aux_samples[s].obs_; // / seg_len;
          pre_vector[s] = aux_samples[s].pre_ / seg_len;
          vis_vector[s] = aux_samples[s].vis_ / seg_len;
          //post_prob_vector[s] = aux_samples[s].post_prob_ / seg_len;
        }
        else {
          pre_vector[s] = 0.0f;
          vis_vector[s] = 0.0f;
          //post_prob_vector[s] = 0.0f;
          obs_vector[s] = typename psm_apm_traits<APM>::obs_datatype(0);
        }
      }

      //double damped_Beta = (Beta + damping_factor)/(damping_factor*Beta + 1.0);
      //cell.alpha *= (float)damped_Beta;

      // compute min and max values for alpha
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(cell_it->first);
      const float cell_len = float(cell_bb.xmax() - cell_bb.xmin());
      const float max_alpha = -vcl_log(1.0f - max_cell_P_)/cell_len;
      const float min_alpha = -vcl_log(1.0f - min_cell_P_)/cell_len;
#ifdef OLD_ALPHA_UPDATE
      if (alpha_denominator > 1e-6f) { // avoid division by 0
        cell.alpha = (float)(-alpha_numerator / alpha_denominator);
      }
      else {
        cell.alpha = 0.0f;
      }
#else
      if (n_valid_obs > 0) {
        cell.alpha = (float)vcl_exp((1.0/n_valid_obs) * log_alpha_sum);
      }
      else {
        cell.alpha = min_alpha;
      }

#endif
      // do bounds check on new alpha value
      if (cell.alpha > max_alpha) {
        cell.alpha = max_alpha;
      }
      if (cell.alpha < min_alpha) {
        cell.alpha = min_alpha;
      }
      if (!((cell.alpha >= min_alpha) && (cell.alpha <= max_alpha)) ){
        vcl_cerr << vcl_endl << "error: cell.alpha = " << cell.alpha << vcl_endl;
        //vcl_cerr << "damped_Beta = " << damped_Beta << vcl_endl;
        //vcl_cerr << "alpha numerator = " << -alpha_numerator << ",  alpha_denominator = " << alpha_denominator << vcl_endl;
        //vcl_cerr << "inv_alpha_sum = " << inv_alpha_sum << " , len_sum = " << len_sum << vcl_endl;
        cell.alpha = 0.0f;
      }
      // update with new appearance
      psm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, pre_vector, vis_vector, cell.appearance);
      //psm_opt_appearance_estimator<APM>::compute_appearance(obs_vector, pre_vector, vis_vector, cell.appearance);
    }
    for(unsigned int i=0; i<aux_readers.size(); ++i) {
      delete aux_readers[i];
    }
  }
  vcl_cout << "done with all cells" << vcl_endl;
  return true;
}


#define PSM_OPT_RT_BAYESIAN_OPTIMIZER_INSTANTIATE(T1,T2) \
  template class psm_opt_rt_bayesian_optimizer<T1,T2>











#endif

