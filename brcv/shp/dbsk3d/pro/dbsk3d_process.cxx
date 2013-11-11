//: This is lemsvxlsrc/brcv/shp/dbsk3d/pro/dbsk3d_process.cxx
//  Creation: Dec 24, 2005   Ming-Ching Chang

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_vector_fixed.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_kd_tree.h>

#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/flowsheet/dbsk3d_fs_flowsheet.h>
#include <dbsk3d/flowsheet/dbsk3d_fs_flowsheet_bucket.h>
#include <dbsk3d/algo/dbsk3d_fs_fileio.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_algos.h>
#include <dbsk3d/algo/dbsk3d_ms_fileio.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>
#include <dbsk3d/algo/dbsk3d_sg_sa_algos.h>
#include <dbsk3d/pro/dbsk3d_process.h>

//###################################################################

void dbsk3d_pro::apply_xform_hmatrix ()
{  
  dbmsh3d_pro_base::apply_xform_hmatrix ();

  //rigid transform the position of shocks.
  dbmsh3d_apply_xform (fs_mesh_, hmatrix_);

  //rigid transform the position of shocks.
  dbsk3d_apply_xform (sg_sa_, hmatrix_);
}

//: Add noise to the boundary point set.
//  max perturbation is the % * avg_sample_dist.
void dbsk3d_pro::add_perturb (const float perturb)
{  
  if (perturb != 0) {
    float max_perturb = perturb * fs_mesh_->median_A122_dist();
    perturb_points (mesh_, max_perturb);
  }
}

bool dbsk3d_pro::shock_detect_bucketing ()
{
  //: New implementation of the FlowSheet method.
  run_flowsheet_bucketing (mesh_, dir_file().c_str());

  return true;
}

bool dbsk3d_pro::shock_detect_flow_sheet ()
{  
  //: Frederic 's FlowSheet method.
  FreShockFlowSheet (dir_file().c_str());
  
  return true;
}

void dbsk3d_pro::add_bndsphere (const float bs_radius_ratio, const int bs_sample_ratio)
{
  add_bndsphere_genes (mesh_, bs_radius_ratio, bs_sample_ratio);
}

void dbsk3d_pro::save_surfpt (vcl_string filename)
{
  if (filename == "")
    filename = dir_prefix() + "-surfpt.p3d";
  save_unmeshed_p3d (mesh_, filename.c_str());
}

bool dbsk3d_pro::save_p3d_with_sphere (vcl_string filename)
{
  if (filename == "")
    filename = dir_prefix();
  vcl_string strFile_P3D = filename + "-sphere.p3d";

  dbmsh3d_save_p3d (mesh_, strFile_P3D.c_str());

  return true;
}

bool dbsk3d_pro::fs_recover_qhull (vcl_string filename)
{
  vul_printf (vcl_cout, "\ndbsk3d_bndshock::fs_recover_qhull()\n");
  assert (has_fs_mesh() == false);
  if (filename == "")
    filename = dir_prefix();

  //Assume the input is dir_prefix() + "-sphere.p3d";
  vcl_string strFile_Sphere_P3D = filename + "-sphere.p3d";
  vcl_string strFile_Sphere_VOR = filename + "-sphere.vor";

  //Read the number of original generators (not including the bndshpere).

  mesh_->clear();
  dbmsh3d_load_p3d (mesh_, strFile_Sphere_P3D.c_str());

  //: Pass 1 of process the *.vor file
  vcl_vector<vcl_vector<dbmsh3d_face*> > V_F_incidence;
  bool result = load_from_vor_file (fs_mesh_, strFile_Sphere_VOR.c_str());
  if (result == false)
    return result;

  //: The edge is implicit in the IndexedFaceSet, need to create them.
  fs_mesh_->IFS_to_MHE_build_edges (true);
  assert (V_F_incidence.size() == 0);

  //: Pass 2 of process the *.vor file
  vcl_vector<vcl_vector<dbsk3d_fs_face*> > G_S_asso;
  result = load_from_vor_file_pass2 (fs_mesh_, strFile_Sphere_VOR.c_str(), 
                                     n_orig_gene_, G_S_asso);
  assert (result == true);

  //: Remove the bounding sphere and recover the fullshock.
  rm_bndsphere_shock_recovery (fs_mesh_, n_orig_gene_, G_S_asso);

  ///if (dbmsh3d_cmd_valid())
    ///result = fs_mesh_->valid_conn();

  return result;
}

