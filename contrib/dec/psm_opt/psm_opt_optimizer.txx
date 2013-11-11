#ifndef psm_opt_optimizer_txx_
#define psm_opt_optimizer_txx_

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_set.h>
#include <vcl_map.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>


#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_incremental_reader.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene.h>

#include "psm_opt_sample.h"
#include "psm_opt_alpha_cost_function2.h"

#include "psm_opt_optimizer.h"

template <psm_apm_type APM, psm_aux_type AUX>
psm_opt_optimizer<APM,AUX>::psm_opt_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_ids, double min_sigma)
: scene_(scene), image_ids_(image_ids), min_sigma_(min_sigma)
{

}



template<psm_apm_type APM, psm_aux_type AUX>
bool psm_opt_optimizer<APM,AUX>::optimize_cells()
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
      vcl_vector<float> vis_vector;

      for (unsigned int e=0; e<aux_samples.size(); ++e) {
#define PSM_OPT_DEBUG
#ifdef PSM_OPT_DEBUG
        if (c == debug_c) {
          vcl_cout << "edge " << e << ": " << vcl_endl;
          vcl_cout << "   vis = " << aux_samples[e].vis_ << vcl_endl;
          vcl_cout << "   pre = " << aux_samples[e].pre_ << vcl_endl;
          vcl_cout << "   post = " << aux_samples[e].post_ << vcl_endl;
          vcl_cout << "   obs = " << aux_samples[e].obs_ << vcl_endl;
          vcl_cout << "   PI = " << aux_samples[e].PI_ << vcl_endl;
          vcl_cout << "   seg_len = " << aux_samples[e].seg_len_ << vcl_endl;
        }
#endif
        obs_vector.push_back(aux_samples[e].obs_);
        vis_vector.push_back(aux_samples[e].vis_);
      }
      
      if (aux_samples.size() < 3) {
        // not enough samples to perform optimization

      } else {
        //vcl_cout << "c = " << c << vcl_endl;
        // perform optimization

        // estimate alpha
        psm_opt_alpha_cost_function2<APM> alpha_cost_fun(cell, aux_samples, min_sigma_);

#ifdef PSM_OPT_DEBUG

        if (c==debug_c) {

          vcl_cout << " building alpha cost vector" << vcl_endl;
          // debug: evaluate cost function for a regular grid and write in matlab format
          unsigned int test_alpha_res = 500;
          float test_max_alpha = 8.0f;

          vnl_vector<float> test_cost_vector(test_alpha_res);
          for (unsigned int test_i=0; test_i < test_alpha_res; ++test_i) {
            float test_alpha = test_max_alpha / test_alpha_res * test_i;
            vnl_vector_fixed<double,1> test_x(test_alpha);
            vnl_vector<double> test_fx(alpha_cost_fun.get_number_of_residuals());
            alpha_cost_fun.f(test_x,test_fx);
            test_cost_vector[test_i] = float(test_fx.magnitude());
          }

          vcl_ofstream ofs("c:/research/psm/output/alpha_cost_vector.txt");
          vnl_matlab_print(ofs,test_cost_vector,"alpha_cost_vector");
          ofs.close();
        }
#endif
        vnl_levenberg_marquardt lm_a(alpha_cost_fun);
        vnl_vector<double> xa(alpha_cost_fun.get_number_of_unknowns());
        alpha_cost_fun.cell_to_x(cell, xa);
        //lm_a.set_x_tolerance(1e-4);
        //lm_a.set_f_tolerance(1e-8);
        //lm_a.set_g_tolerance(1e-8);
        //lm_a.set_max_function_evals(500);

        lm_a.set_verbose(false);
        lm_a.set_trace(false);
#ifdef PSM_OPT_DEBUG
        if (c==debug_c) {
          lm_a.set_verbose(true);
          lm_a.set_trace(true);
          vcl_cout << "x initial = " << xa << vcl_endl;
        } 
#endif
        //vcl_cout << "minimizing alpha " << vcl_endl;
        lm_a.minimize(xa);

#ifdef PSM_OPT_DEBUG
        if (c==debug_c) {
          lm_a.diagnose_outcome();
          vcl_cout << "x optimized = " << xa << vcl_endl;
          vcl_cout << "color = " << cell.appearance << vcl_endl;
          //vnl_matrix<double> J(cost_fun.get_number_of_residuals(),cost_fun.get_number_of_unknowns());
          //cost_fun.gradf(x,J);
          //vcl_cout << "J(x) = " << vcl_endl << J << vcl_endl;
        }
#endif
        //if (xa[0] >= 1.0) {
        //  vcl_cout << "c = " << c << " xa = " << xa[0] << vcl_endl;
        //}
        alpha_cost_fun.x_to_cell(xa, cell);
    
      }
      // update with new appearance
      //psm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, pre_vector, vis_vector, cell.appearance, (float)min_sigma_);
    }
    for(unsigned int i=0; i<aux_readers.size(); ++i) {
      delete aux_readers[i];
    }
  }

  vcl_cout << "done with all cells" << vcl_endl;

  return true;
}





#define PSM_OPT_OPTIMIZER_INSTANTIATE(T1,T2) \
  template class psm_opt_optimizer<T1,T2>


#endif

