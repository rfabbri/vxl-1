//: This is lemsvxlsrc/brcv/rec/dbsk3dr/vis/dbsk3dr_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <dbgl/dbgl_rigid_align.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3dr/dbmsh3dr_match.h>
#include <dbmsh3dr/pro/dbmsh3dr_cmdpara.h>

#include <dbsk3d/algo/dbsk3d_ms_algos.h>

#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_elm.h>
#include <dbsk3d/cmdproc/dbsk3d_process_vis.h>

#include <dbmsh3dr/vis/dbmsh3dr_vis.h>
#include <dbsk3dr/vis/dbsk3dr_draw.h>
#include <dbsk3dr/vis/dbsk3dr_cmdproc.h>

bool dbsk3dr_check_cmdproc (dbsk3dr_pro_vis* spvr, int argc, char **argv)
{
  if (!dbsk3dr_cmd_fuse() && !dbsk3dr_cmd_smrd() && !dbsk3dr_cmd_smre() && 
      !dbsk3dr_cmd_smatch() && !dbsk3dr_cmd_smatchc() && !dbsk3dr_cmd_smreb() &&
      !dbsk3dr_cmd_subsm())
    return false;
  else
    return true; 
}

void dbsk3dr_setup_provis_parameters (dbsk3dr_pro_vis* spvr)
{
  spvr->pv0()->pt_size_ = dbmsh3d_cmd_psz();
  spvr->pv1()->pt_size_ = dbmsh3d_cmd_psz();

  if (dbmsh3d_cmd_rc() != -1.0f) {
    spvr->pv0()->cube_size_ = dbmsh3d_cmd_rc();
    spvr->pv1()->cube_size_ = dbmsh3d_cmd_rc();
  }
  if (dbmsh3d_cmd_r() != -1.0f) {
    spvr->pv0()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
    spvr->pv1()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
  }
}

