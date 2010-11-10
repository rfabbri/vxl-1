#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <icam_ocl/icam_ocl_search_manager.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <icam/icam_minimizer.h>
#include <icam/icam_sample.h>


bool test_image_parallel_search()
{
//====================== Setup Minimizer ====================
  vcl_string root_dir = testlib_root_dir();
  vcl_string dest_file = "C:/images/Calibration/frame_142.png";
  vcl_string source_file = "C:/images/Calibration/frame_145.png";
  vcl_string depth_file = "C:/images/Calibration/depth_142.tif";
  vcl_string result_file = "C:/images/Calibration/gpu_result.tif";
  vcl_string mask_file = "C:/images/Calibration/gpu_mask.tif";
  vcl_string cpp_src_file = "C:/images/Calibration/cpp_src.tif";
  vcl_string cpp_result_file = "C:/images/Calibration/cpp_result.tif";
  vcl_string cpp_mask_file = "C:/images/Calibration/cpp_mask.tif";
  vil_image_view_base_sptr dest_img_base = vil_load(dest_file.c_str());
  if (!dest_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }
  vil_image_view_base_sptr source_img_base = vil_load(source_file.c_str());
  if (!source_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }

  vil_image_view_base_sptr depth_img_base = vil_load(depth_file.c_str());
  if (!depth_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }
  vil_image_view<vxl_byte> *dest_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dest_img_base.ptr());
  vil_image_view<vxl_byte> *source_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(source_img_base.ptr());
  vil_image_view<float> *depth_img_flt = dynamic_cast<vil_image_view<float>*>(depth_img_base.ptr());
  unsigned ni = dest_img_byte->ni(), nj = dest_img_byte->nj();
  vil_image_view<float> dest_img_flt(ni,nj);
  vil_convert_cast(*dest_img_byte,dest_img_flt);
  vil_image_view<float> source_img_flt(ni,nj);
  vil_convert_cast(*source_img_byte,source_img_flt);
  vil_image_view<double> depth_img_dbl(ni, nj);
  vil_convert_cast(*depth_img_flt,depth_img_dbl);

  // ========================Camera 145 =====================
  // relative rotation for source camera
  double rv [] ={0.9949824417310001, 0.07167609924, -0.06980290590899998,
                 -0.073085399753, 0.997165853331, -0.017858933610000002,
                 0.06832371779200001, 0.02287012861500001, 0.997400346057};
  vnl_matrix_fixed<double, 3, 3> Mr(rv);
  vgl_rotation_3d<double> Rr(Mr);
  vgl_vector_3d<double> tr(0.3207432455793182, 0.04231364883145655, -0.019929923492081336);
  vgl_vector_3d<double> tid(0.0, 0.0, 0.0), tbox(0.5, 0.0, 0.0);
  vnl_matrix_fixed<double, 3, 3> K(0.0);
  K[0][0]=1871.2;   K[1][1]=1871.2; K[0][2] = 640.0; K[1][2]=360.0; K[2][2]=1.0;
  //depth transform
  bool adjust_to_fl = false;
  icam_depth_transform dt(K, depth_img_dbl, Rr, tr, adjust_to_fl);

  // Typical parameters
  unsigned min_pyramid_image_size = 16;
  unsigned box_reduction_k = 2;
  double local_min_thresh = 0.005;
  double axis_search_cone_multiplier = 10.0;
  double polar_range_multiplier = 2.0;
  //vcl_string base_path = "C:/images/Calibration";
  vcl_string base_path = "";
  icam_minimizer minimizer(source_img_flt, dest_img_flt, dt,
                           min_pyramid_image_size, box_reduction_k,
                           axis_search_cone_multiplier,
                           polar_range_multiplier,
                           local_min_thresh, base_path);
  unsigned nl = minimizer.n_levels();
  unsigned lev = nl-1;
  // check native C++ implementation
  icam_cost_func cfn = minimizer.cost_fn(lev);
  double minfo_native = cfn.mutual_info(Rr.as_rodrigues(), tr, 0.5);
  vcl_cout << "Native minfo at correct transf " << minfo_native << '\n';
  vgl_box_3d<double> trans_box;
  trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
  trans_box.add(vgl_point_3d<double>(.5, .5, .5));
  vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
  //==============end of minimizer setup=============

  icam_ocl_search_manager* mgr = icam_ocl_search_manager::instance();

  mgr->set_workgrp_ni(8);   mgr->set_workgrp_nj(8);
  mgr->encode_image_data(minimizer, lev);
  mgr->copy_to_image_buffers();
  mgr->setup_transf_search_space(trans_box, trans_steps, minimizer, lev);
  mgr->create_image_parallel_transf_data();
  mgr->create_image_parallel_transf_buffers();
  mgr->setup_image_parallel_result();
  mgr->create_image_parallel_result_buffers();

  vcl_string kern_path = "/contrib/gel/mrc/vpgl/icam/icam_ocl/image_parallel_transf_search.cl";
  vcl_string path = root_dir + kern_path;
  if (!mgr->load_kernel_source(path))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;
  if (mgr->create_kernel("image_parallel_transf_search")!=SDK_SUCCESS)
    return false;
  if (!mgr->setup_image_parallel_kernel())
    return false;

  vul_timer t;
  for (unsigned i = 0; i<67095; ++i) {
    mgr->set_image_parallel_transf(tr, Rr);
    mgr->copy_to_image_parallel_transf_buffers();
    if (mgr->run_image_parallel_kernel()!=SDK_SUCCESS)
      return false;
  }
  vcl_cout << " search time " << t.real()/1000.0 << " seconds\n";
  cl_int* flag = mgr->image_para_flag();
  vcl_cout << "Flag(" << flag[0] << ' ' << flag[1] << ' '
           << flag[2] << ' ' << flag[3] << ")\n";

  cl_float* cres = mgr->image_para_result();
  vcl_cout << "Image_Para(" << cres[0] << ' ' << cres[1] << ' '
           << cres[2] << ' ' << cres[3] << ")\n";
  unsigned dni = mgr->dest_ni(), dnj = mgr->dest_nj();
  vil_image_view<float> result(dni, dnj);
  vil_image_view<float> mask(dni, dnj);
  unsigned wsni = mgr->work_space_ni();
  cl_float* result_ar = mgr->result_array();
  cl_float* mask_ar = mgr->mask_array();
  for (unsigned j = 0; j<dnj; ++j)
    for (unsigned i = 0; i<dni; ++i)
    {
      unsigned indx = i + wsni*j;
      result(i,j) = result_ar[indx];
      mask(i,j)   = mask_ar[indx];
    }
  vil_image_view<float> cpp_src = minimizer.source(lev);
  vil_image_view<float> cpp_map_dest;
  vil_image_view<float> cpp_map_mask;
  icam_depth_transform dtc = minimizer.depth_trans(lev);
  dtc.set_rotation(Rr);
  dtc.set_translation(tr);
  unsigned nsmp;
  icam_sample::resample(dni,dnj, cpp_src,dtc,cpp_map_dest,cpp_map_mask,nsmp);
  //test differences
  float dif = 0.0f;
  unsigned cnt = 0;
  for (unsigned j = 1; j<(dnj-1); ++j)
    for (unsigned i = 1; i<(dni-1); ++i)
      if (cpp_map_mask(i,j)>0.0f&&mask(i,j)>0.0f) {
        dif += vcl_fabs(cpp_map_dest(i,j)-result(i,j));
        cnt++;
      }
  vcl_cout << "total diff " << dif << " with fraction "<< 1.0*cnt/(dni*dnj)<< '\n';
  double minfo_gpu = cfn.mutual_info(result, mask,0.5);
  vcl_cout << "GPU minfo at correct transf " << minfo_gpu << '\n';
  vil_save(result, result_file.c_str());
  vil_save(mask, mask_file.c_str());
  vil_save(cpp_src, cpp_src_file.c_str());
  vil_save(cpp_map_dest, cpp_result_file.c_str());
  vil_save(cpp_map_mask, cpp_mask_file.c_str());
  mgr->release_queue();
  mgr->release_buffers();
  mgr->clean_image_data();
  mgr->clean_image_parallel_transf_data();
  mgr->clean_image_parallel_result();
  return true;
}

