#include "dbvxm_rec_update_changes_using_mog_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <vul/vul_timer.h>

#include <rec/dbvxm_bg_pair_density.h>
#include <rec/dbvxm_fg_pair_density.h>
#include <rec/dbvxm_fg_bg_pair_density.h>
#include <rec/dbvxm_bayesian_propagation.h>

dbvxm_rec_update_changes_using_mog_process::dbvxm_rec_update_changes_using_mog_process()
{
  //inputs
  input_data_.resize(8,brdb_value_sptr(0));
  input_types_.resize(8);
  //input_types_[0] = "vil_image_view_base_sptr";
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";  // scale
  input_types_[5] = "vil_image_view_base_sptr";      // input change map
  input_types_[6] = "vil_image_view_base_sptr";      // input orig view
  input_types_[7] = "unsigned";      // number of passes

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";      // output new prob map
}


bool dbvxm_rec_update_changes_using_mog_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvxm_voxel_world_sptr>* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr main_world = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

   brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());
  vcl_string voxel_type = input2->value();

  brdb_value_t<unsigned>* input3 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned bin_index = input3->value();

  brdb_value_t<unsigned>* input4 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned scale_index = input4->value();

  brdb_value_t<vil_image_view_base_sptr>* input5 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[5].ptr());
  vil_image_view_base_sptr change_map = input5->value();

  vil_image_view<float> change_map_f(change_map);

  brdb_value_t<vil_image_view_base_sptr>* input6 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[6].ptr());
  vil_image_view_base_sptr orig_view = input6->value();

  brdb_value_t<unsigned>* input7 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[7].ptr());
  unsigned n = input7->value();  // number of passes

  unsigned ni = change_map_f.ni();
  unsigned nj = change_map_f.nj();

  vul_timer t2;
  t2.mark();

  // if the world is not updated yet, we just return an empty image
  if ((voxel_type == "apm_mog_rgb") ){
    if (!main_world->num_observations<APM_MOG_RGB>(bin_index) ) {
      vil_image_view_base_sptr img = new vil_image_view<vxl_byte>(ni, nj);
      brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(img);
      output_data_[0] = output0;

      //brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(img);
      //output_data_[1] = output1;
      return true;
    }
  }
  if ((voxel_type == "apm_mog_grey") )
  {
    if (!main_world->num_observations<APM_MOG_GREY>(bin_index))
    {
      vil_image_view_base_sptr img = new vil_image_view<vxl_byte>(ni, nj);
      brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(img);
      output_data_[0] = output0;

      return true;
    }
  }

  //: create the background pair model
  dbvxm_bg_pair_density bgd(main_world, camera, voxel_type, 0, 0, ni, nj);
  //if (!bgd.generate_mixture_image()) {
  //  vcl_cout << "Unable to create background mixture image with appearance type: " << voxel_type << "\n";
  //  return false;
  //}
  //vcl_cout << "background mixture model created\n";
  vil_image_view<vxl_byte> orig_img(orig_view);

#if 0
  float min, max, mean;
  vil_image_view<vxl_byte> img_b(ni, nj, 1);

  vil_image_view<float> out_map = bgd.prob_density(orig_img_f);
  //vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(out_map);
  vil_math_value_range(out_map, min, max);
  vcl_cout << "saving out_map bg prob density for pair min: " << min  << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(out_map, img_b, 0.0f, max);
  vil_save(img_b, "./out_map_bg_prob_density.png");

  vil_math_mean(mean, out_map, 0);
  vcl_cout << "saving out_map bg prob density mean: " << mean  << vcl_endl;
  vil_convert_stretch_range_limited(out_map, img_b, 0.0f, mean);
  vil_save(img_b, "./out_map_bg_prob_density_wrt_mean.png");

  vil_image_view<float> out_map2 = bgd.prob_density_non_pair(orig_img_f);
  vil_math_value_range(out_map2, min, max);
  vcl_cout << "saving out_map2 _bg_prob_density non pair min: " << min  << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(out_map2, img_b, 0.0f, max);
  vil_save(img_b, "./out_map2_bg_prob_density_non_pair.png");
  
  vil_math_mean(mean, out_map2, 0);
  vcl_cout << "saving out_map2 _bg_prob_density non pair mean: " << mean  << vcl_endl;
  vil_convert_stretch_range_limited(out_map2, img_b, 0.0f, mean);
  vil_save(img_b, "./out_map2_bg_prob_density_non_pair_wrt_mean.png");

  vil_image_view<float> out_map3 = bgd.prob_density_from_likelihood_map(orig_img);
  vil_math_value_range(out_map3, min, max);
  vcl_cout << "saving out_map3 prob density from likelihood min: " << min  << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(out_map3, img_b, 0.0f, max);
  vil_save(img_b, "./out_map3_prob_density_from_likelihood.png");
  vil_math_mean(mean, out_map3, 0);
  vcl_cout << "saving out_map3 prob density from likelihood mean: " << mean  << vcl_endl;
  vil_convert_stretch_range_limited(out_map3, img_b, 0.0f, mean);
  vil_save(img_b, "./out_map3_prob_density_from_likelihood_wrt_mean.png");
#endif

  dbvxm_fg_pair_density fgp;
  fgp.set_sigma(0.058); // set manually for now
  dbvxm_fg_bg_pair_density fgbgp;
  fgbgp.set_sigma(0.058);

  dbvxm_bayesian_propagation bp(orig_img, change_map_f); // initialized with equal probs for background and foreground
  bp.run(fgp, fgbgp, bgd, n);
  
  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(bp.bg_map_);
  
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr);
  output_data_[0] = output;

  vcl_cout << " whole process took: " << t2.real() / (60*1000.0f) << " mins.\n";

  return true;
}

