#ifndef psm_update_h_
#define psm_update_h_

#include <vcl_vector.h>
#include <vbl/vbl_bounding_box.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_image_sample.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

#include <hsds/hsds_fd_tree.h>


#include <psm/psm_mog_grey_processor.h>

#include "psm_raytrace_function.h"

template <psm_apm_type APM>
class psm_update_aux_sample
{
public:
  psm_update_aux_sample() : alpha_sum_(0.0f),  weight_sum_(0.0f), vis_sum_(0.0f), obs_prob_(0.0f), multiplier_temp_(0.0f), last_cell_(false) {}

  float multiplier_temp_;
  float alpha_sum_;
  float weight_sum_;
  float vis_sum_;
  typename psm_apm_traits<APM>::obs_datatype obs_;
  float obs_prob_;
  bool last_cell_;

};
             
template <class S>
void psm_update(psm_scene<S> &scene, vpgl_perspective_camera<double> const& cam, vil_image_view<typename S::obs_mathtype> &img)
{
  const float min_alpha = 0.00001f;
  const float max_alpha = 1000.0f;

  // create auxillary scene
  psm_scene<psm_update_aux_sample<S::apm_type > > aux_scene(scene.storage_dir() + "/update_temp", scene); 

  // iterate through blocks and project each into the image to get the observation value.
  vcl_set<vgl_point_3d<int>,vgl_point_3d_cmp<int> > valid_blocks = scene.valid_blocks();
  vcl_set<vgl_point_3d<int>,vgl_point_3d_cmp<int> >::iterator vbit = valid_blocks.begin();

  for (; vbit != valid_blocks.end(); ++vbit) {
    // get blocks
    hsds_fd_tree<S,3> &block = scene.get_block(*vbit);
    hsds_fd_tree<psm_update_aux_sample<S::apm_type>,3> &aux_block = aux_scene.get_block(*vbit);
    // iterate through each cell of blocks
    hsds_fd_tree<S,3>::iterator block_it = block.begin();
    hsds_fd_tree<psm_update_aux_sample<S::apm_type>,3>::iterator aux_it = aux_block.begin();
    for (; block_it != block.end(); ++block_it, ++aux_it) {
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(block_it->first);
      aux_it->second.obs_ = mean_cell_observation<S::apm_type>(cell_bb, img, cam);
      if (!((aux_it->second.obs_ >= 0) && (aux_it->second.obs_ <= 1.0f)) ) {
        vcl_cout << vcl_endl << vcl_endl << "***************************************************************************" << vcl_endl;
        vcl_cout << "obs = " << aux_it->second.obs_ << vcl_endl;
        vcl_cout << "*************************************************************************" << vcl_endl << vcl_endl;
      }
      aux_it->second.obs_prob_ = psm_apm_traits<S::apm_type>::apm_processor::prob_density(block_it->second.appearance, aux_it->second.obs_);
      if (!(aux_it->second.obs_prob_ >= 0)) {
        vcl_cout << "cell_idx = " << block_it->first << vcl_endl;
        vcl_cout << "obs_prob = " << aux_it->second.obs_prob_ << vcl_endl;
        vcl_cout << "obs = " << aux_it->second.obs_ << vcl_endl;
        vcl_cout << "nmodes = " << block_it->second.appearance.num_components() << vcl_endl;
        aux_it->second.obs_prob_ = psm_apm_traits<S::apm_type>::apm_processor::prob_density(block_it->second.appearance, aux_it->second.obs_);
      }
    }
  }

  vil_image_view<float> norm_img(img.ni(), img.nj());

  vcl_cout << "update: pass 1" << vcl_endl;
  psm_raytrace_function<psm_update_pass1_functor<S::apm_type>, S, psm_update_aux_sample<S::apm_type> > raytrace_fn_pass1(scene, cam, aux_scene);
  psm_raytrace_function<psm_update_functor<S::apm_type>, S, psm_update_aux_sample<S::apm_type> > raytrace_fn_pass2(scene, cam, aux_scene);
  for (unsigned int j=0; j<img.nj(); ++j) {
    vcl_cout << ".";
    for (unsigned int i=0; i<img.ni(); ++i) {
      // pass 1
      psm_update_pass1_functor<S::apm_type> functor_pass1;
      raytrace_fn_pass1.run(i,j,functor_pass1);
      // pass 2
      psm_update_functor<S::apm_type > functor_pass2(functor_pass1.normalizing_factor());
      raytrace_fn_pass2.run(i,j,functor_pass2);
    }
  }
  vcl_cout << vcl_endl;

  vcl_cout << "updating cell values. " << vcl_endl;
  // finally, iterate through cells and update values
  // TODO: only do this for blocks which were touched
  for (vbit = valid_blocks.begin(); vbit != valid_blocks.end(); ++vbit) {
    // get blocks
    hsds_fd_tree<S,3> &block = scene.get_block(*vbit);
    hsds_fd_tree<psm_update_aux_sample<S::apm_type>,3> &aux_block = aux_scene.get_block(*vbit);
    // iterate through each cell of blocks
    hsds_fd_tree<S,3>::iterator block_it = block.begin();
    hsds_fd_tree<psm_update_aux_sample<S::apm_type>,3>::iterator aux_it = aux_block.begin();
    for (; block_it != block.end(); ++block_it, ++aux_it) {
      float weight_sum = aux_it->second.weight_sum_;
      if ( weight_sum > 1e-6 ) {
        block_it->second.alpha = aux_it->second.alpha_sum_ / weight_sum;
        if (!(block_it->second.alpha >= 0) ) {
          vcl_cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << vcl_endl;
          vcl_cout << "new alpha = " << block_it->second.alpha << vcl_endl;
          vcl_cout << "    alpha_sum = " << aux_it->second.alpha_sum_ << vcl_endl;
          vcl_cout << "    weight_sum = " << weight_sum << vcl_endl;
        }
        if (block_it->second.alpha < min_alpha) {
          block_it->second.alpha = min_alpha;
        }
        else if (block_it->second.alpha > max_alpha) {
          block_it->second.alpha = max_alpha;
        }
        float vis_sum = aux_it->second.vis_sum_;
        if (vis_sum > 0.0f) {
          float obs_weight = vis_sum / weight_sum;
          if (!((obs_weight >= 0) && (obs_weight <= 1.0f)) ) {
            vcl_cout << "*******############***********###########********************#################" << vcl_endl;
            vcl_cout << "vis_sum = " << vis_sum << vcl_endl;
            vcl_cout << "weight_sum = " << weight_sum << vcl_endl;
            vcl_cout << "obs_weight = " << obs_weight << vcl_endl;
          }
          if (obs_weight > 1e-6) {
            //if (block_it->first.idx == 0x54b74000) {
            //  vcl_cout << "----------------------------------------------" << vcl_endl;
            //  vcl_cout << "obs = " << aux_it->second.obs_ << vcl_endl;
            //  vcl_cout << "obs_weight = " << obs_weight << vcl_endl;
            // vcl_cout << "-----------------------------------------------" << vcl_endl;
            //}
            psm_apm_traits<S::apm_type>::apm_processor::update(block_it->second.appearance,aux_it->second.obs_, obs_weight);
          }

        }
      }
    }
  }
  return;
 
}

