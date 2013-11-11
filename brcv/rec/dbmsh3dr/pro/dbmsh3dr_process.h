//: This is lemsvxlsrc/brcv/shp/dbmsh3dr/pro/dbmsh3dr_process.h
//  Feb 27, 2007   Ming-Ching Chang

#ifndef dbmsh3dr_process_h_
#define dbmsh3dr_process_h_

#include <vgl/algo/vgl_h_matrix_3d.h>

#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbmsh3dr/dbmsh3dr_match.h>
#include <dbmsh3dr/pro/dbmsh3dr_cmdpara.h>

class rsdl_kd_tree;

class dbmsh3dr_pro_base
{
protected: 
  //: Array of process objects.
  vcl_vector<dbmsh3d_pro_base*> pro_;

  //: Data files
  vcl_vector<vcl_string> data_files_;

  //: Alignment files
  vcl_vector<vcl_string> align_files_;

  //: store the best affine xform from pro_[1] to pro_[0].
  vgl_h_matrix_3d<double> hmatrix_01_;

  //: store the best affine xform from pro_[1] to pro_[0].
  rgrl_trans_rigid      r_xform_;

  //: final convergence error.
  double                error_;

  //: Result of error estimation.
  double                dist_min_;
  double                dist_mean_;
  double                dist_median_;
  double                dist_RMS_;
  double                dist_max_;

  //: store the min_dist from meshp2 to meshp
  //  dimension as # points in meshp2.
  vcl_vector<double>    min_dists_;

  vcl_vector<int>       min_ids_;
  vcl_vector<vgl_point_3d<double> > closest_pts_;

  int                   verbose_;

public:
  //====== Constructor/Destructor ======
  dbmsh3dr_pro_base (dbmsh3d_pro_base* p0, dbmsh3d_pro_base* p1) { 
    //Initialize 2 process objects.
    pro_.resize (2);
    pro_[0] = p0;
    pro_[1] = p1;
    hmatrix_01_.set_identity ();
    dist_min_ = -1.0f;
    dist_mean_ = -1.0f;
    dist_median_ = -1.0f;
    dist_RMS_ = -1.0f;
    dist_max_ = -1.0f;
    verbose_ = 1; //0:off, 1:critial, 2:few, 3:normal, 4+:verb.
  }
  virtual ~dbmsh3dr_pro_base () {    
    //for (unsigned int i=0; i<pro_.size(); i++)
    //  delete pro_[i]; // Do not delete here
    pro_.clear();
    min_dists_.clear ();
    min_ids_.clear ();
    closest_pts_.clear ();
  }

  //====== Data access functions ======
  vcl_vector<dbmsh3d_pro_base*>& pro() {
    return pro_;
  }
  dbmsh3d_pro_base* pro (const int i) {
    return pro_[i];
  }

  vcl_vector<vcl_string>& data_files() {
    return data_files_;
  }
  const vcl_string& data_files (const int i) {
    return data_files_[i];
  }
  vcl_vector<vcl_string>& align_files() {
    return align_files_;
  }
  const vcl_string& align_files (const int i) {
    return align_files_[i];
  }

  const vgl_h_matrix_3d<double>& hmatrix_01() const {
    return hmatrix_01_;
  }
  vgl_h_matrix_3d<double>& hmatrix_01() {
    return hmatrix_01_;
  }
  void set_hmatrix_01 (const vgl_h_matrix_3d<double>& H) {
    hmatrix_01_ = H;
  }
  bool is_hmatrix_01_identity () const {
    for (unsigned int r=0; r<4; r++) {
      for (unsigned int c=0; c<4; c++) {
        if (r==c) {
          if (hmatrix_01_.get(r,c) != 1)
            return false;
        }
        else {
          if (hmatrix_01_.get(r,c) != 0)
            return false;
        }
      }
    }
    return true;
  }
  const vcl_vector<double>& min_dists() {
    return min_dists_;
  }  
  const double& min_dists(const int i) const {
    return min_dists_[i];
  }

  double dist_min () {
    return dist_min_;
  }
  double dist_mean () {
    return dist_mean_;
  }
  double dist_median () {
    return dist_median_;
  }
  double dist_RMS () {
    return dist_RMS_;
  }
  double dist_max () {
    return dist_max_;
  }
  void set_verbose (const int v) {
    verbose_ = v;
  }
  
  //====== Processing ======
  virtual bool load_hmatrix_01 (vcl_string dirfile);

  //: Transform pro[1]'s data (depending on PD_PTS or PD_MESH, etc) using hmatrix_01_
  void xform_hmatrix_01_pro1 ();

