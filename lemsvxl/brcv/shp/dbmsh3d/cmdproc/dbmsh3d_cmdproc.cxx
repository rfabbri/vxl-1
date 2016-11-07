//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <vnl/vnl_random.h>

#include <vil/vil_load.h>

#include <bgui3d/bgui3d_file_io.h>

#include <dbmsh3d/dbmsh3d_hypg.h>
#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>
#include <dbmsh3d/algo/dbmsh3d_pt_normal.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>
#include <dbmsh3d/algo/dbmsh3d_scan_sim.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>

#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_fmm.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>

#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/nodes/SoTexture2.h>

#include <ray_tri/ray_tri.h>

//#####################################################################
// dbmsh3d command-line process execution.
// false: no task specified.
bool dbmsh3d_check_cmdproc (dbmsh3d_pro_vis* pv, int argc, char **argv)
{
  //1) Check if no cmdproc specified, result=PRO_RESULT_NO_PROCESS. 
  if (dbmsh3d_cmd_compare_pts()==0 &&
      !dbmsh3d_cmd_p3d_file() && !dbmsh3d_cmd_g3d_file() && 
      !dbmsh3d_cmd_xyz_file() && 
      !dbmsh3d_cmd_xyzn1_file() && !dbmsh3d_cmd_xyznw_file() &&
      !dbmsh3d_cmd_pts_file() &&
      !dbmsh3d_cmd_ply2_file() && !dbmsh3d_cmd_ply_file() && 
      !dbmsh3d_cmd_off_file() && !dbmsh3d_cmd_obj_file() && 
      !dbmsh3d_cmd_m_file() && !dbmsh3d_cmd_3pi_file() &&
      !dbmsh3d_cmd_iv_file() && !dbmsh3d_cmd_wrl_file() &&   
      !dbmsh3d_cmd_vtk_file() &&
      !dbmsh3d_cmd_tex() &&   
      dbmsh3d_cmd_preproc()==0 && dbmsh3d_cmd_ptb()==0 && 
      dbmsh3d_cmd_1stoct()==0 && dbmsh3d_cmd_crop()==0 && 
      dbmsh3d_cmd_cclab()==0 && dbmsh3d_cmd_rmet()==0 &&
      dbmsh3d_cmd_dup()==0 && dbmsh3d_cmd_sub()==0 &&
      dbmsh3d_cmd_bkt()==0 && 
      dbmsh3d_cmd_iv_pfile()==0 && 
      dbmsh3d_cmd_normal()==0 && dbmsh3d_cmd_sm()==0 &&
      !dbmsh3d_cmd_fix_af() &&
      dbmsh3d_cmd_ssim() ==0 &&
      dbmsh3d_cmd_fmm()==0 && 
      !dbmsh3d_cmd_gen_pgutter()  &&
      !dbmsh3d_cmd_mem() && !dbmsh3d_cmd_test()) {
    //Default Viewer: try to open the argv[1] file and visualize it.
    //Assume the first parameter specifies the file to display.
    if (argc>1 && argv[1][0] != '-') {      
      vcl_string dirfile = dbul_get_dir_file (argv[1]);
      vcl_string suffix = dbul_get_suffix (argv[1]);
      if (suffix == ".p3d") {
        pv->b_view_p3d_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".g3d") {
        pv->b_view_g3d_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".xyz") {
        pv->b_view_xyz_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".xyzn1") {
        pv->b_view_xyzn1_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".xyznw") {
        pv->b_view_xyznw_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".ply") {
        pv->b_view_ply_ = true;    
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".ply2") {
        pv->b_view_ply2_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".off") {
        pv->b_view_off_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".obj") {
        pv->b_view_obj_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".m") {
        pv->b_view_m_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".3pi") {
        pv->b_view_3pi_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".iv") {
        pv->b_view_iv_ = true;  
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".wrl") {
        pv->b_view_wrl_ = true;
        pv->set_dir_file (dirfile);
      }
      else if (suffix == ".vtk") {
        pv->b_view_vtk_ = true;
        pv->set_dir_file (dirfile);
      }
    }
    else {
      return false; 
    }
  }
  return true;
}

void dbmsh3d_setup_provis_parameters_0 (dbmsh3d_pro_vis* pv0)
{
  if (dbmsh3d_cmd_fileprefix()) //set prefix
    pv0->set_dir_file (dbmsh3d_cmd_fileprefix());

  //Setup visualization parameters.  
  pv0->pt_size_ = dbmsh3d_cmd_psz();
  pv0->cube_size_ = dbmsh3d_cmd_rc();

  pv0->m_transp_ = dbmsh3d_cmd_mesh_transp();
  pv0->m_edge_width_ = dbmsh3d_cmd_cw();
  pv0->m_bndcv_width_ = dbmsh3d_cmd_cw();   
  pv0->vector_len_ = dbmsh3d_cmd_len();
  
  if (dbmsh3d_cmd_bgcol() == 1) {
    pv0->m_vertex_ccode_ = COLOR_BLACK;
    pv0->bg_ccode_ = COLOR_WHITE;
  }
  else {
    pv0->m_vertex_ccode_ = COLOR_WHITE;
    pv0->bg_ccode_ = COLOR_BLACK;
  }
}

void dbmsh3d_setup_provis_parameters_1 (dbmsh3d_pro_vis* pv2)
{
  if (dbmsh3d_cmd_fileprefix2()) //set prefix
    pv2->set_dir_file (dbmsh3d_cmd_fileprefix2());

  //Setup visualization parameters.
  pv2->pt_size_ = dbmsh3d_cmd_psz();
  pv2->cube_size_ = dbmsh3d_cmd_rc();

  pv2->m_transp_ = dbmsh3d_cmd_mesh_transp();
  pv2->m_bndcv_width_ = dbmsh3d_cmd_cw();   
  pv2->vector_len_ = dbmsh3d_cmd_len(); //dbmsh3d_cmd_len_tiny();
  
  if (dbmsh3d_cmd_bgcol() == 1) {
    pv2->m_vertex_ccode_ = COLOR_DARKBLUE;
    pv2->bg_ccode_ = COLOR_WHITE;
  }
  else {
    pv2->m_vertex_ccode_ = COLOR_BLUE;
    pv2->bg_ccode_ = COLOR_BLACK;
  }
}