bool test_rot_parallel_search()
{
//====================== Setup Minimizer ====================
  vcl_string root_dir = testlib_root_dir();
  vcl_string dest_file = "C:/images/Calibration/frame_142.png";
  vcl_string source_file = "C:/images/Calibration/frame_145.png";
  vcl_string depth_file = "C:/images/Calibration/depth_142.tif";
  vcl_string gpu_mdest_file = "C:/images/Calibration/gpu_mdest.tif";
  vcl_string cpu_mdest_file = "C:/images/Calibration/cpu_mdest.tif";
  vcl_string mask_file = "C:/images/Calibration/gpu_mask.tif";
  vcl_string cpp_src_file = "C:/images/Calibration/cpp_src.tif";
  vcl_string cpp_result_file = "C:/images/Calibration/cpp_result.tif";
  vcl_string cpp_mask_file = "C:/images/Calibration/cpp_mask.tif";
  vil_image_view_base_sptr dest_img_base = vil_load(dest_file.c_str());
  if (!dest_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }
  vil_image_view_base_sptr source_img_base = vil_load(source_file.c_str());
  if (!source_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }

  vil_image_view_base_sptr depth_img_base = vil_load(depth_file.c_str());
  if (!depth_img_base) {
    vcl_cerr << "error loading image.\n";
    return false;
  }
  vil_image_view<vxl_byte> *dest_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dest_img_base.ptr());
  vil_image_view<vxl_byte> *source_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(source_img_base.ptr());
  vil_image_view<float> *depth_img_flt = dynamic_cast<vil_image_view<float>*>(depth_img_base.ptr());
  unsigned ni = dest_img_byte->ni(), nj = dest_img_byte->nj();
  vil_image_view<float> dest_img_flt(ni,nj);
  vil_convert_cast(*dest_img_byte,dest_img_flt);
  vil_image_view<float> source_img_flt(ni,nj);
  vil_convert_cast(*source_img_byte,source_img_flt);
  vil_image_view<double> depth_img_dbl(ni, nj);
  vil_convert_cast(*depth_img_flt,depth_img_dbl);

  // ========================Camera 145 =====================
  // relative rotation for source camera
  double rv [] ={0.9949824417310001, 0.07167609924, -0.06980290590899998,
                 -0.073085399753, 0.997165853331, -0.017858933610000002,
                 0.06832371779200001, 0.02287012861500001, 0.997400346057};
  vnl_matrix_fixed<double, 3, 3> Mr(rv);
  vgl_rotation_3d<double> Rr(Mr);
  vgl_vector_3d<double> tr(0.3207432455793182, 0.04231364883145655, -0.019929923492081336);
  vgl_vector_3d<double> tid(0.0, 0.0, 0.0), tbox(0.5, 0.0, 0.0);
  vnl_matrix_fixed<double, 3, 3> K(0.0);
  K[0][0]=1871.2;   K[1][1]=1871.2; K[0][2] = 640.0; K[1][2]=360.0; K[2][2]=1.0;
  //depth transform
  bool adjust_to_fl = false;
  icam_depth_transform dt(K, depth_img_dbl, Rr, tr, adjust_to_fl);

  // Typical parameters
  unsigned min_pyramid_image_size = 16;
  unsigned box_reduction_k = 2;
  double local_min_thresh = 0.005;
  double axis_search_cone_multiplier = 10.0;
  double polar_range_multiplier = 2.0;
  //vcl_string base_path = "C:/images/Calibration";
  vcl_string base_path = "";
  icam_minimizer minimizer(source_img_flt, dest_img_flt, dt,
                           min_pyramid_image_size, box_reduction_k,
                           axis_search_cone_multiplier,
                           polar_range_multiplier,
                           local_min_thresh, base_path);
  unsigned nl = minimizer.n_levels();
  unsigned lev = nl-1;
  // check native C++ implementation
  icam_cost_func cfn = minimizer.cost_fn(lev);
  //  double minfo_native = cfn.mutual_info(Rr.as_rodrigues(), tr, 0.5);
  double ent_diff = cfn.entropy_diff(Rr.as_rodrigues(), tr, 0.5);
  vcl_cout << "Entropy diff  at correct transf " << ent_diff << '\n';
  ent_diff = cfn.entropy_diff(Rr.as_rodrigues(), tbox, 0.5);
  vcl_cout << "Entropy diff  at (0.5, 0, 0) " << ent_diff << '\n';

  // test mapping
  vil_image_view<float> mdest_cp = cfn.mapped_dest(Rr.as_rodrigues(),tr);
  unsigned dni = mdest_cp.ni(), dnj = mdest_cp.nj();
  vil_save(mdest_cp, cpu_mdest_file.c_str());
  vgl_box_3d<double> trans_box;
  trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
  trans_box.add(vgl_point_3d<double>(.5, .5, .5));
  vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
  //==============end of minimizer setup=============

  icam_ocl_search_manager* mgr = icam_ocl_search_manager::instance();

  mgr->set_workgrp_ni(16);   mgr->set_workgrp_nj(0);
  mgr->set_nbins(16);
  mgr->encode_image_data(minimizer, lev);
  mgr->set_nbins_buffer();
  mgr->copy_to_image_buffers();
  unsigned nrot  = 67095;
  //  mgr->setup_rot_debug_space(nrot, Rr);
  mgr->setup_transf_search_space(trans_box, trans_steps, minimizer, lev);
  mgr->create_rot_parallel_transf_data();
  mgr->set_rot_parallel_transf_data(tr);
  mgr->create_rot_parallel_transf_buffers();
  mgr->setup_rot_parallel_result();
  mgr->create_rot_parallel_result_buffers();

  vcl_string kern_path = "/contrib/gel/mrc/vpgl/icam/icam_ocl/trans_parallel_transf_search.cl";
  vcl_string path = root_dir + kern_path;
  if (!mgr->load_kernel_source(path))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;
  if (mgr->create_kernel("trans_parallel_transf_search")!=SDK_SUCCESS)
    return false;
  if (!mgr->setup_rot_parallel_kernel())
    return false;

  vul_timer t;
  if (mgr->run_rot_parallel_kernel()!=SDK_SUCCESS)
    return false;
  vcl_cout << "OpenCL search time " << t.real()/1000.0 << " seconds\n";
  cl_int* flag = mgr->rot_para_flag();
  vcl_cout << "Flag(" << flag[0] << ' ' << flag[1] << ' '
           << flag[2] << ' ' << flag[3] << ")\n";

  //cl_float* minfo = mgr->minfo_array();