SoSeparator* dbsk3dr_cmdproc_execute (dbsk3dr_pro_vis* spvr)
{
  bool f1_load_success, f2_load_success;
  SoSeparator* _root = new SoSeparator;
  SoSeparator* vis;

  //############### Read Affine Transform File ###############  
  if (dbsk3dr_cmd_smrd() || dbsk3dr_cmd_smre() || dbsk3dr_cmd_smatch()) { 
    if (dbmsh3d_cmd_align_file()) {
      //-af 02_init_af.txt: initial alignment file to transform f2 (stored in spvr->hmatrix_)
      spvr->load_hmatrix_01 (dbmsh3d_cmd_align_file());
    }
  }

  //############### View CMS File 2 ###############  
  //-cms_2: 
  if (dbsk3d_cmd_cms_file2()) {
    spvr->pv1()->b_view_cms_ = true;
    spvr->pv1()->set_dir_file (dbul_get_dir_file (dbsk3d_cmd_cms_file2()));
  }
  if (spvr->pv1()->b_view_cms_) {
    spvr->pv1()->reset_mesh ();
    spvr->pv1()->reset_fs_mesh ();
    spvr->pv1()->reset_ms_hypg ();
    spvr->pv1()->load_cms ();

    //old code on xform!
    if (dbmsh3d_cmd_align_file()) { //read the alignment from file and perform transformation.
      spvr->pv1()->load_hmatrix (dbmsh3d_cmd_align_file());
      vcl_cout << "Transforming mesh ...\n"; // transform the reference mesh
      dbmsh3d_apply_xform (spvr->pv1()->mesh(), spvr->pv1()->hmatrix());
      dbmsh3d_apply_xform (spvr->pv1()->fs_mesh(), spvr->pv1()->hmatrix());
      vcl_cout << "done.\n";
    }

    if (dbmsh3d_cmd_gui()) {
      spvr->pv1()->compute_vertex_node_size ();
      _root->addChild (spvr->pv1()->vis_ms_hypg());
      
      if (dbsk3d_cmd_bnd() == 1) { //-bnd 1: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (spvr->pv1()->vis_ptset());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (spvr->pv1()->vis_ptset_idv());
      }
      if (dbsk3d_cmd_bnd() == 2) { //-bnd 2: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (spvr->pv1()->vis_mesh_pts_edges());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (spvr->pv1()->vis_ptset_idv());
      }
      else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh in transparency.
        if (spvr->pv1()->m_transp_ == 0.0f)
          spvr->pv1()->m_transp_ = 0.5;
        _root->addChild (spvr->pv1()->vis_mesh (false, dbmsh3d_cmd_shapehints()));
      }
    }

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + spvr->pv1()->dir_file();
  }
  //############### View SG File 2 ###############
  //-sg_2: 
  if (dbsk3d_cmd_sg_file2()) {
    spvr->pv1()->b_view_sg_ = true;
    spvr->pv1()->set_dir_file (dbul_get_dir_file (dbsk3d_cmd_sg_file2()));
  }
  if (spvr->pv1()->b_view_sg_) {
    spvr->pv1()->load_sg ();

    //-osg: 
    if (dbsk3d_cmd_sg_ofile()) {
      spvr->pv1()->save_sg (dbsk3d_cmd_sg_ofile());
    }

    if (dbmsh3d_cmd_gui()) {
      spvr->pv1()->compute_vertex_node_size ();
      _root->addChild (spvr->pv1()->vis_ms_graph_sa ());
    }

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + spvr->pv1()->dir_file();
  }
  //############### List Files ###############
  //-list: process the shock-related items in the list file.
  if (dbmsh3dr_cmd_listfile()) {
    //Read the list file and view each file.
    //-v 0: regular, 1: draw in single color, 2: color each sheet.
    vcl_string listfile = dbmsh3dr_cmd_listfile();
    _root->addChild (spvr->pv0()->vis_list_file (listfile, 0, dbmsh3d_cmd_v()));

    dbmsh3d_app_window_title += " -- " + listfile;
  }  

  //############### Smoothing + Fusing Scan Data ###############  
  //-fuse 1: Read in two 3PI data sets and visualize initial alignment.
  if (dbsk3dr_cmd_fuse() == 1) {
    spvr->p0()->set_dir_prefix (dbmsh3d_cmd_fileprefix1());
    spvr->p1()->set_dir_prefix (dbmsh3d_cmd_fileprefix2());

    //Read in the cropped .3pi file 'f1_crop.3pi' and 'f2_crop.3pi'
    bool result = spvr->p0()->load_3pi (spvr->p0()->dir_prefix() + "_crop.3pi");    
    assert (result);
    result = spvr->p1()->load_3pi (spvr->p1()->dir_prefix() + "_crop.3pi");    
    assert (result);

    //Copy point data into pts[].
    spvr->p0()->move_sg3pi_to_pts();
    spvr->p1()->move_sg3pi_to_pts();

    //Read in the initial alignment file 'f1.txt' and 'f2.txt' and apply xform.
    spvr->p0()->load_hmatrix (spvr->p0()->dir_prefix() + "_fixed.txt");
    spvr->p0()->apply_xform_hmatrix ();
    spvr->p1()->load_hmatrix (spvr->p1()->dir_prefix() + "_fixed.txt");
    spvr->p1()->apply_xform_hmatrix ();

    //Display the init. alignment of f1 and f2.
    _root->addChild (spvr->pv0()->vis_ptset (COLOR_DARKGREEN, dbmsh3d_cmd_idv()!=0));
    _root->addChild (spvr->pv1()->vis_ptset (COLOR_BLUE, dbmsh3d_cmd_idv()!=0));
  }
  //-fuse 2,3,4: Read in two smoothed meshed PLY data set and fine-tune the alignment.
  if (dbsk3dr_cmd_fuse()==2 || dbsk3dr_cmd_fuse()==3 || dbsk3dr_cmd_fuse()==4) {
    spvr->p0()->set_dir_prefix (dbmsh3d_cmd_fileprefix1());
    spvr->p1()->set_dir_prefix (dbmsh3d_cmd_fileprefix2());
    
    //Read in the cropped .3pi file 'f1_crop.3pi' and 'f2_crop.3pi'
    bool result = spvr->p0()->load_3pi (spvr->p0()->dir_prefix() + "_crop.3pi");  
    assert (result);
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    // -icpthr: icp_th_ratio.
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());
    spvr->p0()->reset_data();
    spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_MESH);

    //Read in the two meshed .PLY files.
    vcl_string f1_mesh = spvr->p0()->dir_prefix();
    vcl_string f2_mesh = spvr->p1()->dir_prefix();
    if (dbsk3dr_cmd_fuse() == 2) {
      f1_mesh += ".ply";
      f2_mesh += ".ply";
    }
    else if (dbsk3dr_cmd_fuse() == 3) {
      f1_mesh += "_gsm.ply";
      f2_mesh += "_gsm.ply";
    }
    else if (dbsk3dr_cmd_fuse() == 4) {
      f1_mesh += "_dcs.ply";
      f2_mesh += "_dcs.ply";
    }
    result = spvr->p0()->load_ply (f1_mesh);
    assert (result);
    result = spvr->p1()->load_ply (f2_mesh);
    assert (result);

    //Read in the initial alignment file 'f1.txt' and 'f2.txt' and apply them.
    spvr->p0()->load_hmatrix (spvr->p0()->dir_prefix() + "_fixed.txt");
    spvr->p0()->apply_xform_hmatrix ();
    spvr->p1()->load_hmatrix (spvr->p1()->dir_prefix() + "_fixed.txt");
    spvr->p1()->apply_xform_hmatrix ();

    //Visualize the initial alignment.
    if (dbmsh3d_cmd_gui()) {        
      vis = spvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
      _root->addChild (vis); //Draw the 1st mesh.        
      vis = spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_BLUE, true);
      _root->addChild (vis); //Draw the 2nd mesh.
    }

    //Estimate the Pt-Mesh dist. d_pt-mesh (A*, B*) of init. alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B*) of init. alignment:\n\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_B_mean = spvr->dist_mean();

    //Register A and B using Pt-Mesh ICP.
    int n_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
    if (spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th)) {
      spvr->pv0()->mesh()->build_face_IFS ();
      //draw the mesh of -f2 after alignment.
      spvr->pv1()->mesh()->build_face_IFS ();
      _root->addChild (spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));
    }

    //Estimate the Pt-Mesh dist. d_pt-mesh (A*, B^*) after Pt-Mesh ICP alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B^*) after Pt-Mesh ICP:\n\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_Bh_mean = spvr->dist_mean();

    //6) Overwrite f2's alignmet file with new f2's xform matrix: newH2 = H12 * H2.
    vgl_h_matrix_3d<double> newH2 = spvr->hmatrix_01() * spvr->p1()->hmatrix();
    dbmsh3d_write_xform_file (spvr->p1()->dir_prefix() + "_fixed_new.txt", newH2);

    //7) Output summary.
    vul_printf (vcl_cout, "\n\n Summary: \n\n");
    vul_printf (vcl_cout, "  pp_icp_dist_th %f = icp_thr %.2f * avg_samp_dist %f.\n", 
                pp_icp_dist_th, dbmsh3dr_cmd_icpthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  error_dist_th %f = dist_thr %.2f * avg_samp_dist %f.\n", 
                error_dist_th, dbmsh3dr_cmd_dthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B*)  mean: %f\n", d_pt_mesh_A_B_mean);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B^*) mean: %f\n", d_pt_mesh_A_Bh_mean);
    vul_printf (vcl_cout, "  Alignment error is reduced by %.3f%%.\n", 
                (d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean)/d_pt_mesh_A_B_mean * 100);
  }
  //-fuse 5: estimate edge weight (overlapping) of adjacency graph.
  else if (dbsk3dr_cmd_fuse() == 5) {
    //Read in the list scan file: 'all_scans_init.txt' for data_files[] and align_files[].
    if (dbmsh3d_cmd_fileprefix() == NULL ||
        spvr->read_list_file (dbmsh3d_cmd_fileprefix()) == false) {
      vcl_string file = dbmsh3d_cmd_fileprefix();
      vul_printf (vcl_cout, "  Error reading %s.\n", file.c_str());
    }
    else {
      //if -n1 and -n2 specified, only do the pair of (n1, n2).
      if (dbmsh3d_cmd_n1() != dbmsh3d_cmd_n2()) {
        //Read in the scans[i] .3pi file
        int i = dbmsh3d_cmd_n1();
        int j = dbmsh3d_cmd_n2();
        if (i>=int(spvr->data_files().size()) || j>=int(spvr->data_files().size())) {
          vul_printf (vcl_cout, "  Error: -n1, -n2 too large!\n");
          dbmsh3d_cmd_gui() = 0;
        }
        else {
          spvr->p0()->set_dir_file (spvr->data_files(i));
          bool result = spvr->p0()->load_3pi (spvr->data_files(i));    
          assert (result);

          double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
          //-dthr : error dist ratio.
          float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());

          spvr->p0()->move_sg3pi_to_pts();
          //Read in the initial alignment file 'f1.txt'
          spvr->p0()->load_hmatrix (spvr->align_files(i));
          spvr->p0()->apply_xform_hmatrix (); //Apply hmatrix xform to f1.

          spvr->p1()->reset_data();
          //Read in the scans[j] .3pi file
          spvr->p1()->set_dir_file (spvr->data_files(j));
          result = spvr->p1()->load_3pi (spvr->data_files(j));    
          assert (result);
          spvr->p1()->move_sg3pi_to_pts();
          //Read in the initial alignment file 'f2.txt'
          spvr->p1()->load_hmatrix (spvr->align_files(j));
          spvr->p1()->apply_xform_hmatrix (); //Apply hmatrix xform to f2.

          int n_overlap = spvr->get_overlapped_pts (error_dist_th);

          //Draw data1 in green, overlapped points (marked as valid) in lighter color.
          spvr->p0()->_copy_valid_V_to_pts (true);
          _root->addChild (draw_ptset (spvr->p0()->pts(), COLOR_GREEN));
          spvr->p0()->_copy_valid_V_to_pts (false);
          _root->addChild (draw_ptset (spvr->p0()->pts(), COLOR_DARKGREEN));

          //Draw data1 in blue, overlapped points (marked as valid) in lighter color.
          spvr->p1()->_copy_valid_V_to_pts (true);
          _root->addChild (draw_ptset (spvr->p1()->pts(), COLOR_BLUE));
          spvr->p1()->_copy_valid_V_to_pts (false);
          _root->addChild (draw_ptset (spvr->p1()->pts(), COLOR_DARKBLUE));
        }
      }
      else {
        //Compute the extent of overlap as edge weight of the adjacency graph.
        vnl_matrix<double> adj_graph;
        //-dthr : error dist ratio.
        spvr->compute_adj_graph_weight (dbmsh3dr_cmd_dthr(), adj_graph);
      }
    }
  }
  //-fuse 6: Read in a scan list file and a minimum spanning tree of adjacent scans.
  //         Fine tune each alignment to produce a final mesh.
  else if (dbsk3dr_cmd_fuse() == 6) {
    //-f list scan file: 'all_scans_init_gsm.txt' for data_files[] and align_files[].
    if (dbmsh3d_cmd_fileprefix() == NULL ||
        spvr->read_list_file (dbmsh3d_cmd_fileprefix()) == false) {
      vcl_string file = dbmsh3d_cmd_fileprefix();
      vul_printf (vcl_cout, "  Error reading %s.\n", file.c_str());
    }
    else {
      //Specify the MST (sid1, sid2): # overlapping points.
      vcl_vector<vcl_pair<int, int> > MST;
      MST.resize (19);
      
      //The following is for Q:\knee-cartilage\results\fuse_scans\knee_1133RT\wc_trial_7
      MST[0].first =   7;  MST[0].second =  8;
      MST[1].first =  15;  MST[1].second = 16;
      MST[2].first =   8;  MST[2].second = 16;
      MST[3].first =   9;  MST[3].second = 14;
      MST[4].first =   6;  MST[4].second =  7;
      MST[5].first =  10;  MST[5].second = 11;
      MST[6].first =  17;  MST[6].second = 18;
      MST[7].first =  12;  MST[7].second = 14;
      MST[8].first =   3;  MST[8].second =  4;
      MST[9].first =  19;  MST[9].second = 20;
      MST[10].first =  5; MST[10].second = 20;
      MST[11].first =  7; MST[11].second = 11;
      MST[12].first =  2; MST[12].second =  3;
      MST[13].first =  4; MST[13].second = 18;
      MST[14].first =  4; MST[14].second =  5;
      MST[15].first =  3; MST[15].second = 13;
      MST[16].first =  1; MST[16].second = 16;
      MST[17].first = 12; MST[17].second = 13;
      MST[18].first = 10; MST[18].second = 14;
      int root_sid = 13; //14-1


      //These are Matlab indices, should reduce by 1.
      for (unsigned int i=0; i<MST.size(); i++) {
        MST[i].first--;
        MST[i].second--;
      }

      //Output the fine-tuned alignment file for each scan: knee_1133RT_nc_trial_2_##_mst.txt
      //-n2: 100
      //-dthr:
      //-icpcv:
      int max_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
      spvr->fuse_scan_MST (MST, root_sid, dbmsh3dr_cmd_dthr(),
                           max_ICP_iter, dbmsh3dr_cmd_icpcv());
    }
  }
  //-fuse 7: Refine the MST fusing result.
  else if (dbsk3dr_cmd_fuse() == 7) {
    //-f list scan file: 'all_scans_mst_gsm.txt' for data_files[] and align_files[].
    if (dbmsh3d_cmd_fileprefix() == NULL ||
        spvr->read_list_file (dbmsh3d_cmd_fileprefix()) == false) {
      vcl_string file = dbmsh3d_cmd_fileprefix();
      vul_printf (vcl_cout, "  Error reading %s.\n", file.c_str());
    }
    else {
      //Similar to -fuse 5: compute the weight for all edges.
      vnl_matrix<double> adj_graph;
      //-dthr : error dist ratio.
      spvr->compute_adj_graph_weight (dbmsh3dr_cmd_dthr(), adj_graph);

    }
  }
  //############### Mega Process for Smoothing + Meshing + Registration + Distance Estimation ###############  
  if (dbsk3dr_cmd_smrd()==1) { 
    //-smrd 1: estimate the mean and RMS of d_pt-pt (A, B) after Pt-Pt ICP on init. alignment.

    //Load input 3pi files and optional alignment files.
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    bool result = spvr->p0()->load_3pi ();
    assert (result);
    spvr->p0()->load_hmatrix ();

    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    result = spvr->p1()->load_3pi ();
    assert (result);
    spvr->p1()->load_hmatrix ();

    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    // -icpthr: icp_th_ratio.
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //Copy sg3pi data into pts[].
    spvr->p0()->move_sg3pi_to_pts ();
    spvr->p1()->move_sg3pi_to_pts ();
    
    //Estimate init. error:
    spvr->compute_pp_error (error_dist_th);
    double init_pp_mean_error = spvr->dist_mean();

    //Transform the data using the specified affine xform file.
    if (spvr->p0()->is_hmatrix_identity() == false) 
      spvr->p0()->apply_xform_hmatrix ();
    if (spvr->p1()->is_hmatrix_identity() == false) 
      spvr->p1()->apply_xform_hmatrix ();

    //draw the input point cloud of -f1.
    _root->addChild (spvr->pv0()->vis_ptset (COLOR_DARKGREEN, dbmsh3d_cmd_idv()!=0));
    //draw the input point cloud of -f2.
    _root->addChild (spvr->pv1()->vis_ptset (COLOR_BLUE, dbmsh3d_cmd_idv()!=0));

    //Register using Pt-Pt ICP.
    // -n2: ICP maximum iterations, default 50.    
    int n_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
    result = spvr->run_pp_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), pp_icp_dist_th);
    assert (result);

    //Estimate d_pt-pt (A, B): Pt-Pt distance after init Pt-Pt ICP registration.      
    vul_printf (vcl_cout, "\nPt-Pt dist. d_pt-pt(A, B) of alignment of init. input:\n\n");
    spvr->compute_pp_error (error_dist_th);

    //Output final hmatrix to 02_icp1_af.txt      
    vcl_string out_af_file = "02_icp1_af.txt";
    dbmsh3d_write_xform_file (out_af_file, spvr->hmatrix_01());

    //draw the point cloud of -f2 after alignment.
    _root->addChild (spvr->pv1()->vis_ptset (COLOR_RED, dbmsh3d_cmd_idv()!=0));
    
    //Output summary.
    vul_printf (vcl_cout, "\n Summary:\n");
    vul_printf (vcl_cout, "  pp_icp_dist_th %f = icp_thr %.2f * avg_samp_dist %f.\n", 
                pp_icp_dist_th, dbmsh3dr_cmd_icpthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  error_dist_th %f = dist_thr %.2f * avg_samp_dist %f.\n", 
                error_dist_th, dbmsh3dr_cmd_dthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  init d_pt-pt (A, B) mean: %f\n", init_pp_mean_error);
    vul_printf (vcl_cout, "  d_pt-pt (A, B)  mean: %f\n", spvr->dist_mean());
  }
  else if (dbsk3dr_cmd_smrd()==2) {     
    //-smrd 2: estimate the diff of dist. before/after meshing + Pt-Mesh ICP.

    //1) Load input 3pi files and optional alignment files.
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    bool result = spvr->p0()->load_3pi (dbmsh3d_cmd_fileprefix1());    
    assert (result);
    spvr->p0()->load_hmatrix ();
    
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    result = spvr->p1()->load_3pi (dbmsh3d_cmd_fileprefix2());
    assert (result);
    spvr->p1()->load_hmatrix ();
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //2) Prepare surface meshes.
    //Try to load the mesh surface files if available.
    vcl_string nosm_mesh_file_0 = spvr->p0()->dir_file() + "_pw_init_nosm";
    vcl_string nosm_mesh_file_1 = spvr->p1()->dir_file() + "_pw_init_nosm";
    bool result0 = spvr->p0()->load_ply (nosm_mesh_file_0);
    bool result1 = spvr->p1()->load_ply (nosm_mesh_file_1);

    if (result0 && result1) {
      vul_printf (vcl_cout, "Mesh files %s.ply and %s.ply read successfully.\n",
                  nosm_mesh_file_0.c_str(), nosm_mesh_file_1.c_str());
      vul_printf (vcl_cout, "  Skip the meshing process.\n\n");
      assert (spvr->p0()->mesh()->vertexmap().size() == spvr->p0()->sg3pi()->get_num_points());
      spvr->p0()->reset_sg3pi ();
      spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      assert (spvr->p1()->mesh()->vertexmap().size() == spvr->p1()->sg3pi()->get_num_points());
      spvr->p1()->reset_sg3pi ();
      spvr->p1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
    }
    else {
      //Transform the data using the specified affine xform file.
      if (spvr->p0()->is_hmatrix_identity() == false) 
        spvr->p0()->apply_xform_hmatrix ();
      if (spvr->p1()->is_hmatrix_identity() == false) 
        spvr->p1()->apply_xform_hmatrix ();

      spvr->p0()->move_sg3pi_to_mesh ();
      spvr->p1()->move_sg3pi_to_mesh ();

      //Meshing the two surfaces if the surface file (prefix.ply) is not there.
      //-sphrr 5: bounding sphere radius ratio.
      //-sphsr 1: bounding sphere sample ratio.
      //-msr: max sample dist. ratio
      //-segop 0: topo. option.
      spvr->p0()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
      spvr->p1()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
      //Save the smoothing meshes into .ply files.
      if (dbmsh3d_cmd_ofile()>=1) {
        spvr->p0()->save_ply (nosm_mesh_file_0);
        spvr->p1()->save_ply (nosm_mesh_file_1);
      }
    }    
    if (dbmsh3d_cmd_gui()) { //Visualize the meshed results.
      vis = spvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
      _root->addChild (vis);
      vis = spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_BLUE, true);
      _root->addChild (vis);
    }

    //3) Estimate the Pt-Mesh dist. d_pt-mesh (A*, B*) of init. input.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B*) of alignment of init. input:\n\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_B_mean = spvr->dist_mean();

    //4) Register A and B using Pt-Mesh ICP.
    int n_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
    result = spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th);
    assert (result); 
    //draw the mesh of -f2 after alignment.
    _root->addChild (spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));
    if (dbmsh3d_cmd_ofile()>=1) //Save the registered smoothing results into .ply files.
      spvr->p1()->save_ply (spvr->p1()->dir_file() + "_icp");

    //5) Estimate the Pt-Mesh dist. d_pt-mesh (A*, B^*) after Pt-Mesh ICP alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B^*) after Pt-Mesh ICP:\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_Bh_mean = spvr->dist_mean();

    //6) Output summary.
    vul_printf (vcl_cout, "\n Summary:\n");
    vul_printf (vcl_cout, "  pp_icp_dist_th %f = icp_thr %.2f * avg_samp_dist %f.\n", 
                pp_icp_dist_th, dbmsh3dr_cmd_icpthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  error_dist_th %f = dist_thr %.2f * avg_samp_dist %f.\n", 
                error_dist_th, dbmsh3dr_cmd_dthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  No smoothing is performed.\n");

    vul_printf (vcl_cout, "  d_pt-mesh (A*, B*)  mean: %f\n", d_pt_mesh_A_B_mean);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B^*) mean: %f\n", d_pt_mesh_A_Bh_mean);
    vul_printf (vcl_cout, "  diff. of mean of d(A*, B*) - d(A*, B^*): %f (%.3f%%)\n", 
                d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean,
                (d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean)/d_pt_mesh_A_B_mean * 100);
  }  
  else if (dbsk3dr_cmd_smrd()==3 || dbsk3dr_cmd_smrd()==4) {     
    //-smrd 3,4: estimate the diff of dist. before/after smoothing + meshing + Pt-Mesh ICP.

    //1) Load input 3pi files and optional alignment files.
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    bool result = spvr->p0()->load_3pi (dbmsh3d_cmd_fileprefix1());    
    assert (result);
    spvr->p0()->load_hmatrix ();
    
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    result = spvr->p1()->load_3pi (dbmsh3d_cmd_fileprefix2());
    assert (result);
    spvr->p1()->load_hmatrix ();
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //2) Prepare smoothed surface meshes.
    //Try to load the smoothed mesh surface files if available.
    vcl_string sm_mesh_file_0, sm_mesh_file_1;
    if (dbsk3dr_cmd_smrd() == 3) { 
      sm_mesh_file_0 = spvr->p0()->dir_file() + "_gsm";
      sm_mesh_file_1 = spvr->p1()->dir_file() + "_gsm";
    }
    else if (dbsk3dr_cmd_smrd() == 4) {
      sm_mesh_file_0 = spvr->p0()->dir_file() + "_dcs";
      sm_mesh_file_1 = spvr->p1()->dir_file() + "_dcs";
    }
    bool result0 = spvr->p0()->load_ply (sm_mesh_file_0);
    bool result1 = spvr->p1()->load_ply (sm_mesh_file_1);
    int nstep;

    if (result0 && result1) {
      vul_printf (vcl_cout, "Smoothed Mesh files %s.ply and %s.ply read successfully.\n",
                  sm_mesh_file_0.c_str(), sm_mesh_file_1.c_str());
      vul_printf (vcl_cout, "  Skip the meshing process.\n\n");
      assert (spvr->p0()->mesh()->vertexmap().size() == spvr->p0()->sg3pi()->get_num_points());
      spvr->p0()->reset_sg3pi ();
      spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
      assert (spvr->p1()->mesh()->vertexmap().size() == spvr->p1()->sg3pi()->get_num_points());
      spvr->p1()->reset_sg3pi ();
      spvr->p1()->set_pro_data (dbmsh3d_pro_base::PD_MESH);
    }
    else {
      //Smooth the scan data.
      // -smrd 2: no smoothing.
      // -smrd 3: Gaussian smoothing.
      // -smrd 4: Discrete Curve Shortening (DCS) smoothing. 
      if (dbsk3dr_cmd_smrd() == 3) { 
        //-smrd3: 2D Gaussian smoothing (intra- and inter- scan lines)
        //-n STEPS: default 10.
        nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n();
        //-gsr: G_sigma_ratio
        //-grr: G_radius_ratio
        //-thr: th_ratio
        gaussian_smooth_2d_3pi (spvr->p0()->sg3pi(), nstep, 
                                dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());      
        //Note that the p2->sg3pi() is not xform by init_af.txt yet.
        gaussian_smooth_2d_3pi (spvr->p1()->sg3pi(), nstep, 
                                dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
      }
      else if (dbsk3dr_cmd_smrd() == 4) { 
        //-smrd 4: Iterative DCS smoothing intra- and inter- scanlines 
        //-n STEPS: default 20.
        nstep = (dbmsh3d_cmd_n() == -1) ? 20 : dbmsh3d_cmd_n();
        //-l: DCS_psi step size.
        //-thr: th_ratio
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1      
        vul_printf (vcl_cout, "\n\tApply DCS smoothing %d times (DCS_psi %f, th_r %f).\n", 
                    nstep, DCS_psi, dbmsh3d_cmd_thr());

        //Estimate intra- and inter- scanline sample distance.
        spvr->p0()->sg3pi()->get_sl_sample_dist ();
        spvr->p1()->sg3pi()->get_sl_sample_dist ();

        double d_intra = spvr->p0()->sg3pi()->intra_sl_dist();
        double d_inter = spvr->p0()->sg3pi()->inter_sl_dist();
        int n_intra, n_inter;

        if (d_intra < d_inter) {
          n_intra = 1;
          n_inter = int (d_inter / d_intra);
        }
        else {
          n_inter = 1;
          n_intra = int (d_intra / d_inter);
        }

        for (int i=0; i<nstep; i++) {
          dcs_smooth_scanlines_2dirs_3pi (spvr->p0()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), n_intra, n_inter);
          dcs_smooth_scanlines_2dirs_3pi (spvr->p1()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), n_intra, n_inter);
        }
      }
      else
        assert (0);
      
      //Save the smoothing results (before applying xform) into .3pi files.
      if (dbmsh3d_cmd_ofile()>=1) {
        spvr->p0()->save_3pi (sm_mesh_file_0);
        spvr->p1()->save_3pi (sm_mesh_file_1);  
      }
      //Transform the data using the specified affine xform file.
      if (spvr->p0()->is_hmatrix_identity() == false) 
        spvr->p0()->apply_xform_hmatrix ();
      if (spvr->p1()->is_hmatrix_identity() == false) 
        spvr->p1()->apply_xform_hmatrix ();

      spvr->p0()->move_sg3pi_to_mesh ();
      spvr->p1()->move_sg3pi_to_mesh ();
      
      if (dbmsh3d_cmd_ofile()>1) { //Save the smoothing results into .xyz files.
        spvr->p0()->save_xyz (sm_mesh_file_0);      
        spvr->p1()->save_xyz (sm_mesh_file_1);
      }

      //Meshing the two surfaces for -smreo 2.
      //-sphrr 5: bounding sphere radius ratio.
      //-sphsr 1: bounding sphere sample ratio.
      //-msr: max sample dist. ratio
      //-segop 0: topo. option.
      spvr->p0()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
      spvr->p1()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
      
      //Save the smoothing meshes into .ply files.
      if (dbmsh3d_cmd_ofile()>=1) {
        spvr->p0()->save_ply (sm_mesh_file_0);      
        spvr->p1()->save_ply (sm_mesh_file_1);
      }
    }    
    if (dbmsh3d_cmd_gui()) { //Visualize the smoothing results.
      vis = spvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
      _root->addChild (vis);
      vis = spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                   dbmsh3d_cmd_shapehints(), COLOR_BLUE, true);
      _root->addChild (vis);
    }

    //3) Estimate the Pt-Mesh dist. d_pt-mesh (A*, B*) of init. Pt-Pt ICP alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B*) of alignment of init. input:\n\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_B_mean = spvr->dist_mean();

    //4) Register A and B using Pt-Mesh ICP.
    int n_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
    result = spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th);
    assert (result); 
    //draw the mesh of -f2 after alignment.
    _root->addChild (spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));

    //Save the registered smoothing results into .ply files.
    if (dbmsh3d_cmd_ofile()>=1)
      spvr->p1()->save_ply (sm_mesh_file_1 + "_icp.ply");

    //5) Estimate the Pt-Mesh dist. d_pt-mesh (A*, B^*) after Pt-Mesh ICP alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B^*) after Pt-Mesh ICP:\n\n");
    // -n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    double d_pt_mesh_A_Bh_mean = spvr->dist_mean();

    //6) Output summary.
    vul_printf (vcl_cout, "\n Summary: \n");
    vul_printf (vcl_cout, "  pp_icp_dist_th %f = icp_thr %.2f * avg_samp_dist %f.\n", 
                pp_icp_dist_th, dbmsh3dr_cmd_icpthr(), avg_samp_dist);
    vul_printf (vcl_cout, "  error_dist_th %f = dist_thr %.2f * avg_samp_dist %f.\n", 
                error_dist_th, dbmsh3dr_cmd_dthr(), avg_samp_dist);
    if (dbsk3dr_cmd_smrd() == 3) {
      if (result0 && result1)
        vul_printf (vcl_cout, "  Gaussian smoothing data load from files.\n");
      else
        vul_printf (vcl_cout, "  Gaussian smoothing: %d steps, sigma_r %f.\n", nstep, dbmsh3d_cmd_gsr());
    }
    else if (dbsk3dr_cmd_smrd() == 4) {
      if (result0 && result1)
        vul_printf (vcl_cout, "  Curve shortening data load from files.\n");
      else
        vul_printf (vcl_cout, "  Curve shortening smoothing: %d steps.\n", nstep);
    }
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B*)  mean: %f\n", d_pt_mesh_A_B_mean);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B^*) mean: %f\n", d_pt_mesh_A_Bh_mean);
    vul_printf (vcl_cout, "  diff. of mean of d(A*, B*) - d(A*, B^*): %f (%.3f%%)\n", 
                d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean,
                (d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean)/d_pt_mesh_A_B_mean * 100);
  }
  //############### Mega Process for Smoothing + Meshing + Registration + Error Estimation ###############  
  //-smre:  old obsolete code!!
  if (dbsk3dr_cmd_smre()) { 
    //Example command line: -smre # -f1 -f2 -af.
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    
    //0) Generate optional run files to examine output data files.
    if (dbmsh3d_cmd_ofile())
      gen_smre_run_files (dbmsh3d_cmd_fileprefix1(), dbmsh3d_cmd_fileprefix2(),
                          dbsk3dr_cmd_smre(), dbsk3dr_cmd_smreo());

    //1) Load f1.3pi and f2.3pi. -f1: specify .3pi file1, -f2: specify .3pi file2
    bool result = spvr->p0()->load_3pi ();
    assert (result);
    result = spvr->p1()->load_3pi ();
    assert (result);
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    ///spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    ///spvr->p1()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //Copy point data into pts[] to estimate initial ICP alignment error.
    spvr->p0()->move_sg3pi_to_pts();
    spvr->p1()->move_sg3pi_to_pts();

    //Transform 2nd dataset (p2->pts[]) using the specified affine xform file.
    if (spvr->is_hmatrix_01_identity() == false) 
      spvr->xform_hmatrix_01_pro1 ();

    //Estimate init. alignment error.
    vul_printf (vcl_cout, "\nPt-Pt init. alignment error:\n");
    spvr->compute_pp_error (error_dist_th);

    //Register using Pt-Pt ICP and keep error for later comparison.
    // -dthr: dist_th_ratio for error estimation.
    // -n2: ICP maximum iterations, default 50.
    int n_ICP_iter = (dbmsh3d_cmd_n2() != -1) ? dbmsh3d_cmd_n2() : 50;
    if (spvr->run_pp_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), pp_icp_dist_th)) {
      //Estimate Pt-Pt alignment error after init registration.      
      vul_printf (vcl_cout, "\nPt-Pt alignment error after init. Pt-Pt ICP:\n");
      spvr->compute_pp_error (error_dist_th);
    }

    //Mesh original surface.
    spvr->p0()->move_pts_to_mesh();
    spvr->p1()->move_pts_to_mesh();
    spvr->p0()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                     dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
    spvr->p1()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                     dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);

    //Regsiter original surface using Pt-Mesh ICP
    //Store the error estimation of init. Pt-Mesh ICP result for comparison. 
    vcl_vector<double> di_orig_pp_icp;
    vcl_vector<double> di_orig_pf_icp;
    if (spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th)) {
      vul_printf (vcl_cout, "\nPt-Pt alignment error after init. Pt-Mesh ICP:\n");
      spvr->compute_pp_error (error_dist_th);
      di_orig_pp_icp.insert (di_orig_pp_icp.begin(), spvr->min_dists().begin(), spvr->min_dists().end());

      //Estimate Pt-Mesh alignment error after init registration.      
      vul_printf (vcl_cout, "\nPt-Plane alignment error after init. Pt-Mesh ICP:\n");
      spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
      di_orig_pf_icp.insert (di_orig_pf_icp.begin(), spvr->min_dists().begin(), spvr->min_dists().end());
    }

    spvr->p0()->reset_mesh();
    spvr->p1()->reset_mesh();
    spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    spvr->p1()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);

    //2) Process the scan data
    // -smre 1: no smoothing.
    // -smre 2: Gaussian smoothing.
    // -smre 3: Discrete Curve Shortening (DCS) smoothing.
    // -smre 4: Hybrid (both Gaussian and DCS) smoothing.
    if (dbsk3dr_cmd_smre() == 1) { 
      //-smre 1: no smoothing.
    }
    else if (dbsk3dr_cmd_smre() == 2) { 
      //-smre2: 2D Gaussian smoothing (intra- and inter- scan lines)
      //-n STEPS: default 1.
      int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n();
      //-gsr: G_sigma_ratio
      //-grr: G_radius_ratio
      //-thr: th_ratio
      gaussian_smooth_2d_3pi (spvr->p0()->sg3pi(), nstep, 
                              dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
      
      //Note that the p2->sg3pi() is not xform by init_af.txt yet.
      gaussian_smooth_2d_3pi (spvr->p1()->sg3pi(), nstep, 
                              dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
    }
    else if (dbsk3dr_cmd_smre() == 3) { 
      //-smre 3: Iterative DCS smoothing intra- and inter- scanlines 
      //-n STEPS: default 20.
      int nstep = (dbmsh3d_cmd_n() == -1) ? 20 : dbmsh3d_cmd_n();
      //-l: DCS_psi step size.
      //-thr: th_ratio
      float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1      
      vul_printf (vcl_cout, "\n\tApply DCS smoothing %d times (DCS_psi %f, th_r %f).\n", 
                  nstep, DCS_psi, dbmsh3d_cmd_thr());

      //Estimate intra- and inter- scanline sample distance.
      spvr->p0()->sg3pi()->get_sl_sample_dist ();
      spvr->p1()->sg3pi()->get_sl_sample_dist ();

      for (int i=0; i<nstep; i++) {
        dcs_smooth_scanlines_2dirs_3pi (spvr->p0()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), 1, 1);
        dcs_smooth_scanlines_2dirs_3pi (spvr->p1()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), 1, 1);
      }
    }
  
    //After smoothing, transform 2nd dataset (p2->sg[]) using the init. affine xform file.
    if (dbmsh3d_cmd_align_file()) {
      spvr->load_hmatrix_01 (dbmsh3d_cmd_align_file());
      //transform pro[1] using the specified affine xform file
      spvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
      spvr->xform_hmatrix_01_pro1 ();
    }

    //Copy sg[] to mesh. All following processes are on the mesh data structures.
    spvr->p0()->move_sg3pi_to_mesh ();
    spvr->p1()->move_sg3pi_to_mesh ();

    //Save the smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();

      //Save the resulting .3pi after smoothing
      if (dbsk3dr_cmd_smre() == 2) {
        spvr->p0()->save_3pi (outfile1 + "_gsm.3pi");
        spvr->p1()->save_3pi (outfile2 + "_gsm.3pi");  
      }
      else if (dbsk3dr_cmd_smre() == 3) {
        spvr->p0()->save_3pi (outfile1 + "_dcs.3pi");
        spvr->p1()->save_3pi (outfile2 + "_dcs.3pi");  
      }

      spvr->p0()->reset_sg3pi ();
      spvr->p1()->reset_sg3pi ();

      //Save the original pt_set after init. alignment.
      spvr->p0()->save_xyz ();
      spvr->p1()->save_xyz ();

      //Save the resulting pt_set or mesh after smoothing.
      if (dbsk3dr_cmd_smreo() == 1) { //Save Pt-Set.        
        if (dbsk3dr_cmd_smre() == 2) {
          spvr->p0()->save_xyz (outfile1 + "_gsm.xyz");      
          spvr->p1()->save_xyz (outfile2 + "_gsm.xyz");
        }
        else if (dbsk3dr_cmd_smre() == 3) {
          spvr->p0()->save_xyz (outfile1 + "_dcs.xyz");      
          spvr->p1()->save_xyz (outfile2 + "_dcs.xyz");
        }
      }
    }

    //3) Meshing the two surfaces for -smreo 2.
    if (dbsk3dr_cmd_smreo() == 2) {
      //-sphrr 5: bounding sphere radius ratio.
      //-sphsr 1: bounding sphere sample ratio.
      //-msr: max sample dist. ratio
      //-segop 0: topo. option.
      spvr->p0()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
      spvr->p1()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                       dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
    }
    
    //Save the smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();
      if (dbsk3dr_cmd_smreo() == 2) { //Save Mesh.
        if (dbsk3dr_cmd_smre() == 1) {
          spvr->p0()->save_ply (outfile1 + ".ply");
          spvr->p1()->save_ply (outfile2 + ".ply");
        }
        else if (dbsk3dr_cmd_smre() == 2) {
          spvr->p0()->save_ply (outfile1 + "_gsm.ply");      
          spvr->p1()->save_ply (outfile2 + "_gsm.ply");
        }
        else if (dbsk3dr_cmd_smre() == 3) {
          spvr->p0()->save_ply (outfile1 + "_dcs.ply");      
          spvr->p1()->save_ply (outfile2 + "_dcs.ply");
        }
      }
    }

    //Visualize the smoothing results.
    if (dbmsh3d_cmd_gui()) {
      if (dbsk3dr_cmd_smreo() == 1) { //Point-based: visualize ponit clouds.
        //Draw the 1st point set.
        _root->addChild (spvr->pv0()->vis_ptset(COLOR_DARKGREEN, dbmsh3d_cmd_idv()!=0));
        //Draw the 2nd point set.
        _root->addChild (spvr->pv1()->vis_ptset(COLOR_BLUE, dbmsh3d_cmd_idv()!=0));
      }
      else if (dbsk3dr_cmd_smreo() == 2) { //Mesh-based: visualize meshes.
        //Draw the 1st mesh.
        vis = spvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                     dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
        _root->addChild (vis);
        //Draw the 2nd mesh.
        vis = spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                     dbmsh3d_cmd_shapehints(), COLOR_BLUE, true);
        _root->addChild (vis);
      }
    }

    //Estimate alignment error after smoothing.
    vul_printf (vcl_cout, "\nPt-Pt alignment error after smoothing: \n");
    // -dthr: dist_th_ratio for error estimation.
    spvr->compute_pp_error (error_dist_th);
    if (dbsk3dr_cmd_smreo() == 2) {
      vul_printf (vcl_cout, "\nPt-Plane alignment error after smoothing: \n");
      //-n3: search top_n closest vertices for pt-mesh dist.
      spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    }

    //4) Register the two datasets.
    // -smreo 1: pt-pt ICP.
    // -smreo 2: pt-plane ICP. Need to mesh the two datasets.    
    // -smreo 3: RGRL ICP. Need to estimate point normal from the two datasets.

    // -dthr: dist_th_ratio for error estimation.
    if (dbsk3dr_cmd_smreo() == 1) { //-smreo 1: Pt-Pt ICP.      
      if (spvr->run_pp_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), pp_icp_dist_th)) {
        //draw the point cloud of -f2 after alignment.
        _root->addChild (spvr->pv1()->vis_ptset (COLOR_RED, dbmsh3d_cmd_idv()!=0));
      }
    }
    else if (dbsk3dr_cmd_smreo() == 2) { //-smreo 2: Pt-Mesh ICP. 
      if (spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th)) {
        //draw the mesh of -f2 after alignment.
        spvr->pv1()->mesh()->build_face_IFS ();
        _root->addChild (spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));
      }
    }
    else if (dbsk3dr_cmd_smreo() == 3) { //RGRL ICP.
      assert (0);
    }

    //Save the registered smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();
      if (dbsk3dr_cmd_smreo() == 1) { //Save Pt-Set.  
        if (dbsk3dr_cmd_smre() == 1)
          spvr->p1()->save_xyz ();
        else if (dbsk3dr_cmd_smre() == 2)
          spvr->p1()->save_xyz (outfile2 + "_gsm_icp.xyz");
        else if (dbsk3dr_cmd_smre() == 3)
          spvr->p1()->save_xyz (outfile2 + "_dcs_icp.xyz");
      }
      else if (dbsk3dr_cmd_smreo() == 2) { //Save Mesh.
        if (dbsk3dr_cmd_smre() == 1)
          spvr->p1()->save_ply (outfile2 + "_icp.ply");
        else if (dbsk3dr_cmd_smre() == 2)
          spvr->p1()->save_ply (outfile2 + "_gsm_icp.ply");
        else if (dbsk3dr_cmd_smre() == 3)
          spvr->p1()->save_ply (outfile2 + "_dcs_icp.ply");
      }
    }

    //4) Estimate alignment error after registration.
    vul_printf (vcl_cout, "\nPt-Pt alignment error after registration:\n");
    // -dthr: dist_th_ratio for error estimation.
    spvr->compute_pp_error (error_dist_th);
    
    //Improvement of Pt-Pt distance compared to dist. without smoothing.    
    vul_printf (vcl_cout, "\nImprovement of Pt-Pt dists. compared to dists without smoothing:\n");
    double imprv_min, imprv_mean, imprv_median, imprv_RMS, imprv_max;
    compute_imprv_of_error (di_orig_pp_icp, spvr->min_dists(), error_dist_th,
                            imprv_min, imprv_mean, imprv_median, imprv_RMS, imprv_max);
    di_orig_pp_icp.clear();

    if (dbsk3dr_cmd_smreo() == 2) {
      vul_printf (vcl_cout, "\nPt-Plane alignment error after registration: \n");
      //-n3: search top_n closest vertices for pt-mesh dist.
      spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);

      //Improvement of Pt-Mesh distance compared to dist. without smoothing.    
      vul_printf (vcl_cout, "\nImprovement of Smoothing + Pt-Mesh ICP compared to original Pt-Mesh ICP:\n");
      double imprv_min, imprv_mean, imprv_median, imprv_RMS, imprv_max;
      compute_imprv_of_error (di_orig_pf_icp, spvr->min_dists(), error_dist_th,
                              imprv_min, imprv_mean, imprv_median, imprv_RMS, imprv_max);
    }

    di_orig_pp_icp.clear();
    di_orig_pf_icp.clear();
  }
  //############### Batch for Smoothing + Meshing + Registration + Error Estimation ###############  
  if (dbsk3dr_cmd_smreb()==1) { 
    //Generate batch file only.
    gen_smreb_bats (dbmsh3d_cmd_fileprefix1(), dbmsh3d_cmd_fileprefix2());
    dbmsh3d_cmd_gui() = 0;
  }
  else if (dbsk3dr_cmd_smreb()==2) { 
    //Run Smoothing + Meshing + Registration and output distance files.
    // -smreo 1: no smoothing
    // -smreo 2: Gaussian smoothing.
    // -smreo 3: DCS smoothing.

    //Read in the two scan files 
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());
    bool result = spvr->p0()->load_3pi ();
    assert (result);
    result = spvr->p1()->load_3pi ();
    assert (result);
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    ///spvr->p0()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    ///spvr->p1()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    double avg_samp_dist = spvr->p0()->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    if (dbsk3dr_cmd_smreo()==1) { 
      //-smreo 1: no smoothing
    }
    else if (dbsk3dr_cmd_smreo()==2) {
      //-smreo2: 2D Gaussian smoothing (intra- and inter- scan lines)
      //-n STEPS: default 1.
      int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n();
      //-gsr: G_sigma_ratio
      //-grr: G_radius_ratio
      //-thr: th_ratio
      gaussian_smooth_2d_3pi (spvr->p0()->sg3pi(), nstep, 
                              dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
      
      //Note that the p2->sg3pi() is not xform by init_af.txt yet.
      gaussian_smooth_2d_3pi (spvr->p1()->sg3pi(), nstep, 
                              dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
    }
    else if (dbsk3dr_cmd_smreo()==3) {
      //-smre0 3: Iterative DCS smoothing intra- and inter- scanlines 
      //-n STEPS: default 20.
      int nstep = (dbmsh3d_cmd_n() == -1) ? 20 : dbmsh3d_cmd_n();
      //-l: DCS_psi step size.
      //-thr: th_ratio
      float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1      
      vul_printf (vcl_cout, "\n\tApply DCS smoothing %d times (DCS_psi %f, th_r %f).\n", 
                  nstep, DCS_psi, dbmsh3d_cmd_thr());

      //Estimate intra- and inter- scanline sample distance.
      spvr->p0()->sg3pi()->get_sl_sample_dist ();
      spvr->p1()->sg3pi()->get_sl_sample_dist ();

      for (int i=0; i<nstep; i++) {
        dcs_smooth_scanlines_2dirs_3pi (spvr->p0()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), 1, 1);
        dcs_smooth_scanlines_2dirs_3pi (spvr->p1()->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), 1, 1);
      }
    }

    //After smoothing, transform 2nd dataset (p2->sg[]) using the init. affine xform file.
    if (dbmsh3d_cmd_align_file()) {
      spvr->load_hmatrix_01 (dbmsh3d_cmd_align_file());      
      //transform pro[1] using the specified affine xform file
      spvr->pv1()->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
      spvr->xform_hmatrix_01_pro1 ();
    }

    //Copy sg[] to mesh. All following processes are on the mesh data structures.
    spvr->p0()->move_sg3pi_to_mesh ();
    spvr->p1()->move_sg3pi_to_mesh ();

    //Save the smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();

      //Save the resulting .3pi after smoothing
      if (dbsk3dr_cmd_smre() == 2) {
        spvr->p0()->save_3pi (outfile1 + "_gsm.3pi");
        spvr->p1()->save_3pi (outfile2 + "_gsm.3pi");  
      }
      else if (dbsk3dr_cmd_smre() == 3) {
        spvr->p0()->save_3pi (outfile1 + "_dcs.3pi");
        spvr->p1()->save_3pi (outfile2 + "_dcs.3pi");  
      }

      spvr->p0()->reset_sg3pi ();
      spvr->p1()->reset_sg3pi ();

      //Save the original pt_set after init. alignment.
      spvr->p0()->save_xyz ();
      spvr->p1()->save_xyz ();

      //Save the resulting pt_set or mesh after smoothing.
      if (dbsk3dr_cmd_smreo() == 1) { //Save Pt-Set.        
        if (dbsk3dr_cmd_smre() == 2) {
          spvr->p0()->save_xyz (outfile1 + "_gsm.xyz");      
          spvr->p1()->save_xyz (outfile2 + "_gsm.xyz");
        }
        else if (dbsk3dr_cmd_smre() == 3) {
          spvr->p0()->save_xyz (outfile1 + "_dcs.xyz");      
          spvr->p1()->save_xyz (outfile2 + "_dcs.xyz");
        }
      }
    }

    //Meshing the two surfaces for -smreo 2.
    //-sphrr 5: bounding sphere radius ratio.
    //-sphsr 1: bounding sphere sample ratio.
    //-msr: max sample dist. ratio
    //-segop 0: topo. option.
    spvr->p0()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                     dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
    spvr->p1()->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                     dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);
    
    //Save the smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();
      if (dbsk3dr_cmd_smreo() == 2) { //Save Mesh.
        if (dbsk3dr_cmd_smre() == 1) {
          spvr->p0()->save_ply (outfile1 + ".ply");
          spvr->p1()->save_ply (outfile2 + ".ply");
        }
        else if (dbsk3dr_cmd_smre() == 2) {
          spvr->p0()->save_ply (outfile1 + "_gsm.ply");      
          spvr->p1()->save_ply (outfile2 + "_gsm.ply");
        }
        else if (dbsk3dr_cmd_smre() == 3) {
          spvr->p0()->save_ply (outfile1 + "_dcs.ply");      
          spvr->p1()->save_ply (outfile2 + "_dcs.ply");
        }
      }
    }

    //Visualize the smoothing results.
    if (dbmsh3d_cmd_gui()) {
      if (dbsk3dr_cmd_smreo() == 1) { //Point-based: visualize ponit clouds.
        //Draw the 1st point set.
        _root->addChild (spvr->pv0()->vis_ptset(COLOR_DARKGREEN, dbmsh3d_cmd_idv()!=0));
        //Draw the 2nd point set.
        _root->addChild (spvr->pv1()->vis_ptset(COLOR_BLUE, dbmsh3d_cmd_idv()!=0));
      }
      else if (dbsk3dr_cmd_smreo() == 2) { //Mesh-based: visualize meshes.
        //Draw the 1st mesh.
        vis = spvr->pv0()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                     dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
        _root->addChild (vis);
        //Draw the 2nd mesh.
        vis = spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                                     dbmsh3d_cmd_shapehints(), COLOR_BLUE, true);
        _root->addChild (vis);
      }
    }

    //Estimate alignment error after smoothing.
    vul_printf (vcl_cout, "\nPt-Pt alignment error after smoothing: \n");
    // -dthr: dist_th_ratio for error estimation.
    spvr->compute_pp_error (error_dist_th);
    if (dbsk3dr_cmd_smreo() == 2) {
      vul_printf (vcl_cout, "\nPt-Plane alignment error after smoothing: \n");
      //-n3: search top_n closest vertices for pt-mesh dist.
      spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);
    }

    //Register the two datasets.
    // -dthr: dist_th_ratio for error estimation.
    // -n2: ICP maximum iterations, default 50.
    int n_ICP_iter = 50;
    if (dbmsh3d_cmd_n2() != -1)
      n_ICP_iter = dbmsh3d_cmd_n2();

    if (dbsk3dr_cmd_smreo() == 1) { //-smreo 1: Pt-Pt ICP.      
      if (spvr->run_pp_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), pp_icp_dist_th)) {
        //draw the point cloud of -f2 after alignment.
        _root->addChild (spvr->pv1()->vis_ptset (COLOR_RED, dbmsh3d_cmd_idv()!=0));
      }
    }
    else if (dbsk3dr_cmd_smreo() == 2) { //-smreo 2: Pt-Mesh ICP. 
      if (spvr->run_pf_icp_regstr (n_ICP_iter, dbmsh3dr_cmd_icpcv(), error_dist_th)) {
        //draw the mesh of -f2 after alignment.
        spvr->pv1()->mesh()->build_face_IFS ();
        _root->addChild (spvr->pv1()->vis_mesh (dbmsh3d_cmd_idv()!=0, true, COLOR_RED, true));
      }
    }
    
    //Save the registered smoothing results into .xyz and .3pi files.
    if (dbmsh3d_cmd_ofile()>=1) {
      vcl_string outfile1 = dbmsh3d_cmd_fileprefix1();
      vcl_string outfile2 = dbmsh3d_cmd_fileprefix2();
      if (dbsk3dr_cmd_smreo() == 1) { //Save Pt-Set.  
        if (dbsk3dr_cmd_smre() == 1)
          spvr->p1()->save_xyz ();
        else if (dbsk3dr_cmd_smre() == 2)
          spvr->p1()->save_xyz (outfile2 + "_gsm_icp.xyz");
        else if (dbsk3dr_cmd_smre() == 3)
          spvr->p1()->save_xyz (outfile2 + "_dcs_icp.xyz");
      }
      else if (dbsk3dr_cmd_smreo() == 2) { //Save Mesh.
        if (dbsk3dr_cmd_smre() == 1)
          spvr->p1()->save_ply (outfile2 + "_icp.ply");
        else if (dbsk3dr_cmd_smre() == 2)
          spvr->p1()->save_ply (outfile2 + "_gsm_icp.ply");
        else if (dbsk3dr_cmd_smre() == 3)
          spvr->p1()->save_ply (outfile2 + "_dcs_icp.ply");
      }
    }

    //Estimate alignment error after registration.
    vul_printf (vcl_cout, "\nPt-Pt alignment error after registration:\n");
    // -dthr: dist_th_ratio for error estimation.
    spvr->compute_pp_error (error_dist_th);

    //Save Pt-Pt distance files after alignment

    vul_printf (vcl_cout, "\nPt-Plane alignment error after registration: \n");
    //-n3: search top_n closest vertices for pt-mesh dist.
    spvr->compute_pf_error (dbmsh3d_cmd_n3(), error_dist_th);

  }
  else if (dbsk3dr_cmd_smreb()==3) { 
    //Read in the distance files and generate comparison results.

  }
  //############### Write Affine Transform File ###############  
  if ((dbsk3dr_cmd_smrd() || dbsk3dr_cmd_smre()) && dbmsh3d_cmd_align_ofile()) {
    //write mpvr->hmatrix_ to an alignment file
    vcl_string xform_file = dbmsh3d_cmd_align_ofile();
    dbmsh3d_write_xform_file (xform_file, spvr->hmatrix_01());
  }

  //############### Perform Grad-Assign Shock Graph Matching ###############
  //-smatch : shock graph/hypergraph matching.
  if (dbsk3dr_cmd_smatch()) {
    //-f1: 
    assert (dbmsh3d_cmd_fileprefix1());
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    //-f2:
    assert (dbmsh3d_cmd_fileprefix2());
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());

    spvr->p0()->reset_mesh ();
    spvr->p0()->reset_fs_mesh ();
    spvr->p0()->reset_ms_hypg ();
    spvr->p0()->reset_sg_sa ();

    spvr->p1()->reset_mesh ();
    spvr->p1()->reset_fs_mesh ();
    spvr->p1()->reset_ms_hypg ();
    spvr->p1()->reset_sg_sa ();

    //visualization parameters.
    spvr->pv0()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
    spvr->pv1()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r() / 2;

    //-smatch 1: shock graph matching.
    if (dbsk3dr_cmd_smatch() == 1) {
      ///spvr->p0()->load_p3d (spvr->p0()->dir_prefix());
      ///spvr->p1()->load_p3d (spvr->p1()->dir_prefix());      
      dbmsh3d_app_window_title += " -- Shock Graph Matching ";
    }
    else {      
      //-f1 : Load .cms into p0()->ms_hypg.
      f1_load_success = spvr->p0()->load_cms ();
      if (f1_load_success) {
        //Try read the xform file with the same filename and apply it.
        if (spvr->p0()->load_hmatrix (spvr->p0()->dir_prefix()))
          spvr->p0()->apply_xform_hmatrix ();
      }
      
      //-f2 : Load .cms into p1()->ms_hypg.
      f2_load_success = spvr->p1()->load_cms ();
      if (f2_load_success) {
        //Try read the xform file with the same filename and apply it.
        if (spvr->p1()->load_hmatrix (spvr->p1()->dir_prefix()))
          spvr->p1()->apply_xform_hmatrix ();
      }
      
      if (f1_load_success == false || f2_load_success == false) {
        //Output error message and exit!
        if (dbmsh3d_cmd_verbose())
          vul_printf (vcl_cout, "\nMatching object file load error!\n");
        ///dbmsh3d_cmd_gui() = 0;
      }
      else {
        //-smatch 2: shock hypergraph matching using curve length as compatibility.
        //-smatch 3: shock hypergraph matching using curve sum radius over length as compatibility.
        //-smatch 4: shock hypergraph matching using D.P. curve distance as compatibility.
        //-smatch 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
        //-smatch 6: option 5 plus integration of the global (R, T) in node compatibility.
        //-smatch 7: option 3 plus integration of the global (R, T) in node compatibility.

        //-vb: verbose
        //-regrd: register object1 rigidly to object2
        bool match_subset_of_curves = true;
        //-n: max # of ms curves.
        int max_ms_curves = 50;
        if (dbmsh3d_cmd_n() != -1)
          max_ms_curves = dbmsh3d_cmd_n();

        bool r = spvr->run_shock_match (dbsk3dr_cmd_smatch(), dbsk3dr_cmd_regrd()!=0, 
                                        match_subset_of_curves, max_ms_curves);

          //Visualize the boundary point clouds in Yellow (G) and Cyan (g).
        //-dx: default shift between two objects. 
        _root->addChild (spvr->pv0()->vis_ptset (COLOR_DARKYELLOW));
        vis = spvr->pv1()->vis_ptset (COLOR_DARKCYAN);
        _root->addChild (shift_output (vis, dbmsh3d_cmd_dx(), 0, 0));

        //Visualize the medial scaffold graph matching results.
        //-dx : shift between the two objects
        vis = spvr->vis_sg_match_results (dbmsh3d_cmd_dx());
        _root->addChild (vis);

        //-smicp : ICP to fine-tune the registration result. (default iteration 100)
        if (dbsk3dr_cmd_smicp()) {      
          int n_iter = dbsk3dr_cmd_smicp();
          spvr->run_shock_match_icp (n_iter, dbmsh3dr_cmd_icpcv());
        }
        
        //Visualize the point cloud of G after ICP in Red.
        _root->addChild (spvr->pv0()->vis_ptset (COLOR_DARKRED));

        //-ofile 2:
        if (dbmsh3d_cmd_ofile()>1) {
          spvr->p0()->save_p3d (spvr->p0()->dir_file()+"-icp");

          //-ofile 3: Save xx-vl.sg.
          if (dbmsh3d_cmd_ofile()>2) {
            //save the transform matrix.
            spvr->p0()->save_cms (spvr->p0()->dir_file()+"-icp");
          }
        }

        vcl_string file1 = dbul_get_file (dbmsh3d_cmd_fileprefix1());
        vcl_string file2 = dbul_get_file (dbmsh3d_cmd_fileprefix2());
        dbmsh3d_app_window_title += " - " + file1;
        dbmsh3d_app_window_title += " to ";
        dbmsh3d_app_window_title += file2;
        if (r)
          dbmsh3d_app_window_title += " : Successful";
        else
          dbmsh3d_app_window_title += " : Exponential Explosion!!";

        vul_printf (vcl_cout, "  %s to %s %s\n", 
                    spvr->p0()->dir_file().c_str(), 
                    spvr->p1()->dir_file().c_str(),
                    r ? "" : "exp explode!");
        vul_printf (vcl_cout, "    m%d SN: %.3f (N: %.3f, L: %.3f, C: %.3f) S: %.3f\n", 
                    dbsk3dr_cmd_smatch(),
                    spvr->shock_match()->ga_match()->norm_similarity (),
                    spvr->shock_match()->ga_match()->norm_sim_node (),
                    spvr->shock_match()->ga_match()->norm_sim_curve (),
                    spvr->shock_match()->ga_match()->norm_sim_corner (),
                    spvr->shock_match()->ga_match()->similarity());

        /*if (dbmsh3d_cmd_verbose()) {
          vul_printf (vcl_cout, "\nMatching %s (N%d C%d S%d) to ", spvr->p0()->dir_file().c_str(), 
                      spvr->p0()->ms_hypg()->vertexmap().size(), 
                      spvr->p0()->ms_hypg()->edgemap().size(),
                      spvr->p0()->ms_hypg()->sheetmap().size());
          vul_printf (vcl_cout, "\n         %s (N%d C%d S%d).\n", spvr->p1()->dir_file().c_str(),
                      spvr->p1()->ms_hypg()->vertexmap().size(), 
                      spvr->p1()->ms_hypg()->edgemap().size(),
                      spvr->p1()->ms_hypg()->sheetmap().size());
          vul_printf (vcl_cout, "similarity: %f  ", 
                      spvr->shock_match()->ga_match()->similarity());
          vul_printf (vcl_cout, "normalized similarity: %f\n", 
                      spvr->shock_match()->ga_match()->norm_similarity ());
        }*/
      }
    }
  }  
  
  //############### Test Medial Scaffold Curve D.P. Matching ###############
  //-smatchc 1,2 : test 3d shock curve matching.
  if (dbsk3dr_cmd_smatchc()) {
    //-f1: 
    assert (dbmsh3d_cmd_fileprefix1());
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix1());
    //-f2:
    assert (dbmsh3d_cmd_fileprefix2());
    spvr->p1()->set_dir_file (dbmsh3d_cmd_fileprefix2());

    spvr->p0()->reset_mesh ();
    spvr->p0()->reset_fs_mesh ();
    spvr->p0()->reset_ms_hypg ();
    spvr->p0()->reset_sg_sa ();

    spvr->p1()->reset_mesh ();
    spvr->p1()->reset_fs_mesh ();
    spvr->p1()->reset_ms_hypg ();
    spvr->p1()->reset_sg_sa ();

    //visualization parameters.
    spvr->pv0()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
    spvr->pv1()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r() / 2;

    //Load .cms into p0()->ms_hypg.
    spvr->p0()->load_cms ();
    //Load .cms into p1()->ms_hypg.
    spvr->p1()->load_cms ();

    //-n1: curve id in -f1.
    int cid1 = dbmsh3d_cmd_n1();
    if (cid1 < 0)
      cid1 = 0;
    //-n2: curve id in -f2.
    int cid2 = dbmsh3d_cmd_n2();
    if (cid2 < 0)
      cid2 = 0;
    //-n3 1: flip the dir in curve2.
    bool flip = dbmsh3d_cmd_n3()==1;

    spvr->shock_match()->set_ms_hypg_G (spvr->p0()->ms_hypg());
    spvr->shock_match()->set_ms_hypg_g (spvr->p1()->ms_hypg());

    //-smatchc 1 : test 3d curve matching.
    //-smatchc 2 : test 3d shock curve matching.
    vcl_vector< vcl_pair<int,int> > alignment;
    spvr->shock_match()->test_mc_dp_match (dbsk3dr_cmd_smatchc(), cid1, cid2, flip, alignment);

    dbsk3d_ms_curve* MC1 = (dbsk3d_ms_curve*) spvr->p0()->ms_hypg()->edgemap (cid1);
    dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) spvr->p1()->ms_hypg()->edgemap (cid2);   
    
    //Estimate the optimal rigid xform matrices using the alignment.    
    vcl_vector<vgl_point_3d<double> > cor_PS1, cor_PS2;
    double Eu_dist = spvr->shock_match()->get_curve_align_avg_Eu_dist (MC1, MC2, alignment, cor_PS1, cor_PS2);

    //Rigid transform the location and orientation of curve C1.
    vcl_vector<dbmsh3d_vertex*> MC1_V_vec;
    MC1->get_V_vec (MC1_V_vec);
    for (unsigned int i=0; i<MC1_V_vec.size(); i++) {
      transform_point_3d (spvr->shock_match()->R(), spvr->shock_match()->Cf(), 
                          spvr->shock_match()->Cm(), MC1_V_vec[i]->get_pt());
    }

    //Visualize the two specified shock curves.     
    _root->addChild (draw_ms_curve (MC1, SbColor (1, 0, 0), 3, true));
    _root->addChild (draw_ms_curve (MC2, SbColor (0, 0, 1), 3, true));
    //Visualize the shock curve matching alignment result.
    _root->addChild (draw_match_MC_alignment (MC1, MC2, alignment));

    //draw cos_movPS and cor_fixPS in cubes
    //draw line segment between them to show correspondence.
    _root->addChild (draw_points_cor (cor_PS1, cor_PS2, SbColor (1, 0, 0), SbColor (0, 0, 1), SbColor (1, 1, 0)));

    print_R_C1_C2 (spvr->shock_match()->R(), spvr->shock_match()->Cf(), spvr->shock_match()->Cm());
    vul_printf (vcl_cout, "\n\tEu_dist (C%d, C%d) = %f.\n\n", cid1, cid2, Eu_dist);

    dbmsh3d_app_window_title += " -- Shock Curve D.P. Matching ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_fileprefix1();
    dbmsh3d_app_window_title += " to ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_fileprefix2();

  }
  //-subsm 1: match a shock graph/hypergraph with its subsampled version.
  if (dbsk3dr_cmd_subsm()) {
    //-f: 
    assert (dbmsh3d_cmd_fileprefix());
    spvr->p0()->set_dir_file (dbmsh3d_cmd_fileprefix());

    spvr->p0()->reset_mesh ();
    spvr->p0()->reset_fs_mesh ();
    spvr->p0()->reset_ms_hypg ();
    spvr->p0()->reset_sg_sa ();

    spvr->p1()->reset_mesh ();
    spvr->p1()->reset_fs_mesh ();
    spvr->p1()->reset_ms_hypg ();
    spvr->p1()->reset_sg_sa ();

    //visualization parameters.
    spvr->pv0()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
    spvr->pv1()->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r() / 2;

    //-subsm 1: match a shock graph with its subsampled version.
    if (dbsk3dr_cmd_subsm() == 1) {
      ///spvr->p0()->load_p3d (spvr->p0()->dir_prefix());
      ///spvr->p1()->load_p3d (spvr->p1()->dir_prefix());

      dbmsh3d_app_window_title += " -- Subsampled Shock Graph Matching ";
    }    
    //-subsm 2: match a shock hypergraph with its subsampled version.
    else if (dbsk3dr_cmd_subsm() == 2) {

      //Load .cms into p0()->ms_hypg.
      spvr->p0()->load_cms ();

      //Sub-sampled (with noise) p0()->ms_hypg into p1()->ms_hypg.
      //-noise 0~1: perturbation in percentage.
      float noise = dbmsh3d_cmd_noise();
      dbsk3d_ms_hypg* MSH1 = sub_sampled_w_noise (spvr->p0()->ms_hypg(), noise);
      spvr->p1()->clear_ms_hypg ();
      spvr->p1()->set_ms_hypg (MSH1); 

      if (dbsk3d_cmd_cms_ofile()) {
        //-ocms : save the cloned ms_mesh and ms_hypg.
        spvr->p1()->save_fs (dbsk3d_cmd_cms_ofile());
        spvr->p1()->save_cms (dbsk3d_cmd_cms_ofile());
      }
      
      //Ming: Check option here!!
      
      bool match_subset_of_curves = true;
      //-n: max # of ms curves.
      int max_ms_curves = 50;
      if (dbmsh3d_cmd_n() != -1)
        max_ms_curves = dbmsh3d_cmd_n();
      spvr->run_shock_match (3, dbsk3dr_cmd_regrd()!=0, match_subset_of_curves, max_ms_curves);

      dbmsh3d_app_window_title += " -- Subsampled Shock Hypergraph Matching ";
    }    

    //Visualize the medial scaffold graph matching results.
    vis = spvr->vis_sg_match_results (0);
    _root->addChild (vis); 

    //Visualize the boundary point clouds in Yellow and Cyan.
    _root->addChild (spvr->pv0()->vis_ptset (COLOR_DARKYELLOW));
    _root->addChild (spvr->pv1()->vis_ptset (COLOR_DARKCYAN));

    dbmsh3d_app_window_title += dbmsh3d_cmd_fileprefix();
  }

  ///////////////////////////////////////////////
  //9)Save Output Files
  /*if (dbmsh3d_cmd_rmip()) { //Remove isolated points.
    spv->mesh()->delete_unmeshed_pts ();
    //After deteting points, reset point ids and re-index the IndexedFaceSet.
    spv->mesh()->reset_vertices_ids ();
  }*/

  return _root;
}

