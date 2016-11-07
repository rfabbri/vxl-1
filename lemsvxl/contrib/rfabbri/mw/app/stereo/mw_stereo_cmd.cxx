#include <mw/algo/mw_stereo_app.h>


int
main(int argc, char **argv)
{

  // IDEA
  //  - for reducing the number of parameters, we could write a configuration
  //  file. 
  //
  char *prefix_default="/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/rfabbri/mw/app/david/david-02-26-2006-crop2/small-h100/";
  vul_arg<vcl_string> a_prefix("-prefix", "path to directory of files (suffixed with slash)",prefix_default);
  vul_arg<vcl_string> a_out_path("-out_path", "path to output. Defaults to -prefix");
  vul_arg<vcl_string> a_fname1("-image1", "image fname 1","000-maximage.png");
  vul_arg<vcl_string> a_fname2("-image2", "image fname 2","003-maximage.png");
  vul_arg<vcl_string> a_fname3("-image3", "image fname 3","006-maximage.png");
  vul_arg<vcl_vector<vcl_string> > a_fnames("-images","fname's of images (space-separated)");

  vul_arg<vcl_string> a_corresp_out("-corresp_out", "correspondence output fname 1","out/david-0-3-edges_t10-stereo_result.corresp");
  vul_arg<vcl_string> a_corresp_gt("-corresp_gt", "correspondence ground-truth fname 1","david-0-3-edges_t10.corresp");
  vul_arg<vcl_string> a_precomputed_epip("-corresp_epip","correspondence (pre-computed epipolar candidates) fname", "david-0-3-edges_t10-epipolar.corresp");
  vul_arg<vcl_string> a_corresp_in("-corresp_in", "correspondence (pre-computed) fname","david-0-3-edges_t10-stereo_result-trinocular_costs.corresp");
  
  vul_arg<vcl_string> a_edgels1("-edgels1", "edgels fname 1","000-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgels2("-edgels2", "edgels fname 2","003-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgels3("-edgels3", "edgels fname 3","006-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgel_type("-edgel_type","file format 'edg' or 'vsol'");
  vul_arg<vcl_vector<vcl_string> > a_edgels("-edgels","fname's of edgels (space-separated)");

  vul_arg<bool> a_run_trinocular("-trinocular", "compute trinocular reprojection costs based on point distance",false);
  vul_arg<bool> a_run_trinocular_t_diff("-trinocular_tangent_diff", "compute trinocular reprojection costs based on tangent difference",false);
  vul_arg<bool> a_run_trinocular_k_diff("-trinocular_curvature_diff", "compute trinocular reprojection costs based on curvature difference",false);
  vul_arg<bool> a_write_epipolar_corresp("-write_epip", "write epipolar correspondents to file out/<corresp_epip>",false);
  vul_arg<bool> a_read_epipolar_corresp("-read_epip", "read epipolar correspondents from file <corresp_epip>",false);
  vul_arg<bool> a_compare_corresp("-compare_corresp", "print info on difference between ground-truth and <a_corresp_in>",false);
  vul_arg<bool> a_compute_epipolars("-compute_epipolars", "compute epipolar constraint",false);
  vul_arg<bool> a_symmetric_3("-symmetric_3", "treat trinocular constraints symmetrically",false);
  vul_arg<bool> a_symmetric_n("-symmetric_n", "apply n-view symmetric constraints",false);

  vul_arg<bool> a_run_sel_geometry("-sel_geometry", "compute costs based on differential geometry of Amir's edge linking",false);
  vul_arg<bool> a_read_sel("-read_sel", "read edge linking hypotheses from binary files",false);
  vul_arg<vcl_string> a_sel_in1("-sel_file1", "filename of edge linking hypotheses", "000-maximage.nms.t10.sel2");
  vul_arg<vcl_string> a_sel_in2("-sel_file2", "filename of edge linking hypotheses", "003-maximage.nms.t10.sel2");
  vul_arg<vcl_string> a_sel_in3("-sel_file3", "filename of edge linking hypotheses", "006-maximage.nms.t10.sel2");

  vul_arg<bool> a_synth_data_1("-synthetic_1", "Use synthetic data (ctspheres camera setup)", false);
  vul_arg<bool> a_synth_data_2("-synthetic_2", "Use synthetic data (digicam turntable setup)", false);
  vul_arg<bool> a_synth_data_3("-synthetic_3", "Use synthetic data (digicam turntable setup - sandbox)", false);
  vul_arg<bool> a_synth_data_med("-synthetic_med", "Use synthetic data (digicam turntable setup - sandbox)", false);
  vul_arg<bool> a_run_synth_geometry("-run_synth_geometry", "Compute costs based on differential geometry from synthetic data ", false);
  vul_arg<bool> a_synth_write_edgels("-synth_write_edgels", "Write edgels to a_edgels1 and a_edgels2 when using synthetic data", false);

  vul_arg<double> a_angle1("-angle1", "angle of 1st view in Deg (for synthetic data)", 0);
  vul_arg<double> a_angle2("-angle2", "angle of 2nd view in Deg (for synthetic data)", 5);
  vul_arg<double> a_angle3("-angle3", "angle of 3rd view in Deg (for synthetic data)", 60);
  vul_arg<vcl_vector<double> > a_angles("-angles","angles of views (for synthetic data)");

  vul_arg<bool> a_perturb_camera("-perturb_camera", "perturb camera for synthetic data. -synthetic_x must be set.", false);
  vul_arg<bool> a_write_perturb_camera("-write_perturb_camera", "write perturbed cameras", false);
  vul_arg<unsigned> a_trinocular_radius("-trinocular_nrad", "trinocular neighborhood radius", 10);
  vul_arg<double> a_err_pos("-err_pos", "localization error range", 2);
  vul_arg<bool> a_remove_epitangency("-no_epitangency",false);
  vul_arg<vcl_string> a_cam_type("-cam_type","camera type: intrinsic_extrinsic or projcamera","");

  vul_arg_parse(argc,argv);

  vcl_cout << "fname1: " <<  a_fname1() 
    << "\tfname2: " << a_fname2() 
    << "\tfname3: " << a_fname3() << vcl_endl 
    << "prefix: " << a_prefix()  << vcl_endl
    << "corresp_out: " << a_corresp_out()
    << "\tcorresp_gt: " << a_corresp_gt()
    << "\tprecomputed_epip: " << a_precomputed_epip() << vcl_endl
    << "\tedgels1: " << a_edgels1() 
    << "\tedgels2: " << a_edgels2() 
    << "\tedgels3: " << a_edgels3()  << vcl_endl
    << vcl_endl;

  mw_stereo_app_args arg;

  arg.set(
    a_prefix, 
    a_out_path, 
    a_fname1,
    a_fname2, 
    a_fname3,
    a_fnames,
    a_corresp_gt,
    a_precomputed_epip,
    a_edgels1,
    a_edgels2,
    a_edgels3,
    a_edgel_type,
    a_edgels,
    a_run_trinocular,
    a_run_trinocular_t_diff,
    a_run_trinocular_k_diff,
    a_write_epipolar_corresp,
    a_read_epipolar_corresp,
    a_corresp_in,
    a_compare_corresp,
    a_compute_epipolars,
    a_symmetric_3,
    a_symmetric_n,
    a_run_sel_geometry,
    a_read_sel,
    a_sel_in1,
    a_sel_in2,
    a_sel_in3,
    a_synth_data_1,
    a_synth_data_2,
    a_synth_data_med,
    a_synth_data_3,
    a_run_synth_geometry,
    a_synth_write_edgels,
    a_angle1,
    a_angle2,
    a_angle3,
    a_angles,
    a_perturb_camera,
    a_write_perturb_camera,
    a_trinocular_radius,
    a_err_pos,
    a_remove_epitangency,
    a_cam_type
      );


  mw_stereo_app app;

  bool stat = app.init(arg);

  if (!stat)
    return 1;

  app.run_stereo();
  vcl_string p1 = a_out_path() + a_corresp_out();
  vcl_string p2 = a_out_path() + vcl_string("out/") + a_precomputed_epip();
  app.write_results(p1, p2);

  return 0;
}

