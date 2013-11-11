#include "dbvxm_detect_instance_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
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

dbvxm_detect_instance_process::dbvxm_detect_instance_process()
{
  //inputs
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  //input_types_[0] = "vil_image_view_base_sptr";
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";   // object   (look for object in the world)
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";  // scale

  //output
  output_data_.resize(4,brdb_value_sptr(0));
  output_types_.resize(4);
  output_types_[0]= "vil_image_view_base_sptr";      // output an expected image of the object at the highest prob location overlayed 
  output_types_[1]= "vil_image_view_base_sptr";      // output a probability image reflecting the prob of existence for the object at each pixel
  output_types_[2]= "vil_image_view_base_sptr";      // overlay prob map onto the exp img
  output_types_[3]= "vil_image_view_base_sptr";      // second best

  //output_types_[1]= "float";  // output a
  //output_types_[2]= "float";  // output b

  parameters()->add("ni for output image", "ni", (unsigned)640);
  parameters()->add("nj for output image", "nj", (unsigned)480);
 
  parameters()->add("verbose", "verbose", true);

  parameters()->add("min x", "minx", (unsigned)38);  // min in voxel world in x dimension to start search
  parameters()->add("min y", "miny", (unsigned)35);
  parameters()->add("min z", "minz", (unsigned)0);

  parameters()->add("max x", "maxx", (unsigned)42);  // max in voxel world in x dimension to end search
  parameters()->add("max y", "maxy", (unsigned)39);
  parameters()->add("max z", "maxz", (unsigned)0);
  
  parameters()->add("increment x", "incx", (unsigned)1);  // increments in voxel world in x dimension during search
  parameters()->add("increment y", "incy", (unsigned)1);
  parameters()->add("increment z", "incz", (unsigned)1);

  parameters()->add("angle intervals for rotational search (in degrees)", "angle_int", 20.0f);
}


