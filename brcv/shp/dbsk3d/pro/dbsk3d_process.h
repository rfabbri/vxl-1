//: This is lemsvxlsrc/brcv/shp/dbsk3d/pro/dbsk3d_process.h
//  Creation: Dec 24, 2004   Ming-Ching Chang

#ifndef dbsk3d_process_h_
#define dbsk3d_process_h_

#include <vcl_string.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_richmesh.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>
#include <dbsk3d/dbsk3d_fs_comp_set.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>
#include <dbsk3d/algo/dbsk3d_fs_segre.h>
#include <dbsk3d/algo/dbsk3d_fs_inflow_prop.h>
#include <dbsk3d/algo/dbsk3d_fs_regul.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_build.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_fmm.h>
#include <dbsk3d/algo/dbsk3d_sg_sa_algos.h>
#include <dbsk3d/pro/dbsk3d_cmdpara.h>

//=============================================================
//: The computation parameters
class dbsk3d_proc_param
{
public:

  float   trans_A5_th_;

  int     n_smooth_;

public:
  //: Initialize the Default Parameter Values
  dbsk3d_proc_param () {

    trans_A5_th_  = 2.0f;

    n_smooth_ = 5;
  }
};

class dbsk3d_pro : public dbmsh3d_pro_base
{
public:
  //: the parameter structure
  dbsk3d_proc_param       proc_param_;
  bool                    b_view_fs_;
  bool                    b_view_cms_;
  bool                    b_view_sg_;

protected:
  //: main data structure 
  dbsk3d_fs_mesh*         fs_mesh_;
  dbsk3d_fs_sheet_set*    fs_ss_;
  dbsk3d_fs_comp_set*     fs_comp_set_;
  dbsk3d_ms_hypg*         ms_hypg_;
  dbsk3d_sg_sa*           sg_sa_;
  
  //: number of original point generators.
  //  num_orig_gene_ to bndset_->vertexmap().size(): 
  //    generators on the bounding sphere that simulate points at infinity.
  int   n_orig_gene_;

  //: the process object
  dbsk3d_fs_segre*        fs_segre_;
  dbsk3d_fs_inflow_prop*  fs_ifgp_;
  dbsk3d_fs_regul*        fs_regul_;
  dbsk3d_ms_hypg_build*   ms_hypg_build_;  
  dbsk3d_ms_hypg_trans*   ms_hypg_trans_;
  dbsk3d_ms_hypg_fmm*     ms_hypg_fmm_;
  dbsk3d_sg_sa_trans*     sg_sa_trans_;

public:
  //====== Constructor/Destructor ======
  dbsk3d_pro () : dbmsh3d_pro_base () {
    fs_mesh_        = new dbsk3d_fs_mesh (mesh_);
    fs_ss_          = new dbsk3d_fs_sheet_set (fs_mesh_);
    fs_comp_set_    = new dbsk3d_fs_comp_set (fs_ss_);
    ms_hypg_        = new dbsk3d_ms_hypg (fs_mesh_);
    sg_sa_          = new dbsk3d_sg_sa;
    
    n_orig_gene_ = -1;

    fs_segre_       = new dbsk3d_fs_segre (fs_mesh_);
    fs_ifgp_        = new dbsk3d_fs_inflow_prop (fs_mesh_);
    fs_regul_       = new dbsk3d_fs_regul (fs_ss_);
    ms_hypg_build_  = new dbsk3d_ms_hypg_build (fs_comp_set_, ms_hypg_);
    ms_hypg_trans_  = new dbsk3d_ms_hypg_trans (ms_hypg_);
    ms_hypg_fmm_    = new dbsk3d_ms_hypg_fmm (ms_hypg_);
    sg_sa_trans_    = new dbsk3d_sg_sa_trans (sg_sa_);

    b_view_fs_  = false;
    b_view_cms_ = false;
    b_view_sg_  = false;
  }

  virtual ~dbsk3d_pro () {
    delete ms_hypg_;
    delete fs_ss_;
    delete fs_mesh_;
    delete sg_sa_;
    
    delete fs_segre_;
    delete fs_ifgp_;
    delete fs_regul_;
    delete ms_hypg_build_;
    delete ms_hypg_trans_;
    delete ms_hypg_fmm_;
    delete sg_sa_trans_;
  }

  //====== Data access functions ======
  dbsk3d_fs_mesh* fs_mesh() {
    return fs_mesh_;
  }
  dbsk3d_fs_sheet_set* fs_ss() {
    return fs_ss_;
  }
  dbsk3d_fs_comp_set* fs_comp_set() {
    return fs_comp_set_;
  }
  dbsk3d_ms_hypg* ms_hypg() {
    return ms_hypg_;
  }