bool dbsk3d_pro::save_fs (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".fs";
  else if (dbul_get_suffix (filename) == "")
    filename += ".fs";
  return save_to_fs (fs_mesh_, filename.c_str(), verbose_);
}

bool dbsk3d_pro::load_fs (vcl_string filename, const bool read_gene)
{
  if (filename == "")
    filename = dir_file() + ".fs";
  else if (dbul_get_suffix (filename) == "")
    filename += ".fs";
  return load_from_fs (fs_mesh_, filename.c_str(), read_gene, verbose_);
}

void dbsk3d_pro::run_rmin_trim_xforms (const float rmin_ratio, const bool reasgn_lost_genes)
{
  const double rmin_th = rmin_ratio * fs_mesh_->median_A122_dist();    
  vul_printf (vcl_cout, "run_rmin_trim_xforms(): rmin_th = %f (rmin_ratio = %.2f),\n",
              rmin_th, rmin_ratio);

  //Shock trimming using minimum radius.  
  rmin_trim_xforms (fs_ss_, float(rmin_th));

  //Delete invalid fs_face FF's (and maybe also fs_sheet S's)
  fs_ss_->remove_invalid_FF();  
  vul_printf (vcl_cout, "\n");

  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    fs_mesh_->check_all_G_asgn (unasgn_genes);
  }
}

void dbsk3d_pro::run_rmax_trim_xforms (const float rmax_ratio, const bool reasgn_lost_genes)
{
  const double rmax_th = rmax_ratio * fs_mesh_->median_A122_dist();    
  vul_printf (vcl_cout, "run_rmax_trim_xforms(): rmax_th = %f (rmax_ratio = %.2f),\n",
              rmax_th, rmax_ratio);

  //Shock trimming using maximum radius.  
  rmax_trim_xforms (fs_ss_, float(rmax_th));

  //Delete invalid fs_face FF's (and maybe also fs_sheet S's)
  fs_ss_->remove_invalid_FF();  
  vul_printf (vcl_cout, "\n");

  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    fs_mesh_->check_all_G_asgn (unasgn_genes);
  }
}

void dbsk3d_pro::run_compactness_trim_xforms (const int iter, const float c_th, const bool reasgn_lost_genes)
{
  //Prune boundary shock sheets with low compactness.
  shock_pruning_bnd_FF_compactness (fs_ss_, iter, c_th);

  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    bool result = fs_mesh_->check_all_G_asgn (unasgn_genes);
  }
}

void dbsk3d_pro::run_bbox_pruning (const float box_ratio, const bool reasgn_lost_genes)
{
  //Shock pruning using enlarged bounding box.
  shock_pruning_box (fs_ss_, box_ratio);

  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    bool result = fs_mesh_->check_all_G_asgn (unasgn_genes);
  }
}

void dbsk3d_pro::prune_shocks_of_holes ()
{
  //Remove the shock of generator near boundary holes.
  //Skip removing the 'completely interior' fs_faces.
  prune_shocks_of_bnd_holes (mesh_, fs_mesh_);
}