  bool read_list_file (const char* list_file);

  bool save_all_pts_xyz (const char* file);

  bool save_all_to_ply2 (const char* file);
  
  //: run point-point ICP
  bool run_pp_icp_regstr (const int max_iter, const float conv_th, const float dist_th);
  //: run point-plane (triangle) ICP
  bool run_pf_icp_regstr (const int max_iter, const float conv_th, const float dist_th);
  bool run_rgrl_icp_oripts (const int subsam1, const int subsam2);
  
  //: Error estimation.
  bool compute_pp_error (const float dist_th);
  void normalize_pp_error (const float dist_th);
  bool compute_pf_error (const int top_n, const float dist_th);  
  void normalize_pf_error (const float dist_th);
  void output_min_dist (const char* filename);

  int get_overlapped_pts (const float dist_th);
  
  //: Compute the edge weight of the adjacency graph between scans.
  bool compute_adj_graph_weight (const float dthr,
                                 vnl_matrix<double>& adj_graph);

  
  //: Fuse scans using pt-mesh ICP following MST.
  //  Save results to alignment file scan_##_mst.txt
  bool fuse_scan_MST (const vcl_vector<vcl_pair<int, int> >& MST, 
                      const int root_sid, const float dthr,
                      const int max_ICP_iter, const float icpcv);

  //: Look for the next fusion edge in MST 
  //  connecting already fused (true) and un-visited (false) scans
  //  (stored in the fusion_idx[] array).
  //  Return true if any qualified such edge is found and set 
  //    cur_sid = already fused scan and next_sid = un-visited scan.
  //  Return false if such edge is not found (Fusing is done).
  bool find_next_scan_to_fuse (const vcl_vector<vcl_pair<int, int> >& MST, 
                               const vcl_vector<bool>& fusion_idx,                             
                               int& cur_sid, int& next_sid);

  //: Estimate surface variance along surface normal.
  double estimate_surf_var_along_normal (const float dthr, const int top_n, const int option);
  
  //: Reduce surface thickness by n_iter iterations.
  //  option 1: brute-force, option 4: estimate final avg. dist.
  //  option 2: use bucketing, option 5: estimate final avg. dist.
  //  option 3: only keep one vertex (hack), option 6: estimate final avg. dist.
  bool reduce_surf_thickness (const float dthr, const int n_iter, const int top_n,
                              const int option);
  
  //: Reduce surface thickness using bucketing.
  bool reduce_surf_thickness_bkt (const float dthr, const int n_iter, const int top_n,
                                  const int npbkt);
  
  bool setup_n_surfs (const float dthr, double& avg_sample_dist);

  //: Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
  double compute_M_M_avg_dist (const int top_n, const double& dist_th);
  
  //: Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
  //  Only consider top 1 closest vertices on mesh.
  double compute_M_M_avg_dist_1v (const double& dist_th);

  //: Reduce surfaces thickness by moving toward avg. closest point on other meshes.
  void move_surf_avg_closest_pt (const int top_n, const double& dist_th);

  //: Reduce surfaces thickness by moving toward avg. closest point on other meshes.
  //  Only consider top 1 closest vertices on mesh.
  //  This allows removing the kd-tree & keep only the closest pt for each mesh (of each vertex).
  void move_surf_avg_closest_pt_1v (const double& dist_th);
  
  void setup_M_V_CV (vcl_vector<vcl_vector<dbmsh3d_vertex*> > * M_V_CV);

  //: Reduce surfaces thickness by moving toward avg. closest point using bucketing.
  void move_surf_avg_closest_pt_bkt (const int top_n, const double& dist_th);

  void setup_M_V_CV_bkt (vcl_vector<vcl_vector<dbmsh3d_vertex*> > * M_V_CV);

  //: Loop through each pro_[].mesh and compute mesh-mesh avg-dist using bucketing.
  double compute_M_M_avg_dist_bkt (const int top_n, const double& dist_th);

};

class dbmsh3dr_pro : public dbmsh3dr_pro_base
{
public:
  //====== Constructor/Destructor ======
  dbmsh3dr_pro (dbmsh3d_pro_base* p0, dbmsh3d_pro_base* p1) : dbmsh3dr_pro_base (p0, p1) {
  }
  virtual ~dbmsh3dr_pro () {
  }


  //====== Data access functions ======
  dbmsh3d_pro* p0() {
    return (dbmsh3d_pro*) pro_[0];
  }
  dbmsh3d_pro* p1() {
    return (dbmsh3d_pro*) pro_[1];
  }

  //====== Processing ======

};

#endif
