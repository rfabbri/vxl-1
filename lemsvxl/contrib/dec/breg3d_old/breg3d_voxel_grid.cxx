#include <vcl_iomanip.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_threshold.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_erf.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>

#include <vimt/vimt_transform_2d.h>
//#include <vimt/vimt_resample_bilin.h>
#include <vimt/vimt_sample_grid_bilin.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>

#include <brip/brip_vil_float_ops.h>
#include <vsol/vsol_box_2d.h>

#include <vul/vul_file.h>

#include "breg3d_voxel_grid.h"
#include "breg3d_util.h"



//: create an axis-aligned voxel grid
breg3d_voxel_grid::breg3d_voxel_grid(const char* base_dir, unsigned nx, unsigned ny, unsigned nz, vgl_point_3d<double> corner, double vox_size) : corner_(corner), i_step_(vox_size,0,0), j_step_(0,vox_size,0), k_step_(0,0,vox_size), base_dirname_(base_dir), image_format_("tiff"), mg_image_fnames_(n_mg_modes_), std_init_value_(0.1f), min_vox_prob_(1e-4f), max_vox_prob_(1 - 1e-4f)
{
  assert(vul_file::is_directory(base_dir));

  nx_ = nx;
  ny_ = ny;
  nz_ = nz;

  // create one set of images for each z value.
  for (unsigned z=0; z<nz; z++) {
    vcl_stringstream prob_fname;

    prob_fname << base_dir << "/prob_" << vcl_setw(5) << vcl_setfill('0') << z << "." << image_format_;
    prob_image_fnames_.push_back(prob_fname.str());

    for (unsigned m=0; m < n_mg_modes_; m++) {
      vcl_stringstream mg_fname;   
      mg_fname << base_dir << "/mg_mode" <<vcl_setw(2) << vcl_setfill('0') << m << "_" << vcl_setw(5) << vcl_setfill('0') << z << "." << image_format_;
      mg_image_fnames_[m].push_back(mg_fname.str());
    }
  }
}

//: Initialize the voxel grid with default values. Any exisiting data on disk will be overwritten.
bool breg3d_voxel_grid::init_voxels()
{
  float init_prob = 1.0f / nz_;
  float init_mg_mean = 0.5f;
  float init_mg_std = 0.1f;
  float init_mg_weight = 0.0f; // weight is 0 so mean and std dont really matter.
  
  return init_voxels(init_prob, init_mg_mean, init_mg_std, init_mg_weight);

}

//: Initialize the voxel grid. Any exisiting data on disk will be overwritten.
bool breg3d_voxel_grid::init_voxels(float init_prob, float init_mg_means, float init_mg_stds, float init_mg_weights)
{
  vil_image_view<float> prob_init_view(nx_,ny_,1);
  prob_init_view.fill(init_prob);

  vil_image_view<float> mg_init_view(nx_,ny_,1,3);
  vil_image_view<float> mg_mean_view = vil_plane(mg_init_view,0);
  vil_image_view<float> mg_std_view = vil_plane(mg_init_view,1);
  vil_image_view<float> mg_weight_view = vil_plane(mg_init_view,2);

  mg_mean_view.fill(init_mg_means);
  mg_std_view.fill(init_mg_stds);
  mg_weight_view.fill(init_mg_weights);

  for (unsigned z=0; z<nz_; z++) {
    vil_save(prob_init_view, prob_image_fnames_[z].c_str());
    for (unsigned m=0; m<n_mg_modes_; m++) {
      vil_save(mg_init_view, mg_image_fnames_[m][z].c_str());
    }
  }
  return true;
}


inline void breg3d_voxel_grid::load_prob_slice(vil_image_view<float> &data_prob, unsigned k)
{
  data_prob.clear();
  data_prob = vil_load(prob_image_fnames_[k].c_str());
  return;
}

inline void breg3d_voxel_grid::load_mg_slice(vcl_vector<vil_image_view<float> > &data_mg, unsigned k)
{
  data_mg.clear();
  for (unsigned m=0; m < n_mg_modes_; m++) {
    data_mg.push_back(vil_load(mg_image_fnames_[m][k].c_str()));
  }
  return;
}

inline bool breg3d_voxel_grid::save_prob_slice(vil_image_view<float> &data_prob, unsigned k)
{
  return (vil_save(data_prob, prob_image_fnames_[k].c_str(),image_format_.c_str()));
}

inline bool breg3d_voxel_grid::save_mg_slice(vcl_vector<vil_image_view<float> > &data_mg, unsigned k)
{
  bool result = true;
  for (unsigned m=0; m<n_mg_modes_; m++) {
    result &= vil_save(data_mg[m], mg_image_fnames_[m][k].c_str(),image_format_.c_str());
  }

  return result;
}






//#define WRITE_IMAGES_DEBUG

bool breg3d_voxel_grid::update_voxels(vil_image_view<float> const& frame, vpgl_camera<double>* cam)
{
  // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_up; // homography to layer above, along camera rays
  vcl_vector<vgl_h_matrix_2d<double> > H_down; // homography to layer below, along camera rays

  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p, Hup, Hdown;
    Hdown.set_identity();
    H_down.push_back(Hdown);
    for (unsigned z=0; z < nz_; z++)
    {
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
      if (z > 0) {
        Hdown = H_img_to_plane[z-1]*H_plane_to_img[z];
        Hup=  H_img_to_plane[z]*H_plane_to_img[z-1];
        H_down.push_back(Hdown);
        H_up.push_back(Hup);
      }
    }
    Hup.set_identity();
    H_up.push_back(Hup);
  }

  // compute update in two passes.
  // First, traverse layers top down and compute