  dbsk3d_sg_sa* sg_sa() {
    return sg_sa_;
  }
  
  int n_orig_gene() {
    return n_orig_gene_;
  }
  void set_n_orig_gene (const int n) {
    n_orig_gene_ = n;
  }

  dbsk3d_fs_segre* fs_segre() {
    return fs_segre_;
  }
  dbsk3d_fs_inflow_prop* fs_ifgp() {
    return fs_ifgp_;
  }
  dbsk3d_fs_regul* fs_regul() {
    return fs_regul_;
  }
  dbsk3d_ms_hypg_trans* ms_hypg_trans() {
    return ms_hypg_trans_;
  }
  dbsk3d_ms_hypg_fmm* ms_hypg_fmm() {
    return ms_hypg_fmm_;
  }

  //=============================================================
  //: Object Status
  //  return true if the required data exist.
  bool has_bnd_mesh () {
    return mesh_->vertexmap().size() != 0;
  }
  bool has_fs_mesh () {
    return fs_mesh_->vertexmap().size() != 0;
  }
  bool has_ms_hypg () {
    return ms_hypg_->vertexmap().size() != 0;
  }
  bool has_sg_sa () {
    return sg_sa_->vertexmap().size() != 0;
  }

  //====== Reset the Object ======
  virtual void reset_mesh () {
    //vul_printf (vcl_cout, "reset dbsk3d_pro::mesh.\n");
    delete mesh_;
    mesh_ = new dbmsh3d_mesh ();
    fs_mesh_->set_bnd_mesh (mesh_);
  }
  
  void reset_shock_data () {
    reset_fs_mesh ();
    reset_fs_segre ();
    reset_ms_hypg ();
    reset_sg_sa ();
  }

  void reset_fs_mesh () {
    delete fs_mesh_;
    fs_mesh_ = new dbsk3d_fs_mesh (mesh_);
    fs_ss_->set_fs_mesh (fs_mesh_);
    fs_segre_->set_fs_mesh (fs_mesh_);
  }
  void reset_fs_segre () {
    delete fs_segre_;
    fs_segre_ = new dbsk3d_fs_segre (fs_mesh_);
  }
  void reset_ms_hypg () {
    delete ms_hypg_;
    ms_hypg_ = new dbsk3d_ms_hypg (fs_mesh_);
    ms_hypg_build_->set_ms_hypg (ms_hypg_);
    ms_hypg_trans_->set_ms_hypg (ms_hypg_);
    ms_hypg_fmm_->set_ms_hypg (ms_hypg_);
  }
  //: clear the MSH and its related FSM and BND.
  void clear_ms_hypg () {
    delete mesh_;
    mesh_ = NULL;
    delete fs_mesh_;
    fs_mesh_ = NULL;
    delete ms_hypg_;
    ms_hypg_ = NULL;
  }
  void set_ms_hypg (dbsk3d_ms_hypg* MSH) {
    ms_hypg_ = MSH;
    fs_mesh_ = MSH->fs_mesh();
    mesh_ = MSH->bnd_mesh();
  }
  void reset_sg_sa () {
    delete sg_sa_;
    sg_sa_ = new dbsk3d_sg_sa ();
  }

  //=============================================================
  //: Processes

  virtual void apply_xform_hmatrix ();

  void add_perturb (const float perturb);

  bool shock_detect_bucketing ();
  bool shock_detect_flow_sheet ();

  void add_bndsphere (const float radius_ratio, const int nsample_ratio);
  bool fs_recover_qhull (vcl_string filename = "");
  
  void run_rmin_trim_xforms (const float rmin_ratio, const bool reasgn_lost_genes);

  void run_rmax_trim_xforms (const float rmin_ratio, const bool reasgn_lost_genes);

  void run_compactness_trim_xforms (const int iter, const float c_th, const bool reasgn_lost_genes);
  
  void run_bbox_pruning (const float box_ratio, const bool reasgn_lost_genes);

  void prune_shocks_of_holes ();

  bool determine_inside_shock_comp (vcl_set<int>& ith_comp_list,
                                    const bool reasgn_lost_genes);

  bool build_ms_hypg (const int MS_topo_opt = 0);

  void splice_ms_elm_2_xforms ();

  //: greedy regularization of all transforms.
  void ms_trans_regul (const bool b_merge_xform,
                       const int tab_th, const int A5_th,
                       const int cc_th, const int sc_th, 
                       const int nnm_th, const int ncm_th, const int ccm_th,
                       const int ncm_a1a5_th, const int cmxth,
                       const bool reasgn_lost_genes, 
                       const int debug_stop_id1=-1, const int debug_stop_id2=-1);