#if 0
  float sum = 0.0f;
  for (unsigned i = 0; i<256; ++i)
    sum += minfo[i];
  vcl_cout << "sum joint = " << sum << '\n';
  sum = 0.0f;
  for (unsigned i = 256; i<256+16; ++i)
    sum += minfo[i];
  vcl_cout << "sum marginal = " << sum << '\n';


  unsigned dni = 16;
  for (unsigned i = 0; i< 2*dni; ++i)
    vcl_cout << minfo[i] << '\n';
#endif
#if 0
  vil_image_view<float> md(dni, dnj);
  md.fill(0.0f);
  float dif = 0.0f;
  for (unsigned j = 0; j<dnj; j++)
    for (unsigned i = 0; i<dni; i++)
    {
      unsigned indx = i + dni*j;
      md(i,j) = minfo[indx];
	  if(md(i,j)>0.0f){
		  float d = vcl_fabs(md(i,j) - mdest_cp(i,j));
		  if(d>1.0f)
        vcl_cout << "(" << i << ' ' << j << ") gp = " << md(i,j) << " cp = "
                 << mdest_cp(i,j) << '\n';
        dif += d;
     }
	}
  vil_save(md, gpu_mdest_file.c_str());
  vcl_cout << " Absolute difference in mapped dest " << dif << '\n';

  unsigned off = dni*dnj;
  for(unsigned i = 0; i<16; ++i)
    vcl_cout << minfo[off+i] << '\n';