bool dbsk3d_pro::determine_inside_shock_comp (vcl_set<int>& ith_comp_list, const bool reasgn_lost_genes)
{  
  assert (ith_comp_list.size() != 0);
  unsigned int result = fs_comp_set_->label_shock_components ();
  if (result == 0)
    return false;
  fs_comp_set_->sort_shock_components ();

  unsigned int remain = fs_comp_set_->delete_unspecified_comps (ith_comp_list);

  if (ith_comp_list.find (-1) != ith_comp_list.end()) { //Use all components.
    vul_printf (vcl_cout, "\n all %u components contain %d fs_sheets (%d fs_faces).\n\n",
                fs_comp_set_->comp_list().size(), fs_comp_set_->fs_ss()->sheetmap().size(), remain);
  }
  else {    
    //Use only specified components.
    vcl_set<int>::iterator it = ith_comp_list.begin(); 
    for (; it != ith_comp_list.end(); it++) {
      int j = (*it);
      vul_printf (vcl_cout, "\n  %d-th component (id %d) contains %d fs_sheets.", 
                  j, fs_comp_set_->comp_list(j)->id(), fs_comp_set_->comp_list(j)->fs_sheets().size());
    }
    vul_printf (vcl_cout, "    total fs_faces: %d \n\n", remain);
  }

  if (reasgn_lost_genes) {
    //Check the assignment of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    fs_mesh()->check_all_G_asgn (unasgn_genes); 
    if (unasgn_genes.size() != 0) {
      //Try to recover assignment of 'lost' genes, two solutions:
      //1) build a kd-tree and search for closest fs_vertex.
      //2) traverse the medial scaffold from its native fs_face to a valid one.
      bool result = reasgn_lost_Gs_closest_FV (fs_mesh(), unasgn_genes);
      //reasgn_lost_Gs_via_FF (fs_mesh(), unasgn_genes);

      //Check the assignment of all generators again.
      if (result) {
        unasgn_genes.clear();
        bool result = fs_mesh()->check_all_G_asgn (unasgn_genes); 
        assert (result);
      }
    }
  }
  return true;
}

bool dbsk3d_pro::build_ms_hypg (const int MS_topo_opt)
{
  ms_hypg_ = ms_hypg_build_->build_ms_hypg (MS_topo_opt);  
  return ms_hypg_ != NULL;
}

//########################################################################

void dbsk3d_pro::run_surface_meshing (const float bs_radius_ratio, const int bs_sample_ratio,
                                      const float seg_max_size_ratio, const int seg_topo_option,
                                      const bool clear_mem)
{
  assert (pro_data_ == PD_MESH);
  vul_timer total_timer;
  vcl_string dirprefix = dir_prefix_ + "_tmp";

  //Adding Bounding Sphere. 
  set_n_orig_gene (mesh_->vertexmap().size());
  add_bndsphere_genes (mesh_, bs_radius_ratio, bs_sample_ratio);
  save_p3d_with_sphere (dirprefix);

  //Run QHull.
  vcl_string run_qhull_command_para = "qvoronoi s o Fv < ";
  run_qhull_command_para += dirprefix + "-sphere.p3d";
  run_qhull_command_para += " > ";
  run_qhull_command_para += dirprefix + "-sphere.vor";
  vcl_cout << "\nRun QHull: "<< run_qhull_command_para << vcl_endl;
  system (run_qhull_command_para.c_str());

  //Shock Recovery from QHull.  
  fs_recover_qhull (dirprefix);
  
  // Delete temporary files.
  vcl_string delete_command_para = "del ";
  delete_command_para += dirprefix + "-sphere.p3d";
  vcl_cout << "Delete temporary QHull file: "<< delete_command_para << vcl_endl;
  system (delete_command_para.c_str());

  delete_command_para = "del ";
  delete_command_para += dirprefix + "-sphere.vor";
  system (delete_command_para.c_str());

  //Run Surface Segregation.
  fs_segre_->run_surface_segre (seg_max_size_ratio, seg_topo_option, 1.0f, false);
  vcl_cerr << "\nTotal meshing time (BS + QHull + Shock Recovery + Shock Seg.): " 
           << total_timer.real() << " ms.\n";

  //Clear the memory of shock and other processing objects.
  if (clear_mem) {
    reset_fs_mesh ();
    reset_fs_segre ();
  }
}

//########################################################################

void dbsk3d_pro::save_cms (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".cms";
  if (dbul_get_suffix (filename) == "")
    filename += ".cms";
  save_to_cms (ms_hypg_, filename.c_str(), verbose_);
}