bool dbvxm_detect_instance_process::execute()
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

  brdb_value_t<bvxm_voxel_world_sptr>* input2 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[2].ptr());
  bvxm_voxel_world_sptr obj_world = input2->value();

   brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string voxel_type = input3->value();

  brdb_value_t<unsigned>* input4 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input4->value();

  brdb_value_t<unsigned>* input5 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned scale_index = input5->value();

  // get parameters
  bool verbose = false;
  parameters()->get_value("verbose", verbose);
  unsigned ni, nj;  float angle_int; 
  parameters()->get_value("ni", ni);
  parameters()->get_value("nj", nj);
  parameters()->get_value("angle_int", angle_int);
  unsigned minx, miny, minz, incx, incy, incz, max_x, max_y, max_z;
  parameters()->get_value("minx", minx);
  parameters()->get_value("miny", miny);
  parameters()->get_value("minz", minz);
  parameters()->get_value("maxx", max_x);
  parameters()->get_value("maxy", max_y);
  parameters()->get_value("maxz", max_z);
  parameters()->get_value("incx", incx);
  parameters()->get_value("incy", incy);
  parameters()->get_value("incz", incz);

  if (verbose) {
    vcl_cout << "instance detection parameters to be used in this run:\n"
             << "ni: " << ni << " nj: " << nj << " angle_int: " << angle_int << vcl_endl;
  }

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

      //brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(img);
      //output_data_[1] = output1;
      return true;
    }
  }
 
  if (verbose) {
    vcl_cout << "detecting instance\n\tmain world dims: " << main_world->get_params()->num_voxels().x();
    vcl_cout << " " << main_world->get_params()->num_voxels().y() << " " << main_world->get_params()->num_voxels().z() << "\n";
    vcl_cout << "\tobj world dims: " << obj_world->get_params()->num_voxels().x();
    vcl_cout << " " << obj_world->get_params()->num_voxels().y() << " " << obj_world->get_params()->num_voxels().z() << "\n";
  }

  //unsigned max_x = main_world->get_params()->num_voxels().x()-obj_world->get_params()->num_voxels().x();
  //unsigned max_y = main_world->get_params()->num_voxels().y()-obj_world->get_params()->num_voxels().y();
  //unsigned max_z = main_world->get_params()->num_voxels().z()-obj_world->get_params()->num_voxels().z();
  
  float voxel_length = main_world->get_params()->voxel_length();
  float m_x = main_world->get_params()->corner().x(); 
  float m_y = main_world->get_params()->corner().y();
  float m_z = main_world->get_params()->corner().z();

  // create metadata:
  vil_image_view_base_sptr dummy_img = new vil_image_view<vxl_byte>(ni, nj);
  bvxm_image_metadata observation(dummy_img,camera);  // image is only required to get ni and nj

  //1) get probability mixtures of all pixels in image
  bvxm_voxel_slab_base_sptr mog_image;

  // use the weight slab below to calculate total probability
  bvxm_voxel_slab<float> weights(ni, nj, 1);
  weights.fill(1.0f/(ni * nj));

  vil_image_view_base_sptr best_obj_exp_img;
  vil_image_view_base_sptr second_best_obj_exp_img;
  vil_image_view_base_sptr mix_exp_img;
  float best_prob = 0; float second_best_prob = 0;

  //: create slabs to be used for prob map generation
  bvxm_voxel_slab<float> mother_slab(main_world->get_params()->num_voxels().x(),main_world->get_params()->num_voxels().y(),1);
  bvxm_voxel_slab<float> mother_slab_equalized(main_world->get_params()->num_voxels().x(),main_world->get_params()->num_voxels().y(),1);
  mother_slab.fill(0.01f);
  mother_slab_equalized.fill(0.01f);
  bvxm_voxel_slab<float> mother_slab_img(ni,nj,1);
  vgl_h_matrix_2d<double> Hp2i, Hi2p;
  main_world->compute_plane_image_H(observation.camera,0,Hp2i,Hi2p,scale_index);
  vil_image_view_base_sptr out_img = new vil_image_view<vxl_byte>(ni,nj,1);
  
  vul_timer t, t2;

  if (voxel_type == "apm_mog_rgb") {
    typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type;
    typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype;

    main_world->mixture_of_gaussians_image<APM_MOG_RGB>(observation, mog_image, bin_index);
    bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());

    bvxm_voxel_traits<APM_MOG_RGB>::appearance_processor apm_processor;
    bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
    mix_exp_img = new vil_image_view<vxl_byte>(ni, nj, 3);
    bvxm_util::slab_to_img(exp_img, mix_exp_img);
    
    if (verbose) {  
      vil_save(*mix_exp_img, "./mixture_expected_img.png");
    }
    
    vcl_cout << "color not implemented yet!\n";
    return false;
  }
  else if (voxel_type == "apm_mog_grey") {
    typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
    typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

    main_world->mixture_of_gaussians_image<APM_MOG_GREY>(observation, mog_image, bin_index);
    bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());

    bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;
    bvxm_voxel_slab<float> exp_img = apm_processor.expected_color(*mog_image_ptr);
    mix_exp_img = new vil_image_view<vxl_byte>(ni, nj, 1);
    bvxm_util::slab_to_img(exp_img, mix_exp_img);
      
    if (verbose) {
      vil_save(*mix_exp_img, "./mixture_expected_img.png");
    }

    vcl_cout << "entering exhaustive search\n";
    vcl_cout.flush();
    float best_x = 0, best_y = 0, best_z = 0;
    
    t2.mark();
    for (unsigned cor_x = minx; cor_x < max_x; cor_x += incx) {
      for (unsigned cor_y = miny; cor_y < max_y; cor_y += incy) {
        //for (unsigned cor_z = minz; cor_z < max_z; cor_z += incz) {
        unsigned cor_z = 0;
          t.mark();
          vgl_point_3d<float> cor(m_x + (float)cor_x*voxel_length, m_y + (float)cor_y*voxel_length, m_z + (float)cor_z*voxel_length);
          vcl_cout << "trying: " << cor << " " << " cor_x: " << cor_x << " cor_y: " << cor_y << " cor_z: " << cor_z << vcl_endl;
          vcl_cout.flush();
          obj_world->get_params()->set_corner(cor);
          
          vil_image_view_base_sptr obj_expected_img = new vil_image_view<vxl_byte>(ni,nj,1);
          vil_image_view<float> mask_img(obj_expected_img->ni(),obj_expected_img->nj(),1);
          if (!obj_world->expected_image<APM_MOG_GREY>(observation, obj_expected_img, mask_img, bin_index)) {
            vcl_cout << "In dbvxm_detect_instance_process::execute() -- problems in creating expected image for obj world at corner: " << cor << vcl_endl;
            return false;
          }

          //if (verbose) {
          //  vcl_stringstream s_cx, s_cy, s_cz;
          //  s_cx << cor_x; s_cy << cor_y; s_cz << cor_z;
          //  vcl_string obj_exp_name = "./obj_expected_img_cor_x_" + s_cx.str() + "_y_" + s_cy.str() + "_z_" + s_cz.str() + ".png";
          //  vcl_cout << "saving obj exp img\n"; vcl_cout.flush();
          //  vil_save(*obj_expected_img, obj_exp_name.c_str());
          //}
          
          vil_image_view<float>* input_img_float_stretched = new vil_image_view<float>( ni, nj, 1 );
          vil_convert_stretch_range_limited<vxl_byte>(*obj_expected_img, *input_img_float_stretched, 0, 255, 0.0f, 1.0f);
          vil_image_view_base_sptr nimg_sptr = input_img_float_stretched;

          // convert image to a voxel_slab
          bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
          bvxm_util::img_to_slab(nimg_sptr,image_slab);
          
          bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
          // find the total prob
          bvxm_voxel_slab<float> product(ni, nj, 1);
          bvxm_util::multiply_slabs(prob, weights, product);
          float this_prob = bvxm_util::sum_slab(product);

          vcl_cout << "prob: " << this_prob << "\n";
          if ( this_prob < 0 ) {
            vcl_cout << "In bvxm_normalize_image_process::execute() -- prob is negative, Exiting!\n";
            return false;
          }

          mother_slab(cor_x, cor_y) = this_prob;
          //: prob value is in range [0.4, 0.5], stretch this to [0, 1]
          float eq_prob = (this_prob-0.4f)/0.1f; vcl_cout << "equalized prob: " << eq_prob << "\n";
          mother_slab_equalized(cor_x, cor_y) = eq_prob;

          if ( this_prob > best_prob ) { 
            second_best_prob = best_prob;
            best_prob = this_prob;
            second_best_obj_exp_img = best_obj_exp_img;
            best_obj_exp_img = obj_expected_img;
            vil_save(*obj_expected_img, "./obj_expected_img.png");
            best_x = (float)cor_x; best_y = (float)cor_y; best_z = (float)cor_z;
          } else if ( this_prob > second_best_prob ) { 
            second_best_prob = this_prob;
            second_best_obj_exp_img = obj_expected_img;
          }

          nimg_sptr = 0;  // to clear up space
          vcl_cout << " took: " << t.real() / 1000.0f << " secs.\n";
        //}
      }
    }
