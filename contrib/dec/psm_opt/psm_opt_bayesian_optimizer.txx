#ifndef psm_opt_bayesian_optimizer_txx_
#define psm_opt_bayesian_optimizer_txx_

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

#include "psm_opt_sample.h"
#include "psm_opt_bayesian_optimizer.h"
#include "psm_opt_appearance_estimator.h"

template <psm_apm_type APM, psm_aux_type AUX>
psm_opt_bayesian_optimizer<APM,AUX>::psm_opt_bayesian_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_ids)
: scene_(scene), image_ids_(image_ids), min_cell_P_(0.0001f), max_cell_P_(0.995f)
{}


template<psm_apm_type APM, psm_aux_type AUX>
bool psm_opt_bayesian_optimizer<APM,AUX>::optimize_cells(double damping_factor)
{
  unsigned int c = 0;
  const unsigned int debug_c = 1509732;
  // get auxillary scenes associated with each imput image
  vcl_vector<psm_aux_scene_base_sptr> aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    psm_aux_scene_base_sptr aux_scene_base = scene_.template get_aux_scene<AUX>(image_ids_[i]);
    aux_scenes.push_back(aux_scene_base);
  }

  vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> > aux_samples;

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
        psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> aux_cell;
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


      vcl_vector<typename psm_apm_traits<APM>::obs_datatype> obs_vector;
      vcl_vector<float> vis_vector, pre_vector;//, post_vector;

      for (unsigned int e=0; e<aux_samples.size(); ++e) {
#define PSM_OPT_DEBUG
#ifdef PSM_OPT_DEBUG
        if (c == debug_c) {
          vcl_cout << "edge " << e << ": " << vcl_endl;
          vcl_cout << "   vis = " << aux_samples[e].vis_ << vcl_endl;
          vcl_cout << "   pre = " << aux_samples[e].pre_ << vcl_endl;
          //vcl_cout << "   post = " << aux_samples[e].post_ << vcl_endl;
          vcl_cout << "   obs = " << aux_samples[e].obs_ << vcl_endl;
          vcl_cout << "   seg_len = " << aux_samples[e].seg_len_ << vcl_endl;
        }
#endif
        obs_vector.push_back(aux_samples[e].obs_);
        vis_vector.push_back(aux_samples[e].vis_);
        pre_vector.push_back(aux_samples[e].pre_);
        //post_vector.push_back(aux_samples[e].post_);
      }
      
      if (aux_samples.size() < 1) {
        // not enough samples to perform optimization
        cell.alpha = 0.0f;
      } else {
        // use bayes rule to compute new alpha value
        double alpha_mult = 1.0;

        for (unsigned int s=0; s<aux_samples.size(); ++s) {
          float PI = aux_samples[s].PI_;
          float pass_prob = vcl_exp(cell.alpha * aux_samples[s].seg_len_);
          float total_ray_prob = aux_samples[s].pre_ + PI * aux_samples[s].vis_ + pass_prob*aux_samples[s].vis_*(aux_samples[s].post_ - PI);
          float ray_prob_given_x = (aux_samples[s].pre_ + aux_samples[s].vis_ * PI);
          if (total_ray_prob > 1e-6) {
            alpha_mult *=  ray_prob_given_x / total_ray_prob;
          }
          else if (ray_prob_given_x > 1e-5) {
            // this case indicates observation is extremely improbable given the current model,
            // but at least an order of magnitude more probable given that this cell is an occluder. 
            alpha_mult *= 10.0;
          }
          else {
            // both numerator and denominator are extemely small. 
            // Disregard the observation to keep things numerically stable.
            // alpha_mult *= 1.0;
          }
        }
        // add in damping factor
#ifdef ADDITIVE_DAMP
        double damped_alpha_mult = alpha_mult + damping_factor - alpha_mult * damping_factor;
#else
        double damped_alpha_mult = (alpha_mult + damping_factor)/(damping_factor*alpha_mult + 1.0);
#endif
        cell.alpha *= (float)damped_alpha_mult;
        // do bounds check on new alpha value
        vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(cell_it->first);
        const float cell_len = float(cell_bb.xmax() - cell_bb.xmin());
        const float max_alpha = -vcl_log(1.0f - max_cell_P_)/cell_len;
        const float min_alpha = -vcl_log(1.0f - min_cell_P_)/cell_len;
        if (cell.alpha > max_alpha) {
          cell.alpha = max_alpha;
        }
        if (cell.alpha < min_alpha) {
          cell.alpha = min_alpha;
        }
        if (!((cell.alpha >= min_alpha) && (cell.alpha <= max_alpha)) ){
          vcl_cerr << vcl_endl << "error: cell.alpha = " << cell.alpha << vcl_endl;
          vcl_cerr << "damped_alpha_mult = " << damped_alpha_mult << vcl_endl;
        }
      }
      // update with new appearance
      //psm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, vis_vector, cell.appearance, 0.01f);
      psm_opt_appearance_estimator<APM>::compute_appearance(obs_vector, vis_vector, pre_vector, cell.appearance);
    }
    for(unsigned int i=0; i<aux_readers.size(); ++i) {
      delete aux_readers[i];
    }
  }

  vcl_cout << "done with all cells" << vcl_endl;

  return true;
}





#define PSM_OPT_BAYESIAN_OPTIMIZER_INSTANTIATE(T1,T2) \
  template class psm_opt_bayesian_optimizer<T1,T2>











#endif