#ifdef USE_DISK_STORAGE
  vcl_vector<vcl_string> preX_fname = temporary_file_filenames(0,nz_);
  vcl_vector<vcl_string> PIvisX_fname = temporary_file_filenames(1,nz_);

  vil_image_view<float> preX(nx_,ny_,1,1);
  vil_image_view<float> PIvisX(nx_,ny_,1);


#else
  vil_image_view<float> preX_vol(nx_,ny_,nz_);
  vil_image_view<float> PIvisX_vol(nx_,ny_,nz_);

  vil_image_view<float> preX, PIvisX;
#endif

  vil_image_view<float> visX(nx_,ny_,1,1);
  vil_image_view<float> PIPX(nx_,ny_,1);

  vil_image_view<float> preX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> visX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> img_scratch(frame.ni(),frame.nj(),1);

  preX_accum.fill(0.0f);
  visX_accum.fill(1.0f);
 
  vcl_cout << "Pass 1: z = " << vcl_endl;

  vil_image_view<float> frame_backproj(nx_,ny_,1);


  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << z << " ";

#ifndef USE_DISK_STORAGE
    preX = vil_plane(preX_vol,z);
    PIvisX = vil_plane(PIvisX_vol,z);
#endif

    vil_image_view<float> slice_prob;
    vcl_vector<vil_image_view<float> > slice_mgs;
    
    this->load_prob_slice(slice_prob,z);
    this->load_mg_slice(slice_mgs,z);

    // transform preX to voxel plane for this level
    transform_plane(preX_accum, H_plane_to_img[z], preX);
    // transform visX to voxel plane for this level
    transform_plane(visX_accum, H_plane_to_img[z], visX);

#ifdef USE_DISK_STORAGE
    // save previous sum as preX
    vil_save(preX,preX_fname[z].c_str());
#endif

    // backproject image onto voxel plane

    transform_plane(frame,H_plane_to_img[z],frame_backproj);
    
    // initialize PIvisX with PI(X)
    vil_image_view<float> PI = pixel_probabilities(frame_backproj,slice_mgs);

#ifdef WRITE_IMAGES_DEBUG
    vil_save(PI,"d:/dec/matlab/reg3d/data/seq2_voxels/debug/PI.tiff");
#endif

    // now multiply by visX
 
    vil_math_image_product(visX,PI,PIvisX);

#ifdef USE_DISK_STORAGE
    // save P(I)*visX for next pass
    vil_save(PIvisX,PIvisX_fname[z].c_str());
#endif

    // update appearance model
    this->update_mg_models(slice_mgs, slice_prob, frame_backproj);
    // save apearance model
    this->save_mg_slice(slice_mgs,z);
    
  
    vil_math_image_product(PI,slice_prob,PIPX);
    transform_plane(PIPX, H_img_to_plane[z], img_scratch);

    // multiply by visX to get PI*PX*visX
    vil_math_image_product(img_scratch,visX_accum,img_scratch);
    // add to preX 
    vil_math_image_sum(img_scratch,preX_accum,preX_accum);

    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    transform_plane(slice_prob, H_img_to_plane[z], img_scratch);
    // note: doing scale and offset in image domain so invalid pixels become 1.0 and dont affect visX
    vil_math_scale_and_offset_values(img_scratch, -1.0, 1.0);
    // updatate visX
    vil_math_image_product(img_scratch, visX_accum, visX_accum);

#ifdef WRITE_IMAGES_DEBUG
    // save for debug
    vil_save(preX_accum,"d:/dec/matlab/reg3d/data/seq2_voxels/debug/preX_accum.tiff");
     //DEBUG
    vil_save(visX_accum,"d:/dec/matlab/reg3d/data/seq2_voxels/debug/visX_accum.tiff");
#endif
  }


  // now traverse layers bottom up, computing new P(X) along the way.

  vil_image_view<float> preX_accum_vox(nx_,ny_,1);
  vil_image_view<float> visX_accum_vox(nx_,ny_,1);

  vcl_cout << vcl_endl << "Pass 2. z = ";
  for (unsigned z = 0; z < nz_; z++) {
    vcl_cout << z << " ";

    // load current layers probability computed in pass #1
#ifdef USE_DISK_STORAGE
    PIvisX = vil_load(PIvisX_fname[z].c_str());
    // load current layer's preX computed in pass #1
    preX = vil_load(preX_fname[z].c_str());
#else
    PIvisX = vil_plane(PIvisX_vol,z);
    preX = vil_plane(preX_vol,z);
#endif
    
    // load slice probability
    vil_image_view<float> slice_prob;
    load_prob_slice(slice_prob,z);

    // transform preX_sum to current level
    transform_plane(preX_accum, H_plane_to_img[z], preX_accum_vox);
    // transform visX_sum to current level
    transform_plane(visX_accum, H_plane_to_img[z], visX_accum_vox);
    // DEBUG

#ifdef WRITE_IMAGES_DEBUG
    vil_save(preX_accum_vox,"d:/dec/matlab/reg3d/data/seq2_voxels/debug/preX_accum_vox.tiff");
    // backproject image onto voxel plane
    vil_image_view<float> frame_backproj(nx_,ny_,1);
    transform_plane(frame,H_plane_to_img[z],frame_backproj);
    vil_save(frame_backproj,"d:/dec/matlab/reg3d/data/seq2_voxels/debug/frame_backproj.tiff");
#endif

    const float preX_sum_thresh = 0.01f;

    vil_image_view<float>::const_iterator preX_it = preX.begin(), PIvisX_it = PIvisX.begin(), preX_sum_it = preX_accum_vox.begin(), visX_sum_it = visX_accum_vox.begin();
    vil_image_view<float>::iterator PX_it = slice_prob.begin();
    for (; PX_it != slice_prob.end(); PX_it++, preX_it++, PIvisX_it++, preX_sum_it++, visX_sum_it++) {
      // if preX_sum is zero at the voxel, no ray passed through the voxel (out of image)
      if (*preX_sum_it > preX_sum_thresh) {
        float multiplier = (*PIvisX_it + *preX_it) / *preX_sum_it;
        float ray_norm = 1 - *visX_sum_it; //normalize based on probability that a surface voxel is located along the ray. This was not part of the original Pollard + Mundy algorithm.
        //if (*PX_it * multiplier > 1.0)
          //vcl_cout << "uh oh.\n";
        *PX_it *= multiplier * ray_norm;
      }
      if (*PX_it < min_vox_prob_)
        *PX_it = min_vox_prob_;
      if (*PX_it > max_vox_prob_)
        *PX_it = max_vox_prob_;
    }
    
    // save updated voxel probabilites
    this->save_prob_slice(slice_prob,z);

  }
  vcl_cout << vcl_endl << "done." << vcl_endl;
  return true;
}