template <psm_apm_type APM>
class psm_update_pass1_functor
{
public:

 //: default constructor
  psm_update_pass1_functor() :  alpha_vis_PI_integral_(0.0f), alpha_integral_(0.0f), vis_prob_(1.0f) {}

  //: accumulate 
  inline bool step_cell(vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, psm_update_aux_sample<APM> &aux_value, hsds_fd_tree_node_index<3> cell_index)
  {
    if (cell_value.alpha <= 0) {
      vcl_cout << "cell_value.alpha =  " << cell_value.alpha << vcl_endl;
      return true;
    }

    float cell_len = (float)(s1 - s0).length();

    // update expected value of imaged pixel
    const float PI = aux_value.obs_prob_;
    
    alpha_integral_ += cell_value.alpha * cell_len;

    float vis_prob_end = vcl_exp(-alpha_integral_);

    float seg_integral = PI * (vis_prob_ - vis_prob_end);
    if (!(seg_integral >= 0)) {
      vcl_cout << "seg_integral = " << seg_integral << vcl_endl;
      vcl_cout << "PI = " << PI << vcl_endl;
      vcl_cout << "vis_prob_ = " << vis_prob_ << vcl_endl;
      vcl_cout << "vis_prob_end = " << vis_prob_end << vcl_endl;
      vcl_cout << "cell_len = " << cell_len << vcl_endl;
      vcl_cout << "alpha = " << cell_value.alpha << vcl_endl;
    }

    alpha_vis_PI_integral_ += seg_integral;

    // store unnormalized multiplier - will get normalized on pass 2
    aux_value.multiplier_temp_ = (alpha_vis_PI_integral_ + (PI * vis_prob_)) * cell_len;
    if (!(aux_value.multiplier_temp_ >= 0) ){ 
      vcl_cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << vcl_endl;
      vcl_cout << "multiplier_temp = " << aux_value.multiplier_temp_ << vcl_endl;
      vcl_cout << "   alpha_vis_PI_integral = " << alpha_vis_PI_integral_ << vcl_endl;
      vcl_cout << "   PI = " << PI << vcl_endl;
      vcl_cout << "   vis_prob_ = " << vis_prob_ << vcl_endl;
      vcl_cout << "   cell_len = " << cell_len << vcl_endl;
    }
    // accumulate weights for alpha and observation
    aux_value.weight_sum_ += cell_len;
    aux_value.vis_sum_ += vis_prob_ * cell_len;

    vis_prob_ = vis_prob_end;

    //if (vis_prob_ < 1e-4) {
    //  aux_value.last_cell_ = true;
    //  return false;
    //}
    return true;
  }

