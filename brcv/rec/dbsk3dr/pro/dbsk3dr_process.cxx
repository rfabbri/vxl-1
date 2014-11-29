//: This is dbsk3dr_process.cxx
//  Ming-Ching Chang
//  16 February 2005

#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <dbgl/dbgl_rigid_align.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbsk3dr/dbsk3dr_match.h>

#include <dbsk3dr/pro/dbsk3dr_process.h>

bool dbsk3dr_pro::run_shock_match (const int option, const bool register_rigid,
                                   const bool match_subset_of_curves,
                                   int max_ms_curves)
{
  if (verbose_)
    vul_printf (vcl_cout, "run_shock_match(): \n");
  shock_match_->set_match_subset_of_curves (match_subset_of_curves);

  //Initializing the matching of the ms_hypgs of processes p0() and p1().
  shock_match_->set_ms_hypg_G (p0()->ms_hypg());
  shock_match_->set_ms_hypg_g (p1()->ms_hypg());

  if (shock_match()->match_subset_of_curves()) {
    //find the min of # of ms_curves in G and g.
    int min_ms_curves = vcl_min (p0()->ms_hypg()->edgemap().size(), p1()->ms_hypg()->edgemap().size());
    if (max_ms_curves > min_ms_curves) {
      max_ms_curves = min_ms_curves;
      if (verbose_)
        vul_printf (vcl_cout, "  Set max_ms_curves to %d (min # ms_curves in G and g).\n", max_ms_curves);
    }
    const int ms_curve_min_elm = 10;
    p0()->ms_hypg()->select_salient_ms_curves (ms_curve_min_elm, max_ms_curves, verbose_);
    p1()->ms_hypg()->select_salient_ms_curves (ms_curve_min_elm, max_ms_curves, verbose_);
  }
  else {
    p0()->ms_hypg()->select_all ();
    p1()->ms_hypg()->select_all ();
  }

  // (vb 1 == debug_out 3).
  shock_match_->ga_match()->set_debug_out (verbose_); // + 2

  //Allocate nodes, also compute hypg similarity here!
  shock_match_->alloc_ga_hypg_G (verbose_);
  shock_match_->alloc_ga_hypg_g (verbose_);

  dbasn_params GA_params;
  if (verbose_)
    vcl_cout<< GA_params;
  shock_match_->ga_match()->set_params (GA_params);
  shock_match_->ga_match()->_reset_mem ();

  //1) Pre-processing for Matching.
  //option 1: shock graph matching.
  //option 2: shock hypergraph matching using curve length as compatibility.
  //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
  //option 4: shock hypergraph matching using D.P. curve distance as compatibility.
  //option 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
  //option 6: option 5 + plus integration of the global R in node compatibility.
  //option 7: option 3 plus integration of the global (R, T) in node compatibility.
  
  if (option==6 || option==7) {
    shock_match_->ga_match()->set_use_R_T (true);
    //Initialize the (R, T) from an initial guess (by sample the rotation space.
    shock_match_->ga_match()->init_R_T ();
    shock_match_->ga_match()->get_max_len_between_nodes ();
  }
  else
    shock_match_->ga_match()->set_use_R_T (false);

  if (option==1 || option==2 || option==3 || option==7)
    shock_match_->ga_match()->set_use_DP_cost (false);
  else {
    shock_match_->ga_match()->set_use_DP_cost (true);
    shock_match_->setup_G_g_dp_curve_dist (option, verbose_);
  }

  shock_match_->setup_G_g_curve_similarity (option, verbose_);
  shock_match_->setup_G_g_corner_similarity (verbose_);
  shock_match_->setup_G_g_corner_vl (verbose_);

  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  //option abs_max: true: use absolute max of G and g.
  //                false: normalize w.r.t. the max of each graph.
  //should use relative max to handle scale properly!!
  //true: discriminate objects of different scale.
  //false: allow matching across different scales.
  shock_match_->ga_match()->normalize_costs (true);
  
  if (verbose_) {
    //Print the final compatibility in tables.
    shock_match_->print_C_ai_table ();
    shock_match_->print_C_aibj_table ();
    shock_match_->print_C_aibjck_table ();
  }

  //2) Compute Matching (Assignment).
  shock_match_->compute_matching (verbose_);

  //Find the best alignment (best Euclidean Transform)
  //Move hypg_G (and its boundary shape) to hypg_g.
  if (register_rigid) {
    if (verbose_)
      vul_printf (vcl_cout, "\nRigidly transform G to g from the node & curve assignemnts:");
    //Option use node correspondence only: set to false.
    shock_match_->get_rigid_xform_matrices (false, verbose_);
    if (verbose_)
      print_R_C1_C2 (shock_match_->R(), shock_match_->Cf(), shock_match_->Cm());

    shock_match_->transform_scaffold_graph_1_to_2 ();
    shock_match_->transform_point_G_1_to_2 (p0()->mesh());    
  }
  else {
    if (verbose_)
      vul_printf (vcl_cout, "\nSkip the rigidly transform of G to g from the node assignemnt.\n");
  }

  //output the final matching score into a file.
  vcl_string smatch_match_result_file = "smatch";
  vcl_string tmp = vul_sprintf ("%d", option);
  smatch_match_result_file += tmp;
  smatch_match_result_file += "_";      
  vcl_string file1 = dbul_get_file(pro(0)->dir_file());
  vcl_string file2 = dbul_get_file(pro(1)->dir_file());
  smatch_match_result_file += file1;
  smatch_match_result_file += "__";
  smatch_match_result_file += file2;
  smatch_match_result_file += ".txt";
  save_shock_match_result_file (smatch_match_result_file, verbose_);

  return shock_match_->ga_match()->num_stable();
}