vil_image_view<float> breg3d_voxel_grid::camera_cost(vil_image_view<float> const& frame, vpgl_camera<double>* cam)
{
  // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_up; // homography to layer above, along camera rays
  vcl_vector<vgl_h_matrix_2d<double> > H_down; // homography to layer below, along camera rays

  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p, Hup, Hdown;
    Hdown.set_identity();
    H_down.push_back(Hdown);
    for (unsigned z=0; z < nz_; z++)
    {
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
      if (z > 0) {
        Hdown = H_img_to_plane[z-1]*H_plane_to_img[z];
        Hup=  H_img_to_plane[z]*H_plane_to_img[z-1];
        H_down.push_back(Hdown);
        H_up.push_back(Hup);
      }
    }
    Hup.set_identity();
    H_up.push_back(Hup);
  }



  vil_image_view<float> preX, visX, slice_prob, PIPX;
  vil_image_view<float> frame_backproj(nx_,ny_,1);

  //vil_image_view<float> preX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> visX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> cost_accum(frame.ni(), frame.nj(),1);

  vil_image_view<float> img_scratch(frame.ni(),frame.nj(),1);
  vcl_vector<vil_image_view<float> > slice_mgs;

  //preX_accum.fill(0.0f);
  cost_accum.fill(1.0f);
  visX_accum.fill(1.0f);
 
  vcl_cout << "Computing camera cost: ";

  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;
  
    this->load_prob_slice(slice_prob,z);
    this->load_mg_slice(slice_mgs,z);

    // transform preX to voxel plane for this level
    //transform_plane(preX_accum, H_plane_to_img[z], preX);
    // transform visX to voxel plane for this level
    transform_plane(visX_accum, H_plane_to_img[z], visX);

    // backproject image onto voxel plane
    transform_plane(frame,H_plane_to_img[z],frame_backproj);
 
    // initialize PI*PX with PI
    vil_image_view<float> PIPX = this->pixel_probabilities_range(frame_backproj,slice_mgs);
   // float maxprob,minprob;
   // vil_math_value_range(PIPX,minprob,maxprob);
    //vcl_cout << "min PI = " << minprob << "   max PI = " << maxprob << vcl_endl;

    // and multiply by PX
    vil_math_image_product(slice_prob,PIPX,PIPX);

    // now transform to image plane
    transform_plane(PIPX, H_img_to_plane[z], img_scratch);

    vil_image_view<float> slice_prob_img(frame.ni(), frame.nj(), 1);
    transform_plane(slice_prob, H_img_to_plane[z], slice_prob_img);

    vil_image_view<float>::const_iterator PIPX_it = img_scratch.begin(), PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator visX_it = visX_accum.begin();//, preX_it = preX_accum.begin();
    vil_image_view<float>::iterator cost_it = cost_accum.begin();

    for (; cost_it != cost_accum.end(); PX_it++, PIPX_it++, visX_it++, cost_it++) {
      float PIPXvisX = *PIPX_it * *visX_it;
      //*preX_it += PIPXvisX;
      *cost_it *= (1 - PIPXvisX);
      *visX_it *= (1 - *PX_it);
    }
  }
  vcl_cout << vcl_endl;

  return cost_accum;
}


vil_image_view<float> breg3d_voxel_grid::camera_score(vil_image_view<float> const& frame, vpgl_camera<double>* cam)
{

 // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_up; // homography to layer above, along camera rays
  vcl_vector<vgl_h_matrix_2d<double> > H_down; // homography to layer below, along camera rays

  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p, Hup, Hdown;
    Hdown.set_identity();
    H_down.push_back(Hdown);
    for (unsigned z=0; z < nz_; z++)
    {
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
      if (z > 0) {
        Hdown = H_img_to_plane[z-1]*H_plane_to_img[z];
        Hup=  H_img_to_plane[z]*H_plane_to_img[z-1];
        H_down.push_back(Hdown);
        H_up.push_back(Hup);
      }
    }
    Hup.set_identity();
    H_up.push_back(Hup);
  }



  vil_image_view<float> preX, visX, slice_prob, PIPX;
  vil_image_view<float> frame_backproj(nx_,ny_,1);

  vil_image_view<float> preX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> visX_accum(frame.ni(),frame.nj(),1);
  //vil_image_view<float> score_accum(frame.ni(), frame.nj(),1);

  vil_image_view<float> img_scratch(frame.ni(),frame.nj(),1);
  vcl_vector<vil_image_view<float> > slice_mgs;

  preX_accum.fill(0.0f);
  //score_accum.fill(1.0f);
  visX_accum.fill(1.0f);
 
  vcl_cout << "Computing camera score: z = " << vcl_endl;


  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;
  
    this->load_prob_slice(slice_prob,z);
    this->load_mg_slice(slice_mgs,z);

    // transform preX to voxel plane for this level
    transform_plane(preX_accum, H_plane_to_img[z], preX);
    // transform visX to voxel plane for this level
    transform_plane(visX_accum, H_plane_to_img[z], visX);

    // backproject image onto voxel plane
    transform_plane(frame,H_plane_to_img[z],frame_backproj);
 
    // initialize PI*PX with PI
    vil_image_view<float> PIPX = pixel_probabilities(frame_backproj,slice_mgs);

    // and multiply by PX
    vil_math_image_product(slice_prob,PIPX,PIPX);

    // now transform to image plane
    transform_plane(PIPX, H_img_to_plane[z], img_scratch);

