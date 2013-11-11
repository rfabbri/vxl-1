//: This is lemsvxlsrc/brcv/shp/dbsk3d/vis/dbsk3d_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_timer.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_fs_fileio.h>
#include <dbsk3d/algo/dbsk3d_ms_recon.h>
#include <dbsk3d/algo/dbsk3d_fs_segre_bkt_stitch.h>

#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_sheet.h>
#include <dbsk3d/vis/dbsk3d_cmdproc.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_segre.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_flow_type.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_flow_complex.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_hypg_trans.h>


//#####################################################################
// dbsk3d_cmdproc execute process.

bool dbsk3d_check_cmdproc (dbsk3d_pro_vis* pv, int argc, char **argv)
{
  //1) Check if no cmdproc specified, result=PRO_RESULT_NO_PROCESS.  
  if (!dbsk3d_cmd_fs_file() && !dbsk3d_cmd_cms_file() && !dbsk3d_cmd_cms_file2() &&
      !dbsk3d_cmd_sg_file() && !dbsk3d_cmd_sg_file2() &&   
      dbsk3d_cmd_all()==0 &&
      dbsk3d_cmd_bsphr()==0 && dbsk3d_cmd_qhfs()==0 &&
      dbsk3d_cmd_seg()==0 && dbsk3d_cmd_ifgp()==0 && dbsk3d_cmd_reg()==0 && 
      dbsk3d_cmd_isc()==0 && dbsk3d_cmd_ms()==0 && dbsk3d_cmd_gdts()==0 &&
      dbsk3d_cmd_trans()==0 && dbsk3d_cmd_transv()==0 && dbsk3d_cmd_smrib()==0 &&
      dbsk3d_cmd_transg()==0 && dbsk3d_cmd_sgsm()==0 &&
      dbsk3d_cmd_bktseg()==0 && dbsk3d_cmd_bktpst()==0 && dbsk3d_cmd_merge()==0 &&
      dbsk3d_cmd_ridge()==0 && dbsk3d_cmd_spd()==0 && 
      dbsk3d_cmd_vsf()==0 && dbsk3d_cmd_vfc()==0 && dbsk3d_cmd_vss()==0 &&
      dbsk3d_cmd_smsh()==0) {
    //Default Viewer: try to open the argv[1] file and visualize it.
    //Assume the first parameter specifies the file to display.
    if (argc>1 && argv[1][0] != '-') {
      //Assume the first parameter specifies the file to display.
      vcl_string dir_file = dbul_get_dir_file (argv[1]);
      vcl_string suffix = dbul_get_suffix (argv[1]);
      if (suffix == ".fs") {
        pv->b_view_fs_ = true;
        pv->set_dir_file (dir_file);
      }
      else if (suffix == ".cms") {
        pv->b_view_cms_ = true;
        pv->set_dir_file (dir_file);            
      }
      else if (suffix == ".sg") {
        pv->b_view_sg_ = true;
        pv->set_dir_file (dir_file);
      }
    }
    else {
      return false; 
    }
  }

  //Check the command with a required prefix -f.
  if (dbsk3d_cmd_all()!=0 || dbsk3d_cmd_ms()!=0 || dbsk3d_cmd_seg()!=0 || dbsk3d_cmd_ifgp()!=0 ||
      dbsk3d_cmd_reg()!=0 || dbsk3d_cmd_isc()!=0) {
    if (!dbmsh3d_cmd_fileprefix()) {
      vul_printf (vcl_cerr, "ERROR:\tMissing -f PREFIX. Use -h, -hh 1,2,3, for more help.\n");
      return false;
    }
  }

  return true;
}

void dbsk3d_setup_provis_parameters (dbsk3d_pro_vis* pv)
{
  if (dbmsh3d_cmd_fileprefix()) //set prefix
    pv->set_dir_file (dbmsh3d_cmd_fileprefix());

  //Setup visualization parameters.
  pv->pt_size_ = dbmsh3d_cmd_psz();
  pv->cube_size_ = dbmsh3d_cmd_rc();
  pv->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();

  pv->m_edge_width_ = dbmsh3d_cmd_cw();   
  pv->m_bndcv_width_ = dbmsh3d_cmd_cw();  
  pv->m_transp_ = dbmsh3d_cmd_mesh_transp(); 
  pv->ms_vis_param_.sheet_transp_ = dbsk3d_cmd_sheet_transp();
  pv->ms_vis_param_.curve_width_ = dbmsh3d_cmd_cw()*2;
  pv->ssheet_transp_ = dbsk3d_cmd_sheet_transp();

  pv->view_dx_ = dbmsh3d_cmd_dx();
  pv->view_dy_ = dbmsh3d_cmd_dy();
  pv->view_dz_ = dbmsh3d_cmd_dz();

  if (dbmsh3d_cmd_iv_ofile() || dbmsh3d_cmd_iv_ofilebin() || 
      dbmsh3d_cmd_wrl_ofile() || dbmsh3d_cmd_wrl_ofile2())
    pv->user_defined_class_ = false;
  
  switch (dbmsh3d_cmd_id()) {
  case 1:
    pv->ms_vis_param_.vertex_show_id_ = true;
  break;
  case 2:
    pv->ms_vis_param_.curve_show_id_ = true;
  break;
  case 3:
    pv->ms_vis_param_.sheet_show_id_ = true;
  break;
  case 4:
    pv->ms_vis_param_.vertex_show_id_ = true;
    pv->ms_vis_param_.curve_show_id_ = true;
  break;
  case 5:
    pv->ms_vis_param_.vertex_show_id_ = true;
    pv->ms_vis_param_.curve_show_id_ = true;
    pv->ms_vis_param_.sheet_show_id_ = true;
  break;
  default:
  break;
  }

  if (dbmsh3d_cmd_bgcol() == 1) {
    pv->m_vertex_ccode_ = COLOR_BLACK;
    pv->bg_ccode_ = COLOR_WHITE;
  }
  else {
    pv->m_vertex_ccode_ = COLOR_WHITE;
    pv->bg_ccode_ = COLOR_BLACK;
  }
}