bool dbsk3d_pro::load_cms (vcl_string filename, const bool load_surf)
{
  if (filename == "")
    filename = dir_file();
  vcl_string dir_file = dbul_get_dir_file (filename);
  vcl_string dir_prefix = dbul_get_dir_prefix (filename);

  //Load the fine-scale medial scaffold (dir_file.fs) first.
  vcl_string fs_file = dir_file + ".fs";
  if (load_fs (fs_file) == false)
    return false;

  //Setup the pointer of the SHG to the underlying FS
  ms_hypg_->set_mesh (fs_mesh_);

  if (load_surf) { //Load the surface faces from mesh files (*-surface.ply, etc.).
    if (load_faces_files (dir_prefix + "-surface"))
      mesh_->IFS_to_MHE();
    ///else
      ///load_points_files (dir_prefix);
  }

  //Now load the coarse-scale medial scaffold file (*.cms).
  if (dbul_get_suffix (dir_file) == "")
    dir_file += ".cms";
  return load_from_cms (ms_hypg_, dir_file.c_str(), verbose_);
}

// #####################################################################

void dbsk3d_pro::splice_ms_elm_2_xforms ()
{
  //Collect the set of fs_faces sharing in 3+ ms_curves.
  int n_xform_th = 2;
  vcl_vector<vcl_pair<dbsk3d_ms_sheet*, dbsk3d_fs_face*> > SS_Ps_n_SCxforms;
  get_SS_P_n_SCxforms (ms_hypg_, n_xform_th, SS_Ps_n_SCxforms);

  //Perform splice xform on the Ps if they are on shock boundary.
  //for (unsigned int i=0; i<Ps_n_SCxforms.


}

// #####################################################################

//: greedy regularization of all transforms.
void dbsk3d_pro::ms_trans_regul (const bool b_merge_xform,
                                 const int tab_th, const int A5_th,
                                 const int cc_th, const int sc_th, 
                                 const int nnm_th, const int ncm_th, const int ccm_th,
                                 const int ncm_a1a5_th, const int cmxth,
                                 const bool reasgn_lost_genes, 
                                 const int debug_stop_id1, const int debug_stop_id2)
{
  //Brute-force debug:
  vul_printf (vcl_cout, "\tms_hypg.check_integrity() before xforms.");
  if (ms_hypg_->check_integrity())
    vul_printf (vcl_cout, "pass.");
  else
    vul_printf (vcl_cout, "fail!");

  vul_printf (vcl_cout, "\nperform_ms_trans_regul():\n");
  assert (fs_mesh_->is_MHE());

  ms_hypg_trans_->set_b_merge_xform (b_merge_xform);
  ms_hypg_trans_->ms_trans_regul_set_params (0.5, tab_th, 1, cc_th, 0.3, A5_th, 0.8, sc_th,
                                             1, nnm_th, 1, ncm_th, 1, ccm_th, ncm_a1a5_th, cmxth);
  ms_hypg_trans_->ms_trans_regul_init ();
  ms_hypg_trans_->ms_trans_regul_iters (debug_stop_id1, debug_stop_id2);
  
  //Brute-force debug:
  vul_printf (vcl_cout, "\tms_hypg.check_integrity() after xforms.");
  if (ms_hypg_->check_integrity())
    vul_printf (vcl_cout, "pass.\n\n");
  else
    vul_printf (vcl_cout, "fail!\n\n");
  
  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    fs_mesh_->check_all_G_asgn (unasgn_genes);
    
    if (unasgn_genes.size() != 0) {
      //Recover assignment of 'lost' genes, two solutions:
      //1) build a kd-tree and search for closest fs_vertex.
      //2) traverse the medial scaffold from its native fs_face to a valid one.
      bool result = reasgn_lost_Gs_closest_FV (fs_mesh(), unasgn_genes);
      //reasgn_lost_Gs_via_FF (fs_mesh(), unasgn_genes);
      
      if (result) { //Check the assignment of all generators again.
        unasgn_genes.clear();
        bool result = fs_mesh()->check_all_G_asgn (unasgn_genes); 
        assert (result);
      }
    }
  }
}

// #####################################################################