#ifdef USE_VIL_MATH_OPS
    // and multiply by visX to get PI*PX*visX
    vil_math_image_product(img_scratch,visX_accum,img_scratch);
    // add to preX 
    vil_math_image_sum(img_scratch,preX_accum,preX_accum);

    // we need 1 - PIPXvisX for the score
    vil_math_scale_and_offset_values(img_scratch,-1.0,1.0);
    // accumulate score
    vil_math_image_product(img_scratch,score_accum,score_accum);

    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    transform_plane(slice_prob, H_img_to_plane[z], img_scratch);
    // note: doing scale and offset in image domain so invalid pixels become 1.0 and dont affect visX
    vil_math_scale_and_offset_values(img_scratch, -1.0, 1.0);
    // updatate visX
    vil_math_image_product(img_scratch, visX_accum, visX_accum);
#else
    vil_image_view<float> slice_prob_img(frame.ni(), frame.nj(), 1);
    transform_plane(slice_prob, H_img_to_plane[z], slice_prob_img);

    vil_image_view<float>::const_iterator PIPX_it = img_scratch.begin(), PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator visX_it = visX_accum.begin(), preX_it = preX_accum.begin();
    //vil_image_view<float>::iterator score_it = score_accum.begin();

    for (; preX_it != preX_accum.end(); PX_it++, PIPX_it++, visX_it++, preX_it++/*, score_it++*/) {
      float PIPXvisX = *PIPX_it * *visX_it;
      *preX_it += PIPXvisX;
      //*score_it += PIPXvisX;
      *visX_it *= (1 - *PX_it);
    }

#endif


  }
  // should somehow invert preX_accum if you want small = good
  // A - preX_accum is troublesome because there is no well-defined max value A, since PI is a sample of a PDF function, not a probability
  // A/preX_accum is no good because 0 is a perfectly valid value.
  // perhaps A/(preX_accum + A) ?

  return preX_accum;
}

#ifdef VIRTUAL_VIEW1

//: Given an image, corresponding camera, and virtual camera, produce a rendered image from the virtual camera
//: virtual_frame and vis should be pre-sized accordingly.
void breg3d_voxel_grid::virtual_view(vil_image_view<float> const& frame, vpgl_camera<double> const* cam, vpgl_camera<double> const* vcam, 
                    vil_image_view<float> virtual_frame, vil_image_view<float> vis)
{
  // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;

  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;

  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < nz_; z++)
    {
      // real camera
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
      // virtual camera
      compute_plane_image_H(vcam,z,Hp2i,Hi2p);
      H_plane_to_virtual_img.push_back(Hp2i);
      H_virtual_img_to_plane.push_back(Hi2p);
    }
  }

  // volume to hold the backprojected color from the original frame
  vil_image_view<float> color_vol(nx_,ny_,nz_);
  // volume to hold the probability of visibility from the original frame
  vil_image_view<float> visX_vol(nx_,ny_,nz_);


  vil_image_view<float> slice_prob;
  vil_image_view<float> slice_prob_img(frame.ni(), frame.nj(), 1);

  vcl_cout << "Generating virtual frame, pass 1:";

  // first, fill voxel grid with color from frame and calcuate visibilty 
  vil_image_view<float> visX_accum(frame.ni(),frame.nj(),1);
  visX_accum.fill(1.0f);

  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;
  
    vil_image_view<float> visX = vil_plane(visX_vol,z);
    transform_plane(visX_accum, H_plane_to_img[z], visX);

    // backproject image onto voxel plane
    vil_image_view<float> frame_backproj = vil_plane(color_vol,z);
    transform_plane(frame,H_plane_to_img[z],frame_backproj);
 
    // load slice probability..
    this->load_prob_slice(slice_prob,z);
    transform_plane(slice_prob, H_img_to_plane[z], slice_prob_img);

    // ..and update visX
    vil_image_view<float>::const_iterator PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator visX_it = visX_accum.begin();
    for (; visX_it != visX_accum.end(); visX_it++, PX_it++) {
      *visX_it *= (1 - *PX_it);
    }
    //vil_save(visX_accum,"d:/dec/matlab/reg3d/results/visX_debug.tiff");

  }
  vcl_cout << vcl_endl;

  // now project color of most likely voxel into virtual image

  // initialize the virtual frame and visibility
  virtual_frame.fill(0.0);
  vis.fill(0.0);

  vil_image_view<float> max_prob_image(virtual_frame.ni(),virtual_frame.nj(),1);
  max_prob_image.fill(0.0f);

  vil_image_view<vxl_byte> depth_img(virtual_frame.ni(),virtual_frame.nj(),1);
  depth_img.fill(255);

  vil_image_view<float> slice_image(virtual_frame.ni(),virtual_frame.nj(),1);
  vil_image_view<float> visX_real_image(virtual_frame.ni(),virtual_frame.nj(),1);

  vil_image_view<float> visX_virtual(nx_,ny_,1);
  // reinitialize visX
  visX_accum.set_size(virtual_frame.ni(),virtual_frame.nj(),1);
  visX_accum.fill(1.0f);

  vcl_cout << "Generating virtual frame, pass 2:";
  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;

    // project current slice into image
    vil_image_view<float> slice = vil_plane(color_vol,z);
    transform_plane(slice,H_virtual_img_to_plane[z],slice_image);
    // project current (real camera) vis probs into image
    vil_image_view<float> visX_real = vil_plane(visX_vol,z);
    transform_plane(visX_real, H_virtual_img_to_plane[z],visX_real_image);
 
    // get visX for virtual camera
    transform_plane(visX_accum, H_plane_to_img[z], visX_virtual);

    // load slice probability
    this->load_prob_slice(slice_prob,z);
    transform_plane(slice_prob, H_virtual_img_to_plane[z], slice_prob_img);

    // compute PXvisX for virtual cam and update visX
    vil_image_view<float>::const_iterator PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator visX_it = visX_accum.begin(), max_it = max_prob_image.begin(), img_it = slice_image.begin(), real_vis_it = visX_real_image.begin();
    vil_image_view<float>::iterator vimg_it = virtual_frame.begin(), visout_it = vis.begin();
    vil_image_view<vxl_byte>::iterator depth_it = depth_img.begin();
    for (; vimg_it != virtual_frame.end(); PX_it++, visX_it++, max_it++, img_it++, real_vis_it++, vimg_it++, visout_it++, depth_it++) {
      float PXvisX = (*visX_it) * (*PX_it);
      if (PXvisX > *max_it) {
        *max_it = PXvisX;
        *vimg_it = *img_it;
        *visout_it = *real_vis_it;
        *depth_it = z;
      }
      // update virtual visX
      *visX_it *= (1 - *PX_it);
    }

  }
  vcl_cout << vcl_endl;

  vil_save(depth_img,"d:/dec/matlab/reg3d/results/seq2/depth_img.tiff");

  return;
}

