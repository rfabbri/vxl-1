//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_cmdproc.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

#include <dbmsh3d/dbmsh3d_pt_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3dr/dbmsh3dr_match.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>
#include <dbmsh3dr/vis/dbmsh3dr_cmdproc.h>

bool dbmsh3dr_check_cmdproc (dbmsh3dr_pro_vis* mpvr, int argc, char **argv)
{
  if (!dbmsh3dr_cmd_listfile() && 
      !dbmsh3d_cmd_align_file() && !dbmsh3d_cmd_align_listfile() &&
      !dbmsh3d_cmd_p3d_file2() && !dbmsh3d_cmd_xyz_file2() && !dbmsh3d_cmd_xyzn1_file2() && 
      !dbmsh3d_cmd_3pi_file2() && !dbmsh3d_cmd_ply_file2() && !dbmsh3d_cmd_ply2_file2() && 
      !dbmsh3d_cmd_iv_file2() &&
      !dbmsh3dr_cmd_scalem() && !dbmsh3dr_cmd_icp() && 
      dbmsh3dr_cmd_verr()==0 && dbmsh3dr_cmd_debug_verr()==0 &&
      dbmsh3dr_cmd_rsnv()==0)
    return false;
  else
    return true; 
}

void dbmsh3dr_setup_provis_parameters (dbmsh3dr_pro_vis* mpvr)
{
}