void dbsk3d_pro::add_MS_virtual_curves (const int tab_th, const int A5_th,
                                        const int cc_th, const int sc_th)
{
  //Brute-force debug:
  vul_printf (vcl_cout, "\tms_hypg.check_integrity() before xforms.");
  if (ms_hypg_->check_integrity())
    vul_printf (vcl_cout, "pass.");
  else
    vul_printf (vcl_cout, "fail!");

  assert (fs_mesh_->is_MHE());

  //Skip all merge transforms (for now).
  ms_hypg_trans_->set_b_merge_xform (false);
  float nnm_th=0, ncm_th=0, ccm_th=0, ncm_a1a5_th=0, cmxth=0;

  ms_hypg_trans_->ms_trans_regul_set_params (0.5, tab_th, 1, cc_th, 0.3, A5_th, 0.8, sc_th,
                                             1, nnm_th, 1, ncm_th, 1, ccm_th, ncm_a1a5_th, cmxth);
  ms_hypg_trans_->ms_trans_regul_init ();

  //Add virtual curves from the detected MS transition queue.
  ms_hypg_trans_->add_trans_virtual_curves ();
}

// #####################################################################

void dbsk3d_pro::smooth_ms_hypg_ribs_within_face (const float psi, const int iter, const float tiny_rib_ratio)
{
  ms_hypg_rib_smooth_within_face (ms_hypg_, psi, iter, tiny_rib_ratio);
}

void dbsk3d_pro::smooth_ms_hypg_ribs_DCS (const float psi, const int iter, const float tiny_rib_ratio)
{
  ms_hypg_rib_smooth_DCS (ms_hypg_, psi, iter, tiny_rib_ratio);
}

void dbsk3d_pro::smooth_ms_hypg_ribs_gaussian (const int iter, const float sigma)
{
  ms_hypg_rib_smooth_gaussian (ms_hypg_, iter, sigma);
}

// #####################################################################

void dbsk3d_pro::build_ms_graph_sa_from_cms ()
{
  //Release memory of the previous sg_sa
  if (sg_sa_)
    delete sg_sa_;
  sg_sa_ = build_sg_sa (ms_hypg_);
  sg_sa_trans_->set_sg_sa (sg_sa_);
}

void dbsk3d_pro::save_sg (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".sg";
  if (dbul_get_suffix (filename) == "")
    filename += ".sg";
  save_to_sg (sg_sa_, filename.c_str());
}

bool dbsk3d_pro::load_sg (vcl_string filename, const bool load_surf)
{
  if (filename == "")
    filename = dir_file() + ".sg";
  if (dbul_get_suffix (filename) == "")
    filename += ".sg";
  vcl_string dir_prefix = dbul_get_dir_prefix (filename);
  
  bool result = load_from_sg (sg_sa_, filename.c_str());

  if (load_surf) { //Load the surface mesh (*-surface.ply).
    if (load_meshes_files (dir_prefix + "-surface"))
      mesh_->IFS_to_MHE();
    else
      load_points_files (dir_prefix);
  }

  return result;
}

bool dbsk3d_pro::save_fs_vtk (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".vtk";
  if (dbul_get_suffix (filename) == "")
    filename += ".vtk";
  fs_mesh_->reset_vertices_ids ();
  fs_mesh_->build_face_IFS ();  
  return save_fs_vtk_attrib (fs_mesh_, filename.c_str());
}

bool dbsk3d_pro::graph_trans_A5 ()
{
  return sg_sa_trans_->graph_trans_A5 (proc_param_.trans_A5_th_);
}

bool dbsk3d_pro::graph_trans_A15 (const float cc_th)
{
  return sg_sa_trans_->graph_trans_A15 (cc_th);
}

bool dbsk3d_pro::graph_trans_remove_dummy_nodes ()
{
  return sg_sa_trans_->graph_trans_remove_dummy_nodes ();
}


bool dbsk3d_pro::ms_graph_sa_smooth ()
{
  //Gaussian smooth the shock curves of the medial scaffold.
  gaussian_sm_sg_sa (sg_sa_, proc_param_.n_smooth_);
  return true;
}