#else

//: Given an image, corresponding camera, and virtual camera, produce a rendered image from the virtual camera
//: virtual_frame and vis should be pre-sized accordingly.
void breg3d_voxel_grid::virtual_view(vil_image_view<float> const& frame, vpgl_camera<double> const* cam, vpgl_camera<double> const* vcam, 
                    vil_image_view<float> &virtual_frame, vil_image_view<float> &vis, vil_image_view<unsigned char> &height_map)
{
  // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;

  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_img;

  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < nz_; z++)
    {
      // real camera
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
      // virtual camera
      compute_plane_image_H(vcam,z,Hp2i,Hi2p);
      H_plane_to_virtual_img.push_back(Hp2i);
      H_virtual_img_to_plane.push_back(Hi2p);
      // image to image
      H_virtual_img_to_img.push_back(H_plane_to_img.back()*H_virtual_img_to_plane.back());
    }
  }

  vil_image_view<float> slice_prob;
  vil_image_view<float> slice_prob_img(virtual_frame.ni(), virtual_frame.nj(), 1);

  vil_image_view<float> visX_accum(virtual_frame.ni(),virtual_frame.nj(),1);
  visX_accum.fill(1.0f);

  // assuming voxel grid has 256 or less levels by using unsigned char here
  vil_image_view<unsigned char> height_map_rough(virtual_frame.ni(),virtual_frame.nj(),1);
  height_map_rough.fill(255);

  vil_image_view<float> max_prob_image(virtual_frame.ni(),virtual_frame.nj(),1);
  max_prob_image.fill(0.0f);
 
  vcl_cout << "Pass1: generating height map from virtual camera:";
  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;

    // load slice probability
    this->load_prob_slice(slice_prob,z);
    transform_plane(slice_prob, H_virtual_img_to_plane[z], slice_prob_img);

    // compute PXvisX for virtual cam and update visX
    vil_image_view<float>::const_iterator PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator max_it = max_prob_image.begin(), visX_it = visX_accum.begin();
    vil_image_view<unsigned char>::iterator height_it = height_map_rough.begin();
   
    for (; height_it != height_map_rough.end(); height_it++, PX_it++, max_it++, visX_it++) {
      float PXvisX = (*visX_it) * (*PX_it);
      if (PXvisX > *max_it) {
        *max_it = PXvisX;       
        *height_it = z;
      }
      // update virtual visX
      *visX_it *= (1 - *PX_it);
    }
  }
  vcl_cout << vcl_endl;
  
  // now clean up height_map
  unsigned n_smooth_iterations = 10;
  float conf_thresh = 0.05f;

  vcl_cout << "smoothing height map:";
  vil_image_view<bool> conf_mask(virtual_frame.ni(),virtual_frame.nj(),1);
  vil_threshold_above(max_prob_image,conf_mask,conf_thresh);
  vil_image_view<float> filtered_height;

  filtered_height = brip_vil_float_ops::convert_to_float(height_map_rough);
  for (unsigned i=0; i < n_smooth_iterations; i++) {
    vcl_cout << ".";
    // smooth heightmap
    filtered_height = brip_vil_float_ops::gaussian(filtered_height,1.0);
    // reset values we are confident enough in
    vil_image_view<float>::iterator filtered_it = filtered_height.begin();
    vil_image_view<unsigned char>::iterator height_it = height_map_rough.begin();
    vil_image_view<bool>::iterator mask_it = conf_mask.begin();
    for (; filtered_it != filtered_height.end(); filtered_it++, mask_it++, height_it++) {
      if (*mask_it)
        *filtered_it = *height_it;
    }
  }
  height_map = brip_vil_float_ops::convert_to_byte(filtered_height);
  vcl_cout << "done." << vcl_endl;
    
  // create virtual image based on smoothed height_map
  vil_image_view<float> frame_backproj(nx_,ny_,1);
  vil_image_view<float> frame_virtualproj(virtual_frame.ni(),virtual_frame.nj(),1);


  slice_prob_img.set_size(frame.ni(),frame.nj(),1);

  vil_image_view<float> PX_accum(frame.ni(),frame.nj(),1);
  vil_image_view<float> PX_accum_virtualproj(virtual_frame.ni(),virtual_frame.nj(),1);
  PX_accum.fill(0.0f);

  vcl_cout << "Pass2: generating virtual image:";

  virtual_frame.fill(0.0f);
  vis.fill(0.0f);

  for (unsigned zcount=0; zcount < nz_; zcount++) {
    //int z = nz_ - zcount - 1;
    int z = zcount;
    vcl_cout << "." ;

    // project image to virtual image
    transform_plane(frame,H_virtual_img_to_img[z],frame_virtualproj);

    // project slice probs into real camera
    load_prob_slice(slice_prob,z);
    transform_plane(slice_prob,H_img_to_plane[z],slice_prob_img);

    // update PX_accum
    vil_image_view<float>::iterator PX_it = slice_prob_img.begin(), PX_accum_it = PX_accum.begin();
    for(; PX_accum_it != PX_accum.end(); PX_accum_it++, PX_it++) {
      *PX_accum_it += *PX_it;
    }

    // project PX_accum from image to virtual image
    transform_plane(PX_accum,H_virtual_img_to_img[z],PX_accum_virtualproj);

    vil_image_view<float>::iterator frame_it = frame_virtualproj.begin(), vframe_it = virtual_frame.begin();
    vil_image_view<unsigned char>::iterator height_it = height_map.begin();
    vil_image_view<float>::iterator PX_accum_vproj_it = PX_accum_virtualproj.begin(), vis_it = vis.begin();

    for(; vframe_it != virtual_frame.end(); frame_it++, vframe_it++, height_it++, vis_it++, PX_accum_vproj_it++) {
      // fill in virtual image
      if (*height_it == z) {
        *vframe_it = *frame_it;
        *vis_it = *PX_accum_vproj_it;
      }
    }
  }
  vcl_cout << vcl_endl;

  // normalize vis
  vcl_cout << " Normalizing vis:";
  for (unsigned z=0; z < nz_; z++) {
    vcl_cout << ".";

    // project PX_accum from image to virtual image
    transform_plane(PX_accum,H_virtual_img_to_img[z],PX_accum_virtualproj);

    vil_image_view<unsigned char>::iterator height_it = height_map.begin();
    vil_image_view<float>::iterator PX_accum_vproj_it = PX_accum_virtualproj.begin(), vis_it = vis.begin();
    for(; vis_it != vis.end(); vis_it++, height_it++, PX_accum_vproj_it++) {
      if (*height_it == z) {
        *vis_it /= *PX_accum_vproj_it;
      }
    }
  }
  vcl_cout << vcl_endl;


  vil_save(height_map_rough,"c:/project/registration/results/seq2/height_img.tiff");
  //vil_save(height_map,"d:/dec/matlab/reg3d/results/seq2/filtered_height_img.tiff");
  vil_save(max_prob_image,"c:/project/registration/results/seq2/conf_img.tiff");
  
  return;
}