/*
    vcl_cout << "making second pass around: " << best_x << " " << best_y << " " << best_z << "\n";
    //: make a second refined pass around the best
    for (unsigned cor_x = best_x-1 < 0 ? 0 : best_x - 1; cor_x < best_x+1; cor_x += 1) {
      for (unsigned cor_y = best_y-1 < 0 ? 0 : best_y - 1; cor_y < best_y+1; cor_y += 1) {
        //for (unsigned cor_z = best_z; cor_z < best_z+1; cor_z += 1) {
        unsigned cor_z = 0;
          if (cor_x == best_x && cor_y == best_y)
            continue;

          //vgl_point_3d<float> cor((float)cor_x, (float)cor_y, (float)cor_z);
          vgl_point_3d<float> cor(m_x + (float)cor_x*voxel_length, m_y + (float)cor_y*voxel_length, m_z + (float)cor_z*voxel_length);
          vcl_cout << "trying: " << cor << " ";
          obj_world->get_params()->set_corner(cor);
          
          vil_image_view_base_sptr obj_expected_img = new vil_image_view<vxl_byte>(ni,nj,1);
          vil_image_view<float> mask_img(obj_expected_img->ni(),obj_expected_img->nj(),1);
          if (!obj_world->expected_image<APM_MOG_GREY>(observation, obj_expected_img, mask_img, bin_index)) {
            vcl_cout << "In dbvxm_detect_instance_process::execute() -- problems in creating expected image for obj world at corner: " << cor << vcl_endl;
            return false;
          }

          
          vil_image_view<float>* input_img_float_stretched = new vil_image_view<float>( ni, nj, 1 );
          vil_convert_stretch_range_limited<vxl_byte>(*obj_expected_img, *input_img_float_stretched, 0, 255, 0.0f, 1.0f);
          vil_image_view_base_sptr nimg_sptr = input_img_float_stretched;

          // convert image to a voxel_slab
          bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
          bvxm_util::img_to_slab(nimg_sptr,image_slab);
          
          bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
          // find the total prob
          bvxm_voxel_slab<float> product(ni, nj, 1);
          bvxm_util::multiply_slabs(prob, weights, product);
          float this_prob = bvxm_util::sum_slab(product);

          vcl_cout << "prob: " << this_prob << "\n";
          if ( this_prob < 0 ) {
            vcl_cout << "In bvxm_normalize_image_process::execute() -- prob is negative, Exiting!\n";
            return false;
          }

          mother_slab(cor_x, cor_y) = this_prob;

          if ( this_prob > best_prob ) { 
            best_prob = this_prob;
            best_obj_exp_img = obj_expected_img;
            vil_save(*obj_expected_img, "./obj_expected_img.png");
            best_x = cor_x; best_y = cor_y; best_x = cor_z;
          }

          nimg_sptr = 0;  // to clear up space
        //}
      }
    }
*/
  }
  else {
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }


  //: put the best obj exp image as red channel onto the mix_exp_img
  
  vil_image_view_base_sptr out;
  vil_image_view<vxl_byte> best_obj(best_obj_exp_img);
  vil_image_view<vxl_byte> mix_exp(mix_exp_img);
  if (mix_exp_img->nplanes() == 3) {
    vil_image_view<vxl_byte> R = vil_plane(best_obj, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 1);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 2);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  } else {
    vil_image_view<vxl_byte> R = vil_plane(best_obj, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 0);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 0);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  }

  vil_image_view_base_sptr img = new vil_image_view<vxl_byte>(ni, nj);
  //brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(best_obj_exp_img);
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out);
  output_data_[0] = output0;

  //: generate the prob map from mother_slab
  //bvxm_util::warp_slab_bilinear(mother_slab, Hi2p, mother_slab_img);
  bvxm_util::warp_slab_bilinear(mother_slab_equalized, Hi2p, mother_slab_img);
  bvxm_util::slab_to_img(mother_slab_img, out_img);

  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(out_img);
  output_data_[1] = output1;

  vil_image_view_base_sptr out2;
  vil_image_view<vxl_byte> slab_prob(out_img);
  if (mix_exp_img->nplanes() == 3) {
    vil_image_view<vxl_byte> R = vil_plane(slab_prob, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 1);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 2);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out2 = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  } else {
    vil_image_view<vxl_byte> R = vil_plane(slab_prob, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 0);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 0);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out2 = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  }
  brdb_value_sptr output2 = new brdb_value_t<vil_image_view_base_sptr>(out2);
  output_data_[2] = output2;

  vil_image_view_base_sptr out3;
  if (!second_best_obj_exp_img)
    second_best_obj_exp_img = best_obj_exp_img;
  vil_image_view<vxl_byte> second_best_obj(second_best_obj_exp_img);
  if (mix_exp_img->nplanes() == 3) {
    vil_image_view<vxl_byte> R = vil_plane(second_best_obj, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 1);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 2);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out3 = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  } else {
    vil_image_view<vxl_byte> R = vil_plane(second_best_obj, 0);
    vil_image_view<vxl_byte> G = vil_plane(mix_exp, 0);
    vil_image_view<vxl_byte> B = vil_plane(mix_exp, 0);
    vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(R, G, B);
    out3 = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  }

  brdb_value_sptr output3 = new brdb_value_t<vil_image_view_base_sptr>(out3);
  output_data_[3] = output3;


  vcl_cout << " whole process took: " << t2.real() / (60*1000.0f) << " mins.\n";

  return true;
}