  //: add virtual curves to the MSH.
  void add_MS_virtual_curves (const int tab_th, const int A5_th,
                              const int cc_th, const int sc_th);


  void smooth_ms_hypg_ribs_within_face (const float psi, const int iter, const float tiny_rib_ratio);
  void smooth_ms_hypg_ribs_DCS (const float psi, const int iter, const float tiny_rib_ratio);
  void smooth_ms_hypg_ribs_gaussian (const int iter, const float sigma);

  void build_ms_graph_sa_from_cms ();

  //: Scaffold Graph Transition Removal
  bool graph_trans_A5 ();
  bool graph_trans_A15 (const float cc_th);
  bool graph_trans_remove_dummy_nodes ();

  bool ms_graph_sa_smooth ();

  //====== Mega Process ======
  void run_surface_meshing (const float radius_ratio, const int nsample_ratio,
                            const float max_sample_ratio, const int topo_option,
                            const bool clear_mem);

  //=============================================================
  //: File I/O
  
  //: Save the unmeshed surface points to a -surfpt.p3d file.
  void save_surfpt (vcl_string filename = "");

  bool save_p3d_with_sphere (vcl_string filename = "");

  bool save_fs (vcl_string filename = "");
  bool load_fs (vcl_string filename = "", const bool read_gene = true);

  void save_cms (vcl_string filename = "");
  bool load_cms (vcl_string filename = "", const bool load_surf = true);

  void save_sg (vcl_string filename = "");
  bool load_sg (vcl_string filename = "", const bool load_surf = true);

  bool save_fs_vtk (vcl_string filename = "");

  void print_mem_usage ();

};

//###########################################################################
//  The Surface Mesh Reconstruction Wrapping Function.

dbmsh3d_mesh* run_surface_meshing (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                                   const vcl_vector<vcl_vector<int> >& init_faces,
                                   const vcl_string& dirprefix,
                                   const float radius_ratio, const int nsample_ratio,
                                   const float max_sample_ratio, const int topo_option,
                                   double& d_median);

//The recovered id in the richvertex's "id" property.
dbmsh3d_richmesh* run_surface_meshing_rm (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                                          const vcl_vector<vcl_vector<int> >& init_faces,
                                          const vcl_string& dirprefix,
                                          const float radius_ratio, const int nsample_ratio,
                                          const float max_sample_ratio, const int topo_option,
                                          double& d_median);

void surface_meshing_pro (dbsk3d_pro* p1,
                          const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& input_idpts,
                          const vcl_vector<vcl_vector<int> >& init_faces,
                          const vcl_string& dirprefix,
                          const float radius_ratio, const int nsample_ratio,
                          const float max_sample_ratio, const int topo_option,
                          double& d_median);

// ###########################################################################

void get_sausage_init_faces (const vcl_vector<vgl_point_3d<double> >& input_pts, 
                             //const vcl_vector<vcl_vector<int> >& stitchS_faces, 
                             dbmsh3d_mesh* stitchM, 
                             const vgl_box_3d<double>& sboxin, 
                             const vgl_box_3d<double>& sboxout, 
                             vcl_vector<vcl_vector<int> >& S_initM_faces);

// ###########################################################################

void get_stitch_surface_pts (dbmsh3d_pt_bktstr* BktStruct, 
                             vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& stitchS_idpts, 
                             const float bkt_box_ratio);

void get_pts_local_bucket_meshing (const dbmsh3d_pt_bucket* B, 
                                   const vcl_vector<vcl_vector<int> >& B_initM_faces, 
                                   const vcl_vector<vgl_point_3d<double> >& all_pts, 
                                   vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& B_meshing_idpts);

int merge_meshes (const vcl_vector<vgl_point_3d<double> >& finalM_pts, 
                  vcl_vector<vcl_vector<int> >& finalM_faces, 
                  dbmsh3d_mesh* M);

int check_merge_meshes (const vcl_vector<vgl_point_3d<double> >& finalM_pts, 
                        vcl_vector<vcl_vector<int> >& finalM_faces, 
                        dbmsh3d_mesh* M);

dbmsh3d_mesh* create_intersect_mesh (const vcl_vector<vgl_point_3d<double> >& pts, 
                                     vcl_vector<vcl_vector<int> >& faces,
                                     const vgl_box_3d<double>& M_box);

// ###########################################################################

//For click-to-delete shock objects.
void _init_click_to_del_shock_ptr (dbsk3d_pro* spv0);

#endif