#endif

void breg3d_voxel_grid::expected_image(const vpgl_camera<double> *cam, vil_image_view<float> &expected_image, vil_image_view<float> &mask)
{

  // compute homographies from voxel planes to image cooridnates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < nz_; z++)
    {
      compute_plane_image_H(cam,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  vil_image_view<float> slice_prob;
  vil_image_view<float> slice_prob_img(expected_image.ni(),expected_image.nj(),1);
  vil_image_view<float> intensities_img(expected_image.ni(),expected_image.nj(),1);

  vil_image_view<float> PXvisX_accum(expected_image.ni(),expected_image.nj(),1);
  vil_image_view<float> visX_accum(expected_image.ni(), expected_image.nj(),1);

  vcl_vector<vil_image_view<float> > slice_mgs;

  PXvisX_accum.fill(0.0f);
  visX_accum.fill(1.0f);
  expected_image.fill(0.0f);
 
  vcl_cout << "Generating Expected Image:" << vcl_endl;
  for (unsigned zcount=0; zcount < nz_; zcount++) {
    int z = nz_ - zcount - 1;
    vcl_cout << "." ;
  
    this->load_prob_slice(slice_prob,z);
    this->load_mg_slice(slice_mgs,z);

    // generate expected level view
    vil_image_view<float> intensities = expected_intensities(slice_mgs);
    // and project to image plane
    transform_plane(intensities,H_img_to_plane[z],intensities_img);

    // transform slice prob. to image plane
    transform_plane(slice_prob,H_img_to_plane[z],slice_prob_img);

    vil_image_view<float>::const_iterator I_it = intensities_img.begin(), PX_it = slice_prob_img.begin();
    vil_image_view<float>::iterator img_it = expected_image.begin(), visX_it = visX_accum.begin(), W_it = PXvisX_accum.begin();

    for(;img_it != expected_image.end(); img_it++, visX_it++, W_it++, I_it++, PX_it++) {
      float w = *PX_it * *visX_it;
      *W_it += w;
      *img_it += *I_it * w;
      // update visX for next level 
      *visX_it *= (1.0f - *PX_it);
    }
  }
  vcl_cout << vcl_endl;

  // normalize expected image by weight sum
  vil_math_image_ratio(expected_image,PXvisX_accum,expected_image);
  // create mask of visible region
  //vil_threshold_above(PXvisX_accum,mask,0.001f);
  mask = brip_vil_float_ops::threshold(PXvisX_accum,0.001f,1.0f);

  return;
}

vil_image_view<float> breg3d_voxel_grid::pixel_probabilities(const vil_image_view<float> &backproj_frame, const vcl_vector<vil_image_view<float> > &mg_modes)
{

  const double one_over_sqrt2pi = 1/sqrt(2*vnl_math::pi);

  vil_image_view<float> pix_probs(nx_,ny_,1);
  // fill with some epsilon value to ensure probability is never exactly zero.
  pix_probs.fill(0.0f);
  vil_image_view<float> weight_sum(nx_,ny_,1);
  weight_sum.fill(0.0f);

  // compute sum of mode weights at each voxel
  for (unsigned m=0; m<mg_modes.size(); m++) {
    vil_image_view<float> weight_plane = vil_plane(mg_modes[m],2);
    vil_math_image_sum(weight_plane,weight_sum,weight_sum);
  }
  // sum probability value of each mode
  for (unsigned m=0; m<mg_modes.size(); m++) {
    //vil_image_view<float> mg_mean = vil_plane(mg_modes[m],0);
    //vil_image_view<float> mg_std = vil_plane(mg_modes[m],1);
    //vil_image_view<float> mg_weight = vil_plane(mg_modes[m],2);

    // no vil function for exponentials, compute in a loop
    vil_image_view<float> nu(nx_,ny_,1);

    vil_image_view<float>::const_iterator img_it=backproj_frame.begin(), W_it = weight_sum.begin(), mg_it = mg_modes[m].begin();
    vil_image_view<float>::iterator prob_it=pix_probs.begin();
    
    for (; prob_it!= pix_probs.end(); img_it++, W_it++, prob_it++) {
      float mg_mean = *mg_it++;
      float mg_std = *mg_it++;
      float mg_weight = *mg_it++;
      if (mg_weight > 0.0)
        *prob_it += (float)((mg_weight * one_over_sqrt2pi) / (mg_std * *W_it)) * vcl_exp(-vnl_math_sqr((*img_it - mg_mean)/(vnl_math::sqrt2 * mg_std)));
      else if (*W_it == 0)
        *prob_it = 1.0;
    }
  }
  return pix_probs;
}


vil_image_view<float> breg3d_voxel_grid::pixel_probabilities_range(const vil_image_view<float> &backproj_frame, const vcl_vector<vil_image_view<float> > &mg_modes, float delta)
{

  const double one_over_sqrt2pi = 1/sqrt(2*vnl_math::pi);

  vil_image_view<float> pix_probs(nx_,ny_,1);
  // fill with some epsilon value to ensure probability is never exactly zero.
  pix_probs.fill(0.0f);
  vil_image_view<float> weight_sum(nx_,ny_,1);
  weight_sum.fill(0.0f);

  // compute sum of mode weights at each voxel
  for (unsigned m=0; m<mg_modes.size(); m++) {
    vil_image_view<float> weight_plane = vil_plane(mg_modes[m],2);
    vil_math_image_sum(weight_plane,weight_sum,weight_sum);
  }
  // sum probability value of each mode
  for (unsigned m=0; m<mg_modes.size(); m++) {
    //vil_image_view<float> mg_mean = vil_plane(mg_modes[m],0);
    //vil_image_view<float> mg_std = vil_plane(mg_modes[m],1);
    //vil_image_view<float> mg_weight = vil_plane(mg_modes[m],2);

    // no vil function for exponentials, compute in a loop
    vil_image_view<float> nu(nx_,ny_,1);

    vil_image_view<float>::const_iterator img_it=backproj_frame.begin(), W_it = weight_sum.begin(), mg_it = mg_modes[m].begin();
    vil_image_view<float>::iterator prob_it=pix_probs.begin();
    
    for (; prob_it!= pix_probs.end(); img_it++, W_it++, prob_it++) {
      float mg_mean = *mg_it++;
      float mg_std = *mg_it++;
      float scale_factor = *mg_it++ / (*W_it*2);
      if (scale_factor > 0.0) {
         float cdf_plus  = (float)(scale_factor * (1 + vnl_erf((*img_it + delta - mg_mean)/(mg_std * vnl_math::sqrt2))));
         float cdf_minus = (float)(scale_factor * (1 + vnl_erf((*img_it - delta - mg_mean)/(mg_std * vnl_math::sqrt2))));
        *prob_it += (cdf_plus - cdf_minus);
      }
      else if (*W_it == 0)
        *prob_it = 1.0;
    }
  }
  return pix_probs;
}

vil_image_view<float> breg3d_voxel_grid::expected_intensities(vcl_vector<vil_image_view<float> > const& mg_modes)
{
  
  vil_image_view<float> expected(nx_,ny_,1);
  expected.fill(0.0);

  vil_image_view<float> weight_sum(nx_,ny_,1);
  weight_sum.fill(0.0f);

  // compute sum of mode weights at each voxel
  for (unsigned m=0; m<mg_modes.size(); m++) {
    vil_image_view<float> weight_plane = vil_plane(mg_modes[m],2);
    vil_math_image_sum(weight_plane,weight_sum,weight_sum);
  }
  // sum probability value of each mode
  for (unsigned m=0; m<mg_modes.size(); m++) {
   
    vil_image_view<float>::const_iterator mg_it =  mg_modes[m].begin();
    vil_image_view<float>::iterator img_it= expected.begin(), W_it = weight_sum.begin();
    
    for (; img_it!= expected.end(); img_it++, W_it++) {
      float mg_mean = *mg_it++;
      float mg_std = *mg_it++;
      float scale_factor = *mg_it++;
      if (*W_it > 0)
        *img_it += (mg_mean * scale_factor)/(*W_it);
    }
  }
  
  return expected;
}


vcl_vector<vcl_string> breg3d_voxel_grid::temporary_file_filenames(unsigned temp_idx, unsigned nfiles)
{
  vcl_vector<vcl_string> fnames;
  vcl_stringstream temp_dir_ss;
  temp_dir_ss << base_dirname_ << "/temp";
  vcl_string temp_dir = temp_dir_ss.str();

  if (!vul_file::is_directory(temp_dir)) {
    vul_file::make_directory(temp_dir);
  }

  for (unsigned i=0; i < nfiles; i++) {
    vcl_stringstream fname;
    fname << temp_dir << "/temp" << temp_idx << "_" << vcl_setw(5) << vcl_setfill('0') << i << "." << image_format_;
    fnames.push_back(fname.str());
  }

  return fnames;
}



void breg3d_voxel_grid::compute_plane_image_H(vpgl_camera<double> const* cam, unsigned grid_zval, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane)
{
    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_img;
    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_vox;

    // create vectors containing four corners of grid, and their projections into the image
    double u=0, v=0;
    vgl_point_3d<double> corner_world;

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,0));
    corner_world = corner_ + k_step_*grid_zval;
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(nx_-1,0));
    corner_world = corner_ + k_step_*grid_zval + i_step_*(nx_-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(nx_-1,ny_-1));
    corner_world = corner_ + k_step_*grid_zval + i_step_*(nx_-1) + j_step_*(ny_-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,(ny_-1)));
    corner_world = corner_ + k_step_*grid_zval + j_step_*(ny_-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));


    vgl_h_matrix_2d_compute_linear comp_4pt;
    if (!comp_4pt.compute(voxel_corners_img,voxel_corners_vox, H_image_to_plane)) {
      vcl_cerr << "ERROR computing homography from image to voxel slice. " << vcl_endl;
    }
    if (!comp_4pt.compute(voxel_corners_vox,voxel_corners_img, H_plane_to_image)) {
      vcl_cerr << "ERROR computing homography from voxel slice to image. " << vcl_endl;
    }
    return;
}





