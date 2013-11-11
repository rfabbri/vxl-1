//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbgdt3d_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <bgui3d/bgui3d_file_io.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbgdt3d/vis/dbgdt3d_vis_interval.h>

#include <dbgdt3d/pro/dbgdt3d_cmdpara.h>
#include <dbgdt3d/vis/dbgdt3d_vis_path_iso_contour.h>
#include <dbgdt3d/vis/dbgdt3d_vis_mesh.h>
#include <dbgdt3d/vis/dbgdt3d_vis_welm.h>
#include <dbgdt3d/vis/dbgdt3d_vis_shock.h>
#include <dbgdt3d/vis/dbgdt3d_vis_ws.h>

#include <dbgdt3d/cmdproc/dbgdt3d_process_vis.h>
#include <dbgdt3d/cmdproc/dbgdt3d_cmdproc.h>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>

#include <ray_tri/ray_tri.h>

//#####################################################################
// dbmsh3d command-line process execution.
// false: no task specified.
bool dbgdt3d_check_cmdproc (dbgdt3d_pro_vis* gpv, int argc, char **argv)
{
  //1) Check if no cmdproc specified, result=PRO_RESULT_NO_PROCESS. 
  if (dbgdt3d_cmd_gdt()==0)
    return false; 
  else
    return true;
}

void dbgdt3d_setup_provis_parameters (dbgdt3d_pro_vis* gpv)
{
  if (dbmsh3d_cmd_fileprefix()) //set prefix
    gpv->set_dir_file (dbmsh3d_cmd_fileprefix());

  //Setup visualization parameters.  
  gpv->pt_size_ = dbmsh3d_cmd_psz();
  gpv->cube_size_ = dbmsh3d_cmd_rc();

  gpv->m_transp_ = dbmsh3d_cmd_mesh_transp();
  gpv->m_edge_width_ = dbmsh3d_cmd_cw();
  gpv->m_bndcv_width_ = dbmsh3d_cmd_cw();   
  gpv->vector_len_ = dbmsh3d_cmd_len();
  
  if (dbmsh3d_cmd_bgcol() == 1) {
    gpv->m_vertex_ccode_ = COLOR_BLACK;
    gpv->bg_ccode_ = COLOR_WHITE;
  }
  else {
    gpv->m_vertex_ccode_ = COLOR_WHITE;
    gpv->bg_ccode_ = COLOR_BLACK;
  }
}