SoSeparator* dbmsh3dr_cmdproc_execute (dbmsh3dr_pro_vis* mpvr)
{
  SoSeparator* _root = new SoSeparator;

  //############### Read Affine Transform File ###############  
  if (dbmsh3d_cmd_p3d_file2() || dbmsh3d_cmd_xyz_file2() || dbmsh3d_cmd_xyzn1_file2() ||
      dbmsh3d_cmd_3pi_file2() || 
      dbmsh3d_cmd_ply_file2() || dbmsh3d_cmd_ply2_file2() || dbmsh3d_cmd_iv_file2() ||
      dbmsh3dr_cmd_icp() || 
      dbmsh3dr_cmd_verr() || dbmsh3dr_cmd_debug_verr() ||
      dbmsh3d_cmd_align_ofile()) {
    if (dbmsh3d_cmd_align_file()) { 
      //-af xform_file.txt: read the alignment from file to mpvr->hmatrix_
      vcl_string xform_file = dbmsh3d_cmd_align_file();
      mpvr->load_hmatrix_01 (xform_file);
    }
  }
  if (dbmsh3d_cmd_align_listfile()) {
    //-aflist xform_list.txt: read the alignments from list file and compute final xform to mpvr->hmatrix_
    vcl_string xform_listfile = dbmsh3d_cmd_align_listfile();
    dbmsh3d_read_xform_listfile (xform_listfile, mpvr->hmatrix_01());
  }
  //##########################################################################
  //############################## View Files ################################
  //##########################################################################

  //############### View P3D File 2 ###############  
  //-p3d_2: 
  if (dbmsh3d_cmd_p3d_file2()) {
    mpvr->pv1()->b_view_p3d_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_p3d_file2()));
  }
  if (mpvr->pv1()->b_view_p3d_) {
    mpvr->pv1()->reset_mesh ();
    if (mpvr->pv1()->load_p3d()) {
      //transform pro[1] using the specified affine xform file
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      mpvr->xform_hmatrix_01_pro1 ();

      if (dbmsh3d_cmd_fileprefix2()) { //-f2: specify output file name    
        vcl_string dirfile = dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".p3d";
        vul_printf (vcl_cout, "Writing transformed file (*.p3d) %s...\n", dirfile.c_str());
        dbmsh3d_save_p3d (mpvr->pv1()->mesh(), dirfile.c_str());        
      }

      _root->addChild (mpvr->pv1()->vis_ptset (dbmsh3d_cmd_colorcode2()));
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### View XYZ File 2 ###############  
  //-xyz_2: 
  if (dbmsh3d_cmd_xyz_file2()) {
    mpvr->pv1()->b_view_xyz_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_xyz_file2()));
  }
  if (mpvr->pv1()->b_view_xyz_) {
    mpvr->pv1()->reset_mesh ();
    if (mpvr->pv1()->load_xyz()) {
      //transform pro[1] using the specified affine xform file
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      mpvr->xform_hmatrix_01_pro1 ();

      if (dbmsh3d_cmd_fileprefix2()) { //-f2: specify output file name    
        vcl_string dirfile = dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".xyz";
        vul_printf (vcl_cout, "Writing transformed file (*.xyz) %s...\n", dirfile.c_str());
        dbmsh3d_save_xyz (mpvr->pv1()->mesh(), dirfile.c_str());        
      }

      _root->addChild (mpvr->pv1()->vis_ptset (dbmsh3d_cmd_colorcode2()));
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### View XYZN1 File 2 ############### 
  //-xyzn1_2: 
  if (dbmsh3d_cmd_xyzn1_file2()) {
    mpvr->pv1()->b_view_xyzn1_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_xyzn1_file2()));
  }
  if (mpvr->pv1()->b_view_xyzn1_) {
    vcl_string xyzn1_file = mpvr->pv1()->dir_file() + ".xyzn1";
    mpvr->pv1()->set_pro_data (dbmsh3d_pro::PD_ORIPTS);
    mpvr->pv1()->oripts().clear();

    if (dbmsh3d_load_xyzn1 (mpvr->pv1()->oripts(), xyzn1_file.c_str()) == false)
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", mpvr->pv1()->dir_file().c_str());
    else { 
      //transform pro[1] using the specified affine xform file
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      mpvr->xform_hmatrix_01_pro1 ();

      _root->addChild (draw_oriented_ptset (mpvr->pv1()->oripts(), 
                                            mpvr->pv1()->m_vertex_ccode_, COLOR_BLUE, 
                                            dbmsh3d_cmd_len(), dbmsh3d_cmd_psz(), 1));
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### View 3PI File 2 ###############  
  //-3pi_2: 
  if (dbmsh3d_cmd_3pi_file2()) {
    mpvr->pv1()->b_view_3pi_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_3pi_file2()));
  }
  if (mpvr->pv1()->b_view_3pi_) {
    if (mpvr->pv1()->load_3pi(mpvr->pv1()->dir_file())) {
      //transform pro[1] using the specified affine xform file
      mpvr->xform_hmatrix_01_pro1 ();      
      mpvr->pv1()->move_sg3pi_to_mesh ();

      if (dbmsh3d_cmd_fileprefix2()) { //-f2: specify output file name    
        vcl_string dirfile = dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".xyz";
        vul_printf (vcl_cout, "Writing transformed file (*.xyz) %s...\n", dirfile.c_str());
        dbmsh3d_save_xyz (mpvr->pv1()->mesh(), dirfile.c_str());        
      }

      _root->addChild (mpvr->pv1()->vis_ptset (dbmsh3d_cmd_colorcode2()));
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### View PLY File 2 ###############
  //-ply_2: 
  if (dbmsh3d_cmd_ply_file2()) {
    mpvr->pv1()->b_view_ply_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_ply_file2()));
  }
  if (mpvr->pv1()->b_view_ply_) {
    if (mpvr->pv1()->load_ply (mpvr->pv1()->dir_file()) == false)
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", mpvr->pv1()->dir_file().c_str());
    else {    
      //transform pro[1] using the specified affine xform file
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      mpvr->xform_hmatrix_01_pro1 ();

      if (dbmsh3d_cmd_fileprefix2()) { //-f2: specify output file name    
        vcl_string dirfile = dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".ply";
        vul_printf (vcl_cout, "Writing transformed file (*.ply) %s...\n", dirfile.c_str());
        dbmsh3d_save_ply (mpvr->pv1()->mesh(), dirfile.c_str());        
      }
 
      _root->addChild (mpvr->pv1()->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode2()));
      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (mpvr->pv1()->vis_ptset_idv());
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### View PLY2 File 2 ###############
  //-ply2_2: 
  if (dbmsh3d_cmd_ply2_file2()) {
    mpvr->pv1()->b_view_ply2_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_ply2_file2()));
  }
  if (mpvr->pv1()->b_view_ply2_) {
    mpvr->pv1()->reset_mesh ();
    if (mpvr->pv1()->load_ply2 (mpvr->pv1()->dir_file()) == false)
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", mpvr->pv1()->dir_file().c_str());
    else {
      //transform pro[1] using the specified affine xform file
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      mpvr->xform_hmatrix_01_pro1 ();

      if (dbmsh3d_cmd_fileprefix2()) { //-f2: specify output file name    
        vcl_string dirfile = dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".ply2";
        vul_printf (vcl_cout, "Writing transformed file (*.ply2) %s...\n", dirfile.c_str());
        dbmsh3d_save_ply2 (mpvr->pv1()->mesh(), dirfile.c_str());        
      }

      _root->addChild (mpvr->pv1()->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode2()));
      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (mpvr->pv1()->vis_ptset_idv());
      dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
    }
  }
  //############### Viewing IV File 2 ###############
  //-iv_2: 
  if (dbmsh3d_cmd_iv_file2()) {
    mpvr->pv1()->b_view_iv_ = true;
    mpvr->pv1()->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_iv_file2()));
  }
  if (mpvr->pv1()->b_view_iv_) {
    _root->addChild (mpvr->pv1()->view_iv ());
    dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
  }    
  //############### List Files ###############
  //-list: 
  if (dbmsh3dr_cmd_listfile()) {
    //Read the list file and view each file.
    //-v 0: regular, 1: draw in single color.
    //-op3d, -oply, -oply2: save output file.
    vcl_string listfile = dbmsh3dr_cmd_listfile();
    if (dbmsh3d_cmd_xyz_ofile() || dbmsh3d_cmd_p3d_ofile())
      _root->addChild (mpvr->pv1()->vis_list_file (listfile, 1, dbmsh3d_cmd_v()));
    else if (dbmsh3d_cmd_ply_ofile() || dbmsh3d_cmd_ply2_ofile())
      _root->addChild (mpvr->pv1()->vis_list_file (listfile, 2, dbmsh3d_cmd_v()));
    else 
      _root->addChild (mpvr->pv1()->vis_list_file (listfile, 0, dbmsh3d_cmd_v()));

    //output to xyz or p3d point cloud file.
    if (dbmsh3d_cmd_xyz_ofile() || dbmsh3d_cmd_p3d_ofile()) {
      assert (mpvr->pv1()->pts().size() != 0);
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_PTS);
      if (dbmsh3d_cmd_xyz_ofile())
        mpvr->pv1()->save_xyz (dbmsh3d_cmd_xyz_ofile());
      if (dbmsh3d_cmd_p3d_ofile())
        mpvr->pv1()->save_p3d (dbmsh3d_cmd_p3d_ofile());
    }

    //output to ply or ply2 mesh file.
    if (dbmsh3d_cmd_ply_ofile() || dbmsh3d_cmd_ply2_ofile()) {
      assert (mpvr->pv1()->ifs_faces().size() != 0);
      mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_PTS);
      if (dbmsh3d_cmd_ply_ofile())
        mpvr->pv1()->save_ply (dbmsh3d_cmd_ply_ofile());
      if (dbmsh3d_cmd_ply2_ofile())
        mpvr->pv1()->save_ply2 (dbmsh3d_cmd_ply2_ofile());
    }

    dbmsh3d_app_window_title += " -- " + listfile;
  }  

  //##########################################################################
  //############################ Processing ##################################
  //##########################################################################

  //############### Scaling to Match Variance ####################
  //-scalem 1: Scale the input file (-f1) to match the reference object in (-f2)
  if (dbmsh3dr_cmd_scalem()) {
    //-f1: read in the input object (-f1) and compute its variance.
    mpvr->pv0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    mpvr->pv0()->load_points_files ();
    vgl_point_3d<double> C0;
    double var0 = compute_cen_var (mpvr->pv0()->mesh(), C0);

    //-f2: read the points in the reference object (-f2) and compute its variance.
    mpvr->pv1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    mpvr->pv1()->load_points_files ();
    vgl_point_3d<double> C1;
    double var1 = compute_cen_var (mpvr->pv1()->mesh(), C1);

    //compute the matched scale (identical variance).
    float scale = float (var1 / var0);

    vul_printf (vcl_cout, "\n var0: %f, var1: %f, scale: %f.\n",
                var0, var1, scale);

    //Scale the input object and translate to C1.
    translate_scale_points (mpvr->pv0()->mesh(), scale, C0, C1);
    
    //-ofile 1: output the mesh/point file.
    if (dbmsh3d_cmd_ofile() && !(dbmsh3d_cmd_xyz_ofile() || dbmsh3d_cmd_p3d_ofile())) {
      //pv->save_p3d (pv->dir_prefix() + "_pp.p3d");
      mpvr->pv0()->save_xyz (mpvr->pv0()->dir_prefix() + "_s.xyz");
    }

    //-gui 1: visualize the processed file.
    if (dbmsh3d_cmd_gui()) {
      _root->addChild (mpvr->pv0()->vis_ptset());
      _root->addChild (mpvr->pv1()->vis_ptset());
    }
  }

  //############### ICP to Register Two Point Cloud Files ####################
  //-icp 1,2,3:
  if (dbmsh3dr_cmd_icp()) {
    //-f1: 
    mpvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    //-f2: 
    mpvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());

    //-n: number of ICP maximum iteration, default 20.
    int max_iter = 20;
    if (dbmsh3d_cmd_n() != -1)
      max_iter = dbmsh3d_cmd_n();

    if (dbmsh3dr_cmd_icp() == 1) { //-icp 1: Run point-point ICP.
      //-f1: Load and visualize input pt cloud files.
      if (mpvr->pv0()->load_points_files()) { 
        //Try read the xform file with the same filename and apply it.
        if (mpvr->pv0()->load_hmatrix (mpvr->pv0()->dir_prefix()))
          mpvr->pv0()->apply_xform_hmatrix ();

        //draw the input point cloud of -f1.
        _root->addChild (mpvr->pv0()->vis_ptset (COLOR_DARKGREEN, dbmsh3d_cmd_idv()!=0));
      }
      //-f2: Load and visualize input pt cloud files.
      if (mpvr->pv1()->load_points_files()) {
        //Try read the xform file with the same filename and apply it.
        if (mpvr->pv1()->load_hmatrix (mpvr->pv1()->dir_prefix()))
          mpvr->pv1()->apply_xform_hmatrix ();

        //draw the input point cloud of -f2.
        _root->addChild (mpvr->pv1()->vis_ptset (COLOR_BLUE, dbmsh3d_cmd_idv()!=0));
      }

      //Estimate dist. threshold for error estimation.
      // -dthr: dist_th_ratio for error estimation.
      double avg_samp_dist = mpvr->p0()->get_avg_samp_dist();
      float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
      float icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

      //-msr: partial overlap dist_th ratio.
      //-icpcv: convergence threshold.
      if (mpvr->run_pp_icp_regstr (max_iter, dbmsh3dr_cmd_icpcv(), icp_dist_th)) 
        //draw the point cloud of -f2 after alignment.
        _root->addChild (mpvr->pv1()->vis_ptset (COLOR_RED, dbmsh3d_cmd_idv()!=0));
    }
    else if (dbmsh3dr_cmd_icp() == 2) { //-icp 2: Run point-plane ICP.
      //Load and visualize input pt cloud files.
      if (mpvr->pv0()->load_meshes_files()) //draw the input mesh of -f1.
        _root->addChild (mpvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_DARKGREEN, true));
      
      if (mpvr->pv1()->load_meshes_files()) //draw the input mesh of -f2.
        _root->addChild (mpvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_BLUE, true));

      //-msr: partial overlap dist_th ratio.
      if (mpvr->run_pf_icp_regstr (max_iter, dbmsh3dr_cmd_icpcv(), dbmsh3d_cmd_msr())) {
        //draw the mesh of -f2 after alignment.
        mpvr->pv1()->mesh()->build_face_IFS ();
        _root->addChild (mpvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));
      }
    }
    else if (dbmsh3dr_cmd_icp() == 3) { //-icp 3: Run RGRL ICP.
      bool result = mpvr->pv0()->load_xyzn1 (dbul_get_dir_file (dbmsh3d_cmd_fileprefix1()) + ".xyzn1");
      assert (result);
      result = mpvr->pv1()->load_xyzn1 (dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".xyzn1");
      assert (result);

      //Register the two oriented point sets.
      // if -af specified, use it as initial guess of xform.
      // result in mpvr->hmatrix_.
      mpvr->run_rgrl_icp_oripts (dbmsh3d_cmd_n1(), dbmsh3d_cmd_n2());

      //Align together: Transform the 2nd to the first point set.
      mpvr->xform_hmatrix_01_pro1 ();

      _root->addChild (mpvr->vis_oripts());
      _root->addChild (mpvr->vis_oripts2());
    }
    
    vul_printf (vcl_cout, "\n\nRegistration Result:\n");
    vul_printf (vcl_cout, "  Input fixed point cloud (-f1 %s, %u points) in Green.\n", 
                mpvr->p0()->dir_prefix().c_str(), mpvr->p0()->mesh()->vertexmap().size());
    vul_printf (vcl_cout, "  Input moving point cloud (-f2 %s, %u points) in Blue.\n", 
                mpvr->p1()->dir_prefix().c_str(), mpvr->p1()->mesh()->vertexmap().size());
    vul_printf (vcl_cout, "  Resulting moving point cloud in Red.\n");

    vul_printf (vcl_cout, "\n  Final transformation matrix:\n");
    vcl_cerr << mpvr->hmatrix_01();
    vul_printf (vcl_cout, "\n\n");    

    dbmsh3d_app_window_title += " -- ICP on " + mpvr->pv0()->dir_file() + " and " + mpvr->pv1()->dir_file();    
  }
  //############### Visualize the min-dist (error) between two datasets ###############  
  if (dbmsh3dr_cmd_verr() == 1) { //-verr 1: compute and visualize pt-pt dist
    //Load file 1 via -f1 prefix
    mpvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    bool result0 = mpvr->p0()->load_points_files ();
    if (dbmsh3d_cmd_n1() != -1) //-n1: draw the first point cloud.
      _root->addChild (mpvr->pv0()->vis_ptset ());
    
    //Load file 2 via -f2 prefix
    mpvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    bool result1 = mpvr->p1()->load_points_files ();

    if (result0 && result1) {      
      mpvr->p0()->load_hmatrix ();
      if (mpvr->p0()->is_hmatrix_identity() == false) 
        mpvr->p0()->apply_xform_hmatrix ();
      mpvr->p1()->load_hmatrix ();
      if (mpvr->p1()->is_hmatrix_identity() == false) 
        mpvr->p1()->apply_xform_hmatrix ();

      //Estimate dist. threshold for error estimation.
      // -dthr: dist_th_ratio for error estimation.
      double avg_samp_dist = mpvr->p0()->get_avg_samp_dist();
      float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
      float icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

      mpvr->compute_pp_error (error_dist_th); 
      mpvr->normalize_pp_error (error_dist_th); 
      
      //draw the 2nd points coloring in error.
      mpvr->pv1()->compute_mesh_vertex_color_set (mpvr->min_dists());
      //-l 0: hide the lines connecting matching points.
      _root->addChild (mpvr->vis_min_dist_pt_color_SoPoint (dbmsh3d_cmd_psz(), dbmsh3d_cmd_len()!=0));
    }
    dbmsh3d_app_window_title += " -- " + mpvr->pv0()->dir_file() + " - " + mpvr->pv1()->dir_file();
  }
  else if (dbmsh3dr_cmd_verr() == 2) { //-verr 2: compute and visualize pt-pt dist (showing surface)
    //Load file 1 via -f1 prefix
    mpvr->pv0()->load_ply2 (dbul_get_dir_file (dbmsh3d_cmd_fileprefix1()) + "-surface.ply2");
    
    if (dbmsh3d_cmd_n1() != -1) //draw the first point cloud.
      _root->addChild (mpvr->pv0()->vis_mesh (false, true));
    
    //Load file 2 via -f2 prefix
    mpvr->pv1()->load_p3d (dbul_get_dir_file (dbmsh3d_cmd_fileprefix2()) + ".p3d");
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    double avg_samp_dist = mpvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //transform pro[1] using the specified affine xform file
    mpvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
    mpvr->xform_hmatrix_01_pro1 ();
    
    //-msr: dist_th_ratio for error estimation.
    mpvr->compute_pp_error (error_dist_th); 
    mpvr->normalize_pp_error (error_dist_th); 

    //draw the 2nd points coloring in error.
    mpvr->pv1()->compute_mesh_vertex_color_set (mpvr->min_dists());
    //_root->addChild (mpvr->vis_min_dist_pt_color_SoCube (mpvr->dist_median()/10)); //dbmsh3d_cmd_rc()
    //-l 0: hide the lines connecting matching points.
    _root->addChild (mpvr->vis_min_dist_pt_color_SoPoint (dbmsh3d_cmd_psz(), dbmsh3d_cmd_len()!=0));

    //dbmsh3d_app_window_title += " -- " + mpvr->pv1()->dir_file();
  }
  else if (dbmsh3dr_cmd_verr() == 3) { //-verr 3: compute and visualize pt-mesh dist
    //Load the first mesh file.
    mpvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    bool result0 = mpvr->pv0()->load_ply ();
    if (result0 && dbmsh3d_cmd_n1() != -1) { //-n1 1: draw the first surface mesh.
      _root->addChild (mpvr->pv0()->vis_mesh (false, true));      
      if (dbmsh3d_cmd_idv()) //-idv 1: draw individual mesh vertex
        _root->addChild (mpvr->pv0()->vis_ptset_idv());
    }

    //Load the second mesh or point cloud file.
    mpvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    bool result1 = false;
    if (mpvr->pv1()->load_ply())
      result1 = true;
    else if (mpvr->pv1()->load_p3d())
      result1 = true;

    if (result0 && result1) {   
      mpvr->p0()->load_hmatrix ();
      if (mpvr->p0()->is_hmatrix_identity() == false) 
        mpvr->p0()->apply_xform_hmatrix ();
      mpvr->p1()->load_hmatrix ();
      if (mpvr->p1()->is_hmatrix_identity() == false) 
        mpvr->p1()->apply_xform_hmatrix ();

      //Estimate dist. threshold for error estimation.
      // -dthr: dist_th_ratio for error estimation.
      double avg_samp_dist = mpvr->p0()->get_avg_samp_dist();
      float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
      float icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

      //-n3: search top_n closest vertices for pt-mesh dist.
      //-msr: dist_th_ratio for error estimation.
      mpvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
      double d_pt_mesh_A_B_mean = mpvr->dist_mean();
      vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A, B): %f.\n\n", d_pt_mesh_A_B_mean);
      mpvr->normalize_pf_error (error_dist_th); 

      //draw the 2nd mesh 
      if (dbmsh3d_cmd_v()==0) { //-v 0: draw mesh points.
        if (dbmsh3d_cmd_idv()==0) //-idv 0: draw in one batch.
          _root->addChild (mpvr->pv1()->vis_mesh_pts_edges());
        else { //-idv 1: draw individual mesh vertex.
          //colorthe 2nd points in error.
          mpvr->pv1()->compute_mesh_vertex_color_set (mpvr->min_dists());
          _root->addChild (mpvr->pv1()->vis_ptset_idv());
        }
      }
      else if (dbmsh3d_cmd_v()==1) { // -v 1: draw mesh faces in color.
        //draw the 2nd mesh faces coloring in error.
        mpvr->pv1()->compute_face_color_by_value (mpvr->min_dists());
        _root->addChild (mpvr->pv1()->vis_mesh_color());
        if (dbmsh3d_cmd_idv()) //-idv 1:
          _root->addChild (mpvr->pv1()->vis_ptset_idv());
      }  
      //-l: show the matching vectors.
      if (dbmsh3d_cmd_len())
        _root->addChild (mpvr->vis_matching_vector());   
    }
    dbmsh3d_app_window_title += " -- " + mpvr->pv0()->dir_file() + " - " + mpvr->pv1()->dir_file();
  }
  //############### Compare Two Point Cloud Files of Corresponding Point Indices ############### 
  //-cnpt 1: 
  if (dbmsh3d_cmd_compare_pts()) {
    mpvr->p0()->reset_data();
    mpvr->p1()->reset_data();
    mpvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    mpvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    if (mpvr->p0()->load_points_files()) {
      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (mpvr->pv0()->vis_ptset_idv ());
      else
        _root->addChild (mpvr->pv0()->vis_ptset (COLOR_DARKGREEN));
    }
    else {      
      vul_printf (vcl_cout, "\n!! Point file %s does not exist!\n", mpvr->p0()->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }

    if (mpvr->p1()->load_points_files()) {
      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (mpvr->pv1()->vis_ptset_idv ());
      else
        _root->addChild (mpvr->pv1()->vis_ptset (COLOR_BLUE));
    }
    else {      
      vul_printf (vcl_cout, "\n!! Point file %s does not exist!\n", mpvr->p1()->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }

    _root->addChild (mpvr->vis_conn ());
  }

  //############### Debug the point-mesh distance code ###############  
  if (dbmsh3dr_cmd_debug_verr() == 1) { //-dbverr 1: debug point-mesh distance
    mpvr->pv0()->load_ply2 (dbul_get_dir_file (dbmsh3d_cmd_fileprefix()) + ".ply2", true);
    
    if (dbmsh3d_cmd_n1() != -1) { //-n1 1: draw the first surface mesh.
      _root->addChild (mpvr->pv0()->vis_mesh (false, true));
      
      if (dbmsh3d_cmd_idv()) //-idv 1: draw individual mesh vertex
        _root->addChild (mpvr->pv0()->vis_ptset_idv());
    }

    //-n2: number of test iterations.
    vgl_point_3d<double> pt;
    
    //-n: search top_n closest vertices.
    int top_n = dbmsh3d_cmd_n(); 
    if (top_n == -1)
      top_n = 10;

    //-l: as max amount of perturbation.
    bool r = test_perturb_compute_dist (mpvr->pv0()->mesh(), dbmsh3d_cmd_n2(), top_n, dbmsh3d_cmd_len(), pt);
    if (r == false) { //If error detected, draw the point pt.
      _root->addChild (draw_cube (pt, dbmsh3d_cmd_rc()*30, SbColor (1,0,0)));
    }
    else {
      vul_printf (vcl_cout, "\n\n  Test of Perturbation & Mesh Distance Query (%d Iterations) Passed!\n", dbmsh3d_cmd_n2());
    }
  }

  //############### Reduce Surface Normal Variance ###############  
  //-rsnv 1: estimate the surface normal variance (sausage thickness).
  //      2: reduce surface normal variance for set of surfaces (from list file).
  //      3: use bucketing.
  if (dbmsh3dr_cmd_rsnv() != 0) {
    //-f : the list file.
    mpvr->read_list_file (dbmsh3d_cmd_fileprefix());
    
    //Resize pro_[] to the size of # data files.
    //The pro[0] and pro[1] has been already initialized.
    const int N_DATA = mpvr->data_files().size();
    mpvr->pro().resize (N_DATA);
    for (int i=2; i<N_DATA; i++) {
      mpvr->pro()[i] = new dbmsh3d_pro_vis;
    }

    if (dbmsh3dr_cmd_rsnv() == 1) {
      //-rsnv 1: Estimate the surface variance (sausage thickness) along normal.
      //-dthr : error threshold ratio.
      //-n3 : top_n (def. 1)
      //-o : option (def. 1)
      mpvr->estimate_surf_var_along_normal (dbmsh3dr_cmd_dthr(), dbmsh3d_cmd_n3(), dbmsh3d_cmd_o());
    }
    else if (dbmsh3dr_cmd_rsnv() == 2) {
      //-rsnv 2: Reduce surface thickness and save point cloud.
      //-n2 : Reduce thickness for -n2 iterations.
      //-n3 : top_n (def. 1)
      //-o : option (def. 1)      
      //  option 1: brute-force, option 4: estimate final avg. dist.
      //  option 2: use bucketing, option 5: estimate final avg. dist.
      //  option 3: only keep one vertex (hack), option 6: estimate final avg. dist.
      mpvr->reduce_surf_thickness (dbmsh3dr_cmd_dthr(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_n3(), dbmsh3d_cmd_o());
      
      //Save all pt clouds into "prefix_all_pts.xyz" file.
      vcl_string all_pts_file = dbul_get_dir_prefix (dbmsh3d_cmd_fileprefix());
      all_pts_file += "-all-pts.xyz";
      mpvr->save_all_pts_xyz (all_pts_file.c_str());
    }
    else if (dbmsh3dr_cmd_rsnv() == 3) {
      //-rsnv 3: Reduce surface thickness using bucketing.
      //-npbkt: number of pts in each bucket, default 200k.
      mpvr->reduce_surf_thickness_bkt (dbmsh3dr_cmd_dthr(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_n3(),
                                       dbmsh3d_cmd_npbkt());
    }

    //Visualize all meshes in random colors.
    _root->addChild (mpvr->vis_meshes());

    //Release mem of pro_[2-]
    for (int i=2; i<N_DATA; i++) {
      delete mpvr->pro(i);
    }
    mpvr->pro().resize (2);

  }

  //############### Write Affine Transform File ###############  
  if (dbmsh3dr_cmd_icp() && dbmsh3d_cmd_align_ofile()) {
    //write mpvr->hmatrix_ to an alignment file
    vcl_string xform_file = dbmsh3d_cmd_align_ofile();
    dbmsh3d_write_xform_file (xform_file, mpvr->hmatrix_01());
  }

  return _root;
}