void breg3d_voxel_grid::transform_plane(vil_image_view<float> const& input, vgl_h_matrix_2d<double> const& invH, vil_image_view<float>& output, double off_x, double off_y)
{
  vimt_transform_2d trans_proj;
  trans_proj.set_projective(invH.get_matrix());

  breg3d_util::resample_image(input, output, trans_proj, off_x,off_y);

  return;
}

void breg3d_voxel_grid::update_mg_models(vcl_vector<vil_image_view<float> >& slice_mg, vil_image_view<float> const& slice_prob, vil_image_view<float> const& backproj_frame)
{

  vcl_vector<vil_image_view<float>::iterator> mg_it;
  for (unsigned m=0; m<slice_mg.size(); m++) 
    mg_it.push_back(slice_mg[m].begin());

  vil_image_view<float>::const_iterator PX_it = slice_prob.begin(), frame_it = backproj_frame.begin();
  for (; frame_it != backproj_frame.end(); frame_it++,PX_it++) {
    float c = *frame_it, dw = *PX_it;
    float best_score = 0, worst_score = vnl_huge_val<float>(0.0f);
    int best_mode = -1, worst_mode = -1;

    for (unsigned m=0; m < mg_it.size(); m++) {
      float mode_mean = *(mg_it[m]);
      float mode_std = *(mg_it[m]+1);
      float mode_w = *(mg_it[m]+2);
      float mode_score = (mode_w > 0)? mode_w / mode_std : -1.0f ;
      // is pixel value within 2.5 std deviations of mode mean?
      float diff = c - mode_mean;
      if ((vcl_fabs(diff) <= 2.5*mode_std) && (mode_w > 0)){
        if (mode_score >= best_score) {
          best_score = mode_score;
          best_mode = m;
        }
      } else if (mode_score <= worst_score) {
        worst_score = mode_score;
        worst_mode = m;
      }
    }
    if (best_mode > -1) {
      unsigned m = best_mode;
      float weight_ratio = dw/(dw+ *(mg_it[m]+2));
      float diff = c - *mg_it[m];
      // update mean
      *(mg_it[m]) +=  weight_ratio * diff;
      // update std
      float std = *(mg_it[m]+1);
      *(mg_it[m]+1) = vcl_sqrt(std*std + weight_ratio *(diff*diff - std*std));
      // update weight
      *(mg_it[m]+2) += dw;
    }
    else {
      // nothing with 2.5 standard deviations, destroy worst mode and create new one.
      unsigned m = worst_mode;
      // mean at c
      *(mg_it[m])= c;
      // standard devation initialized with high value
      *(mg_it[m]+1) = std_init_value_;
      // weight initialized with voxel probability
      *(mg_it[m]+2) = dw;
    }
    // advance mg iterators
    for (unsigned m=0; m < mg_it.size(); m++) {
      mg_it[m] += 3;
    }
  }
  return;
}