#endif
  /* minimize entropy diff */
  vgl_rotation_3d<double> min_rot;
  vnl_vector_fixed<double,3> clrot, cpprot;
  float min_ent;
  mgr->find_min_rot(min_rot, min_ent);
  clrot = min_rot.as_rodrigues();
  vcl_cout << "Min ent dif " << min_ent << " at rotation " 
           <<  clrot << '\n';
  vcl_cout << " Actual rotation " << Rr.as_rodrigues() << '\n';
  double cpp_minfo, ovl_frac;
  t.mark();
  minimizer.exhaustive_rotation_search(tr, lev, 0.5, min_rot,
                                       cpp_minfo, ovl_frac);
  cpprot = min_rot.as_rodrigues();
  vcl_cout << "C++ search time " << t.real()/1000.0 << " seconds\n";
  vcl_cout << "C++ minfo " << cpp_minfo << " at rotation " 
           << cpprot << '\n';
  vcl_cout << " Actual rotation " << Rr.as_rodrigues() << '\n';
  double er = (cpprot - clrot).magnitude();
  TEST_NEAR("CL vs C++ ", er, 0.0, 1e-4);
  
  mgr->release_queue();
  mgr->release_buffers();
  mgr->clean_image_data();
  mgr->clean_rot_parallel_transf_data();
  mgr->clean_rot_parallel_result();
  return true;
}

static void test_depth_trans()
{
  //test_image_parallel_search();
  test_rot_parallel_search();
}

TESTMAIN(test_depth_trans);