SoSeparator* dbsk3d_cmdproc_execute (dbsk3d_pro_vis* pv)
{
  SoSeparator* _root = new SoSeparator;
  SoSeparator* vis;
  
  //##########################################################################
  //############################## Visualization##############################
  //##########################################################################

  //########### View Fine-scale Medial Scaffold (FMS, *.fs) File #############  
  //-fs prefix: 
  if (dbsk3d_cmd_fs_file()) {
    pv->b_view_fs_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbsk3d_cmd_fs_file()));
  }
  if (pv->b_view_fs_) {
    pv->reset_mesh ();
    if (pv->load_fs (pv->dir_file()) == false) {
      vul_printf (vcl_cerr, "\nError: can't load %s.fs.\n", pv->dir_file());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      pv->compute_vertex_node_size ();

      if (dbmsh3d_cmd_gui()) {        
        if (dbsk3d_cmd_dels()) { //If -dels 1 on -fs fs_mesh
          //set -idv 1.
          dbmsh3d_cmd_idv() = 1;          
          //set -bnd 0: hide boundary drawing.
          dbsk3d_cmd_bnd() = 0;
        }

        //Visualize the fine shock scaffold.
        if (dbmsh3d_cmd_v()==0) { //-v 0: Draw the valid fs_mesh shock sheets
          _root->addChild (pv->vis_fs_mesh_valid (dbmsh3d_cmd_idv()!=0));
        }
        else if (dbmsh3d_cmd_v()==1) {
          //-v 1: Visualization with more options.
          //-inv 1: draw invalid sheets.
          //-inf 1: draw unbounded sheets.
          vis = pv->vis_fs_mesh (true, dbmsh3d_cmd_inv()!=0, dbmsh3d_cmd_inf()!=0);
          _root->addChild (vis);
        }
        else if (dbmsh3d_cmd_v()>=2 && dbmsh3d_cmd_v()<=4) { 
          //-v 2,3: Show the A13-shock-link faces within a range.
          pv->fs_segre()->seg_init_shock_queue (dbmsh3d_cmd_msr()); 

          if (dbmsh3d_cmd_v()==2) { //-v 2: show shock links in Queue 1          
            const double rth = pv->fs_segre()->get_Q1_percent_cost ((float) (dbmsh3d_cmd_percent()/100));
            const double rth2 = pv->fs_segre()->get_Q1_percent_cost ((float) (dbmsh3d_cmd_percent2()/100));

            vul_printf (vcl_cerr, "\t Queue 1 shape cost median = %f, 10%% = %f, 90%% = %f\n", 
                         pv->fs_segre()->get_Q1_median_cost(), 
                         pv->fs_segre()->get_Q1_percent_cost (0.1f),
                         pv->fs_segre()->get_Q1_percent_cost (0.9f));

            vul_printf (vcl_cerr, "Visualizing %.0f to %.0f percent of A13-2-links in Queue 1.\n", 
                         dbmsh3d_cmd_percent(), dbmsh3d_cmd_percent2());

            //-n option: -1: color tone, 1: gray, 2: random.
            ///_root->addChild (draw_segre_Q1_L_F (pv->fs_segre(), rth, rth2, dbmsh3d_cmd_n(), 
                                                ///pv->user_defined_class_));
            //-boxr: length ratio (def: 0.1 * 10)
            float len = float (pv->fs_mesh()->median_A122_dist() * dbmsh3d_cmd_boxr() * 10);
            //-n2: 0 (type I only), 1: type I and type II
            _root->addChild (draw_segre_Q1_batch (pv->fs_segre(), rth, rth2, dbmsh3d_cmd_n2()==0, len));
          }
          else if (dbmsh3d_cmd_v()==3) { //-v 3: show shock links in Queue 2   
            const double rth = pv->fs_segre()->get_Q2_percent_cost ((float) (dbmsh3d_cmd_percent()/100));
            const double rth2 = pv->fs_segre()->get_Q2_percent_cost ((float) (dbmsh3d_cmd_percent2()/100));

            vul_printf (vcl_cerr, "\t Queue 2 shape cost median = %f, 10%% = %f, 90%% = %f\n", 
                         pv->fs_segre()->get_Q2_percent_cost (0.5f), 
                         pv->fs_segre()->get_Q2_percent_cost (0.1f),
                         pv->fs_segre()->get_Q2_percent_cost (0.9f));

            vul_printf (vcl_cerr, "Visualizing %.0f to %.0f percent of A13-links in Queue 2.\n", 
                         dbmsh3d_cmd_percent(), dbmsh3d_cmd_percent2());

            _root->addChild (draw_segre_Q2_L_F (pv->fs_segre(), rth, rth2, dbmsh3d_cmd_n(), 
                                                pv->user_defined_class_));
          }
          else if (dbmsh3d_cmd_v()==4) { //-v 4: show oversized shock links
            double size_th = pv->fs_mesh()->median_A122_dist() * dbmsh3d_cmd_msr();  
            _root->addChild (draw_segre_oversize_L_F (pv->fs_mesh(), size_th, pv->user_defined_class_));
          }
        }
        else if (dbmsh3d_cmd_v() == 5) { 
          //-v 5: Show the sheet-component of the fine-scale shock scaffold.
          pv->fs_ss()->build_fs_sheet_set ();
          pv->fs_regul()->init_regul_sheet_queue (dbsk3d_cmd_regth());
          
          ////-n 1: remove all FF whose G has footpoint on it.

          //-rmin: rmin-pruning 
          //-n1 0: to skip rmin-pruning
          if (dbmsh3d_cmd_n1() != 0)
            pv->run_rmin_trim_xforms (dbsk3d_cmd_rmin(), dbsk3d_cmd_gsa()!=0);

          //-boxr: bounding box pruning.
          //-n2 0: to skip bounding box pruning.
          if (dbmsh3d_cmd_n2() != 0)
            shock_pruning_box (pv->fs_ss(), dbmsh3d_cmd_boxr());

          //-shole: Remove shocks of boundary holes
          if (dbsk3d_cmd_shole() == 1) {
            //Load in the surface mesh prefix-surface.
            if (pv->load_faces_files (pv->dir_prefix() + "-surface") == false) {
              vcl_cout << "\ndbsk3d_cmdproc_execute (ms): Error loading " << pv->dir_prefix() + "-surface.\n";
              dbmsh3d_cmd_gui() = 0;
              return _root;
            }
            pv->mesh()->IFS_to_MHE();

            pv->prune_shocks_of_holes ();
          }

          //Re-build the sheet components.
          // (After the rmin pruning, a component can become two components).
          pv->fs_mesh()->detect_valid_FE_type ();
          pv->fs_ss()->clear();
          pv->fs_ss()->build_fs_sheet_set (); 

          //-comp : only keeps the specified component (def 0).
          //-n3 0: to skip the component selection.
          if (dbmsh3d_cmd_n3() != 0) {
            vcl_set<int> ith_comp_list;
            ith_comp_list.insert (dbsk3d_cmd_comp());
            //-gsa 1: re-assigne lost genes to shocks.
            bool result = pv->determine_inside_shock_comp (ith_comp_list, dbsk3d_cmd_gsa()!=0);
          }

          //-idv : draw individual elements.
          _root->addChild (draw_fs_sheet_set (pv->fs_ss(), dbmsh3d_cmd_inf(), 
                                              dbmsh3d_cmd_idv(), pv->user_defined_class_));

          if (dbsk3d_cmd_bnd() == 5) {           
            ///_root->addChild (draw_tabs_bnd_curve (pv->fs_ss(), dbsk3d_cmd_regth()));
          }
        }
        else if (dbmsh3d_cmd_v() == 6) { //-v 6: Show remaining shocks around the holes.
          //load boundary mesh from *-surface.ply2 
          vcl_string surf_file = pv->dir_prefix() + "-surface.ply2";
          pv->load_faces_files (pv->dir_prefix() + "-surface");
          pv->mesh()->IFS_to_MHE ();

          // -n 1: draw only related shock sheet elements with any generator at hole.
          // -n 2: draw only related shock sheet elements with both generators at hole.
          // -n 3: draw only related shock link elements.
          _root->addChild (pv->vis_shocks_of_holes (dbmsh3d_cmd_n()));
        }
        else if (dbmsh3d_cmd_v() == 7) { //-v 7: Visualize fs_faces in pseudo color by their radii (time).
          _root->addChild (pv->vis_fs_mesh_color_by_radius ());
        }
        
        if (dbsk3d_cmd_bnd()==1 || dbsk3d_cmd_bnd()==4) { //-bnd 1,4: Draw the boundary generators. 
          //Enlarge the gene size if gene association is visualized.
          if (dbmsh3d_cmd_n2() > 1 && pv->pt_size_==1)
            pv->pt_size_ = 2;            

          if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
            _root->addChild (pv->vis_ptset());
          else //draw each gene individually to enable getInfo.          
            _root->addChild (pv->vis_ptset_idv());

          //Identify a particular gene in a larger cube.
          ///dbmsh3d_vertex* G = pv->mesh()->vertexmap(188);
          ///_root->addChild (draw_vertex_vispt_SoCube (G, SbColor (1,0,0), 0.2f));

          //-n2 5: Show all P-G, L-G, FV-G assignments.
          //-n2 2: Show patch-gene assignments.
          if (dbmsh3d_cmd_n2() == 2 || dbmsh3d_cmd_n2() == 5) 
            _root->addChild (draw_fs_patch_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0));
          //-n2 3: show link-gene assignments.
          if (dbmsh3d_cmd_n2() == 3 || dbmsh3d_cmd_n2() == 5) 
            _root->addChild (draw_fs_link_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0));
          //-n2 4: show node-gene assignments.
          if (dbmsh3d_cmd_n2() == 4 || dbmsh3d_cmd_n2() == 5) 
            _root->addChild (draw_fs_node_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0));

          //-n1 4: draw unassigned genes in cube.
          if (dbsk3d_cmd_bnd() == 4)
            _root->addChild (pv->vis_unasgn_genes(dbmsh3d_cmd_rc()));
        }
        else if (dbsk3d_cmd_bnd() == 2) {//-bnd 2: draw mesh edges
          //Enlarge the gene size if gene association is visualized.
          if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
            vis = pv->vis_mesh_pts_edges();
          else //draw each gene individually to enable getInfo.          
            vis = pv->vis_ptset_idv();
          _root->addChild (vis); 
        }
        else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh with transparency
          if (pv->m_transp_ == 0.0f)
            pv->m_transp_ = 0.5;
          vis = pv->vis_mesh (false, dbmsh3d_cmd_shapehints());    
          _root->addChild (vis);       
        }
      }
        
      if (dbmsh3d_cmd_ofile() == 2) { //Create the A12-2 time file to produce histogram in Matlab.
        output_A12_2_file (pv->fs_mesh(), dbmsh3d_cmd_n());
      }
      else if (dbmsh3d_cmd_ofile() == 3) { //Create the A13-2 data file to produce histogram in Matlab.
        output_A13_file (pv->fs_mesh());
      }
      else if (dbmsh3d_cmd_ofile() == 4) { //Create the ordered A13 cost file to produce histogram in Matlab.
        pv->fs_segre()->seg_init_shock_queue (dbmsh3d_cmd_msr()); 
        pv->fs_segre()->output_Q1_file ();
      }

      dbmsh3d_pro_contiune = true;
      dbmsh3d_app_window_title += " -- " + pv->dir_file();
    }
  }

  //############### View CMS File ###############  
  //-cms: View the coarse-scale medial scaffold file.
  if (dbsk3d_cmd_cms_file()) {
    pv->b_view_cms_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbsk3d_cmd_cms_file()));
  }
  if (pv->b_view_cms_) {
    pv->reset_mesh ();
    pv->reset_fs_mesh ();
    pv->reset_ms_hypg ();
    pv->load_cms ();

    if (pv->ms_hypg()->check_integrity() == false)
      vul_printf (vcl_cerr, "\n    ms_hypg.check_integrity() fail!\n\n");
    else
      vul_printf (vcl_cerr, "\n    ms_hypg.check_integrity() pass.\n\n");

    //-af : read the alignment from file and perform transformation.
    if (dbmsh3d_cmd_align_file()) { 
      pv->load_hmatrix (dbmsh3d_cmd_align_file());
      vcl_cout << "Transforming mesh ...\n"; // transform the reference mesh
      dbmsh3d_apply_xform (pv->mesh(), pv->hmatrix());
      dbmsh3d_apply_xform (pv->fs_mesh(), pv->hmatrix());
      vcl_cout << "done.\n";
    }

    //-osg: write *.sg file.
    if (dbsk3d_cmd_sg_ofile()) {
      pv->build_ms_graph_sa_from_cms ();
      if (dbmsh3d_cmd_ofile()>0)
        pv->save_sg (dbsk3d_cmd_sg_ofile());
    }

    if (dbmsh3d_cmd_gui()) {
      pv->compute_vertex_node_size ();      
      //-r (def: 0.03) ball radius
      if (dbmsh3d_cmd_r() != 0.03f)
        pv->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();

      if (dbsk3d_cmd_dels()) { //If -dels 1,2,3 on -cms -ms_hypg 
        //set -bnd 0: hide boundary drawing.
        dbsk3d_cmd_bnd() = 0;
        //set -v 3: draw ms_sheets in color and hide ms_curves and ms_vertices.
        dbmsh3d_cmd_v() = 3;        
        if (dbsk3d_cmd_dels()==3) //for -dels 3, turn -idv 1 on.
          dbmsh3d_cmd_idv() = 1;
      }

      //-v 0: draw the medial scaffold graph and randomly color each sheet but avoiding color close to red or blue.
      //   1: draw the medial scaffold graph and draw the whole ms_sheets in a single color.
      //   2: only draw the (medial scaffold) graph and skip drawing all sheets.
      //   3: only draw the ms_sheets in random colors and skip drawing the (medial scaffold) graph.
      //   4: only draw the ms_sheets in a single color and skip drawing the (medial scaffold) graph.
      //   5: color each ms_sheet by cost order (in pseudo color).
      //   6: color each ms_sheet by cost (in pseudo color).
      //   7: color each ms_sheet by geodesic distance.
      //   8: not drawing anything.
      //-cseed: color seed (default DBMSH3D_SHEET_COLOR_SEED: 622).
      //-idv 1: draw individual fs_faces 
      _root->addChild (pv->vis_ms_hypg (dbmsh3d_cmd_v(), dbmsh3d_cmd_cseed(), dbmsh3d_cmd_idv()!=0));
      
      if (dbsk3d_cmd_bnd() == 1) { //-bnd 1: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (pv->vis_ptset());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (pv->vis_ptset_idv());
      }
      else if (dbsk3d_cmd_bnd() == 2) { //-bnd 2: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (pv->vis_mesh_pts_edges());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (pv->vis_ptset_idv());
      }
      else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh in transparency.
        if (pv->m_transp_ == 0.0f)
          pv->m_transp_ = 0.5;
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints()));
      }

      //-vs 2,3,4: draw corresponding boundary mesh patches in color in (0, dy).
      //           def: 1: not drawing it.
      //           vs 2: draw Gfaces.
      //           vs 3: draw Gfaces and Gfaces2.
      //           vs 4: draw Gfaces, Gfaces2, and Gfaces1.
      if (dbsk3d_cmd_vs() > 1) {         
        vis = pv->vis_ms_hypg_bnd_mesh (dbsk3d_cmd_vs());        
        pv->compute_dx_dy_dz ();
        _root->addChild (shift_output (vis, 0, pv->view_dy_, 0)); 
      }

      //-id 6: show sheet id specified by -n .
      if (dbmsh3d_cmd_id()==6) {      
        int id = dbmsh3d_cmd_n();
        assert (id >= 0);
        dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) pv->ms_hypg()->sheetmap (id);
        char id_buf[64];
        vcl_sprintf (id_buf, "%d", id);

        //show id at the middle of the sheet.
        const dbsk3d_fs_vertex* FV = MS->get_middle_FV ();
        const vgl_point_3d<double> pt = FV->pt();
        _root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z(), SbColor (0,0,0)));
      }
      //-id 7: show curve id specified by -n .
      else if (dbmsh3d_cmd_id()==7) {      
        int id = dbmsh3d_cmd_n();
        assert (id >= 0);
        dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) pv->ms_hypg()->edgemap (id);
        char id_buf[64];
        vcl_sprintf (id_buf, "%d", id);

        //show id at the middle of the sheet.
        const vgl_point_3d<double> pt = MC->get_middle_N()->pt();
        _root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z(), SbColor (0,0,0)));
      }
      //-id 8: show vertex id specified by -n .
      else if (dbmsh3d_cmd_id()==7) {      
        int id = dbmsh3d_cmd_n();
        assert (id >= 0);
        dbsk3d_ms_node* MN = (dbsk3d_ms_node*) pv->ms_hypg()->vertexmap (id);
        char id_buf[64];
        vcl_sprintf (id_buf, "%d", id);

        //show id at the middle of the sheet.
        const vgl_point_3d<double> pt = MN->FV()->pt();
        _root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z(), SbColor (0,0,0)));
      }
    }

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }
  //############### View SG File ###############
  //-sg: View the medial scaffold graph file.
  if (dbsk3d_cmd_sg_file()) {
    pv->b_view_sg_ = true;
    pv->set_dir_file (dbul_get_dir_file (dbsk3d_cmd_sg_file()));
  }
  if (pv->b_view_sg_) {
    pv->reset_mesh ();
    pv->load_sg ();

    //Load in the surface mesh prefix-surface.
    if (pv->load_meshes_files (pv->dir_prefix() + "-surface") == false)
      vcl_cout << "\ndbsk3d_cmdproc_execute (isc): Error loading " << pv->dir_prefix() + "-surface.\n";

    //-osg: 
    if (dbsk3d_cmd_sg_ofile()) {
      pv->save_sg (dbsk3d_cmd_sg_ofile());
    }

    if (dbmsh3d_cmd_gui()) {      
      pv->compute_vertex_node_size ();
      _root->addChild (pv->vis_ms_graph_sa ());
      
      if (dbsk3d_cmd_bnd() == 1) { //-bnd 1: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (pv->vis_ptset());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (pv->vis_ptset_idv());
      }
      if (dbsk3d_cmd_bnd() == 2) { //-bnd 2: draw boundary generators.
        //Visualize the object boundary generators.
        if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
          _root->addChild (pv->vis_mesh_pts_edges());
        else //draw each gene individually to enable getInfo.          
          _root->addChild (pv->vis_ptset_idv());
      }
      else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh in transparency.
        if (pv->m_transp_ == 0.0f)
          pv->m_transp_ = 0.5;
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints()));
      }
    }

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }

  //##########################################################################
  //############################## Computations ##############################
  //##########################################################################
  
  //###########################################################################
  //-bks: Leymarie's Bucketing (Buggy).
  if (dbsk3d_cmd_bks()==1) {
    pv->shock_detect_bucketing ();
    dbmsh3d_cmd_gui() = 0;
  }
  //###########################################################################
  //-flsht: Leymarie's FlowSheet Shock Detection (Buggy).
  if (dbsk3d_cmd_flowsheet()==1) {
    pv->shock_detect_flow_sheet ();
    dbmsh3d_cmd_gui() = 0;
  }  
  //###########################################################################
  //-ptb 1: Perturb the point cloud.
  if (dbmsh3d_cmd_ptb() && dbmsh3d_cmd_ssim()==0) {
    if (pv->load_fs (pv->dir_prefix() + "-full.fs", true) == false) {
      vul_printf (vcl_cerr, "\nError: can't load %s-full.fs.\n", pv->dir_prefix());
      dbmsh3d_cmd_gui() = 0;
    }
    else {
      pv->fs_mesh()->compute_median_A122_dist ();
      pv->add_perturb (dbmsh3d_cmd_ptb());

      if (dbmsh3d_cmd_ofile() && dbmsh3d_cmd_p3d_ofile()==NULL)
        pv->save_p3d (pv->dir_prefix() + ".pp.p3d");
      if (dbmsh3d_cmd_gui())
        _root->addChild (pv->vis_ptset());
    }
  }
  //###########################################################################
  //###########################################################################
  //-bsphr 1: Adding BndSphere.  
  if (dbsk3d_cmd_bsphr()==1 || dbsk3d_cmd_spd()==1 || dbsk3d_cmd_seg()==1 || dbsk3d_cmd_ifgp()==1 || 
      dbsk3d_cmd_reg()==1 || dbsk3d_cmd_isc()==1 || dbsk3d_cmd_ms()==1 || 
      dbsk3d_cmd_trans()==1 || dbsk3d_cmd_transg()==1 || dbsk3d_cmd_sgsm()==1 || 
      dbsk3d_cmd_all()==1) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      if (pv->load_points_files())
        dbmsh3d_pro_contiune = true;
      else {
        dbmsh3d_cmd_gui() = 0;
        vcl_cout << "\ndbsk3d_cmdproc_execute (bsphr): Error loading " << pv->dir_prefix() + "\n";
        return _root;
      }
    }

    //save the # of original generators
    pv->set_n_orig_gene (pv->mesh()->vertexmap().size());
    //-sphrr 5: bounding sphere radius ratio.
    //-sphsr 1: bounding sphere sample ratio.
    pv->add_bndsphere (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr());
    pv->save_p3d_with_sphere ();

    if (dbsk3d_cmd_bsphr()==1 && dbmsh3d_cmd_gui())
      _root->addChild (pv->vis_ptset());
  }
  //###########################################################################
  // Run QHull.
  if (dbsk3d_cmd_spd()==1 || dbsk3d_cmd_seg()==1 || dbsk3d_cmd_ifgp()==1 || dbsk3d_cmd_reg()==1 || 
      dbsk3d_cmd_isc()==1 || dbsk3d_cmd_ms()==1 || 
      dbsk3d_cmd_trans()==1 || dbsk3d_cmd_transg()==1 || dbsk3d_cmd_sgsm()==1 || 
      dbsk3d_cmd_all()==1) {
    vcl_string run_qhull_command_para = "qvoronoi s o Fv < ";
    run_qhull_command_para += pv->dir_prefix() + "-sphere.p3d";
    run_qhull_command_para += " > ";
    run_qhull_command_para += pv->dir_prefix() + "-sphere.vor";
    vcl_cout << "\nRun QHull: "<< run_qhull_command_para << vcl_endl;
    system (run_qhull_command_para.c_str());

    if (dbsk3d_cmd_spd())
      dbmsh3d_cmd_gui() = 0;
  }
  //###########################################################################
  // Shock Recovery from QHull.
  //-qhfs 1:
  if (dbsk3d_cmd_qhfs()==1 || dbsk3d_cmd_seg()==1 || dbsk3d_cmd_ifgp()==1 || dbsk3d_cmd_reg()==1 || 
      dbsk3d_cmd_isc()==1 || dbsk3d_cmd_ms()==1 || 
      dbsk3d_cmd_trans()==1 || dbsk3d_cmd_transg()==1 || dbsk3d_cmd_sgsm()==1 || 
      dbsk3d_cmd_all()==1) {
   if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();

      if (pv->load_points_files() == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (qhfs): Error loading " << pv->dir_prefix() << "\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }

      pv->set_n_orig_gene (pv->mesh()->vertexmap().size());
      assert (pv->has_fs_mesh() == false);      
    }

    pv->fs_recover_qhull ();
    dbmsh3d_pro_contiune = true;
    
    //-file 0,1,2: remove the temporary Voronoi files.
    if (dbmsh3d_cmd_ofile() < 3) { 
      vcl_string delete_command_para = "del ";
      delete_command_para += pv->dir_prefix() + "-sphere.p3d";
      vcl_cout << "Delete QHull File: "<< delete_command_para << vcl_endl;
      system (delete_command_para.c_str());

      delete_command_para = "del ";
      delete_command_para += pv->dir_prefix() + "-sphere.vor";
      system (delete_command_para.c_str());
    }
    //-file 2: save prefix-full.fs.
    if (dbmsh3d_cmd_ofile() > 1) 
      pv->save_fs (pv->dir_prefix() + "-full.fs");
    
    if (dbsk3d_cmd_qhfs()==1 && dbmsh3d_cmd_gui()) {
      //View (0, 0): Draw input point cloud. 
      vis = pv->vis_ptset ();
      _root->addChild (vis);

      //View (dx, 0): Draw full scaffold.
      vis = pv->vis_fs_mesh (true, true);
      pv->compute_dx_dy_dz (2.5f);
      _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
    }
  }
  //###########################################################################
  //-seg 1: Run Surface Segregation.   
  //-seg 2: surface segregation continue from prefix-full.fs produced by -qhfs !=0.
  if (dbsk3d_cmd_seg()!=0 || dbsk3d_cmd_reg()==1 || dbsk3d_cmd_reg()==2 || 
      dbsk3d_cmd_isc()==1 || dbsk3d_cmd_isc()==2 || 
      dbsk3d_cmd_ms()==1 || dbsk3d_cmd_ms()==2 || 
      dbsk3d_cmd_trans()==1 || dbsk3d_cmd_trans()==2 || 
      dbsk3d_cmd_transg()==1 || dbsk3d_cmd_transg()==2 || 
      dbsk3d_cmd_sgsm()==1 || dbsk3d_cmd_sgsm()==2 || 
      dbsk3d_cmd_all()==1 || dbsk3d_cmd_all()==2) {    
    if (dbmsh3d_pro_contiune == false) {
      //continue from prefix-full.fs produced by -qhfs !=0.
      pv->reset_mesh();
      pv->reset_fs_mesh();      
      if (pv->load_fs (pv->dir_prefix() + "-full.fs") == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (seg): Error loading " << pv->dir_prefix() + "-full.fs.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }
    }

    //############ -seg 1,2: Greedy Surface Segregation. ############ 
    //Prioritize the initial triangles (if it exists) in reconstruction.
    //-segop 1: prioritize faces with flag 1.
    pv->set_dir_prefix (pv->dir_prefix());
    if (pv->load_faces_files()) {
      vul_printf (vcl_cerr, "\n  %d initial mesh faces loaded from %s.\n", 
                  pv->mesh()->facemap().size(), pv->dir_prefix().c_str());
      pv->mesh()->IFS_to_MHE ();
    }

    //The 3DIM shock segregation.
    //-msr: max_th_ratio 
    //-segop: topo. option (0: 2-manifold, 1: rich mesh, 2: non-2-manifold).
    //-gsa 1: re-assigne lost genes to shocks.
    pv->fs_segre()->run_surface_segre (dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, 
                                       (dbmsh3d_cmd_percent2()/100), dbsk3d_cmd_gsa());

    double stop_rth = pv->fs_segre()->stop_rth();
    dbmsh3d_pro_contiune = true;

    //-fileo 1: save the reconsturcted surface to .ply file.
    if (dbmsh3d_cmd_fileo()==1) 
      pv->save_ply (pv->dir_prefix() + "-surface.ply", true);
    //-fileo 2: save the reconsturcted surface to .ply2 file
    else if (dbmsh3d_cmd_fileo()==2)
      pv->save_ply2 (pv->dir_prefix() + "-surface.ply2");      

    //-file 2: save pruned shock file.
    if (dbmsh3d_cmd_ofile()>1) 
      pv->save_fs (pv->dir_prefix() + "-seg-pruned.fs");
    //-file 3: If there are unmeshed surface points, save them to -surfpt.p3d file.
    if (dbmsh3d_cmd_ofile()==3 && pv->mesh()->count_unmeshed_pts() != 0)        
      pv->save_surfpt (pv->dir_prefix() + "-surfpt.p3d");
    
    if (dbmsh3d_cmd_gui()) {
      ///pv->mesh()->build_face_IFS();
      pv->compute_vertex_node_size();
     
      //-all : View (0, 0): Object Surface Mesh
      if (dbsk3d_cmd_reg()==0) { 
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

      //Additional visualization for surface segregation.        
      if (dbsk3d_cmd_seg()==1) { 
        //-idv 1: draw each gene individually.
        if (dbmsh3d_cmd_idv()==1) 
          _root->addChild (pv->vis_ptset_idv());        

        //-bnd 2: draw only non-1-ring genes individually.
        if (dbsk3d_cmd_bnd()==2)
          _root->addChild (pv->vis_vertices_of_holes ());

        //Extend the bounding box to be the size of the bounding sphere.
        //-sphrr: bounding sphere radius ratio.
        pv->compute_dx_dy_dz (dbsk3d_cmd_sphrr()/1.7);

        //View (dx, 0): Draw remaining shocks
        if (dbsk3d_cmd_vs()==1) { //-vs 1: Draw remaining shock-sheet-elms:          
          vis = pv->vis_fs_mesh (true, false, dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
        }
        else if (dbsk3d_cmd_vs()==2) { //-vs 2: Draw remaining A13-fs_edges.           
          vis = draw_fs_mesh_links (pv->fs_mesh()->edgemap(), 0, stop_rth, 
                                    false, true, 1, true, pv->user_defined_class_);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
        } 
        else if (dbsk3d_cmd_vs()==3) { //-vs 3: Draw remaining A13-fs_edges, including non_A13-2 ones.          
          vis = draw_fs_mesh_links (pv->fs_mesh()->edgemap(), 0, SEG_NON_A13_2_COST, 
                                    false, true, 1, true, pv->user_defined_class_);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
        }

        if (dbmsh3d_cmd_n()==1 || dbmsh3d_cmd_n()==2 || dbmsh3d_cmd_n()==3) {
          //View (0, 0): Draw remaining shocks around the holes.
          // -n 1: draw only related shock sheet elements with any generator at hole.
          // -n 2: draw only related shock sheet elements with both generators at hole.
          // -n 3: draw only related shock link elements.
          _root->addChild (pv->vis_shocks_of_holes (dbmsh3d_cmd_n()));
        }

        //Draw bnd-to-shock assignment.
        if (dbmsh3d_cmd_n2() > 1) {
          //Enlarge the gene size if gene association is visualized.
          if (dbmsh3d_cmd_n2() > 1 && pv->pt_size_==1)
            pv->pt_size_ = 2;            

          vis = pv->vis_mesh_pts_edges();
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));

          //-n2 5: Show all P-G, L-G, FV-G assignments.
          //-n2 2: Show patch-gene assignments.
          if (dbmsh3d_cmd_n2() == 2 || dbmsh3d_cmd_n2() == 5) {
            vis = draw_fs_patch_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
            _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
          }
          //-n2 3: show link-gene assignments.
          if (dbmsh3d_cmd_n2() == 3 || dbmsh3d_cmd_n2() == 5) {
            vis = draw_fs_link_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
            _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
          }
          //-n2 4: show node-gene assignments.
          if (dbmsh3d_cmd_n2() == 4 || dbmsh3d_cmd_n2() == 5) {
            vis = draw_fs_node_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
            _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
          }
        }
      }
    }
  }
  //###########################################################################
  //-ifgp 1: Surface In-flow Generator Propagation and Meshing
  if (dbsk3d_cmd_ifgp()!=0) {
    //-ifgp 2: 
    if (dbmsh3d_pro_contiune == false) {
      //continue from prefix-full.fs produced by -qhfs !=0.
      pv->reset_mesh ();
      pv->reset_fs_mesh(); 
      pv->load_fs (pv->dir_file()+"-full.fs");
    }

    pv->fs_mesh()->build_IFS_mesh ();
    pv->compute_vertex_node_size ();

    pv->fs_ifgp()->compute_surface_inflow ();

    //-v 1: Display the in-flow gene propagation result
    if (dbmsh3d_cmd_gui() && dbmsh3d_cmd_v()==1) {
      //Visualize the input pt cloud.
      if (dbmsh3d_cmd_idv()==1) //-idv 1: draw each gene individually.
        vis = pv->vis_ptset_idv();  
      else 
        vis = draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_);
      _root->addChild (vis);

      //Visualize the surface inflow
      vis = vis_fs_surface_inflow_Ls (pv->fs_mesh());
      _root->addChild (vis);

      //Visualize all Gabriel edges and A12-2 sources.
      vis = vis_fs_gabriel_graph (pv->fs_mesh(), dbmsh3d_cmd_cw(), SbColor (0.0f, 0.0f, 0.8f));
      _root->addChild (vis);

      vis = vis_fs_surface_inflow_Ps (pv->fs_mesh(), pv->m_edge_width_, pv->cube_size_);
      _root->addChild (vis);
    }
    //-v 2: Another old version to display in/out flow result
    if (dbmsh3d_cmd_gui() && dbmsh3d_cmd_v()==2) {
      //Compute the shifting (dx, dy, dz) of the display.
      pv->compute_dx_dy_dz (dbsk3d_cmd_sphrr()/1.7);
      vis = pv->vis_ptset();
      _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));

      //-n2: Color the fs_vertices by the # of associated genes.
      //-n2: n_genes_th, default: 10.
      if (dbmsh3d_cmd_n2() > 3) {
        unsigned int n_genes_th = dbmsh3d_cmd_n2();
        if (n_genes_th < 3)
          n_genes_th = 10;
        vis = vis_fs_nodes_color_nG (pv->fs_mesh(), dbmsh3d_cmd_rc(), n_genes_th, 
                                     pv->user_defined_class_);
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
      }

      //-n3: Color the fs_edges by the # of associated genes.
      //-n3: n_genes_th, default: 10.
      if (dbmsh3d_cmd_n3() > 2) {
        unsigned int n_genes_th = dbmsh3d_cmd_n3();
        if (n_genes_th < 3)
          n_genes_th = 10;
        //-v: option. 0: show all, 1: show only type I, 2: show only type II, 
        //            3: show only type III, 4: show only type I and II.
        vis = vis_fs_links_color_nG (pv->fs_mesh(), dbmsh3d_cmd_cw(), n_genes_th, 
                                     dbmsh3d_cmd_v(), pv->user_defined_class_);
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
      }
    }
    
    //Meshing using the In-flow gene. propagation result.
    pv->fs_ifgp()->build_surface_mesh ();
    
    //Remove extraneous non-2-manifold (1,1,3) and (1,3,3) faces.
    pv->mesh()->IFS_to_MHE ();
    remove_extraneous_faces (pv->mesh());
    pv->mesh()->build_face_IFS ();

    //Save the reconstructed surface mesh.
    //-fileo 1: save the reconsturcted surface to .ply file.
    if (dbmsh3d_cmd_fileo()==1) 
      pv->save_ply (pv->dir_prefix() + "-ifgp.ply", true);
    //-fileo 2: save the reconsturcted surface to .ply2 file
    else if (dbmsh3d_cmd_fileo()==2)
      pv->save_ply2 (pv->dir_prefix() + "-ifgp.ply2");  

    //-v 0: display the surface meshing result
    if (dbmsh3d_cmd_gui() && dbmsh3d_cmd_v()==0) {
      //Color the resulting mesh by edge topology.
      _root->addChild (pv->vis_mesh_options (0, dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()!=0));
    }
  }
  //###########################################################################
  //-reg 1: Shock Regularization (Early Splice Transforms).
  if (dbsk3d_cmd_reg()!=0 || (dbsk3d_cmd_isc()>=1 && dbsk3d_cmd_isc()<=3) || 
      (dbsk3d_cmd_ms()>=1 && dbsk3d_cmd_ms()<=3) || 
      (dbsk3d_cmd_trans()>=1 && dbsk3d_cmd_trans()<=3) || 
      (dbsk3d_cmd_transg()>=1 && dbsk3d_cmd_transg()<=3) || 
      (dbsk3d_cmd_sgsm()>=1 && dbsk3d_cmd_sgsm()<=3) || 
      (dbsk3d_cmd_all()>=1 && dbsk3d_cmd_all()<=3)) {
    //-reg 3: continue from prefix-seg-pruned.fs after -seg !=0.
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();      

      if (pv->load_fs (pv->dir_prefix() + "-seg-pruned.fs") == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (reg): Error loading " << pv->dir_prefix() + "-seg-pruned.fs.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }
      //Load in the surface mesh prefix-surface.
      if (pv->load_faces_files (pv->dir_prefix() + "-surface") == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (reg): Error loading " << pv->dir_prefix() + "-surface.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }
      pv->mesh()->IFS_to_MHE();
      pv->mesh()->build_face_IFS();
    }
 
    //-a122 1: trim A12-2 FF's (Def.1).
    if (dbsk3d_cmd_trim_a122())
      pv->fs_regul()->trim_bnd_A122_FFs (dbmsh3d_cmd_msr());
    pv->fs_ss()->build_fs_sheet_set ();  

    //-reg : Shock regularization. 
    //-gsa 1: re-assigne lost genes to shocks.
    pv->fs_regul()->run_shock_regul (dbsk3d_cmd_regth(), dbsk3d_cmd_gsa()!=0);
    dbmsh3d_pro_contiune = true;

    if (dbmsh3d_cmd_ofile()>1)
      pv->save_fs (pv->dir_prefix() + "-reg-pruned.fs");

    if (dbsk3d_cmd_reg()!=0 && dbmsh3d_cmd_gui()) {  
      pv->compute_dx_dy_dz ();
      
      //View (dx, 0): draw regularized shocks
      // Note: here many shock sheets are unbounded.
      //-n 2: Visualize bnd-shock-asso for shocks with cost < than -regth
      pv->compute_vertex_node_size ();     
      double regth = 0;
      if (dbmsh3d_cmd_n() == 2)
        regth = dbsk3d_cmd_regth();
      //-inf 1: show unbounded sheets.
      //-idv : draw individual elements.
      vis = draw_fs_sheet_set (pv->fs_ss(), dbmsh3d_cmd_inf(), 
                               dbmsh3d_cmd_idv(), pv->user_defined_class_);
      _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 

      if (dbsk3d_cmd_bnd()) { //-bnd 1: Draw the boundary generators. 
        //Enlarge the gene size if gene association is visualized.
        if (dbmsh3d_cmd_n2() > 1 && pv->pt_size_==1)
          pv->pt_size_ = 2;            
        
        //View (dx, 0): draw boundary mesh or pts.      
        if (dbsk3d_cmd_bnd() == 1) {           
          //-bnd 1: draw input point cloud
          if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
            vis = pv->vis_mesh_pts_edges();
          else //draw each gene individually to enable getInfo.          
            vis = pv->vis_ptset_idv();
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));           
        }
        else if (dbsk3d_cmd_bnd() == 2) { 
          //-bnd 2: draw boundary mesh with transparency
          if (pv->m_transp_ == 0.0f)
            pv->m_transp_ = 0.5;
          vis = pv->vis_mesh (false, dbmsh3d_cmd_shapehints());
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
        }

        //-n2 5: Show all P-G, L-G, FV-G assignments.
        //-n2 2: Show patch-gene assignments.
        if (dbmsh3d_cmd_n2() == 2 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_patch_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }
        //-n2 3: show link-gene assignments.
        if (dbmsh3d_cmd_n2() == 3 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_link_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }
        //-n2 4: show node-gene assignments.
        if (dbmsh3d_cmd_n2() == 4 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_node_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }

        //draw unassigned genes in cube.
        vis = pv->vis_unasgn_genes(dbmsh3d_cmd_rc());
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
      }
    }
  }
  //###########################################################################
  //-isc 1: Identify Inside Shock Component: Bounding-box pruning and component extraction.
  if (dbsk3d_cmd_isc()!=0 || (dbsk3d_cmd_ms()>=1 && dbsk3d_cmd_ms()<=4) ||     
      (dbsk3d_cmd_trans()>=1 && dbsk3d_cmd_trans()<=4) || 
      (dbsk3d_cmd_transg()>=1 && dbsk3d_cmd_transg()<=4) || 
      (dbsk3d_cmd_sgsm()>=1 && dbsk3d_cmd_sgsm()<=4) || 
      (dbsk3d_cmd_all()>=1 && dbsk3d_cmd_all()<=4)) {
    //-isc 4: continue from prefix-reg-pruned.fs.
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();

      if (pv->load_fs (pv->dir_prefix() + "-reg-pruned.fs") == false) {
        vcl_cout << "\n-isc: Error loading " << pv->dir_prefix() + "-reg-pruned.fs.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }      
      //Load in the surface mesh prefix-surface.
      if (pv->load_faces_files (pv->dir_prefix() + "-surface") == false) {
        vcl_cout << "\n-isc: Error loading " << pv->dir_prefix() + "-surface.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }
      pv->mesh()->IFS_to_MHE();
      pv->mesh()->build_face_IFS();

      //Recover the pv->fs_ss_
      pv->fs_ss()->build_fs_sheet_set (); 
    }

    //-rmin : Temporary rmin pruning, def 1.0
    //-gsa 1: re-assigne lost genes to shocks.
    pv->run_rmin_trim_xforms (dbsk3d_cmd_rmin(), dbsk3d_cmd_gsa()!=0);

    //-scp : Shock sheet compactness pruning iteration(s) (def: 0).
    //-scpth : compactness threshold for boundary shock sheets (def 0.1).
    //-gsa 1: re-assigne lost genes to shocks.
    if (dbsk3d_cmd_scp() > 0)
      pv->run_compactness_trim_xforms (dbsk3d_cmd_scp(), dbsk3d_cmd_scpth(), dbsk3d_cmd_gsa()!=0);

    //-boxr : Enlarged bounding box pruning
    //-gsa 1: re-assigne lost genes to shocks.
    pv->run_bbox_pruning (dbmsh3d_cmd_boxr(), dbsk3d_cmd_gsa()!=0);

    //-shole 1: Remove shocks of boundary holes
    if (dbsk3d_cmd_shole() == 1)
      pv->prune_shocks_of_holes ();
      
    //Re-build the sheet components.
    // (After the rmin pruning, a component can become two components).
    pv->fs_mesh()->detect_valid_FE_type ();
    pv->fs_ss()->clear();
    pv->fs_ss()->build_fs_sheet_set (); 

    //Re-run shock regularization.  
    //-gsa 1: re-assigne lost genes to shocks.
    pv->fs_regul()->run_shock_regul (dbsk3d_cmd_regth(), dbsk3d_cmd_gsa()!=0);

    //-comp : Extract the largest (i-th) inside shock component (delete all non-component shocks)
    // def: 0, -1: keep all MS compoents.
    vcl_set<int> ith_comp_list;
    ith_comp_list.insert (dbsk3d_cmd_comp());

    //For david_sub7
    /*ith_comp_list.insert (0);
    ith_comp_list.insert (14);
    ith_comp_list.insert (7);
    ith_comp_list.insert (15);
    ith_comp_list.insert (4);
    ith_comp_list.insert (1);*/

    //For david_sub6
    /*ith_comp_list.insert (0);
    ith_comp_list.insert (3);*/

    //For david_sub8
    /*ith_comp_list.insert (0);
    ith_comp_list.insert (4);
    ith_comp_list.insert (8);
    ith_comp_list.insert (1);
    ith_comp_list.insert (9);
    ith_comp_list.insert (10);
    ith_comp_list.insert (11);*/

    //-gsa 1: re-assigne lost genes to shocks.
    bool result = pv->determine_inside_shock_comp (ith_comp_list, dbsk3d_cmd_gsa()!=0);

    if (result) {
      dbmsh3d_pro_contiune = true;
      pv->save_fs (pv->dir_prefix() + "-isc-pruned.fs");
    }
    else {
      //Stop all subsequent processing.
      dbmsh3d_pro_contiune = false;
      dbsk3d_cmd_ms() = 0;
      dbsk3d_cmd_trans() = 0;
      dbsk3d_cmd_transg() = 0;
      dbsk3d_cmd_sgsm() = 0;
      dbsk3d_cmd_all() = 0;
      vul_printf (vcl_cerr, "\n\n  !!  No remaining shocks to process  !!  \n\n\n");
    }

    if (dbmsh3d_pro_contiune && dbsk3d_cmd_isc()!=0 && dbmsh3d_cmd_gui()) {
      pv->compute_vertex_node_size ();
      pv->compute_dx_dy_dz ();

      //View (dx, 0): draw regularized shocks
      //-regth: Visualize bnd-shock-asso for shocks with cost < than regth
      //-idv : draw individual elements.
      vis = draw_fs_sheet_set (pv->fs_ss(), dbmsh3d_cmd_inf(), 
                               dbmsh3d_cmd_idv(), pv->user_defined_class_);
      if (dbmsh3d_cmd_dy() >= 0)
        _root->addChild (shift_output (vis, pv->view_dx_, pv->view_dy_, 0)); 
      else
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 
      
      if (dbsk3d_cmd_bnd()) { //-bnd : Draw the boundary generators. 
        //View (dx, 0): draw boundary mesh or pts.      
        if (dbsk3d_cmd_bnd() == 1) { //-bnd 1: draw input point cloud
          if (dbmsh3d_cmd_idv() == 0)
            vis = pv->vis_ptset();
          else //draw each gene individually for getInfo.          
            vis = pv->vis_ptset_idv();
        }
        else if (dbsk3d_cmd_bnd() == 2) { //-bnd 2: draw mesh edges
          //Enlarge the gene size if gene association is visualized.
          if (dbmsh3d_cmd_idv() == 0) //draw the whole point set together.
            vis = pv->vis_mesh_pts_edges();
          else //draw each gene individually to enable getInfo.          
            vis = pv->vis_ptset_idv();
        }
        else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh with transparency
          if (pv->m_transp_ == 0.0f)
            pv->m_transp_ = 0.5;
          vis = pv->vis_mesh (false, dbmsh3d_cmd_shapehints());          
        }
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0)); 

        //-n2 5: Show all P-G, L-G, FV-G assignments.
        //-n2 2: Show patch-gene assignments.
        if (dbmsh3d_cmd_n2() == 2 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_patch_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }
        //-n2 3: show link-gene assignments.
        if (dbmsh3d_cmd_n2() == 3 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_link_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }
        //-n2 4: show node-gene assignments.
        if (dbmsh3d_cmd_n2() == 4 || dbmsh3d_cmd_n2() == 5) {
          vis = draw_fs_node_to_gene (pv->fs_mesh(), dbmsh3d_cmd_inf()!=0);
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }

        if (dbsk3d_cmd_bnd() != 1) {
          //draw unassigned genes in cube.
          vis = pv->vis_unasgn_genes(dbmsh3d_cmd_rc());
          _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
        }
      }
    }
  }
  //###########################################################################
  //-ms 1: Coarse-scale Medial Scaffold Computation.
  if (dbsk3d_cmd_ms()!=0 || 
      (dbsk3d_cmd_trans()>=1 && dbsk3d_cmd_trans()<=5) || 
      (dbsk3d_cmd_transg()>=1 && dbsk3d_cmd_transg()<=5) || 
      (dbsk3d_cmd_sgsm()>=1 && dbsk3d_cmd_sgsm()<=5) || 
      (dbsk3d_cmd_all()>=1 && dbsk3d_cmd_all()<=5)) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();      

      if (pv->load_fs (pv->dir_prefix() + "-isc-pruned.fs") == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (ms): Error loading " << pv->dir_prefix() + "-isc-pruned.fs.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }      
      //Load in the surface mesh prefix-surface.
      if (pv->load_faces_files (pv->dir_prefix() + "-surface") == false) {
        vcl_cout << "\ndbsk3d_cmdproc_execute (ms): Error loading " << pv->dir_prefix() + "-surface.\n";
        dbmsh3d_cmd_gui() = 0;
        return _root;
      }
      pv->mesh()->IFS_to_MHE();
      pv->mesh()->build_face_IFS();
    }

    //Build Medial Scaffold hypergraph.
    //-ssopt : shock sheet topology option for building ms_hypg (def 0).
    //  0: do not allow shock sheet to have 2+ rib boundary.
    //     outside shocks with such topology will be deleted.
    //  1: allow a general case that shock sheet can have 2+ rib boundary.
    bool result = pv->build_ms_hypg (dbsk3d_cmd_ssopt());

    if (result) {      
      dbmsh3d_pro_contiune = true;

      //Save output prefix.cms and prefix.fs files.
      if (dbmsh3d_cmd_ofile()>0) {
        pv->save_fs ();
        pv->save_cms ();
      }

      pv->build_ms_graph_sa_from_cms ();

      //-ofile 2: save prefix.sg.
      if (dbmsh3d_cmd_ofile()>1)
        pv->save_sg ();

      if (dbmsh3d_cmd_gui()) {
        pv->compute_dx_dy_dz ();
        pv->compute_vertex_node_size ();
        //-r (def: 0.03) ball radius
        if (dbmsh3d_cmd_r() != 0.03f)
          pv->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();

        //-all : View (-dx, 0): Medial Scaffold Hypergraph.
        vis = pv->vis_ms_hypg();
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0));
        
        //View (dx, 0): draw boundary mesh or pts.      
        if (dbsk3d_cmd_bnd() == 1) { //-bnd 1: draw input point cloud
          if (dbmsh3d_cmd_idv() == 0)
            vis = pv->vis_ptset();
          else //draw each gene individually to enable getInfo.          
            vis = pv->vis_ptset_idv();
        }
        if (dbsk3d_cmd_bnd() == 2) {//-bnd 2: draw mesh edges
          //Enlarge the gene size if gene association is visualized.
          if (dbmsh3d_cmd_idv() == 0)
            vis = pv->vis_mesh_pts_edges();
          else //draw each gene individually to enable getInfo.          
            vis = pv->vis_ptset_idv();
        }
        else if (dbsk3d_cmd_bnd() == 3) { //-bnd 3: draw boundary mesh with transparency
          if (pv->m_transp_ == 0.0f)
            pv->m_transp_ = 0.5;
          vis = pv->vis_mesh (false, dbmsh3d_cmd_shapehints());          
        }
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0)); 

        if (dbmsh3d_cmd_gui()==2) {
          //View (0, dy): fs_faces colored by their radii.
          vis = pv->vis_fs_mesh_color_by_radius ();
          _root->addChild (shift_output (vis, 0, pv->view_dy_, 0));

          if (dbsk3d_cmd_ms()!=0) {
            //View (-dx, dy): Scaffold Graph.
            vis = pv->vis_ms_graph ();
            _root->addChild (shift_output (vis, -pv->view_dx_, pv->view_dy_, 0));
            vis = pv->vis_mesh_pts_edges ();
            _root->addChild (shift_output (vis, -pv->view_dx_, pv->view_dy_, 0));
          }
          else { //For dbsk3d_cmd_all()
            //View (-2dx, 0): Scaffold Graph.
            vis = pv->vis_ms_graph ();
            _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), 0, 0));
          }
        }
      }
    }
  }
  //###########################################################################
  //-trans 1: Medial Scaffold Hypergraph Transition Regularization.
  if (dbsk3d_cmd_trans()!=0 ||     
      (dbsk3d_cmd_transg()>=1 && dbsk3d_cmd_transg()<=6) || 
      (dbsk3d_cmd_sgsm()>=1 && dbsk3d_cmd_sgsm()<=6) || 
      (dbsk3d_cmd_all()>=1 && dbsk3d_cmd_all()<=6)) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      pv->load_cms ();
    }
    
    //Draw the medial scaffold before transition regularization.
    if (dbmsh3d_pro_contiune==false && dbmsh3d_cmd_gui()) {
      pv->compute_dx_dy_dz ();
      pv->compute_vertex_node_size ();

      if (dbmsh3d_pro_contiune==false) { 
        //View (-dx, 0): Medial Scaffold Before Transition.
        vis = pv->vis_ms_hypg ();
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0));
        vis = pv->vis_ptset();
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0));
      }
    }

    //Splice xform onboundary fine-scale MS element involving in 2+ shock transforms.
    ///pv->splice_bnd_ms_elm_2_xforms ();

    //Perform MS hypergraph transition regularization.
    //-tab (def 50): Remove shock tabs with > # generators.
    //-cc (def 15): Remove shock curve with > # fs_edges.
    //-a5 (def 30): Remove A5 with > # A5 fs_edges.
    //-gsa 1: re-assigne lost genes to shocks.
    //-n1 : default debug id, if it's not 1.
    if (dbmsh3d_cmd_n1() == 1)
      dbmsh3d_cmd_n1() = -1;
    pv->perform_ms_trans_regul (dbsk3d_cmd_tab(), dbsk3d_cmd_cc(), dbsk3d_cmd_a5(), dbsk3d_cmd_gsa()!=0, dbmsh3d_cmd_n1());
    
    //Build scaffold_graph_stand_alone again after the transition.
    pv->build_ms_graph_sa_from_cms ();
    dbmsh3d_pro_contiune = true;

    pv->set_dir_file (pv->dir_prefix() + "-ht");
    if (dbmsh3d_cmd_ofile()) {
      pv->save_cms ();
      pv->save_fs ();
      pv->save_sg ();
    }

    if (dbmsh3d_cmd_gui()) {
      //-all : View (-2dx, dy): Medial Scaffold Hypergraph After Regularization.
      if (pv->ms_vis_param_.sheets_ctable_.size() == 0)
        vis = pv->vis_ms_hypg ();
      else
        vis = pv->vis_ms_hypg_ctable ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), 0, 0));

      vis = pv->vis_ptset ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), 0, 0));
    }
  }
  //###########################################################################
  //-transv : Medial Scaffold Hypergraph Transition Vis. for Debug.
  if (dbsk3d_cmd_transv()!=0) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      if (pv->load_cms()) 
        dbmsh3d_pro_contiune = true;
      else {
        dbmsh3d_cmd_gui() = 0;
        vcl_cout << "\ndbsk3d_cmdproc_execute (transv): Error loading " << pv->dir_prefix() + ".\n";
        return _root;
      }
    }

    //Draw the medial scaffold before transition regularization.
    pv->compute_dx_dy_dz ();
    pv->compute_vertex_node_size ();
    //-r (def: 0.03) ball radius
    if (dbmsh3d_cmd_r() != 0.03f)
      pv->ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();

    if (dbsk3d_cmd_transv() == 1) {
      //-transv 1: visualize all fs_faces and fs_edges involving in two 
      //           or more shock transforms. They should be removed in high priority.
      //-n : specify # of trans threshold (e.g. 2 for more than 2 transitions).
      //-id 6: show ids.
      vis = vis_SH_elms_n_SCxforms (pv->ms_hypg(), dbmsh3d_cmd_n(),
                                    pv->ms_vis_param_.curve_width_,
                                    pv->ms_vis_param_.ball_size_,
                                    dbmsh3d_cmd_id()>5);
      _root->addChild (vis);

    }
    else if (dbsk3d_cmd_transv() == 2) {
      //-transv 2: visualize A5 transition shock transform process.
      //-n : A5 MC id.
      //-id 6: show id for U, M, V.
      vis = vis_A5_curve_contract_xform (pv->ms_hypg(), dbmsh3d_cmd_n(), 
                                         pv->ms_vis_param_.curve_width_,
                                         pv->ms_vis_param_.ball_size_,
                                         dbmsh3d_cmd_id()>5);
      _root->addChild (vis);      
    }
    else if (dbsk3d_cmd_transv() == 3) {
      //-transv 3: visualize A12A3-I transition shock transform process.
      //-n : A12A3-I MC id.
      //-id 6: show id for U, M, A, B, C.
      vis = vis_A12A3I_curve_contract_xform (pv->ms_hypg(), dbmsh3d_cmd_n(), 
                                             pv->ms_vis_param_.curve_width_,
                                             pv->ms_vis_param_.ball_size_,
                                             dbmsh3d_cmd_id()>5);
      _root->addChild (vis);      
    }

    //-v : visualization option: 0-random color table, 1-single color.
    //     -v 2 only show ms_graph, -v 3: not showing anything.
    //-cseed: color seed (default DBMSH3D_SHEET_COLOR_SEED: 622).
    //-idv 1: draw individual fs_faces 
    _root->addChild (pv->vis_ms_hypg(dbmsh3d_cmd_v(), dbmsh3d_cmd_cseed(), dbmsh3d_cmd_idv()!=0));
    ///_root->addChild (pv->vis_ms_graph());
    _root->addChild (pv->vis_ptset());
  }
  //###########################################################################
  //-smrib : Medial Scaffold Rib Curve Smoothing.
  if (dbsk3d_cmd_smrib()!=0) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      if (pv->load_cms() == false) {
        dbmsh3d_cmd_gui() = 0;
        vcl_cout << "\ndbsk3d_cmdproc_execute (transv): Error loading " << pv->dir_prefix() + ".\n";
        return _root;
      }
    }

    //Draw the medial scaffold before transition regularization.
    if (dbmsh3d_pro_contiune==false && dbmsh3d_cmd_gui()) {
      pv->compute_dx_dy_dz ();
      pv->compute_vertex_node_size ();

      if (dbmsh3d_pro_contiune==false) { 
        //View (-dx, 0): Medial Scaffold Before Transition.
        vis = pv->vis_ms_hypg ();
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0));
        vis = pv->vis_ptset();
        _root->addChild (shift_output (vis, -pv->view_dx_, 0, 0));
      }
    }

    //Perform MS hypergraph transition regularization.
    //-n : # of smoothing iterations (def. 1).
    int n_step = dbmsh3d_cmd_n();
    if (n_step < 0)
      n_step = 1;
    //-l : default step size psi (def 1).
    float DCS_psi = dbmsh3d_cmd_len();
    //-thr : tiny rib length ratio (def. 0.001).
    float rib_ratio = 0.001;
    if (dbmsh3d_cmd_thr() != 5)
      rib_ratio = dbmsh3d_cmd_thr();
    pv->smooth_ms_hypg_ribs (DCS_psi, n_step, rib_ratio);
    
    //Build scaffold_graph_stand_alone again after the transition.
    pv->build_ms_graph_sa_from_cms ();
    dbmsh3d_pro_contiune = true;

    pv->set_dir_file (pv->dir_file() + "-smrib");
    if (dbmsh3d_cmd_ofile()) {
      pv->save_cms ();
      pv->save_fs ();
      pv->save_sg ();
    }

    if (dbmsh3d_cmd_gui()) {
      //-all : View (-2dx, dy): Medial Scaffold Hypergraph After Regularization.
      if (pv->ms_vis_param_.sheets_ctable_.size() == 0)
        vis = pv->vis_ms_hypg ();
      else
        vis = pv->vis_ms_hypg_ctable ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), 0, 0));

      vis = pv->vis_ptset ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), 0, 0));
    }
  }
  //###########################################################################
  //-transg : Medial Scaffold Graph Transition Regularization.
  if (dbsk3d_cmd_transg()!=0 ||     
      (dbsk3d_cmd_sgsm()>=1 && dbsk3d_cmd_sgsm()<=7) || 
      (dbsk3d_cmd_all()>=1 && dbsk3d_cmd_all()<=7)) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      bool r = pv->load_meshes_files ();
      if (r == false)
        pv->load_points_files ();
      pv->load_sg ();
    }

    if (dbmsh3d_pro_contiune==false && dbmsh3d_cmd_gui()) {
      pv->compute_dx_dy_dz ();
      pv->compute_vertex_node_size ();
      //View (-2dx, dy): Medial Scaffold Graph Before Transition.
      vis = pv->vis_ms_graph_sa ();
      _root->addChild (shift_output (vis, float(-pv->view_dx_*2), pv->view_dy_, 0));
    }

    //-a5 (def 10): Run A5 Graph Transitions
    if (dbsk3d_cmd_a5() > 0.0f) {
      pv->proc_param_.trans_A5_th_ = dbsk3d_cmd_a5();
      pv->graph_trans_A5 ();
    }
    //-cc (def 15): Run curve contract transforms
    if (dbsk3d_cmd_cc() > 0.0f) {
      pv->graph_trans_A15 (dbsk3d_cmd_cc());
    }
    dbmsh3d_pro_contiune = true;

    if (dbmsh3d_cmd_ofile()) {
      vcl_string sg_file = pv->dir_file() + "-gt.sg";
      vul_printf (vcl_cerr, "\n  Saving the regularized medial scaffold graph to %s.\n", sg_file.c_str());
      pv->save_sg (sg_file);
    }
    pv->set_dir_file (pv->dir_file() + "-gt");

    if (dbmsh3d_cmd_valid()) {
      bool r = pv->sg_sa()->check_integrity ();
      if (r)
        vul_printf (vcl_cerr, "\n\n\n Validation Successfully completed.\n\n\n");
      else
        vul_printf (vcl_cerr, "\n\n\n Validation Failed!!\n\n\n");
    }

    if (dbmsh3d_pro_contiune==false && dbmsh3d_cmd_gui()) {
      //View (-2dx, 0): Scaffold Graph After Transition.
      vis = pv->vis_ms_graph_sa ();
      _root->addChild (shift_output (vis, float(-pv->view_dx_*2), 0, 0));
    }
  }
  //###########################################################################
  //-sgsm 1: Smooth the Mesial Scaffold Curves.
  if (dbsk3d_cmd_sgsm()!=0 || dbsk3d_cmd_all()!=0) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      bool r = pv->load_meshes_files (pv->dir_prefix());
      if (r == false)
        pv->load_points_files (pv->dir_prefix());
      pv->load_sg ();
    }

    if (dbmsh3d_pro_contiune==false && dbmsh3d_cmd_gui()) {
      pv->compute_dx_dy_dz ();
      pv->compute_vertex_node_size ();
      //View (-2dx, dy): Medial Scaffold Graph Before Smoothing.
      vis = pv->vis_ms_graph_sa ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*2), pv->view_dy_, 0));
    }

    //-smi: smoothing iterations. If smi == -1, make default = 5.
    if (dbmsh3d_cmd_smi() == -1)
      dbmsh3d_cmd_smi() = 5;

    //-sgsm 8 , -smi : Run Shock Curve Smoothing
    pv->proc_param_.n_smooth_ = dbmsh3d_cmd_smi();
    pv->ms_graph_sa_smooth ();

    if (dbmsh3d_cmd_ofile()) {
      char namebuf[128];
      sprintf (namebuf, "-sm%d.sg", dbmsh3d_cmd_smi());
      pv->save_sg (pv->dir_file() + namebuf);
    }

    if (dbmsh3d_cmd_gui()) {
      //-all : View (-3dx, 0): Scaffold Graph After Smoothing.
      vis = pv->vis_ms_graph_sa ();
      _root->addChild (shift_output (vis, (float)(-pv->view_dx_*3), 0, 0));
    }
  }
  //###########################################################################
  //-bktseg : Run Surface Reconstruction with Bucketing + Stitching
  if (dbsk3d_cmd_bktseg()) {
    //Store input points and reconstrcuted triangles.
    vcl_vector<vgl_point_3d<double> > input_pts;
    vcl_vector<vcl_vector<int> > finalM_faces;

    vcl_string xyz_file = pv->dir_prefix() + ".xyz";
    vcl_string p3d_file = pv->dir_prefix() + ".p3d";
    if (dbmsh3d_load_xyz (input_pts, xyz_file.c_str()))
      dbmsh3d_pro_contiune = true;
    else if (dbmsh3d_load_p3d (input_pts, p3d_file.c_str()))
      dbmsh3d_pro_contiune = true;

    if (dbmsh3d_pro_contiune == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cerr, "ERROR:\t File %s Not Found!\n", pv->dir_prefix());
    }
    else {      
      if (dbsk3d_cmd_bktseg()==1) {
        //-bktseg 1: run adaptive-bucketing.  
        vis = run_seg_adpt_bucketing (input_pts, finalM_faces, pv->dir_prefix(),
                                      dbmsh3d_cmd_npbkt(), dbmsh3d_cmd_dup()!=0,
                                      dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                      dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1); 
        _root->addChild (vis);
      }
      if (dbsk3d_cmd_bktseg()==2) 
        //-bktseg 2: run adaptive-bucketing with several run files.
        //-bktbr : box ratio 
        run_seg_adpt_bucketing_2 (input_pts, dbmsh3d_cmd_npbkt(), pv->dir_prefix(), 
                                dbmsh3d_cmd_dup()!=0, dbmsh3d_cmd_bktbr(), dbmsh3d_cmd_msr()); 
      else if (dbsk3d_cmd_bktseg()==3) 
        //-bktseg 3: original adpt-bucketing with a separate stitching file.
        run_seg_adpt_bucketing_3 (input_pts, dbmsh3d_cmd_npbkt(), pv->dir_prefix(), 
                                  dbmsh3d_cmd_dup()!=0, dbmsh3d_cmd_bktbr(), 
                                  dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                  dbmsh3d_cmd_msr(), dbsk3d_cmd_segop());  
      else if (dbsk3d_cmd_bktseg()==4) 
        //-bktseg 4: 
        run_seg_adpt_bucketing_4 (input_pts, finalM_faces, pv->dir_prefix(),
                                  dbmsh3d_cmd_npbkt(), dbmsh3d_cmd_dup()!=0, dbmsh3d_cmd_bktbr(), 
                                  dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                                  dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, dbmsh3d_cmd_ne()); 
      else if (dbsk3d_cmd_bktseg()==5) //-n1, n2, n3 for # of buckets in (x, y, z).
        //-bktseg 5: run cell-bucketing. 
        run_seg_cell_bucketing (pv->mesh(), pv->dir_file(), 
                                dbmsh3d_cmd_n1(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_n3(),
                                dbmsh3d_cmd_msr());
      else if (dbsk3d_cmd_bktseg()==6) 
        //-bktseg 6: old cell-bucketing
        run_seg_cell_bucketing_2 (pv->mesh(), pv->dir_file(), 
                                     dbmsh3d_cmd_n1(), dbmsh3d_cmd_n2(), dbmsh3d_cmd_n3(),
                                     dbmsh3d_cmd_msr());
      //Visualize the result.
      if (dbmsh3d_cmd_gui())
        _root->addChild (draw_ifs (input_pts, finalM_faces));
    }
  }
  //############### Pre-stitching the Bucketing Meshes ###############
  //-bktpst 1: Recover the original vertex id of surfaces in each bucket.
  if (dbsk3d_cmd_bktpst()==1) {
    //Read in the prefix-bktinfo.txt
    //Recover the original vertex id into a rich mesh and save.
    if (bkt_recover_vid_richmesh (dbmsh3d_cmd_fileprefix()) == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cerr, "ERROR in recovery vertex id to a rich mesh!\n");
    }
  }
  //-bktpst 2: 
  //  Read in the prefix-bktinfo.txt
  //  Produce all 'interior' meshes: prefix_00_00_00_int.ply.
  //  Produce the 'candidate stitching' mesh: prefix_stitch_cand.ply.  
  else if (dbsk3d_cmd_bktpst()==2) {
    //-bktbr: box ratio
    if (bkt_pre_stitch_buckets (dbmsh3d_cmd_fileprefix(), dbmsh3d_cmd_bktbr()) == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cerr, "ERROR in pre sitich buckets!\n");
    }
  }
  //-bktpst 3: Recover the original vertex id of the stitching surface.
  else if (dbsk3d_cmd_bktpst()==3) {
    if (bkt_stitchsurf_recover_vid (dbmsh3d_cmd_fileprefix()) == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cerr, "ERROR in recovery vertex id to a rich mesh!\n");
    }
  }
  //############### Produce the final mesh of bucketing + stitching ###############
  //Merge several partial meshes together
  if (dbsk3d_cmd_merge() == 1) { //-merge 1 -f prefix
    pv->reset_mesh ();
    if (bkt_merge_final_surface(dbmsh3d_cmd_fileprefix(), pv->mesh()) == false) {
      dbmsh3d_cmd_gui() = 0;
      vul_printf (vcl_cerr, "ERROR in merging the final surface mesh!\n");
    }

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
      _root->addChild (pv->vis_mesh_options (dbmsh3d_cmd_v(), dbmsh3d_cmd_idv(), dbmsh3d_cmd_id()!=0));

    dbmsh3d_pro_contiune = true;
    dbmsh3d_app_window_title += " -- " + pv->dir_file();
  }
  //###########################################################################
  //-spd 1: Compute the shock prior distribution.
  if (dbsk3d_cmd_spd()!=0) {
    //Read the ground truth from file prefix-orig.ply2.
    if (pv->load_ply2 (pv->dir_prefix() + "-orig.ply2") == false) {
      vul_printf (vcl_cerr, "\n!! File %s-orig.ply2 does not exist!\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }
    else {    
      pv->mesh()->IFS_to_MHE ();
      pv->mesh()->build_face_IFS ();
    }

    //Read in the full-shock scaffold from file prefix-full.fs.
    if (pv->load_fs (pv->dir_prefix() + "-full.fs", false) == false) {
      vul_printf (vcl_cerr, "\n!! File %s-full.fs does not exist!\n", pv->dir_prefix().c_str());
      dbmsh3d_cmd_gui() = 0;
    }

    if (dbsk3d_cmd_spd()==1 || dbsk3d_cmd_spd()==3) { 
      //First label the A12-2 shock sheets with mesh edge as visited.
      label_edge_A12_2_sheets (pv->fs_mesh());

      //-n 1: Output only the visited A12 shock sheets.
      //-n 2: Output only unvisited A12 shock sheets.
      output_A12_2_file (pv->fs_mesh(), dbmsh3d_cmd_n());

      if (dbmsh3d_cmd_gui()) {
        //Draw the original mesh
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
        _root->addChild (draw_fs_mesh_patches (pv->fs_mesh()->facemap(), true,
                                               true, false, false, 
                                               pv->user_defined_class_));
      }
    }
    if (dbsk3d_cmd_spd()==2 || dbsk3d_cmd_spd()==3) { 
      //First label the A13 shock links with mesh face as visited.
      label_face_A13_links (pv->fs_mesh());

      output_A13_file (pv->fs_mesh());

      if (dbmsh3d_cmd_gui()) {
        //Draw the original mesh
        _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints(), dbmsh3d_cmd_colorcode()));
        _root->addChild (draw_fs_mesh_links (pv->fs_mesh()->edgemap(), 
                                             0, FLT_MAX, true, false, 1, false,
                                             pv->user_defined_class_));
      }
    }
  }
  //###########################################################################
  //-vsf: View Shock Flow Type.
  if (dbsk3d_cmd_vsf()!=0) {
    pv->reset_mesh ();
    pv->load_fs (pv->dir_file());
    pv->fs_mesh()->build_IFS_mesh ();
    pv->compute_vertex_node_size ();

    //Visualize the object boundary.      
    if (dbsk3d_cmd_bnd()) { 
      //-bnd 1: Draw the boundary generators. 
      if (dbsk3d_cmd_vfc() != 4)
        _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
    }

    if (dbsk3d_cmd_vsf()==1) {
      //-vsf 1: visualize flow type of all shock elements.

    }
    else if (dbsk3d_cmd_vsf()==2) {
      //-vsf 2: visualize flow type of all shock sheet elements.
      _root->addChild (vis_fs_patches_flow_type (pv->fs_mesh()));
    }
    else if (dbsk3d_cmd_vsf()==3) {
      //-vsf 3: visualize flow type of all shock curve elements.
      _root->addChild (vis_fs_links_flow_type (pv->fs_mesh()));
    }
    else if (dbsk3d_cmd_vsf()==4) {
      //-vsf 4: visualize in-out-flow type of all shock node elements.
      if (dbmsh3d_cmd_idv() == 0)
        _root->addChild (vis_fs_nodes_iof_type (pv->fs_mesh(), pv->pt_size_));
      else
        _root->addChild (vis_fs_nodes_iof_type_idv (pv->fs_mesh(), pv->cube_size_));
    }
    else if (dbsk3d_cmd_vsf()==5) {
      //visualize all fs_edges.
      _root->addChild (draw_M_edges (pv->fs_mesh(), SbColor (1, 1, 0), pv->m_edge_width_));

      //-vsf 5: visualize the 9 cases of flow type of all shock node elements.
      if (dbmsh3d_cmd_idv() == 0)
        _root->addChild (vis_fs_nodes_flow_type (pv->fs_mesh(), pv->pt_size_));
      else
        _root->addChild (vis_fs_nodes_flow_type_idv (pv->fs_mesh(), pv->cube_size_));
    }
  }
  //###########################################################################
  //-vfc: View Flow Complex and other subsets (Gaberial graph, EMST).
  if (dbsk3d_cmd_vfc()!=0) {
    pv->reset_mesh ();
    pv->load_fs (pv->dir_file());
    pv->fs_mesh()->build_IFS_mesh ();
    pv->compute_vertex_node_size ();

    //Visualize the object boundary.      
    if (dbsk3d_cmd_bnd()) { 
      //-bnd 1: Draw the boundary generators in size 1. 
      if (dbsk3d_cmd_vfc() != 4)
        _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, 1));
    }
    
    //Also draw all shock sheets.
    //vis = pv->vis_fs_mesh (true, dbmsh3d_cmd_v()==1, dbmsh3d_cmd_inf()!=0);
    //_root->addChild (vis);

    if (dbsk3d_cmd_vfc() == 1) {
      //-vfc 1: visualize all shock points.
      //Visualize the fine shock scaffold.
      if (dbmsh3d_cmd_v()==0) { //-v 0: Draw the valid sheets as a single mesh object.
        _root->addChild (pv->vis_fs_mesh_valid ());
      }
    }
    else if (dbsk3d_cmd_vfc() == 2) {
      //-vfc 2: visualize all flow complex points.
      //-n: vis. option.
      //-n2: 1: not drawing shock-elms, 2: drawing shock-elms.
      _root->addChild (vis_fs_flow_complex (pv->fs_mesh(), dbmsh3d_cmd_n(), dbmsh3d_cmd_n2()>1, pv->pt_size_));
    }
    else if (dbsk3d_cmd_vfc() == 3) {
      //-vfc 3: visualize all Gaberial Edges.
      _root->addChild (vis_fs_gabriel_graph (pv->fs_mesh(), dbmsh3d_cmd_cw(), SbColor (0.0f, 0.0f, 0.8f)));
    }
    else if (dbsk3d_cmd_vfc() == 4) {
      //-vfs 4: visualize all types of A13 shock curves (type I, II, III).
      //-boxr: length ratio (def: 0.1 * 10)
      float len = float (pv->fs_mesh()->median_A122_dist() * dbmsh3d_cmd_boxr() * 10);
      //-n: type number
      if (dbmsh3d_cmd_n() == 4) { //-n 4: show both type I and II
        if (dbmsh3d_cmd_n2()) {
          _root->addChild (vis_fs_links_type_tris (pv->fs_mesh(), 1, dbmsh3d_cmd_v()==2));
          _root->addChild (vis_fs_links_type_tris (pv->fs_mesh(), 2, dbmsh3d_cmd_v()==2));
        }
        if (dbmsh3d_cmd_n3()) {
          _root->addChild (vis_fs_links_type (pv->fs_mesh(), 1, len));
          _root->addChild (vis_fs_links_type (pv->fs_mesh(), 2, len));
        }
      }
      else { //-n 1, 2, 3
        //-n2: show dual Delaunay triangles, default 1 (on).
        //-v 2: view only large triangles.
        if (dbmsh3d_cmd_n2())
          _root->addChild (vis_fs_links_type_tris (pv->fs_mesh(), dbmsh3d_cmd_n(), dbmsh3d_cmd_v()==2));

        //-n3: show shock link itself, default 1 (on).
        if (dbmsh3d_cmd_n3())
          _root->addChild (vis_fs_links_type (pv->fs_mesh(), dbmsh3d_cmd_n(), len));
      }
      
      if (dbsk3d_cmd_bnd()) //-bnd 1: show generators. 
        _root->addChild (draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_));
    }
    else if (dbsk3d_cmd_vfc() == 5) {
      //-vfc 5: visualize all Euclidean minimal spanning tree (EMST).
    }
    else if (dbsk3d_cmd_vfc() == 6) {
      //-vfc 6: visualize other shock singular points.
      //-n -1: visualize all type III shock curves.
      //-n 2: visualize the type II shock curve (A13-2 points).
      if (dbmsh3d_cmd_n() == -1 || dbmsh3d_cmd_n() == 2)
        //-n2 option: 1: draw points, 2: draw tris, 3: draw both.
        _root->addChild (vis_fs_link_type_2_points (pv->fs_mesh(), dbmsh3d_cmd_n2(),
                                                    pv->pt_size_, SbColor (0.4f, 0.4f, 0.0f)));

      //-n 3: visualize the type III shock curve.
      if (dbmsh3d_cmd_n() == -1 || dbmsh3d_cmd_n() == 3) {
        //-n2 option: 1: draw points, 2: draw tris, 3: draw both.
        if (dbmsh3d_cmd_n2()==1 || dbmsh3d_cmd_n2()==3)
          _root->addChild (vis_fs_link_type_3_pts (pv->fs_mesh(), pv->pt_size_, SbColor (0.8f, 0.4f, 0.0f)));
        if (dbmsh3d_cmd_n2()==2 || dbmsh3d_cmd_n2()==3)
          _root->addChild (vis_fs_link_type_3 (pv->fs_mesh(), dbmsh3d_cmd_n2(),
                                               dbmsh3d_cmd_cw(), SbColor (0.8f, 0.4f, 0.0f)));
      }
    }
  }
  //###########################################################################
  //-vss : View Shock Sheet and Boundary Info (including the camouflage color).
  if (dbsk3d_cmd_vss()!=0) {
    //Read in the shock (.fs) and boundary mesh or points file.
    pv->reset_mesh ();
    pv->reset_fs_mesh ();
    if (pv->load_fs (pv->dir_file()) == false) {
      vcl_cout << "\ndbsk3d_cmdproc_execute (seg): Error loading " << pv->dir_file() + ".\n";
      dbmsh3d_cmd_gui() = 0;
      return _root;
    }
    //Load in the surface mesh prefix-surface.
    if (pv->load_faces_files (pv->dir_prefix() + "-surface") == false) {
      vcl_cout << "\ndbsk3d_cmdproc_execute (ms): Error loading " << pv->dir_prefix() + "-surface.\n";
      ///dbmsh3d_cmd_gui() = 0;
      ///return _root;
    }
    pv->mesh()->IFS_to_MHE ();
    pv->fs_mesh()->build_IFS_mesh ();
    pv->compute_vertex_node_size ();

    if (dbsk3d_cmd_dels()) { //If -dels 1 on -vss -f prefix
      //set -idv 1.
      dbmsh3d_cmd_idv() = 1;          
      //set -bnd 0: hide boundary drawing.
      dbsk3d_cmd_bnd() = 0;
    }
    
    //Draw the shock sheets (components).
    pv->fs_ss()->build_fs_sheet_set ();

    //-regth: Visualize shock sheet with cost < than regth in with bnd_options
    pv->fs_regul()->init_regul_sheet_queue (dbsk3d_cmd_regth());
    //-idv : draw individual elements.
    _root->addChild (draw_fs_sheet_set (pv->fs_ss(), dbmsh3d_cmd_inf(), 
                     dbmsh3d_cmd_idv(), pv->user_defined_class_));

    //Draw boundary mesh or pts. -bnd 0 to turn off.
    if (dbsk3d_cmd_bnd()==1) { 
      //-bnd 1: draw input point cloud
      if (dbmsh3d_cmd_idv()==1) //-idv 1: draw each gene individually.
        vis = pv->vis_ptset_idv();  
      else 
        vis = draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_);
      _root->addChild (vis);
    }
    else if (dbsk3d_cmd_bnd()==2) { 
      //-bnd 1: draw input point cloud
      if (dbmsh3d_cmd_idv()==1) //-idv 1: draw each gene individually.
        vis = pv->vis_ptset_idv();  
      else //-gsize : use 3
        vis = draw_ptset (pv->mesh(), pv->m_vertex_ccode_, pv->pt_size_); //3
      _root->addChild (vis);
      
      //Draw bnd mesh edges (-idv 2 to draw them individually)
      _root->addChild (pv->vis_mesh_edges (COLOR_GRAY, 1.0f, dbmsh3d_cmd_idv()>1));
    }
    else if (dbsk3d_cmd_bnd()==2) { 
      //-bnd 2: draw boundary mesh with transparency
      _root->addChild (pv->vis_mesh (false, dbmsh3d_cmd_shapehints()));
    }

    //-n2 2: bnd-shock vis. option: 
    if (dbmsh3d_cmd_n2() != 1) //Visualize all bnd-shock asso. and unassociated (lost) genes.
      _root->addChild (draw_fs_sheet_set_bnd_asgn (pv->fs_ss(), dbmsh3d_cmd_n2(), pv->cube_size_));

    //-vss 1: bnd-shock asso. of a specified sheet.
    if (dbsk3d_cmd_vss() == 1) { 
      //-n (sheet_id), draw bnd-shock association of the specified sheet.
      if (dbmsh3d_cmd_n() != -1) {      
        dbsk3d_fs_sheet* S = pv->fs_ss()->sheetmap (dbmsh3d_cmd_n());

        //Draw the bnd-to-shock assignment.
        _root->addChild (draw_fs_sheet_bnd_asgn (S, dbmsh3d_cmd_idv()!=0, float(pv->cube_size_*1.5f)));

        //Draw fs_sheet's A3 and A13 boundary curves.
        _root->addChild (draw_fs_sheet_A13_A3_bnd_curves (S, 3));

        //Draw the corresponding boundary surface patch.
        //-n3: boundary surface patch option (1: default, 2: faces sharing 2 points, 3: all)
        _root->addChild (draw_fs_sheet_bnd_mesh (S, dbmsh3d_cmd_n3(), dbmsh3d_cmd_idv(), 0.0f));
      }
    }
    //-vss 2,3,4: bnd-shock asso. of all sheets (camouflage color).
    //   vss 2: draw Gfaces.
    //   vss 3: draw Gfaces and Gfaces2.
    //   vss 4: draw Gfaces, Gfaces2, and Gfaces1.
    else {
      vis = draw_fs_sheet_set_bnd_mesh (pv->fs_ss(), dbsk3d_cmd_vss(), dbmsh3d_cmd_mesh_transp());
      
      if (pv->view_dx_ == -1 && pv->view_dy_ == -1) {        
        _root->addChild (vis);
      }
      else { //If specified, shift vis by (dx, 0).
        ///if (pv->view_dx_ == 0)
          ///pv->compute_dx_dy_dz ();
        _root->addChild (shift_output (vis, pv->view_dx_, 0, 0));
      }
    }
  }
  //###########################################################################
  //-smsh : Smoothing + Meshing of 3PI Scan Dataset.
  //-smsh 1: no smoothing, -smsh: 2 Discrete Curve Shortening (DCS), -smsh 3: Gaussian.
  if (dbsk3d_cmd_smsh() != 0) {
    pv->set_dir_prefix (dbmsh3d_cmd_fileprefix());

    //Read in the cropped .3pi file 'f_crop.3pi'.
    bool result = pv->load_3pi (pv->dir_prefix() + "_crop.3pi");
    assert (result);
    
    //Estimate dist. threshold for error estimation.
    // -dthr: dist_th_ratio for error estimation.
    // -icpthr: icp_th_ratio.
    pv->set_pro_data (dbmsh3d_pro_base::PD_SG3PI);
    double avg_samp_dist = pv->get_avg_samp_dist();
    ///float error_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_dthr());
    ///float pp_icp_dist_th = float (avg_samp_dist * dbmsh3dr_cmd_icpthr());

    //1) Smooth f1 and f2.
    int nstep;
    if (dbsk3d_cmd_smsh() == 1) {
      //-sm 1: no smoothing.
    }      
    else if (dbsk3d_cmd_smsh() == 2) {
      //-sm 2: Iterative DCS smoothing intra- and inter- scanlines 
      //-n STEPS: default 20.
      nstep = (dbmsh3d_cmd_n() == -1) ? 20 : dbmsh3d_cmd_n();
      //-l: DCS_psi step size.
      //-thr: th_ratio
      float DCS_psi = dbmsh3d_cmd_len(); //default step size: 1      
      vul_printf (vcl_cerr, "\n\tApply DCS smoothing %d times (DCS_psi %f, th_r %f).\n", 
                  nstep, DCS_psi, dbmsh3d_cmd_thr());

      //Estimate intra- and inter- scanline sample distance.
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
    else if (dbsk3d_cmd_smsh() == 3) {
      //-sm 3: 2D Gaussian smoothing (intra- and inter- scan lines)
      //-n STEPS: default 1.
      nstep = (dbmsh3d_cmd_n() == -1) ? 10 : dbmsh3d_cmd_n();
      //-gsr: G_sigma_ratio
      //-grr: G_radius_ratio
      //-thr: th_ratio
      gaussian_smooth_2d_3pi (pv->sg3pi(), nstep, 
                              dbmsh3d_cmd_gsr(), dbmsh3d_cmd_grr(), dbmsh3d_cmd_thr());
    }
          
    //Copy sg[] to mesh. All following processes are on the mesh data structures.
    //pv->reset_mesh();
    assert (pv->mesh()->vertexmap().size() == 0);
    pv->move_sg3pi_to_mesh ();
    
    //Mesh the scan dataset.
    //-sphrr 5: bounding sphere radius ratio.
    //-sphsr 1: bounding sphere sample ratio.
    //-msr: max sample dist. ratio
    //-segop 0: topo. option.
    pv->run_surface_meshing (dbsk3d_cmd_sphrr(), dbsk3d_cmd_sphsr(),
                             dbmsh3d_cmd_msr(), dbsk3d_cmd_segop()>1, true);

    //Visualize the smoothing + meshing results.
    if (dbmsh3d_cmd_gui()) {        
      vis = pv->vis_mesh (dbmsh3d_cmd_idv()!=0, 
                          dbmsh3d_cmd_shapehints(), COLOR_DARKGREEN, true);
      _root->addChild (vis);
    }

    //Save the registered smoothing results into .ply files.
    if (dbmsh3d_cmd_ofile()>=1) {
      if (dbsk3d_cmd_smsh()==1)
        pv->save_ply (pv->dir_prefix() + ".ply");
      else if (dbsk3d_cmd_smsh()==2)
        pv->save_ply (pv->dir_prefix() + "_dcs.ply");
      else if (dbsk3d_cmd_smsh()==3)
        pv->save_ply (pv->dir_prefix() + "_gsm.ply");
    }
  }
  //###########################################################################
  //-ridge 1: Compute Ridge on Object Surface Using Medial Scaffold.
  if (dbsk3d_cmd_ridge()) {
    vcl_string dir_file = vcl_string (dbmsh3d_cmd_fileprefix());
    pv->set_dir_file (dir_file);

    pv->load_cms ();

    //Old code on xform
    if (dbmsh3d_cmd_align_file()) { //read the alignment from file and perform transformation.
      pv->load_hmatrix (dbmsh3d_cmd_align_file());
      
      vcl_cout << "Transforming mesh ...\n"; // transform the reference mesh
      dbmsh3d_apply_xform (pv->mesh(), pv->hmatrix());
      dbmsh3d_apply_xform (pv->fs_mesh(), pv->hmatrix());
      vcl_cout << "done.\n";
    }

    pv->compute_vertex_node_size ();

    //Visualize the object boundary generators.
    if (dbsk3d_cmd_bnd()==1) {
      if (dbmsh3d_cmd_idv()==0) //draw the whole point set as a single object.
        _root->addChild (pv->vis_mesh_pts_edges());
      else //draw each gene individually to enable getInfo.     
        _root->addChild (pv->vis_ptset_idv());
    }
    else if (dbsk3d_cmd_bnd() == 2)
      _root->addChild (pv->vis_mesh(true, true));      
    else if (dbsk3d_cmd_bnd() == 3) {
      _root->addChild (pv->vis_mesh(true, true)); 
    }      
    else if (dbsk3d_cmd_bnd() == 4) {
      _root->addChild (pv->vis_mesh(true, true));      
      _root->addChild (pv->vis_mesh_pts_edges());
    }

    //Visualize the Medial Scaffold graph
    if (dbmsh3d_cmd_v()==0)
      _root->addChild (pv->vis_ms_graph());
    else
      _root->addChild (pv->vis_ms_hypg());

    //Visualize the shock-bnd correspondence.
    SbColor A13_recon_color, A3_recon_color;
    SbColor A3_sect_tris_color, A3_ridge_vector_color;
    SbColor A3_ridgebnd_color;
    SbColor A13_surfbnd_color;
    if (dbmsh3d_cmd_bgcol()==0) {
      A13_recon_color = SbColor (1, 1, 0.5);
      A3_recon_color = SbColor (0.5, 1, 1);
      A3_sect_tris_color = SbColor (1, 1, 0);
      A3_ridgebnd_color = SbColor (0, 1, 1);
      A13_surfbnd_color = SbColor (1, 1, 0);
      A3_ridge_vector_color = SbColor (1, 0.5, 0);
    }
    else {
      A13_recon_color = SbColor (1, 0.5, 0);
      A3_recon_color = SbColor (0, 0.5, 1);
      A3_sect_tris_color = SbColor (0.5, 0.5, 0);
      A3_ridgebnd_color = SbColor (0, 0.5, 0.5);
      A13_surfbnd_color = SbColor (0.5, 0.5, 0);
      A3_ridge_vector_color = SbColor (0.5, 0, 0);
    }
    
    if (dbsk3d_cmd_ridge() == 1 || dbsk3d_cmd_ridge() == 3 || dbsk3d_cmd_ridge() == 10 || dbsk3d_cmd_ridge() == 11)
      _root->addChild (draw_A13axial_bnd_link (pv->ms_hypg(), A13_recon_color));
    if (dbsk3d_cmd_ridge() == 2 || dbsk3d_cmd_ridge() == 3 || dbsk3d_cmd_ridge() == 5 || dbsk3d_cmd_ridge() == 11)
      _root->addChild (draw_A3rib_bnd_link (pv->ms_hypg(), A3_recon_color));

    //Save shock-bnd correspondence to output xx-tcurve.g3d file.
    if (dbmsh3d_cmd_ofile()==2) {
      if (dbsk3d_cmd_ridge() == 1 || dbsk3d_cmd_ridge() == 2 || dbsk3d_cmd_ridge() == 3) {
        save_ms_bnd_link_g3d (pv->ms_hypg(), pv->mesh(), pv->dir_prefix() + "-tcurve.g3d");
      }
    }

    //-ridge : Draw sectional triangles along A3 ribs.
    if (dbsk3d_cmd_ridge() == 4 || dbsk3d_cmd_ridge() == 6) 
      _root->addChild (draw_A3_sect_tris (pv->ms_hypg(), A3_sect_tris_color));

    //-ridge : Draw ridge_region bnd curves along A3 ribs.
    if (dbsk3d_cmd_ridge() == 5 || dbsk3d_cmd_ridge() == 6 || dbsk3d_cmd_ridge() == 9 || dbsk3d_cmd_ridge() == 11) {
      _root->addChild (draw_A3_ridgebnd_curves (pv->ms_hypg(), 3.0f, 
                                                A3_ridgebnd_color, A3_ridgebnd_color));
    }

    //-ridge : Draw ridge_curve and ridge vectors along A3 ribs.
    if (dbsk3d_cmd_ridge() == 7) 
      _root->addChild (draw_A3_ridge_vectors (pv->ms_hypg(), SbColor (1, 0, 0), dbmsh3d_cmd_len()));
    else if (dbsk3d_cmd_ridge() == 8)
      //-r : ridge point (ball size)
      _root->addChild (draw_A3_ridge_pts (pv->ms_hypg(), dbmsh3d_cmd_r(), SbColor (1, 0, 0)));      
    else if (dbsk3d_cmd_ridge() == 9)
      //-r : ridge point (ball size)
      _root->addChild (draw_A3_ridge_curves (pv->ms_hypg(), dbmsh3d_cmd_r(), SbColor (1, 0, 0), 3.0f));
    
    //-ridge : Draw A13_bump_region bnd curves along A13 axials.
    if (dbsk3d_cmd_ridge() == 10 || dbsk3d_cmd_ridge() == 11)
      _root->addChild (draw_A13_surfbnd_curves (pv->ms_hypg(), 3.0f, 
                                                A13_surfbnd_color, A13_surfbnd_color));
  }
  //###########################################################################
  // Compute Goedesic Distance Transform on Shock Sheets.
  if (dbsk3d_cmd_gdts()==1 || dbsk3d_cmd_all()==3) {
    if (dbmsh3d_pro_contiune == false) {
      pv->reset_mesh();
      pv->reset_fs_mesh();
      pv->load_cms ();
    }

    pv->ms_hypg_fmm()->build_sheets_fmm_mesh ();
    pv->ms_hypg_fmm()->run_fmm_on_sheet_meshes ();

    dbmsh3d_pro_contiune = true;

    if (dbmsh3d_cmd_gui()) {
      pv->compute_vertex_node_size ();

      vis = pv->vis_ms_hypg (7);
      _root->addChild (shift_output (vis, pv->view_dx_, pv->view_dy_, 0));
      vis = pv->vis_mesh_pts_edges ();
      _root->addChild (shift_output (vis, pv->view_dx_, pv->view_dy_, 0));
    }
  }
  //############### Print Object Memory Size ###############
  if (dbmsh3d_cmd_mem()) {
    pv->print_mem_usage ();
  }

  //############### Save Output Files ############### 
  if (pv->mesh()->vertexmap().size()!=0) {
    vcl_string dirfile;
    //-op3d:
    if (dbmsh3d_cmd_p3d_ofile()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_p3d_ofile());
      dirfile += ".p3d";
      vul_printf (vcl_cerr, "Writing ASCII point cloud file (*.p3d) %s...\n", dirfile);
      pv->save_p3d (dirfile);
    }
    //-oply:
    if (dbmsh3d_cmd_ply_ofile()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofile());
      dirfile += ".ply";
      vul_printf (vcl_cerr, "Writing ASCII PLY mesh file (*.ply) %s...\n", dirfile);
      pv->save_ply (dirfile, true);
    }
    //-oplyb:
    if (dbmsh3d_cmd_ply_ofileb()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofileb());
      dirfile += ".ply";
      vul_printf (vcl_cerr, "Writing binary PLY mesh file (*.ply) %s...\n", dirfile);
      pv->save_ply (dirfile, false);
    }
    //-oply2:
    if (dbmsh3d_cmd_ply2_ofile()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply2_ofile());
      dirfile += ".ply2";
      vul_printf (vcl_cerr, "Writing PLY2 mesh file (*.ply2) %s...\n", dirfile);
      pv->save_ply2 (dirfile);
    }
    //-ooff:
    if (dbmsh3d_cmd_off_ofile()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_off_ofile());
      dirfile += ".off";
      vul_printf (vcl_cerr, "Writing Geomview OFF mesh file (*.off) %s...\n", dirfile);
      pv->save_off (dirfile);
    }
    //-oobj:
    if (dbmsh3d_cmd_obj_ofile()) {
      dirfile = dbul_get_dir_file (dbmsh3d_cmd_obj_ofile());
      dirfile += ".obj";
      vul_printf (vcl_cerr, "Writing Wavefront OBJ mesh file (*.obj) %s...\n", dirfile);
      pv->save_obj (dirfile);
    }
  }
  //############### Save Output Shock Files ############### 

  return _root;
}