SoSeparator* dbmsh3d_cmdproc_execute (dbmsh3d_pro_vis* pv)
{
  SoSeparator* _root = new SoSeparator;
  SoSeparator* vis;
    
  if (dbmsh3d_cmd_light()) //Assign lighting to the scene graph
    assign_rgb_light (_root);

  //##########################################################################
  //############################## View Files ################################
  //##########################################################################

  //############### Load the Polywork Alignment Matrix File ###############  
  //-af xform_file.txt: read the alignment from file to mpvr->hmatrix_
  if (dbmsh3d_cmd_align_file() != 0) {      
    pv->load_hmatrix (dbmsh3d_cmd_align_file());
  }
  //############### View P3D File ###############  
  //-p3d: 
  if (dbmsh3d_cmd_p3d_file()) {
    pv->b_view_p3d_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_p3d_file()));
  }
  if (pv->b_view_p3d_) {
    pv->reset_mesh ();
    if (pv->load_p3d()) {         
      //-idv 1: draw each point individually. 
      _root->addChild (pv->vis_ptset (0, dbmsh3d_cmd_idv()!=0));

      vgl_box_3d<double> bbox;
      detect_bounding_box (pv->mesh(), bbox);
      vcl_cout << bbox;

      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View G3D File ###############  
  //-g3d: 
  if (dbmsh3d_cmd_g3d_file()) {
    pv->b_view_g3d_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_g3d_file()));
  }
  if (pv->b_view_g3d_) {
    pv->reset_mesh ();
    _root->addChild (pv->view_g3d (pv->dir_file()));

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }

  //############### View XYZ File ###############  
  //-xyz: 
  if (dbmsh3d_cmd_xyz_file()) {
    pv->b_view_xyz_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_xyz_file()));
  }
  if (pv->b_view_xyz_) {
    pv->reset_mesh ();
    if (pv->load_xyz() == false) {   
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      //-idv 1: draw each point individually. 
      _root->addChild (pv->vis_ptset (0, dbmsh3d_cmd_idv()!=0));

      vgl_box_3d<double> bbox;
      detect_bounding_box (pv->mesh(), bbox);
      vcl_cout << bbox;

      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View XYZN1 File ############### 
  //-xyzn1: 
  if (dbmsh3d_cmd_xyzn1_file()) {
    pv->b_view_xyzn1_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_xyzn1_file()));
  }
  if (pv->b_view_xyzn1_) {
    vcl_string xyzn1_file = pv->dir_file() + ".xyzn1";
    pv->set_pro_data (dbmsh3d_pro::PD_ORIPTS);
    pv->oripts().clear();

    if (dbmsh3d_load_xyzn1 (pv->oripts(), xyzn1_file.c_str()))
      dbmsh3d_pro_contiune = true;

    if (dbmsh3d_pro_contiune == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      _root->addChild (draw_oriented_ptset (pv->oripts(), pv->m_vertex_ccode_, COLOR_BLUE, 
                                            dbmsh3d_cmd_len(), dbmsh3d_cmd_psz(), 1));

      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View XYZNW File ############### 
  //-xyznw: 
  if (dbmsh3d_cmd_xyznw_file()) {
    pv->b_view_xyznw_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_xyznw_file()));
  }
  if (pv->b_view_xyznw_) {
    pv->set_pro_data (dbmsh3d_pro::PD_ORIPTS);
    vcl_string xyznw_file = pv->dir_file() + ".xyznw";
    if (dbmsh3d_load_xyznw (pv->oripts(), xyznw_file.c_str()))
      dbmsh3d_pro_contiune = true;

    if (dbmsh3d_pro_contiune == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      _root->addChild (draw_oriented_ptset (pv->oripts(), pv->m_vertex_ccode_, COLOR_BLUE, 
                                            dbmsh3d_cmd_len(), dbmsh3d_cmd_psz(), 1));
      
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View PTS File ############### 
  //-pts: 
  if (dbmsh3d_cmd_pts_file()) {
    pv->b_view_pts_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_pts_file()));
  }
  if (pv->b_view_pts_) {
    pv->set_pro_data (dbmsh3d_pro::PD_MESH);
    vcl_string pts_file = pv->dir_file() + ".pts";
    if (dbmsh3d_load_pts (pv->mesh(), pts_file.c_str()))
      dbmsh3d_pro_contiune = true;

    if (dbmsh3d_pro_contiune == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View 3PI File ############### 
  //-3pi: 
  if (dbmsh3d_cmd_3pi_file()) {
    pv->b_view_3pi_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_3pi_file()));
  }
  if (pv->b_view_3pi_) {
    pv->reset_mesh ();
    if (pv->load_3pi()) {
      pv->sg3pi()->get_sl_sample_dist ();

      if (pv->is_hmatrix_identity() == false)
        pv->apply_xform_hmatrix ();

      if (dbmsh3d_cmd_sm()==2) { 
        //-sm 2: no smoothing.
      }
      else if (dbmsh3d_cmd_sm()==3) { 
        //-sm 3: 2D Gaussian smoothing (intra- and inter- scan lines) -n STEPS
        //default nstep: 1
        int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n(); 
        //-gsr: G_sigma_ratio
        //-grr: G_radius_ratio
        //-thr: th_ratio
        gaussian_smooth_2d_3pi (pv->sg3pi(), nstep, 
                                dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
      }
      else if (dbmsh3d_cmd_sm()==4) { 
        //-sm 4: Iterative DCS smoothing intra- and inter- scanlines 
        //-n STEPS: default 20
        int nstep = (dbmsh3d_cmd_n() == -1) ? 20 : dbmsh3d_cmd_n();
        //-l: DCS_psi step size.
        //-thr: th_ratio.
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1
        vul_printf (vcl_cout, "\n\tApply DCS smoothing %d times (DCS_psi %f, th_r %f).\n", 
                    nstep, DCS_psi, dbmsh3d_cmd_thr());

        //Estimate intra- and inter- scanline sample distance.
        pv->sg3pi()->get_sl_sample_dist ();
        pv->sg3pi()->get_sl_sample_dist ();

        double d_intra = pv->sg3pi()->intra_sl_dist();
        double d_inter = pv->sg3pi()->inter_sl_dist();
        int n_intra, n_inter;

        if (d_intra < d_inter) {
          n_intra = 1;
          n_inter = int (d_inter / d_intra);
        }
        else {
          n_inter = 1;
          n_intra = int (d_intra / d_inter);
        }

        for (int i=0; i<nstep; i++)
          dcs_smooth_scanlines_2dirs_3pi (pv->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), n_intra, n_inter);
      }

      /*if (dbmsh3d_cmd_sm()==1) { 
        //-sm 1: DCS smooth each scan line: -l STEPSIZE -n STEPS        
        int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n(); //default nstep size: 10      
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1
        //-thr: th_ratio
        dcs_smooth_scanlines_3pi (pv->sg3pi(), nstep, DCS_psi, dbmsh3d_cmd_thr());
      }
      if (dbmsh3d_cmd_sm()==2) { 
        //-sm 2: DCS smooth scan lines + DCS smooth across scan lines -n STEPS
        int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n(); //default nstep size: 10
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1
        //-thr: th_ratio
        dcs_smooth_scanlines_3pi (pv->sg3pi(), nstep, DCS_psi, dbmsh3d_cmd_thr());
        //-ns STEPS
        nstep = dbmsh3d_cmd_ns();
        dcs_smooth_across_scanlines_3pi (pv->sg3pi(), nstep, DCS_psi, dbmsh3d_cmd_thr()); 
      }
      else if (dbmsh3d_cmd_sm()==4) { 
        //-sm 4: DCS smooth each scan line + median filtering
        int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n(); //default nstep size: 10
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1
        //-thr: th_ratio
        dcs_smooth_scanlines_3pi (pv->sg3pi(), nstep, DCS_psi, dbmsh3d_cmd_thr());

        //-n2: option of median filtering, -ns: STEPS (default 1)
        median_filter_3pi (pv->sg3pi(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_ns());
      }
      else if (dbmsh3d_cmd_sm()==5) { 
        //-sm 5: Gaussian smooth each scan line -n STEPS
        int nstep = (dbmsh3d_cmd_n() == -1) ? 5 : dbmsh3d_cmd_n(); //default nstep size: 5
        //-thr: th_ratio
        gaussian_smooth_scanlines_3pi (pv->sg3pi(), nstep, dbmsh3d_cmd_thr());
      }
      else if (dbmsh3d_cmd_sm()==7) { 
        //-sm 7: -sm3 + -sm6
        int nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n();
        float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1
        for (int i=0; i<nstep; i++)
          dcs_smooth_scanlines_2dirs_3pi (pv->sg3pi(), DCS_psi, dbmsh3d_cmd_thr(), 1, 1);

        nstep = (dbmsh3d_cmd_n() == -1) ? 3 : dbmsh3d_cmd_n();
        //-thr: th_ratio
        gaussian_smooth_scanlines_3pi (pv->sg3pi(), nstep, dbmsh3d_cmd_thr());
      }*/
  
      //-o3pi: output 3pi file.
      if (dbmsh3d_cmd_3pi_ofile()) {
        vcl_string file_3pi = dbul_get_dir_file (dbmsh3d_cmd_3pi_ofile()) + ".3pi";
        dbmsh3d_save_sg3pi (pv->sg3pi(), file_3pi.c_str());
      }

      pv->move_sg3pi_to_mesh ();
      if (dbmsh3d_cmd_gui()) //-idv 1: draw each point individually. 
        _root->addChild (pv->vis_ptset (COLOR_NOT_SPECIFIED, dbmsh3d_cmd_idv()!=0));
      if (dbmsh3d_cmd_ply2_ofile() || dbmsh3d_cmd_ply_ofile() || dbmsh3d_cmd_ply_ofileb())
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints()));
      
      dbmsh3d_app_window_title += " -- "; 
      dbmsh3d_app_window_title += pv->dir_file();
    }
  }
  //############### View PLY File ###############
  //-ply: 
  if (dbmsh3d_cmd_ply_file()) {
    pv->b_view_ply_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_ply_file()));
  }
  if (pv->b_view_ply_) {
    if (pv->load_ply () == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
      else {
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          int ccode = dbmsh3d_cmd_colorcode();
          if (pv->mesh()->b_watertight())
            ccode = COLOR_GOLD;
          //if -n 0 specified, also make surface watertight.
          if (dbmsh3d_cmd_n() == 0)
            ccode = COLOR_GOLD;
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), ccode));
          if (dbmsh3d_cmd_n() != 0)
            _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.          
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View PLY2 File ###############
  //-ply2: 
  if (dbmsh3d_cmd_ply2_file()) {
    pv->b_view_ply2_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_ply2_file()));
  }
  if (pv->b_view_ply2_) {
    pv->reset_mesh ();
    if (pv->load_ply2 (pv->dir_file()) == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {     
      //-sh : dbmsh3d_cmd_shapehints (default true)
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
      else { 
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else if ( dbmsh3d_cmd_v()==3) //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
        else if ( dbmsh3d_cmd_v()==5) { //-v 5: draw mesh vertices
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          //-rc : cube size.
          _root->addChild (draw_mesh_vertices (pv->mesh(), dbmsh3d_cmd_rc()));
        }
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View OFF File ###############
  //-off: 
  if (dbmsh3d_cmd_off_file()) {
    pv->b_view_off_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_off_file()));
  }
  if (pv->b_view_off_) {
    pv->reset_mesh ();
    if (pv->load_off (pv->dir_file()) == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {    
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
      else {
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View OBJ File ###############
  //-obj: 
  if (dbmsh3d_cmd_obj_file()) {
    pv->b_view_obj_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_obj_file()));
  }
  if (pv->b_view_obj_) {
    pv->reset_mesh ();
    if (pv->load_obj (pv->dir_file()) == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {    
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
      else {
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### View M File ###############
  //-m:
  if (dbmsh3d_cmd_m_file()) {
    pv->b_view_m_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_m_file()));
  }
  if (pv->b_view_m_) {
    pv->reset_mesh ();
    if (pv->load_m (pv->dir_file()) == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {    
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
      else {
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.          
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  } 
  //############### Viewing IV File ###############
  //-iv:
  if (dbmsh3d_cmd_iv_file()) {
    pv->b_view_iv_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_iv_file()));
  }
  if (pv->b_view_iv_) {
    if (dbmsh3d_cmd_ply2_ofile()) {
      //-iv , -oply2: read in ascii format.
      if (pv->load_iv_ifs() == false) {
        //error message.
      }
      else if (dbmsh3d_cmd_gui()) { //visualize the IV file.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
      }
    }
    else if (dbmsh3d_cmd_gui()) { //-iv: visualiza the IV file.
      _root->addChild (pv->view_iv (pv->dir_file()));
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }
  //############### Viewing VRML File ###############
  //-vrml: 
  if (dbmsh3d_cmd_wrl_file()) {
    pv->b_view_wrl_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_wrl_file()));
  }
  if (pv->b_view_wrl_) {
    _root->addChild (pv->view_wrl (pv->dir_file()));
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }

  //############### View VTK File ###############
  //-vtk: 
  if (dbmsh3d_cmd_vtk_file()) {
    pv->b_view_vtk_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_vtk_file()));
  }
  if (pv->b_view_vtk_) {
    pv->reset_mesh ();
    if (pv->load_vtk (pv->dir_file()) == false) {
      vul_printf (vcl_cout, "\n!! File %s does not exist!\n", pv->dir_file().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {     
      //-sh : dbmsh3d_cmd_shapehints (default true)
      if (dbmsh3d_cmd_v()==0 && dbmsh3d_cmd_orient()==0) { //-v 0: Draw whole surface as one object.
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode(), false));
      }
      else { 
        pv->mesh()->IFS_to_MHE ();
        pv->mesh()->build_face_IFS ();
        ///manifold_fix_faces_orientation (pv->bnd_mesh(), 0, true);
        pv->mesh()->print_topo_summary ();

        if (dbmsh3d_cmd_v()==1) { //-v 1: Draw surface with boundary
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          _root->addChild (pv->vis_mesh_bndcurve (dbmsh3d_cmd_colorcodebnd(), dbmsh3d_cmd_cw()));
        }
        else if  (dbmsh3d_cmd_v()==4) { //-v 4: animation in creation order.
          if (dbmsh3d_cmd_psz()!=0) //-gsize 0: not showing initial pt cloud.
            _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
          //-n1: nF_batch.
          //-n2: Delay time before animation starts: def. 100 (around 3 secs).
          //-n3: Re-iterate delay time: def. 100 (around 3 secs).
          _root->addChild (pv->vis_mesh_anim (dbmsh3d_cmd_n1()));
        }
        else if ( dbmsh3d_cmd_v()==3) //-v 3: Distinguish triangled topology.
          _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
        else if ( dbmsh3d_cmd_v()==5) { //-v 5: draw mesh vertices
          _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
          //-rc : cube size.
          _root->addChild (draw_mesh_vertices (pv->mesh(), dbmsh3d_cmd_rc()));
        }
      }

      if (dbmsh3d_cmd_idv()==1)
        _root->addChild (pv->vis_ptset_idv());
      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }


  //############### Fix the Polywork Alignment Matrix File ###############  
  //-fixaf 1
  if (dbmsh3d_cmd_fix_af() != 0) {
    //Read in the alignment matrix file.
    pv->set_dir_prefix (dbmsh3d_cmd_fileprefix());
    vcl_string file_af = pv->dir_prefix() + ".txt";
    pv->load_hmatrix (file_af);

    //Fix orientation
    pv->fix_hmatrix_orient ();

    //Save to "f_fixed.txt"
    vcl_string af_new = pv->dir_prefix() + "_fixed.txt";
    dbmsh3d_write_xform_file (af_new, pv->hmatrix());

    vul_printf (vcl_cout, "\n\n Alignment file %s saved.\n", af_new.c_str());
    dbmsh3d_cmd_gui() = 0;
  }
  //############### Texture Mapping the Mesh ###############  
  //-tex 1 : texture mapping
  if (dbmsh3d_cmd_tex() != 0) {
    dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;
    //-f prefix.
    bool r = dbmsh3d_load_ply2 (M, (pv->dir_file()+".ply2").c_str());
    assert (r);

    _root->addChild (draw_M_edges (M, SbColor(0,0,1), 1));
    ///_root->addChild (draw_M (M, dbmsh3d_cmd_shapehints(), 0, dbmsh3d_cmd_colorcode()));

    //texture mapping on face 1443 using file texture_1443.png
    dbmsh3d_textured_face_mc* F = (dbmsh3d_textured_face_mc*) M->facemap(1443);
    vcl_string texture_img = pv->dir_file() + "_texture_1443.png";
    F->set_tex_map_uri (texture_img);

    _root->addChild (draw_F_textured (F));
  }

  //##########################################################################
  //############################ Mesh Processing #############################
  //##########################################################################

  //############### Parse/Sample Polygonal Mesh ###############
  if (dbmsh3d_cmd_iv_pfile()) {
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_iv_pfile()));  
    pv->sample_iv_to_pts (1, 100, dbmsh3d_cmd_ptb());
    
    if (dbmsh3d_cmd_gui()) {
      _root->addChild (pv->vis_ptset());
      dbmsh3d_app_window_title += " -- preprocess IV -- " + pv->dir_file();
    }
  }
  //############### Check Duplicate Points ###############
  //-dup :
  if (dbmsh3d_cmd_bkt()==0 && (dbmsh3d_cmd_dup()==1 || dbmsh3d_cmd_dup()==2)) {
    pv->set_pro_data (dbmsh3d_pro::PD_IDPTS);    
    int pro_option = 0;

    vcl_string xyz_file = pv->dir_file() + ".xyz";
    if (dbmsh3d_load_xyz (pv->idpts(), xyz_file.c_str()))
      pro_option = 1;

    if (pro_option == 0) {
      vcl_string p3d_file = pv->dir_file() + ".p3d";
      if (dbmsh3d_load_p3d (pv->idpts(), p3d_file.c_str()))
        pro_option = 2;
    }

    if (pro_option == 0) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cout, "ERROR:\t File %s Not Found!\n", pv->dir_prefix().c_str());
    }
    else {
      if (dbmsh3d_cmd_dup()==1) { //-dup 1: bucketing + dup checking
        //-boxr: epsilon, def 0.1
        double epsilon = dbmsh3d_cmd_boxr();
        check_dup_adpt_bucketing (pv->idpts(), 18, epsilon);
      }
      else if (dbmsh3d_cmd_dup()==2) //-dup 2: brute-force checking for duplicate points.
        check_dup_pts (pv->idpts());

      if (pro_option == 1) {
        vcl_string xyz_file = pv->dir_prefix() + "_nodup.xyz";
        dbmsh3d_save_xyz (pv->idpts(), xyz_file.c_str());
      }
      else if (pro_option == 2) {
        vcl_string p3d_file = pv->dir_prefix() + "_nodup.p3d";
        dbmsh3d_save_p3d (pv->idpts(), p3d_file.c_str());
      }

      if (dbmsh3d_cmd_gui()) {
        _root->addChild (draw_idpts (pv->idpts(), pv->m_vertex_ccode_, pv->pt_size_));
      }
    }
  }
  //############### Cropping Data Points ###############
  //-crop: 
  if (dbmsh3d_cmd_crop()) {
    vgl_box_3d<double> bbox, crop_box; 
    if (pv->load_3pi ()) { //Try the 3PI file for the prefix -f.
      sg_detect_bbox (pv->sg3pi(), bbox); //detect bounding box.
    }      
    else if (pv->load_p3d ()) { //Try the P3D file for the prefix -f.
      detect_bounding_box (pv->mesh(), bbox);
    }
    else {
      vul_printf (vcl_cout, "\nError: can't load file %u.\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }

    if (dbmsh3d_cmd_minx()==FLT_MAX && dbmsh3d_cmd_miny()==FLT_MAX && dbmsh3d_cmd_minz()==FLT_MAX && 
        dbmsh3d_cmd_maxx()==FLT_MIN && dbmsh3d_cmd_maxy()==FLT_MIN && dbmsh3d_cmd_maxz()==FLT_MIN) {
      //-crop 1 without specifying min/max: display bounding box.
      vul_printf (vcl_cout, "\n bounding box min: (%lf, %lf, %lf) - \n",
                  bbox.min_x(), bbox.min_y(), bbox.min_z());
      vul_printf (vcl_cout, "              max: (%lf, %lf, %lf).\n",                    
                  bbox.max_x(), bbox.max_y(), bbox.max_z());
    }
    else { //crop the data, if min/max not specified, use bounding box.*/
      vgl_box_3d<double> crop_box;
      if (dbmsh3d_cmd_minx() != FLT_MAX)
        crop_box.set_min_x (dbmsh3d_cmd_minx());
      else
        crop_box.set_min_x (bbox.min_x() - 1E-5);

      if (dbmsh3d_cmd_miny() != FLT_MAX)
        crop_box.set_min_y (dbmsh3d_cmd_miny());
      else
        crop_box.set_min_y (bbox.min_y() - 1E-5);

      if (dbmsh3d_cmd_minz() != FLT_MAX)
        crop_box.set_min_z (dbmsh3d_cmd_minz());
      else
        crop_box.set_min_z (bbox.min_z() - 1E-5);

      if (dbmsh3d_cmd_maxx() != FLT_MIN)
        crop_box.set_max_x (dbmsh3d_cmd_maxx());
      else
        crop_box.set_max_x (bbox.max_x() + 1E-5);

      if (dbmsh3d_cmd_maxy() != FLT_MIN)
        crop_box.set_max_y (dbmsh3d_cmd_maxy());
      else
        crop_box.set_max_y (bbox.max_y() + 1E-5);

      if (dbmsh3d_cmd_maxz() != FLT_MIN)
        crop_box.set_max_z (dbmsh3d_cmd_maxz());
      else
        crop_box.set_max_z (bbox.max_z() + 1E-5);

      //Crop the data
      if (pv->pro_data() == dbmsh3d_pro::PD_SG3PI) { 
        sg_crop_3pi (pv->sg3pi(), 
                     crop_box.min_x(), crop_box.min_y(), crop_box.min_z(), 
                     crop_box.max_x(), crop_box.max_y(), crop_box.max_z());    
        if ( dbmsh3d_cmd_ofile() && dbmsh3d_cmd_3pi_ofile()==NULL)
          pv->save_3pi (pv->dir_prefix() + "_crop.3pi");
      }
      else {
        crop_points (pv->mesh(), 
                     crop_box.min_x(), crop_box.min_y(), crop_box.min_z(), 
                     crop_box.max_x(), crop_box.max_y(), crop_box.max_z());
        if ( dbmsh3d_cmd_ofile() && dbmsh3d_cmd_p3d_ofile()==NULL)
          pv->save_p3d (pv->dir_prefix() + "_crop.p3d");
      }     
    }

    //Display the cropping result.
    if (dbmsh3d_cmd_gui() && pv->pro_data() == dbmsh3d_pro::PD_SG3PI) {
      pv->move_sg3pi_to_mesh ();
      _root->addChild (pv->vis_ptset());
    }
  }
  //############### Mesh Connected Component Labelling ###############
  //-cclab 1 -f prefix -n 1000
  if (dbmsh3d_cmd_cclab()) {
    pv->reset_mesh();
    if (pv->load_meshes_files () == false) {
      vul_printf (vcl_cout, "\nError: can't load %s\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      //Label components and save large components to various mesh files.
      //-n : th_n def 1000
      int th_n = dbmsh3d_cmd_n();
      if (th_n < 0)
        th_n = 1000;
      //-n2: option 1 (default), 2: IFS_to_MHE_bf.
      cclabel_save_mesh (pv->mesh(), pv->dir_prefix(), th_n, dbmsh3d_cmd_n2());
    }
  }
  //############### Remove Extraneous Faces (1,1,3) or (1,3,3) ###############
  if (dbmsh3d_cmd_rmet()==1) {
    pv->reset_mesh();
    if (pv->load_meshes_files () == false) {
      vul_printf (vcl_cout, "\nError: can't load %s\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      pv->mesh()->IFS_to_MHE ();

      //Remove extraneous non-2-manifold (1,1,3) and (1,3,3) faces.
      remove_extraneous_faces (pv->mesh());

      //-v 3: Color the resulting mesh by edge topology.
      pv->mesh()->build_face_IFS ();
      _root->addChild (pv->vis_mesh_options (3, dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()));
    }
  }
  //############### Preprocessing Point Cloud ###############
  //-pp 1: 
  //-sub n: randomly sub-sample the input pt clouds into n points.
  //        if n < 100, sub-sample by skipping every n points.
  if (dbmsh3d_cmd_preproc() || dbmsh3d_cmd_1stoct() || dbmsh3d_cmd_sub() || dbmsh3d_cmd_dup()==3) {
    pv->reset_mesh();
    bool load_file_success = false;
    //currently only handle points.
    if (pv->load_points_files () == false) {
      vul_printf (vcl_cout, "\nError: can't load %s.\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      //-scale : specified scaling factor.
      pv->process_pts (dbmsh3d_cmd_tx(), dbmsh3d_cmd_ty(), dbmsh3d_cmd_tz(), 
                       dbmsh3d_cmd_rx(), dbmsh3d_cmd_ry(), dbmsh3d_cmd_rz(), 
                       dbmsh3d_cmd_scale(), 
                       dbmsh3d_cmd_1stoct(), dbmsh3d_cmd_dup()==3, 
                       dbmsh3d_cmd_sub());
      
      //-ofile 1: output the mesh/point file.
      if (dbmsh3d_cmd_ofile() && !(dbmsh3d_cmd_xyz_ofile() || dbmsh3d_cmd_p3d_ofile())) {
        //pv->save_p3d (pv->dir_prefix() + "_pp.p3d");
        pv->save_xyz (pv->dir_prefix() + "_pp.xyz");
      }

      //-gui 1: visualize the processed file.
      if (dbmsh3d_cmd_gui())
        _root->addChild (pv->vis_ptset());
    }
  }
  //############### Bucketing the Point Cloud ###############
  if (dbmsh3d_cmd_bkt()==1 || dbmsh3d_cmd_bkt()==3) {
    //Store slices of points in vcl_vector<vcl_pair<id, pt> >.
    pv->set_pro_data (dbmsh3d_pro::PD_PTS);

    vcl_string xyz_file = pv->dir_prefix() + ".xyz";
    vcl_string p3d_file = pv->dir_prefix() + ".p3d";
    if (dbmsh3d_load_xyz (pv->pts(), xyz_file.c_str()))
      dbmsh3d_pro_contiune = true;
    else if (dbmsh3d_load_p3d (pv->pts(), p3d_file.c_str()))
      dbmsh3d_pro_contiune = true;

    if (dbmsh3d_pro_contiune == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cout, "ERROR:\t File %s Not Found!\n", pv->dir_prefix().c_str());
    }
    else {      
      if (dbmsh3d_cmd_bkt()==1) //-bktbr: box ratio 
        //-bkt 1: adpt-bucketing, save files.
        run_adpt_bucketing (pv->pts(), dbmsh3d_cmd_npbkt(), 
                            pv->dir_prefix(), dbmsh3d_cmd_dup()!=0);  
      else if (dbmsh3d_cmd_bkt()==2) 
        //-bkt 2: cell-bucketing, save files.
        run_cell_bucketing (pv->mesh(), pv->dir_file(), 
                            dbmsh3d_cmd_n1(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_n3(),
                            dbmsh3d_cmd_msr());
      if (dbmsh3d_cmd_gui()) //-gui 1: show results.
        _root->addChild (pv->vis_list_file(pv->dir_prefix() + "_bktlst", 0, dbmsh3d_cmd_v()));
    }
  }
  //############### Estimate Point Normal ###############
  if (dbmsh3d_cmd_normal()==1) { 
    //-normal 1: use rgrl to estimate normal from point cloud.
    if (pv->pro_data() == dbmsh3d_pro::PD_PTS) {
      //copy pv->pts() to pv->oripts()
      assert (pv->oripts().size() == 0);
      for (unsigned int i=0; i<pv->pts().size(); i++) {
        vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > oript;
        oript.first.set (pv->pts(i).x(), pv->pts(i).y(), pv->pts(i).z());
        pv->oripts().push_back (oript);
      }

      compute_point_normal (pv->oripts());
    }
    else if (pv->pro_data() == dbmsh3d_pro::PD_ORIPTS) {    
      assert (0);
    }
    else if (pv->pro_data() == dbmsh3d_pro::PD_MESH) {
      //copy pv->mesh()'s points to pv->oripts()
      assert (pv->oripts().size() == 0);
      for (unsigned int i=0; i<pv->mesh()->vertexmap().size(); i++) {
        dbmsh3d_vertex* V = pv->mesh()->vertexmap (i);
        vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > oript;
        oript.first.set (V->pt().x(), V->pt().y(), V->pt().z());
        pv->oripts().push_back (oript);
      }

      compute_point_normal (pv->oripts());
    }

    pv->set_pro_data (dbmsh3d_pro::PD_ORIPTS);

    _root->addChild (draw_oriented_ptset (pv->oripts(), pv->m_vertex_ccode_, COLOR_BLUE, 
                                          dbmsh3d_cmd_len(), dbmsh3d_cmd_psz(), 1));
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }
  //############### Triangulate the Input Mesh ###############
  //-tri 1: triangulate the mesh
  if (dbmsh3d_cmd_meshtri()) { 
    if (dbmsh3d_pro_contiune == false) {
      vul_printf (vcl_cout, "Error in -tri: specify input mesh file.\n");
    }
    else {
      dbmsh3d_mesh* tri_mesh = generate_tri_mesh (pv->mesh());
      pv->reset_mesh ();
      pv->set_mesh (tri_mesh);
      
      if (dbmsh3d_cmd_gui()) {
        vis = draw_M (pv->mesh(), dbmsh3d_cmd_shapehints());
        _root->addChild (shift_output (vis, -dbmsh3d_cmd_dx(), 0, 0));
      }
    }
  }
  //############### 3D Scan Simulation ###############
  //-ssim 1
  if (dbmsh3d_cmd_ssim()!=0) { 
    pv->reset_mesh();
    if (pv->load_meshes_files () == false) {
      vul_printf (vcl_cout, "\nError: can't load %s\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      //Run the 3D scan simulator.
      //Parameters:
      //-n :    n_view, def: 8
      if (dbmsh3d_cmd_n() == -1)
        dbmsh3d_cmd_n() = 8;
      //-n1:    view_min, def: 0
      if (dbmsh3d_cmd_n1() == -1)
        dbmsh3d_cmd_n1() = 0;
      //-n2:    view_max, def: 7
      if (dbmsh3d_cmd_n2() == -1)
        dbmsh3d_cmd_n2() = 7;
      //-ry:    inter_sl_ratio, def: 0.2
      if (dbmsh3d_cmd_ry() == 0)
        dbmsh3d_cmd_ry() = 0.2;
      //-rx:    intra_sl_ratio, def: 0.1
      if (dbmsh3d_cmd_rx() == 0)
        dbmsh3d_cmd_rx() = 0.1;
      //-miny:  min inter_sl_range, def: -100
      if (dbmsh3d_cmd_miny() == FLT_MAX)
        dbmsh3d_cmd_miny() = -100;
      //-maxy:  max inter_sl_range, def: 100
      if (dbmsh3d_cmd_maxy() == FLT_MIN)
        dbmsh3d_cmd_maxy() = 100;
      //-minx:  min intra_sl_range, def: -120
      if (dbmsh3d_cmd_minx() == FLT_MAX)
        dbmsh3d_cmd_minx() = -120;
      //-maxx:  max intra_sl_range, def: 120
      if (dbmsh3d_cmd_maxx() == FLT_MIN)
        dbmsh3d_cmd_maxx() = 120;
      //-ptb: Perturb (noise) by % of intra_samp_dist, def: 0

      if (dbmsh3d_cmd_n1() > dbmsh3d_cmd_n())
        dbmsh3d_cmd_n1() = dbmsh3d_cmd_n()-1;
      if (dbmsh3d_cmd_n2() >= dbmsh3d_cmd_n())
        dbmsh3d_cmd_n2() = dbmsh3d_cmd_n()-1;

      scan_sim (pv->mesh(), dbmsh3d_cmd_n(), dbmsh3d_cmd_n1(), dbmsh3d_cmd_n2(),
                dbmsh3d_cmd_ry(), dbmsh3d_cmd_rx(), 
                dbmsh3d_cmd_miny(), dbmsh3d_cmd_maxy(), 
                dbmsh3d_cmd_minx(), dbmsh3d_cmd_maxx(),
                dbmsh3d_cmd_ptb(), pv->dir_prefix().c_str());

      //Visualize the simulated scans and save list file.
      vcl_vector<vcl_string> list_3pi;
      vcl_vector<vcl_string> list_af;

      pv->reset_sg3pi ();
      for (int i = dbmsh3d_cmd_n1(); i <= dbmsh3d_cmd_n2(); i++) {
        vcl_string file_3pi = vul_sprintf ("%s_scan_%02d.3pi", pv->dir_prefix().c_str(), i);
        list_3pi.push_back (file_3pi);
        bool result = dbmsh3d_load_sg3pi (pv->sg3pi(), file_3pi.c_str());
        assert (result);

        vcl_string file_af = vul_sprintf ("%s_scan_%02d.txt", pv->dir_prefix().c_str(), i);
        list_af.push_back (file_af);
        pv->load_hmatrix (file_af);
        pv->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
        pv->apply_xform_hmatrix ();

        //Visualize the sg3pi file in random color.
        pv->move_sg3pi_to_pts ();
        _root->addChild (draw_ptset (pv->pts(), COLOR_RANDOM, pv->pt_size_));
        pv->pts().clear();
      }

      //Save list file for all simulated scans.
      vcl_string list_file = vul_sprintf ("%s_scan_sim_list.txt", pv->dir_prefix().c_str());
      dbmsh3d_save_list_file (list_file, list_3pi, list_af);

      //Save list view run file.      
      vcl_string list_view_run = vul_sprintf ("%s_scan_sim_list_view.bat", pv->dir_prefix().c_str());
      dbmsh3d_save_list_view_run_file (list_view_run, list_file);
    }    
  }    

  //##########################################################################
  //######################### Geodesic Computations ##########################
  //##########################################################################

  //##########################################################################
  //######### Geodesic Shortest Path via FMM #########
  //-fmm 1
  if (dbmsh3d_cmd_fmm()) {
    pv->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_fileprefix()));
    vcl_string suffix = dbul_get_suffix (dbmsh3d_cmd_fileprefix());

    //: read in the mesh file
    pv->load_ply2_fmm ();
    pv->fmm_mesh()->IFS_to_MHE();

    pv->fmm_mesh()->setup_fast_marching ();

    //: for multiple sources, randomly generate NS of sources
    //  if needed, randomly generate the source and ending vertex.
    vcl_srand ( (unsigned int) vcl_time(NULL) );
    unsigned int vsize = pv->fmm_mesh()->vertexmap().size();

    for (int i=0; i<dbmsh3d_cmd_ns(); i++) {
      double random = (double) vcl_rand() / RAND_MAX;
      int idx = (int) vcl_floor (random * vsize);

      //: if the sV specified
      if (i==0 && dbmsh3d_cmd_s()!=-1)
        idx = dbmsh3d_cmd_s();

      dbmsh3d_fmm_vertex_3d* src_vertex = (dbmsh3d_fmm_vertex_3d*) pv->fmm_mesh()->vertexmap (idx);
      pv->fmm_mesh()->add_source_vertex (src_vertex);

      vul_printf (vcl_cout, "The starting vertex[%d]: id[%d] (%.16f %.16f %.16f)\n",
                   i, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));
    }

    //: compute FMM from the starting vertices
    pv->fmm_mesh()->run_fmm ();

    //: visualize the mesh by coloring the distance
    _root->addChild (dbmsh3d_draw_fmm_mesh (pv->fmm_mesh()));

    //: The ending vertex for back-tracking of the geodesic shortest path.
    for (int i=0; i<dbmsh3d_cmd_ne(); i++) {
      double random = (double) vcl_rand() / RAND_MAX;
      int idx = (int) vcl_floor (random * vsize);

      //: if the eV specified
      if (i==0 && dbmsh3d_cmd_e()!=-1)
        idx = dbmsh3d_cmd_e();

      dbmsh3d_fmm_vertex_3d* ending_vertex = (dbmsh3d_fmm_vertex_3d*) pv->fmm_mesh()->vertexmap (idx);
    
      vul_printf (vcl_cout, "The ending vertex[%d]: id[%d] (%.16f %.16f %.16f)\n", 
                   i, ending_vertex->id(), 
                   ending_vertex->pt().x(), 
                   ending_vertex->pt().y(), 
                   ending_vertex->pt().z());
      //: draw the ending vertex in a sphere
      _root->addChild (draw_sphere (ending_vertex->pt(), (float) (dbmsh3d_cmd_r()/1.5), SbColor(1.0f,0.0f,1.0f)));

      //: back-tracking
      fmm_track_geodesic geodesic_tracker;
      geodesic_tracker.back_track_geodesic (ending_vertex);

      //: put the result in the geodesic_path_points.
      vcl_vector<vgl_point_3d<double> > geodesic_path_points;
      geodesic_path_points.clear();
      geodesic_tracker.get_geodesic_polyline (geodesic_path_points);

      if (i==0) {
        vul_printf (vcl_cout, "\nThe (first) geodesic shortest path (e -> s):\n");
        for (unsigned int i=0; i<geodesic_path_points.size(); i++) {
          vgl_point_3d<double> p = geodesic_path_points[i];
          vul_printf (vcl_cout, "\t(%lf, %lf, %lf)\n", p.x(), p.y(), p.z());
        }
      }

      //: visualize the geodesic shortest path
      _root->addChild (draw_polyline (geodesic_path_points, 3.0f, SbColor(0,0,1)));
    
      geodesic_path_points.clear();
    }

    dbmsh3d_app_window_title += " -- Geodesic Shortest Path on ";
    dbmsh3d_app_window_title += pv->dir_prefix();
  }

  //###########################################################################
  //######### Shortest path (graph) on mesh #########
  //-shp 1 -f prefix -s (source vid) -d (dest vid)
  /*if (dbgdt3d_cmd_shp()) {
    //
    if (pv->load_meshes_files() == false) {
      vul_printf (vcl_cout, "ERROR:\t File %s Not Found!\n", pv->dir_prefix());
    }
    else {
      //-s: src
      //-e: dest
      int src = dbmsh3d_cmd_s();
      int dest = dbmsh3d_cmd_e();
      vcl_vector<dbmsh3d_edge*> Edges;

      pv->mesh()->IFS_to_MHE();
      bool result = find_shortest_Es_on_M (pv->mesh(), src, dest, Edges);

      if (dbmsh3d_cmd_gui()) {
        //draw the mesh
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));

        //draw the start/end mesh vertex.
        dbmsh3d_vertex* Src = pv->mesh()->vertexmap (src);
        dbmsh3d_vertex* Dest = pv->mesh()->vertexmap (dest);
        _root->addChild (draw_vertex_vispt_SoCube (Src, SbColor(1,0,0), 0.1f));
        _root->addChild (draw_vertex_vispt_SoCube (Dest, SbColor(0,0,1), 0.1f));
        //draw the shortest path.
        //-cw: curve width
        for (unsigned int i=0; i<Edges.size(); i++) {
          _root->addChild (draw_edge (Edges[i], SbColor(0,1,0), dbmsh3d_cmd_cw()));
        }
      }
    }
  }*/

  //###########################################################################
  //############### Print Object Memory Size ###############
  if (dbmsh3d_cmd_mem()) {
    dbmsh3d_hypg_print_object_size ();
  }
  //###########################################################################
  //############ Generate Data File ############
  if (dbmsh3d_cmd_gen_pgutter()) {
    dbmsh3d_mesh* _Boundary = new dbmsh3d_mesh ();
    dbmsh3d_save_p3d (_Boundary, dbmsh3d_cmd_gen_pgutter());

    dbmsh3d_app_window_title += " -- ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_gen_pgutter();
  }
  if (dbmsh3d_cmd_gen_box()) {
    dbmsh3d_mesh* _Boundary = new dbmsh3d_mesh ();
    dbmsh3d_save_p3d (_Boundary, dbmsh3d_cmd_gen_box());

    dbmsh3d_app_window_title += " -- ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_gen_box();
  }
  if (dbmsh3d_cmd_gen_rand_box()) {
    dbmsh3d_mesh* _Boundary = new dbmsh3d_mesh ();
    dbmsh3d_save_p3d (_Boundary, dbmsh3d_cmd_gen_rand_box());

    dbmsh3d_app_window_title += " -- ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_gen_rand_box();
  }
  ///sli_cons_to_nuages_cnt (sli_file, cnt_file);

  //##########################################################################
  //##################### Process and Save Output Files ######################
  //##########################################################################

  //########################### Save Output Files ############################
  vcl_string dirfile;  
  if (dbmsh3d_cmd_p3d_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_p3d_ofile());
    dirfile += ".p3d";
    vul_printf (vcl_cout, "Writing ASCII point cloud file (*.p3d) %s...\n", dirfile.c_str());
    pv->save_p3d (dirfile);
  }
  if (dbmsh3d_cmd_g3d_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_g3d_ofile());
    dirfile += ".g3d";
    vul_printf (vcl_cout, "Writing color point cloud file (*.g3d) %s...\n", dirfile.c_str());
    pv->save_g3d (dirfile);
  }
  if (dbmsh3d_cmd_xyz_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_xyz_ofile());
    dirfile += ".xyz";
    vul_printf (vcl_cout, "Writing ASCII point cloud file (*.xyz) %s...\n", dirfile.c_str());
    pv->save_xyz (dirfile);
  }
  if (dbmsh3d_cmd_xyzn1_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_xyzn1_ofile());
    dirfile += ".xyzn1";
    vul_printf (vcl_cout, "Writing oriented point cloud file (*.xyzn1) %s...\n", dirfile.c_str());
    pv->save_xyzn1 (dirfile);
  }
  if (dbmsh3d_cmd_xyznw_ofile()) {
    ///vul_printf (vcl_cout, "Writing Oriented Point Cloud (*.xyznw) file %s...\n", dbmsh3d_cmd_xyznw_ofile());
    ///pv->save_xyznw (vcl_string (dbmsh3d_cmd_xyznw_ofile()) + ".xyznw");
  }
  if (dbmsh3d_cmd_ply_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofile());
    dirfile += ".ply";
    vul_printf (vcl_cout, "Writing ASCII PLY mesh file (*.ply) %s...\n", dirfile.c_str());
    pv->save_ply (dirfile, true);
  }
  if (dbmsh3d_cmd_ply_ofileb()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofileb());
    dirfile += ".ply";
    vul_printf (vcl_cout, "Writing binary PLY mesh file (*.ply) %s...\n", dirfile.c_str());
    pv->save_ply (dirfile, false);
  }
  if (dbmsh3d_cmd_ply2_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply2_ofile());
    dirfile += ".ply2";
    vul_printf (vcl_cout, "Writing ASCII PLY2 mesh file (*.ply2) %s...\n", dirfile.c_str());
    pv->save_ply2 (dirfile);
  }
  if (dbmsh3d_cmd_off_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_off_ofile());
    dirfile += ".off";
    vul_printf (vcl_cout, "Writing Geomview OFF mesh file (*.off) %s...\n", dirfile.c_str());
    pv->save_off (dirfile);
  }
  if (dbmsh3d_cmd_obj_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_obj_ofile());
    dirfile += ".obj";
    vul_printf (vcl_cout, "Writing Wavefront OBJ mesh file (*.obj) %s...\n", dirfile.c_str());
    pv->save_obj (dirfile);
  }
  if (dbmsh3d_cmd_vtk_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_vtk_ofile());
    dirfile += ".vtk";
    vul_printf (vcl_cout, "Writing VTK mesh file (*.vtk) %s...\n", dirfile.c_str());
    pv->save_vtk (dirfile);
  }
  
  //: ############ OpenInventor Demo and Test ############
  //-test :
  if (dbmsh3d_cmd_test()) {

    //Produce a twisting surface with `+'-like cross section.
    dbmsh3d_mesh* M = create_twist_cross ();
    dbmsh3d_save_p3d (M, "d:\\twist_cross.p3d");
    _root->addChild (draw_ptset (M, COLOR_BLACK));

    /*
    //test texture mapping.
    dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;

    dbmsh3d_vertex* v[4];

    v[0] = M->_new_vertex ();
    v[0]->set_pt (0,0,0);
    M->_add_vertex (v[0]);

    v[1] = M->_new_vertex ();
    v[1]->set_pt (300, 0, 0);
    M->_add_vertex (v[1]);

    v[2] = M->_new_vertex ();
    v[2]->set_pt (300, 349, 0);
    M->_add_vertex (v[2]);
    
    v[3] = M->_new_vertex ();
    v[3]->set_pt (0, 349, 0);
    M->_add_vertex (v[3]);

    dbmsh3d_textured_face_mc* f0 = M->_new_face ();
    M->_add_face (f0);  
    f0->_add_vertex (v[0], vgl_point_2d<double> (v[0]->pt().x(), v[0]->pt().y()));
    f0->_add_vertex (v[1], vgl_point_2d<double> (v[1]->pt().x(), v[1]->pt().y()));
    f0->_add_vertex (v[2], vgl_point_2d<double> (v[2]->pt().x(), v[2]->pt().y()));
    f0->_add_vertex (v[3], vgl_point_2d<double> (v[3]->pt().x(), v[3]->pt().y()));

    vcl_string texture_filename = "texture.png";
    f0->set_tex_map_uri (texture_filename);

    ///vis = new SoSeparator;
    ///draw_M_ifs_geom (vis, M);
    _root->addChild (draw_M_textured (M));*/


    //Debug ray-tri intersection!!
    /*vgl_point_3d<double> pt11 (0.19187803948405169, 0.00069384501684954607, 0.0025251314556233393);
    vgl_point_3d<double> pt12 (-0.19187803948405169, 0.00069384501684954607, 0.0025251314556233393);
    vis = draw_line (pt11, pt12, SbColor (0,1,0));
    _root->addChild (vis);

    vgl_point_3d<double> pt21 (0.19187803948405169, 0.00069384501684954607, 0.0026716873794280357);
    vgl_point_3d<double> pt22 (-0.19187803948405169, 0.00069384501684954607, 0.0026716873794280357);
    vis = draw_line (pt21, pt22, SbColor (0,0,1));
    _root->addChild (vis);

    double v1[3], v2[3], v3[3], ori1[3], ori2[3], vec[3];

    ori1[0] = 0.19187803948405169;
    ori1[1] = 0.00069384501684954607;
    ori1[2] = 0.0025251314556233393;
    
    ori2[0] = 0.19187803948405169;
    ori2[1] = 0.00069384501684954607;
    ori2[2] = 0.0026716873794280357;

    vec[0] = -1;
    vec[1] = 0;
    vec[2] = 0;

    v1[0] = 0.012822999999999999;
    v1[1] = 0.00088199999999999997;
    v1[2] = 0.0030130000000000001;

    v2[0] = 0.012175000000000000;
    v2[1] = 0.00016899999999999999;
    v2[2] = 0.0024099999999999998;
    
    v3[0] = 0.012106000000000000;
    v3[1] = 0.0011100000000000001;
    v3[2] = 0.0021879999999999998;

    //

    double t, u, v;
    vgl_point_3d<double> iP;
    int result = intersect_triangle (ori1, vec, v1, v2, v3, &t, &u, &v); 
    //check t>0 to make sure correct ray direction.
    if (result && t >0) {
      double rx = (1-u-v)*v1[0] + u*v2[0] + v*v3[0];
      double ry = (1-u-v)*v1[1] + u*v2[1] + v*v3[1];
      double rz = (1-u-v)*v1[2] + u*v2[2] + v*v3[2];
      iP.set (rx, ry, rz);
      ///return true;
    }

    result = intersect_triangle (ori2, vec, v1, v2, v3, &t, &u, &v); 
    //check t>0 to make sure correct ray direction.
    if (result && t >0) {
      double rx = (1-u-v)*v1[0] + u*v2[0] + v*v3[0];
      double ry = (1-u-v)*v1[1] + u*v2[1] + v*v3[1];
      double rz = (1-u-v)*v1[2] + u*v2[2] + v*v3[2];
      iP.set (rx, ry, rz);
      ///return true;
    }

    //Draw the triangle v1, v2, v3.
    SbVec3f* vertices = new SbVec3f[3];
    vertices[0].setValue (v1[0], v1[1], v1[2]);
    vertices[1].setValue (v2[0], v2[1], v2[2]);
    vertices[2].setValue (v3[0], v3[1], v3[2]);
    
    vis = draw_filled_polygon (vertices, 3, SbColor(0.5f, 0.5f, 0.5f), 0.0f); //Gray
    _root->addChild (vis);*/

    /*vcl_string file_ply = "15.ply";
    pv->load_ply (file_ply);

    //: y -> -y, z -> -z.
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pv->mesh()->vertexmap().begin();
    for (; it != pv->mesh()->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      vgl_point_3d<double> P (V->pt().x(), -V->pt().y(), -V->pt().z());
      V->set_pt (P);
    }

    pv->mesh()->IFS_to_MHE ();
    pv->mesh()->build_face_IFS ();
    _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));

    vcl_string file_3pi = "15.3pi";
    dbmsh3d_load_sg3pi (pv->sg3pi(), file_3pi.c_str());
    pv->reset_mesh();
    build_mesh_vertices_3pi (pv->sg3pi(), pv->mesh());   
    pv->set_pro_data (dbmsh3d_pro::PD_MESH);
    _root->addChild (pv->vis_ptset (3, dbmsh3d_cmd_idv()!=0));*/

    /*vul_printf (vcl_cout, "Test Coin3D controls......\n");
    
    SoSeparator * group = new SoSeparator;

    vgl_point_3d<double> p0 (0,0,0);
    vgl_point_3d<double> p1 (0,0,1);

    //Dragger for p0 along y
    SoTranslation * trans = new SoTranslation;
    group->addChild(trans);
    trans->translation.setValue(0, 0, 0.0f);

    SoRotationXYZ * rotate = new SoRotationXYZ;
    group->addChild(rotate);
    rotate->axis = SoRotationXYZ::Z;
    rotate->angle = 1 * M_PI / 180.0f;

    group->addChild(new SoTranslate1Dragger);
    
    _root->addChild (group);*/

  }

  return _root;
}