SoSeparator* dbgdt3d_cmdproc_execute (dbgdt3d_pro_vis* gpv)
{
  SoSeparator* _root = new SoSeparator;
  SoSeparator* vis;
    
  //###########################################################################
  //######### Geodesic Distance Transform #########
  if (dbgdt3d_cmd_gdt()) {
    gpv->set_dir_file (dbmsh3d_cmd_fileprefix());

    //read in the mesh file
    gpv->load_ply2_gdt ();
    gpv->gdt_mesh()->IFS_to_MHE();

    int s_idx;
    if (dbmsh3d_cmd_s() == -1) {
      vnl_random idxrand;
      idxrand.reseed ( (unsigned int) vcl_time(NULL));
      float r = (float) idxrand.drand32 (0, 1);
      unsigned int vsize = gpv->gdt_mesh()->vertexmap().size();
      s_idx = (int) vcl_floor (r * vsize);
    }
    else
      s_idx = dbmsh3d_cmd_s();

    //compute mesh edge lengths and store them in each edge's intervel_section
    gpv->gdt_mesh()->compute_edge_lengths ();
    gpv->gdt_mesh()->compute_vertex_sum_angles ();

    unsigned int n_iter = (unsigned int) dbmsh3d_cmd_n();

    //############### 1 Surazhsky's interval-based propagation via gdt_i_manager ###############
    if (dbgdt3d_cmd_gdt() == GDT_METHOD_I) { //1
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) gpv->gdt_mesh()->vertexmap (s_idx);
      gpv->i_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cout, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                  0, src_vertex->id(), src_vertex->pt().x(), 
                  src_vertex->pt().y(), src_vertex->pt().z());
      //Draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      //Perform geodesic distance transform
      //Surazhsky & Kirsanov's original interval based method
      gpv->i_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso;
      if (dbmsh3d_cmd_ntestcontour() !=0)
        vis_iso = gdt_draw_result (gpv->f_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      gpv->i_manager()->print_results (dbgdt3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1)
          _root->addChild (vis_iso);

        gpv->gdt_mesh()->build_IFS_mesh ();
  
        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw only intervals on the front
          vis = gdt_draw_I_on_front_flag (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);

          //draw the whole mesh as one object
          vis = draw_M (gpv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);
        }
        else if (dbmsh3d_cmd_v()==1) {
          //: a temporary solution, to make all edges inactive!
          gpv->i_manager()->gdt_mesh()->init_traverse();

          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (gpv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (gpv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery(), 3);
          _root->addChild (vis);
        }
      }

      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += gpv->dir_file();
    }

    //############### 2 Proposed Face-based propagation via gdt_f_manager ###############
    else if (dbgdt3d_cmd_gdt() == GDT_METHOD_F) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) gpv->gdt_mesh()->vertexmap (s_idx);
      gpv->f_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cout, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      gpv->f_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso = gdt_draw_result (gpv->f_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      gpv->f_manager()->print_results (dbgdt3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1)
          _root->addChild (vis_iso);

        gpv->gdt_mesh()->build_IFS_mesh ();
      
        gpv->f_manager()->mark_front_edges_visited ();

        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw the whole mesh as one object
          vis = draw_M (gpv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          //draw only intervals on the front
          vis = gdtf_draw_I_on_front (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
        else {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (gpv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (gpv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
      }

      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += gpv->dir_file();
    }

    //############### 3 Proposed face-based propagation using shocks via gdt_fs_manager ###############
    else if (dbgdt3d_cmd_gdt() == GDT_METHOD_FS) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) gpv->gdt_mesh()->vertexmap (s_idx);
      gpv->fs_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cout, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      gpv->fs_manager()->run_gdt (n_iter);

      ///SoSeparator* vis_iso = gdt_draw_result (&fs_manager, dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      gpv->fs_manager()->print_results (dbgdt3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx);

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        ///if (dbmsh3d_cmd_ntestcontour() > 1)
          ///_root->addChild (vis_iso);

        gpv->gdt_mesh()->build_IFS_mesh ();
      
        gpv->fs_manager()->mark_front_edges_visited ();

        //: draw the mesh
        if (dbmsh3d_cmd_v()==0) {
          //draw the whole mesh as one object
          vis = draw_M (gpv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          //draw only intervals on the front
          vis = gdtf_draw_I_on_front (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
        else if (dbmsh3d_cmd_v()==1) {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (gpv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (gpv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw gdt_mesh intervals
          vis = gdt_draw_intervals (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }
      }
      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += gpv->dir_file();
    }

    //############### 4 Proposed simutaneous discrete wavefront and shock propagation via gdt_ws_manager ###############
    if (dbgdt3d_cmd_gdt() == GDT_METHOD_WS) {
      dbmsh3d_gdt_vertex_3d* src_vertex = (dbmsh3d_gdt_vertex_3d*) gpv->gdt_mesh()->vertexmap (s_idx);
      gpv->ws_manager()->add_source_vertex (src_vertex);
      vul_printf (vcl_cout, "Source_vertex[%d]: id[%d] (%lf, %lf, %lf)\n",
                   0, src_vertex->id(),
                   src_vertex->pt().x(), 
                   src_vertex->pt().y(), 
                   src_vertex->pt().z());
      //: draw the vertex 0 in a sphere
      _root->addChild (draw_sphere (src_vertex->pt(), dbmsh3d_cmd_r(), SbColor(0.0f,1.0f,1.0f)));

      gpv->ws_manager()->run_gdt (n_iter);

      SoSeparator* vis_iso = NULL;
      if (n_iter == (unsigned int) -1)
        vis_iso = gdt_draw_result (gpv->ws_manager(), dbmsh3d_cmd_ntestcontour(), dbmsh3d_cmd_verbose(), 3);

      gpv->ws_manager()->print_results (dbgdt3d_cmd_gdt(), dbmsh3d_cmd_fileprefix(), s_idx, n_iter == (unsigned int) -1);
      gpv->ws_manager()->print_statistics ();

      if (dbmsh3d_cmd_gui()) {
        //: draw iso-contours
        if (dbmsh3d_cmd_ntestcontour() > 1 && n_iter == (unsigned int) -1)
          _root->addChild (vis_iso);

        gpv->gdt_mesh()->build_IFS_mesh ();

        //: draw the mesh
        if (dbmsh3d_cmd_v() < 2) {
          //draw the whole mesh as one object
          vis = draw_M (gpv->gdt_mesh(), dbmsh3d_cmd_shapehints());
          _root->addChild (vis);

          gpv->gdt_mesh()->init_traverse ();
          vis = gdt_draw_edges_vispt (gpv->gdt_mesh(), 3);
          _root->addChild (vis);
        }
        else {
          //draw each mesh face as a separate object
          vis = gdt_draw_faces_vispt (gpv->gdt_mesh(), true, 0.0f);
          _root->addChild (vis);

          //draw each mesh edge as a separate object
          vis = gdt_draw_edges_vispt (gpv->gdt_mesh(), 3);
          _root->addChild (vis);

          //draw finalized gdt_mesh intervals
          vis = gdt_draw_welms (gpv->gdt_mesh(), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);

          //draw intervals in the queue Qw
          vis = gdtws_draw_W_in_Qw (gpv->ws_manager(), SbColor (0.2f, 0.2f, 1.0f), dbmsh3d_cmd_ntestquery());
          _root->addChild (vis);
        }

        if (dbmsh3d_cmd_v() > 0) {
          //draw each mesh vertex as a separate object
          vis = gdt_draw_vertices_vispt (gpv->gdt_mesh(), dbmsh3d_cmd_r());
          _root->addChild (vis);

          //draw each shock node as a separate object
          vis = gdt_draw_snodes_vispt (gpv->ws_manager(), dbmsh3d_cmd_r());
          _root->addChild (vis);

        }

        // Draw the shocks
        vis = gdt_draw_shocks (gpv->ws_manager(), 5);
        _root->addChild (vis);
      }
      dbmsh3d_app_window_title += " -- Geodesic DT on ";
      dbmsh3d_app_window_title += gpv->dir_file();
    }
  }

  return _root;
}


