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
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_fmm.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>

#include <gdt/vis/gdtvis_mesh.h>
#include <gdt/vis/gdtvis_path_iso_contour.h>
#include <gdt/vis/gdtvis_interval.h>
#include <gdt/vis/gdtvis_welm.h>
#include <gdt/vis/gdtvis_shock.h>
#include <gdt/vis/gdtvis_ws.h>


#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>

#include <ray_tri/ray_tri.h>

//#####################################################################
// dbmsh3d command-line process execution.
// false: no task specified.
bool gdt_check_cmdproc (gdt_pro_vis* pv, int argc, char **argv)
{
  //1) Check if no cmdproc specified, result=PRO_RESULT_NO_PROCESS. 
  if (dbmsh3d_cmd_gdt()==0 && dbmsh3d_cmd_fmm()==0)) {
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


  //###########################################################################
  //######### Geodesic Distance Transform #########
  if (dbmsh3d_cmd_gdt()) {
    pv->set_dir_file (dbmsh3d_cmd_fileprefix());

    //read in the mesh file
    pv->load_ply2_gdt ();
    pv->gdt_mesh()->IFS_to_MHE();

    int s_idx;
    if (dbmsh3d_cmd_s() == -1) {
      vnl_random idxrand;
      idxrand.reseed ( (unsigned int) vcl_time(NULL));
      float r = (float) idxrand.drand32 (0, 1);
      unsigned int vsize = pv->gdt_mesh()->vertexmap().size();
      s_idx = (int) vcl_floor (r * vsize);
    }
    else
      s_idx = dbmsh3d_cmd_s();

    //compute mesh edge lengths and store them in each edge's intervel_section
    pv->gdt_mesh()->compute_edge_lengths ();
    pv->gdt_mesh()->compute_vertex_sum_angles ();

    unsigned int n_iter = (unsigned int) dbmsh3d_cmd_n();

    //############### 1 Surazhsky's interval-based propagation via gdt_i_manager ###############
    if (dbmsh3d_cmd_gdt() == GDT_METHOD_I) { //1
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) pv->gdt_mesh()->vertexmap (s_idx);
      pv->i_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cerr, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                  0, src_vertex->id(), src_vertex->pt().x(), 
                  src_vertex->pt().y(), src_vertex->pt().z());
      //Draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      //Perform geodesic distance transform
      //Surazhsky & Kirsanov's original interval based method
      pv->i_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso;
      if (dbmsh3d_cmd_ntestcontour() !=0)
        vis_iso = gdt_draw_result (pv->f_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      pv->i_manager()->print_results (dbmsh3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1)
          _root->addChild (vis_iso);

        pv->gdt_mesh()->build_IFS_mesh ();
  
        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw only intervals on the front
          vis = gdt_draw_I_on_front_flag (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);

          //draw the whole mesh as one object
          vis = draw_M (pv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);
        }
        else if (dbmsh3d_cmd_v()==1) {
          //: a temporary solution, to make all edges inactive!
          pv->i_manager()->gdt_mesh()->init_traverse();

          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (pv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (pv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery(), 3);
          _root->addChild (vis);
        }
      }

      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += pv->dir_file();
    }

    //############### 2 Proposed Face-based propagation via gdt_f_manager ###############
    else if (dbmsh3d_cmd_gdt() == GDT_METHOD_F) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) pv->gdt_mesh()->vertexmap (s_idx);
      pv->f_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cerr, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      pv->f_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso = gdt_draw_result (pv->f_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      pv->f_manager()->print_results (dbmsh3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1)
          _root->addChild (vis_iso);

        pv->gdt_mesh()->build_IFS_mesh ();
      
        pv->f_manager()->mark_front_edges_visited ();

        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw the whole mesh as one object
          vis = draw_M (pv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          //draw only intervals on the front
          vis = gdtf_draw_I_on_front (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
        else {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (pv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (pv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
      }

      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += pv->dir_file();
    }

    //############### 3 Proposed face-based propagation using shocks via gdt_fs_manager ###############
    else if (dbmsh3d_cmd_gdt() == GDT_METHOD_FS) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) pv->gdt_mesh()->vertexmap (s_idx);
      pv->fs_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cerr, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      pv->fs_manager()->run_gdt (n_iter);

      ///SoSeparator* vis_iso = gdt_draw_result (&fs_manager, dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      pv->fs_manager()->print_results (dbmsh3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        ///if (dbmsh3d_cmd_ntestcontour() > 1)
          ///_root->addChild (vis_iso);

        pv->gdt_mesh()->build_IFS_mesh ();
      
        pv->fs_manager()->mark_front_edges_visited ();

        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw the whole mesh as one object
          vis = draw_M (pv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          //draw only intervals on the front
          vis = gdtf_draw_I_on_front (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
        else if (dbmsh3d_cmd_v()==1) {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (pv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (pv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
      }
      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += pv->dir_file();
    }

    //############### 4 Proposed simutaneous discrete wavefront and shock propagation via gdt_ws_manager ###############
    if (dbmsh3d_cmd_gdt() == GDT_METHOD_WS) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) pv->gdt_mesh()->vertexmap (s_idx);
      pv->ws_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cerr, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      pv->ws_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso = NULL;
      if (n_iter == (unsigned int) -1)
        vis_iso = gdt_draw_result (pv->ws_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      pv->ws_manager()->print_results (dbmsh3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx, n_iter == (unsigned int) -1);
      pv->ws_manager()->print_statistics ();

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1 && n_iter == (unsigned int) -1)
          _root->addChild (vis_iso);

        pv->gdt_mesh()->build_IFS_mesh ();

        //: draw the mesh
        if (dbmsh3d_cmd_v() < 2) {
          //draw the whole mesh as one object
          vis = draw_M (pv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          pv->gdt_mesh()->init_traverse ();
          vis = gdt_draw_edges_vispt (pv->gdt_mesh(), 3);
          _root->addChild (vis);
        }
        else {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (pv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (pv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw finalized gdt_mesh intervals
          vis = gdt_draw_welms (pv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);

          //draw intervals in the queue Qw
          vis = gdtws_draw_W_in_Qw (pv->ws_manager(), SbColor (0.2f, 0.2f, 1.0f), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }

        if (dbmsh3d_cmd_v() > 0) {
          //draw each mesh vertex as a separate object
          vis = gdt_draw_vertices_vispt (pv->gdt_mesh(), dbmsh3d_cmd_r());
          _root->addChild (vis);

          //draw each shock node as a separate object
          vis = gdt_draw_snodes_vispt (pv->ws_manager(), dbmsh3d_cmd_r());
          _root->addChild (vis);

        }

        // Draw the shocks
        vis = gdt_draw_shocks (pv->ws_manager(), 5);
        _root->addChild (vis);
      }
      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += pv->dir_file();
    }
  }


  //##########################################################################
  //##################### Process and Save Output Files ######################
  //##########################################################################

  //########################### Save Output Files ############################
  vcl_string dirfile;
  if (dbmsh3d_cmd_p3d_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_p3d_ofile());
    dirfile += ".p3d";
    vul_printf (vcl_cerr, "Writing ASCII point cloud file (*.p3d) %s...\n", dirfile.c_str());
    pv->save_p3d (dirfile);
  }
  if (dbmsh3d_cmd_g3d_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_g3d_ofile());
    dirfile += ".g3d";
    vul_printf (vcl_cerr, "Writing color point cloud file (*.g3d) %s...\n", dirfile.c_str());
    pv->save_g3d (dirfile);
  }
  if (dbmsh3d_cmd_xyz_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_xyz_ofile());
    dirfile += ".xyz";
    vul_printf (vcl_cerr, "Writing ASCII point cloud file (*.xyz) %s...\n", dirfile.c_str());
    pv->save_xyz (dirfile);
  }
  if (dbmsh3d_cmd_xyzn1_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_xyzn1_ofile());
    dirfile += ".xyzn1";
    vul_printf (vcl_cerr, "Writing oriented point cloud file (*.xyzn1) %s...\n", dirfile.c_str());
    pv->save_xyzn1 (dirfile);
  }
  if (dbmsh3d_cmd_xyznw_ofile()) {
    ///vul_printf (vcl_cerr, "Writing Oriented Point Cloud (*.xyznw) file %s...\n", dbmsh3d_cmd_xyznw_ofile());
    ///pv->save_xyznw (vcl_string (dbmsh3d_cmd_xyznw_ofile()) + ".xyznw");
  }
  if (dbmsh3d_cmd_ply_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofile());
    dirfile += ".ply";
    vul_printf (vcl_cerr, "Writing ASCII PLY mesh file (*.ply) %s...\n", dirfile.c_str());
    pv->save_ply (dirfile, true);
  }
  if (dbmsh3d_cmd_ply_ofileb()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply_ofileb());
    dirfile += ".ply";
    vul_printf (vcl_cerr, "Writing binary PLY mesh file (*.ply) %s...\n", dirfile.c_str());
    pv->save_ply (dirfile, false);
  }
  if (dbmsh3d_cmd_ply2_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_ply2_ofile());
    dirfile += ".ply2";
    vul_printf (vcl_cerr, "Writing ASCII PLY2 mesh file (*.ply2) %s...\n", dirfile.c_str());
    pv->save_ply2 (dirfile);
  }
  if (dbmsh3d_cmd_off_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_off_ofile());
    dirfile += ".off";
    vul_printf (vcl_cerr, "Writing Geomview OFF mesh file (*.off) %s...\n", dirfile.c_str());
    pv->save_off (dirfile);
  }
  if (dbmsh3d_cmd_obj_ofile()) {
    dirfile = dbul_get_dir_file (dbmsh3d_cmd_obj_ofile());
    dirfile += ".obj";
    vul_printf (vcl_cerr, "Writing Wavefront OBJ mesh file (*.obj) %s...\n", dirfile.c_str());
    pv->save_obj (dirfile);
  }
  
  //: ############ OpenInventor Demo and Test ############
  if (dbmsh3d_cmd_test()) {

    //Debug ray-tri intersection!!

    vgl_point_3d<double> pt11 (0.19187803948405169, 0.00069384501684954607, 0.0025251314556233393);
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
    _root->addChild (vis);

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

    /*vul_printf (vcl_cerr, "Test Coin3D controls......\n");
    
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

/*void dbmsh3d_save_vis_to_iv_wrl (SoSeparator* _root)
{
  //Save Visualization Output Files
  if (dbmsh3d_cmd_iv_ofile()) {
    vul_printf (vcl_cerr, "Writing OpenInventor IV (*.iv) file %s...\n", dbmsh3d_cmd_iv_ofile());
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

    vul_printf (vcl_cerr, "Writing VRML (*.wrl) file %s...\n", dbmsh3d_cmd_wrl_ofile());

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
  
    vul_printf (vcl_cerr, "Writing VRML2 (*.wrl) file %s...\n", dbmsh3d_cmd_wrl_ofile2());
  
    SoOutput out;
    out.openFile (dbmsh3d_cmd_wrl_ofile2());
    out.setHeaderString("#VRML V2.0 utf8");
    SoWriteAction wra(&out);
    wra.apply (_VRML2root);
    out.closeFile();
  
    _VRML2root->unref();
  }
}*/