SoSeparator* dbmsh3d_cmdproc_execute_2 (dbmsh3d_pro_vis* mpv2)
{
  SoSeparator* _root = new SoSeparator;
    
  //############### Viewing IV File 3 ###############
  if (dbmsh3d_cmd_iv_file3()) {
    mpv2->b_view_iv_ = true;
    mpv2->set_dir_file (dbul_get_dir_file (dbmsh3d_cmd_iv_file3()));
  }
  if (mpv2->b_view_iv_) {
    _root->addChild (mpv2->view_iv ());
    dbmsh3d_app_window_title += " -- " + mpv2->dir_file();
  }

  return _root;
}


//#######################################################################

void dbmsh3d_save_vis_to_iv_wrl (SoSeparator* _root)
{
  //Save Visualization Output Files
  if (dbmsh3d_cmd_iv_ofile()) {
    vul_printf (vcl_cout, "Writing OpenInventor IV (*.iv) file %s...\n", dbmsh3d_cmd_iv_ofile());
    bgui3d_export_iv (_root, vcl_string (dbmsh3d_cmd_iv_ofile()) + ".iv");
  }
  if (dbmsh3d_cmd_iv_ofilebin()) {
    ////writeIV (_root, dbmsh3d_cmd_iv_ofilebin(), false);
  }
  if (dbmsh3d_cmd_wrl_ofile()) {
    SoToVRMLAction tovrml;
    tovrml.apply (_root);
    SoNode *_VRMLroot = tovrml.getVRMLSceneGraph(); //THERE ARE SOME PROBLEM HERE!
    _VRMLroot->ref();

    vul_printf (vcl_cout, "Writing VRML (*.wrl) file %s...\n", dbmsh3d_cmd_wrl_ofile());

    SoOutput out;
    out.openFile (dbmsh3d_cmd_wrl_ofile());
    out.setHeaderString("#VRML V1.0 ascii");
    SoWriteAction wra(&out);
    wra.apply (_VRMLroot);
    out.closeFile();
  
    _VRMLroot->unref();
  }
  if (dbmsh3d_cmd_wrl_ofile2()) {
    SoToVRML2Action tovrml2;
    tovrml2.apply (_root);
    SoVRMLGroup *_VRML2root = tovrml2.getVRML2SceneGraph();
    _VRML2root->ref();
  
    vul_printf (vcl_cout, "Writing VRML2 (*.wrl) file %s...\n", dbmsh3d_cmd_wrl_ofile2());
  
    SoOutput out;
    out.openFile (dbmsh3d_cmd_wrl_ofile2());
    out.setHeaderString("#VRML V2.0 utf8");
    SoWriteAction wra(&out);
    wra.apply (_VRML2root);
    out.closeFile();
  
    _VRML2root->unref();
  }
}
