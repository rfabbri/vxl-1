#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_gauss_reduce.h>

#include <brip/brip_vil_float_ops.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vul/vul_timer.h>

#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_minimizer.h>

#include <breg3d/breg3d_voxel_grid.h>
#include <breg3d/breg3d_proj_camera_cost_function.h>
//#include <breg3d/breg3d_image_cost_function.h>
#include <breg3d/breg3d_util.h>

#include <vcl_iomanip.h>



int main(int argc, char* argv[])
{
  vgl_point_3d<double> grid_corner(2170.0, -1480.0, -30.0);
  const double vox_size = 0.5;

  //#define PC1
#ifdef PC1
  breg3d_voxel_grid voxels("d:/dec/matlab/reg3d/data/seq2_voxels",300,300,120,grid_corner,vox_size);
#else
  breg3d_voxel_grid voxels("c:/project/registration/data/seq2_voxels_bootstrap",300,300,120,grid_corner,vox_size);
#endif
  //voxels.init_voxels();
  bool first_camera = false;

  vcl_vector<vnl_double_3x3> Ks, Rs, Ks_opt, Rs_opt;
  vcl_vector<vnl_double_3x1> Ts, Ts_opt;
#ifdef PC1
  if(!breg3d_util::read_cameras("d:/dec/matlab/reg3d/data/seq2_KRT_cameras_90interp.txt",Ks,Rs,Ts)) {
    //if (!breg3d_util::read_cameras("d:/dec/matlab/reg3d/data/seq1_KRT_cameras.txt",Ks,Rs,Ts)) {
#else
  if(!breg3d_util::read_cameras("c:/project/registration/data/seq2/calib/seq2_KRT_cameras_90interp.txt",Ks,Rs,Ts)) {
#endif
    vcl_cerr << "error reading camera file." << vcl_endl;
    return -1;
  }

  // create virtual camera
  vnl_matrix_fixed<double,3,4> RT;

  //#define TOPDOWN_VCAM
#ifndef TOPDOWN_VCAM
  RT.set_columns(0,Rs[49]);
  RT.set_column(3,Ts[49].get_column(0));
#else
  vnl_matrix_fixed<double,3,3> vcamR;
  vcamR.set_identity();
  vcamR[1][1] = -1.0; vcamR[2][2] = -1.0;
  vnl_vector_fixed<double,3> vcam_center(2245.0, -1405.0, 475.0);
  vnl_vector<double> vcamT = -vcamR * vcam_center;
  RT.set_columns(0,vcamR);
  RT.set_column(3,vcamT);
#endif

  vnl_matrix_fixed<double,3,4> P = Ks[0] * RT;
  vpgl_proj_camera<double> vcam(P);



  //{ unsigned camera_idx = 49; 
  for (unsigned camera_idx = 0; camera_idx < 100; camera_idx++) {

    vcl_stringstream frame_fname;
#ifdef PC1
    frame_fname << "d:/dec/matlab/reg3d/data/seq2/frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".png";
    //frame_fname << "d:/dec/matlab/reg3d/data/seq1/vlcsnap-" << vcl_setw(2) << vcl_setfill('0') << (int)camera_idx << ".png";
#else
    frame_fname << "c:/project/registration/data/seq2/frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".png";
#endif
    vcl_cout << vcl_endl << "****  loading " << frame_fname.str() << "  ****" << vcl_endl << vcl_endl;

    vil_image_view<float> frame_view = breg3d_util::load_image(frame_fname.str());

    vcl_cout << "ni = " << frame_view.ni() << " nj = " << frame_view.nj() << vcl_endl;

#define OPTIMIZE_CAMERAS
#ifdef  OPTIMIZE_CAMERAS

    // randomly perturb data
    vnl_random randgen;
    vnl_double_3x1 camera_center = -Rs[camera_idx].transpose()*Ts[camera_idx];

    // perturb data 
    double Trange = 0.5;  // meters
    double Rrange = 0.0087; // radians

    vnl_double_3 dR_rodrigues;
    vnl_double_3x1 pos_offset;

    pos_offset[0][0] = first_camera? 0 : randgen.drand32(-Trange,Trange); 
    pos_offset[1][0] = first_camera? 0 : randgen.drand32(-Trange,Trange);
    pos_offset[2][0] = first_camera? 0 : randgen.drand32(-Trange,Trange);
    dR_rodrigues[0] = first_camera? 0 : randgen.drand32(-Rrange,Rrange);
    dR_rodrigues[1] = first_camera? 0 : randgen.drand32(-Rrange,Rrange);
    dR_rodrigues[2] = first_camera? 0 : randgen.drand32(-Rrange,Rrange);

    vgl_rotation_3d<double> dR(dR_rodrigues);
    camera_center += pos_offset;

    vnl_double_3x3 Rinit = dR.as_matrix()*Rs[camera_idx];
    vnl_double_3x1 Tinit = -Rinit * camera_center;

    vcl_cout << "perturbing camera center by < " << pos_offset.get_column(0) << " >  magnitude = " << pos_offset.get_column(0).magnitude() << vcl_endl;
    vcl_cout << "perturbing Rotation by < " << dR_rodrigues << " >  magnitude = " << dR_rodrigues.magnitude() << vcl_endl << vcl_endl;

    // optimize camera

    if (!first_camera) {
      double tol = 0.01;
      double prev_error, curr_error = vnl_numeric_traits<double>::maxval;
      unsigned it_count = 0;

      vnl_matrix_fixed<double,3,3> K = Ks[camera_idx];

      do {
        prev_error = curr_error;


        //generate an expected image from camera point of view
        //vnl_matrix_fixed<double,3,4> RT;
        RT.set_columns(0,Rinit);
        RT.set_column(3,Tinit.get_column(0));
        vnl_matrix_fixed<double,3,4> P = K * RT;

        vpgl_proj_camera<double> cam(P);

        vil_image_view<float> expected_image(frame_view.ni(),frame_view.nj(),1);
        vil_image_view<float> mask;//(frame_view.ni(),frame_view.nj(),1);
        voxels.expected_image(&cam,expected_image,mask);
        // debug
        breg3d_util::save_image(expected_image,"c:/project/registration/expected_image.tiff");
        breg3d_util::save_image(mask,"c:/project/registration/expected_image_mask.tiff");

        // now optimize camera parameters based on 2D image translation and rotation
        int border = 10;
        dbvrl_world_roi roi(expected_image.ni()- 2*border, expected_image.nj()- 2*border,vgl_point_2d<double>(border,border));

        vimt_transform_2d init_xform;
        init_xform.set_rigid_body(0.0, 0.0, 0.0);

        vimt_image_2d_of<float> curr_img(expected_image, vimt_transform_2d());
        vimt_image_2d_of<float> base_img(frame_view, init_xform);
        vimt_image_2d_of<float> curr_mask_img(mask, vimt_transform_2d());

        dbvrl_minimizer minimizer(curr_img, base_img, curr_mask_img, roi, true);
        vcl_cout << " minimizing 2D image error..";
        minimizer.minimize(init_xform,true);
        vcl_cout << "..done." << vcl_endl;
        curr_error = minimizer.get_end_error();
        // normalize by size of mask
        float mask_size;
        vil_math_sum(mask_size,mask,0);
        curr_error /= mask_size;

        vcl_cout << init_xform << vcl_endl;

        vnl_vector<double> rigid_body_params(3);
        init_xform.params(rigid_body_params);

        // returned rotation is around image origin.  We need rotation around principal axis
        // assume principal axis is image center
        double ox = expected_image.ni() / 2.0;
        double oy = expected_image.nj() / 2.0;

        double img_rot = rigid_body_params[0];
        double img_tx = rigid_body_params[1] - ox + vcl_cos(img_rot)*ox - vcl_sin(img_rot)*oy;
        double img_ty = rigid_body_params[2] - oy + vcl_sin(img_rot)*ox + vcl_cos(img_rot)*oy;
        
        // now we need to invert the rotation
        //double inv_rot = -img_rot;
        //double inv_tx = -(vcl_cos(inv_rot)*img_tx + vcl_sin(inv_rot)*img_ty);
        //double inv_ty = -(-vcl_sin(inv_rot)*img_tx + vcl_cos(inv_rot)*img_ty);

        double rotz = img_rot;
        double roty = img_tx * K[2][2] / K[1][1];
        double rotx = -img_ty * K[2][2] / K[0][0];

        vcl_cout << "init rotx, roty, rotz = " << rotx << " " << roty << " " << rotz << vcl_endl;

        vgl_rotation_3d<double> rot(rotx,roty,rotz);
        Rinit = rot.as_matrix() * Rinit;
        Tinit = rot.as_matrix() * Tinit;

        // debug
        //RT.set_columns(0,Rinit);
        //RT.set_column(3,Tinit.get_column(0));
        //vnl_matrix_fixed<double,3,4> Popt = K * RT;
        //vpgl_proj_camera<double> cam_opt(Popt);
        //voxels.expected_image(&cam_opt,expected_image,mask);
        //breg3d_util::save_image(expected_image,"c:/project/registration/expected_image_opt.tiff");
  

        // now optimize camera parameters based on 2D image translation and rotation

        ++it_count;
        vcl_cout << "after " << it_count <<" iterations: normalized error = " << curr_error << vcl_endl;
      }while( (prev_error - curr_error) > tol);
    }
    first_camera = false;

    vnl_double_3x3 newR = Rinit;
    vnl_double_3x1 newT = Tinit;

    Ks_opt.push_back(Ks[camera_idx]);
    Rs_opt.push_back(newR);
    Ts_opt.push_back(newT);

    // re-write each iteration in case something bad happens..
#ifdef PC1
    breg3d_util::write_cameras("d:/dec/matlab/reg3d/results/seq2_KRT_cameras_optimized.txt",Ks_opt,Rs_opt,Ts_opt);
#else
    breg3d_util::write_cameras("c:/project/registration/results/seq2_KRT_cameras_optimized.txt",Ks_opt,Rs_opt,Ts_opt);
#endif

    vcl_cout << "optimized R = " << newR << vcl_endl;
    vcl_cout << "optimized T = " << newT << vcl_endl;
#else
    vnl_double_3x3 newR = Rs[camera_idx];
    vnl_double_3x1 newT = Ts[camera_idx];
#endif

    RT.set_columns(0,newR);
    RT.set_column(3,newT.get_column(0));
    P = Ks[camera_idx] * RT;
    vpgl_proj_camera<double> cam(P);

#define UPDATE_VOXELS
#ifdef UPDATE_VOXELS

    voxels.update_voxels(frame_view,&cam);

#endif

#define GENERATE_VIRTUAL_VIEW
#ifdef GENERATE_VIRTUAL_VIEW
    // now generate virtual view
    vil_image_view<float> virtual_frame(frame_view.ni(),frame_view.nj(),1);
    vil_image_view<float> vis_frame(frame_view.ni(),frame_view.nj(),1);
    vil_image_view<unsigned char> height_map;
    voxels.virtual_view(frame_view,&cam,&vcam,virtual_frame,vis_frame,height_map);

    // write virtual views
    vcl_stringstream virtual_frame_fname, vis_frame_fname, height_map_fname;
#ifdef PC1
    virtual_frame_fname << "d:/dec/matlab/reg3d/results/seq2/virtual_frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
    vis_frame_fname << "d:/dec/matlab/reg3d/results/seq2/vis_frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
    height_map_fname << "d:/dec/matlab/reg3d/results/seq2/height_map_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
#else
    virtual_frame_fname << "c:/project/registration/results/seq2/virtual_frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
    vis_frame_fname << "c:/project/registration/results/seq2/vis_frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
    height_map_fname << "c:/project/registration/results/seq2/height_map_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".tiff";
#endif

    breg3d_util::save_image(virtual_frame,virtual_frame_fname.str());
    vil_save(vis_frame,vis_frame_fname.str().c_str());
    vil_save(height_map,height_map_fname.str().c_str());
#endif

  }


  return 0;
}