  float normalizing_factor()
  { 
    if (!(alpha_vis_PI_integral_ >= 0)) {
      vcl_cout << "alpha_vis_PI_integral = " << alpha_vis_PI_integral_ << vcl_endl;
    }
    if (!((vis_prob_ >= 0.0f) && (vis_prob_ <= 1.0f)) ) {
      vcl_cout << "vis_prob = " << vis_prob_ << vcl_endl;
    }
    float norm_factor = alpha_vis_PI_integral_ + vis_prob_;
    return norm_factor;
  }

private:

  float alpha_vis_PI_integral_;
  float alpha_integral_;
  float vis_prob_;


};



template <psm_apm_type APM>
class psm_update_functor
{
public:

  //: default constructor
  psm_update_functor( float normalizing_factor) :  normalizing_factor_(normalizing_factor) {}

  //: accumulate 
  inline bool step_cell(vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, psm_update_aux_sample<APM> &aux_value, hsds_fd_tree_node_index<3> cell_index)
  {
    if (cell_value.alpha <= 0) {
      vcl_cout << "cell_value.alpha =  " << cell_value.alpha << vcl_endl;
      return true;
    }
    if (normalizing_factor_ > 1e-6) {
      aux_value.alpha_sum_ += cell_value.alpha * (aux_value.multiplier_temp_ / normalizing_factor_);
    }
    if (!( (aux_value.alpha_sum_ >= 0) && (aux_value.alpha_sum_ < 1e8) ) ){
      vcl_cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << vcl_endl;
      vcl_cout << "alpha_sum_ = " << aux_value.alpha_sum_ << vcl_endl;
      vcl_cout << "    alpha = " << cell_value.alpha << vcl_endl;
      vcl_cout << "    multplier_temp = " << aux_value.multiplier_temp_ << vcl_endl;
      vcl_cout << "    norm_factor = " << normalizing_factor_ << vcl_endl;
    }
   
    //if (aux_value.last_cell_) {
    //  // reset to false for next ray
    //  aux_value.last_cell_ = false;
    //  return false;
   // }
    return true;
  }


private:
 
  float normalizing_factor_; //computed from first pass

};





#endif