bool dbsk3dr_pro::save_shock_match_result_file (const vcl_string& filename, const int verbose)
{
  FILE*   fp = vcl_fopen (filename.c_str(), "w");
  if (fp == NULL) {
      vul_printf (vcl_cerr, "ERROR: Can't write file %s.\n", filename.c_str());
    return false; 
  }

  if (verbose)
    vul_printf (vcl_cout, "  saving %s\n", filename.c_str());

  vcl_fprintf (fp, "3d_shock_matching_result v1.0\n");
  vcl_fprintf (fp, "file1: %s\n", p0()->dir_file().c_str());
  vcl_fprintf (fp, "file2: %s\n", p1()->dir_file().c_str());
  double similarity = shock_match_->ga_match()->similarity ();
  vcl_fprintf (fp, "similarity: %f\n", similarity);
  double norm_similarity = shock_match_->ga_match()->norm_similarity ();
  vcl_fprintf (fp, "normalized_similarity: %f\n", norm_similarity);
  vcl_fprintf (fp, "norm_sim_node: %f\n", shock_match_->ga_match()->norm_sim_node());
  vcl_fprintf (fp, "norm_sim_curve: %f\n", shock_match_->ga_match()->norm_sim_curve());
  vcl_fprintf (fp, "norm_sim_corner: %f\n", shock_match_->ga_match()->norm_sim_corner());  
  bool num_stable = shock_match_->ga_match()->num_stable();
  vcl_fprintf (fp, "numerically_stable: %s\n", num_stable ? "1" : "0");
  vcl_fclose (fp);
  return true;
}

bool dbsk3dr_pro::run_shock_match_icp (const int max_iter, const float conv_th)
{
  if (verbose_)
    vul_printf (vcl_cout, "run_shock_match_icp(): max_iter %d, conv_th %f.\n", 
                max_iter, conv_th);
  bool b_conv;

  //Register G (moving) to g (fixed).
  vcl_vector<vgl_point_3d<double> > ptsG;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = p0()->mesh()->vertexmap().begin();
  for (; it != p0()->mesh()->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    ptsG.push_back (V->pt());
  }

  vcl_vector<vgl_point_3d<double> > ptsg;
  it = p1()->mesh()->vertexmap().begin();
  for (; it != p1()->mesh()->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    ptsg.push_back (V->pt());
  }

  b_conv = dbmsh3dr_pp_icp_regstr (ptsg, ptsG, max_iter, conv_th, FLT_MAX, hmatrix_01_);

  //Should transform pro_[0]->mesh() using hmatrix_01_.
  //But for now just copy value from ptsg to p0->mesh.
  it = p0()->mesh()->vertexmap().begin();
  for (unsigned int i=0; it != p0()->mesh()->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;
    V->get_pt().set (ptsG[i].x(), ptsG[i].y(), ptsG[i].z());
  }

  return b_conv;
}