void dbsk3d_pro::print_mem_usage ()
{
  unsigned int size;
  unsigned int total = 0;

  vul_printf (vcl_cout, "\n\n");
  vul_printf (vcl_cout, "            Object     Size (bytes)      #        Total\n");
  vul_printf (vcl_cout, "----------------------------------------------------------\n");

  size = sizeof (dbsk3d_fs_vertex) * fs_mesh_->vertexmap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_fs_vertex            %3u %8u    %9u\n",
               sizeof (dbsk3d_fs_vertex), fs_mesh_->vertexmap().size(), size);

  size = sizeof (dbsk3d_fs_edge) * fs_mesh_->edgemap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_fs_edge            %3u %8u    %9u\n",
               sizeof (dbsk3d_fs_edge), fs_mesh_->edgemap().size(), size);

  size = sizeof (dbsk3d_fs_face) * fs_mesh_->facemap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_fs_face           %3u %8u    %9u\n",
               sizeof (dbsk3d_fs_face), fs_mesh_->facemap().size(), size);

  size = sizeof (dbsk3d_ms_node) * ms_hypg_->vertexmap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_ms_node              %3u %8u    %9u\n",
               sizeof (dbsk3d_ms_node), ms_hypg_->vertexmap().size(), size);

  size = sizeof (dbsk3d_ms_curve) * ms_hypg_->edgemap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_ms_curve               %3u %8u    %9u\n",
               sizeof (dbsk3d_ms_curve), ms_hypg_->edgemap().size(), size);
  
  size = sizeof (dbsk3d_ms_sheet) * ms_hypg_->sheetmap().size();
  total += size;
  vul_printf (vcl_cout, "dbsk3d_ms_sheet               %3u %8u    %9u\n",
               sizeof (dbsk3d_ms_sheet), ms_hypg_->sheetmap().size(), size);

  vul_printf (vcl_cout, "----------------------------------------------------------\n");
  vul_printf (vcl_cout, "         Total                                %9u\n", total);
  
  vul_printf (vcl_cout, "dbsk3d_fs_mesh                 %u\n", sizeof (dbsk3d_fs_mesh));
  vul_printf (vcl_cout, "dbmsh3d_graph                  %u\n", sizeof (dbmsh3d_graph));
  vul_printf (vcl_cout, "dbsk3d_ms_hypg                 %u\n", sizeof (dbsk3d_ms_hypg));
}


//###########################################################################
//  The Surface Mesh Reconstruction Wrapping Function.

dbmsh3d_mesh* run_surface_meshing (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                                   const vcl_vector<vcl_vector<int> >& init_faces,
                                   const vcl_string& dirprefix,
                                   const float bs_radius_ratio, const int bs_sample_ratio,
                                   const float seg_max_size_ratio, const int seg_topo_option,
                                   double& d_median)
{
  dbsk3d_pro* sp = new dbsk3d_pro;
  surface_meshing_pro (sp, input_idpts, init_faces, dirprefix,
                       bs_radius_ratio, bs_sample_ratio, seg_max_size_ratio, seg_topo_option, 
                       d_median);

  //Clone resulting mesh and return.
  dbmsh3d_mesh* resultM = clone_mesh_ifs (sp->mesh());
  delete sp;
  return resultM;
}


dbmsh3d_richmesh* run_surface_meshing_rm (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                                          const vcl_vector<vcl_vector<int> >& init_faces,
                                          const vcl_string& dirprefix,
                                          const float bs_radius_ratio, const int bs_sample_ratio,
                                          const float seg_max_size_ratio, const int seg_topo_option,
                                          double& d_median)
{
  dbsk3d_pro* sp = new dbsk3d_pro;
  surface_meshing_pro (sp, input_idpts, init_faces, dirprefix,
                       bs_radius_ratio, bs_sample_ratio, seg_max_size_ratio, seg_topo_option, 
                       d_median);
  
  //Rich mesh properties to store the original vertex id.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  //Clone resulting mesh and return.
  dbmsh3d_richmesh* resultM = clone_richmesh_ifs (sp->mesh(), bkt_vplist, bkt_fplist);
  delete sp;
  return resultM;
}

void surface_meshing_pro (dbsk3d_pro* sp,
                          const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                          const vcl_vector<vcl_vector<int> >& init_faces,
                          const vcl_string& dirprefix,
                          const float bs_radius_ratio, const int bs_sample_ratio,
                          const float seg_max_size_ratio, const int seg_topo_option,
                          double& d_median)
{
  vul_timer total_timer;

  // Put all points in input_idpts to mesh_.
  for (unsigned int i=0; i<input_idpts.size(); i++) {
    int id = input_idpts[i].first;
    vgl_point_3d<double> FF = input_idpts[i].second;
    dbmsh3d_vertex* V = sp->mesh()->_new_vertex (id);
    V->get_pt().set (FF.x(), FF.y(), FF.z());
    sp->mesh()->_add_vertex (V);
  }

  // Adding Bounding Sphere. 
  sp->set_n_orig_gene (sp->mesh()->vertexmap().size());
  add_bndsphere_genes (sp->mesh(), bs_radius_ratio, bs_sample_ratio);
  sp->save_p3d_with_sphere (dirprefix);

  // Run QHull.
  vcl_string run_qhull_command_para = "qvoronoi s o Fv < ";
  run_qhull_command_para += dirprefix + "-sphere.p3d";
  run_qhull_command_para += " > ";
  run_qhull_command_para += dirprefix + "-sphere.vor";
  vcl_cout << "\nRun QHull: "<< run_qhull_command_para << vcl_endl;
  system (run_qhull_command_para.c_str());

  // Shock Recovery from QHull.  
  sp->fs_recover_qhull (dirprefix);

  //Build a kdtree for the input points.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_idpts (input_idpts);

  //Keep all mesh vertices into a vector.
  vcl_vector<dbmsh3d_vertex*> vertices;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = sp->mesh()->vertexmap().begin();
  for (; it != sp->mesh()->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    vertices.push_back (V);
  }
  sp->mesh()->vertexmap().clear();

  //Recover the original id of boundary mesh vertices.  
  for (unsigned int i=0; i<vertices.size(); i++) {
    dbmsh3d_vertex* V = vertices[i];

    //Find the closest id from V to idpts
    int top_n = 1;
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;

    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (V->pt().x(), V->pt().y(), V->pt().z());
    query_pt.set_cartesian (P3);
    kdtree->n_nearest (query_pt, top_n, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == top_n);
    int orig_id = near_neighbor_indices[0];
    orig_id = input_idpts[orig_id].first;

    V->set_id (orig_id);
    sp->mesh()->_add_vertex (V);
    if (orig_id >= sp->mesh()->vertex_id_counter())
      sp->mesh()->set_vertex_id_counter (orig_id+1);
  }
  vertices.clear();
  delete kdtree;
  
  // Delete temporary files.
  vcl_string delete_command_para = "del ";
  delete_command_para += dirprefix + "-sphere.p3d";
  vcl_cout << "Delete temporary QHull file: "<< delete_command_para << vcl_endl;
  system (delete_command_para.c_str());

  delete_command_para = "del ";
  delete_command_para += dirprefix + "-sphere.vor";
  system (delete_command_para.c_str());

  // Add init_faces to mesh_.
  for (unsigned int i=0; i<init_faces.size(); i++) {
    dbmsh3d_face* F = sp->mesh()->_new_face ();
    for (unsigned int j=0; j<init_faces[i].size(); j++) {     
      int id = init_faces[i][j];
      dbmsh3d_vertex* V = sp->mesh()->vertexmap (id);
      assert (V->id() == id);
      F->_ifs_add_bnd_V (V);
      V->set_meshed (true);
    }
    sp->mesh()->_add_face (F);
  }

  // Run Surface Segregation.
  sp->fs_segre()->run_surface_segre (seg_max_size_ratio, seg_topo_option, 1.0f, false);
  vcl_cerr << "\nTotal meshing time (Init. Data + BS + QHull + Sh Recov. + Shk. Seg.): " 
           << total_timer.real() << " ms.\n";

  d_median = sp->fs_mesh()->median_A122_dist();
}

// ###########################################################################

//For click-to-delete shock objects.
//This is the global pointer to the ms_hypg object to perform click-to-delete.
dbsk3d_pro* _spv_ = NULL;

void _init_click_to_del_shock_ptr (dbsk3d_pro* spv0)
{
  _spv_ = spv0;
}


