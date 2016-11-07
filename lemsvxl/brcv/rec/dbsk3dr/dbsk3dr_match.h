//: This is brcv/rec/dbsk3dr/dbsk3dr_match.h
// Graduated Assignment Shock Matching
// MingChing Chang
// June 2004.

#ifndef dbsk3dr_match_h_
#define dbsk3dr_match_h_

#include <vcl_string.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbasn/dbasnh_hypg.h>
#include <dbasn/dbasnh_gradasgn.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>

class dbsk3dr_match
{
protected:
  dbsk3d_ms_hypg*         ms_hypg_G_;
  dbsk3d_ms_hypg*         ms_hypg_g_;

  bool match_subset_of_curves_;

  dbasnh_gradasgn_aug*    ga_match_;

  //: map storing the mapping of <nid, sid> for G and g.
  vcl_map<int, int>       G_sid_;
  vcl_map<int, int>       g_sid_;
  //: map storing the mapping of <sid, nid> for G and g.
  vcl_map<int, int>       G_nid_;
  vcl_map<int, int>       g_nid_;

  //: the centroids and rotation matrix R for rigid transformation of registration.
  vnl_matrix_fixed<double,3,3> R_; 
  vnl_vector_fixed<double,3> Cf_; 
  vnl_vector_fixed<double,3> Cm_;

public:
  //###### Constructor & Destructor ######
  dbsk3dr_match () {
    ms_hypg_G_ = NULL;
    ms_hypg_g_ = NULL;
    match_subset_of_curves_ = false;
    ga_match_ = new dbasnh_gradasgn_aug ();
  }
  virtual ~dbsk3dr_match () {
    delete ga_match_;
  }

  //###### Data Access Functions ######
  dbsk3d_ms_hypg* ms_hypg_G () {
    return ms_hypg_G_;
  }
  void set_ms_hypg_G (dbsk3d_ms_hypg* G) {
    ms_hypg_G_ = G;
  }
  dbsk3d_ms_hypg* ms_hypg_g () {
    return ms_hypg_g_;
  }
  void set_ms_hypg_g (dbsk3d_ms_hypg* g) {
    ms_hypg_g_ = g;
  }

  dbasnh_hypg_aug* ga_hypg_G () {
    return ga_match_->hypg_G_aug();
  }
  dbasnh_hypg_aug* ga_hypg_g () {
    return ga_match_->hypg_g_aug();
  }

  dbasnh_gradasgn_aug* ga_match () {
    return ga_match_;
  }
  const bool match_subset_of_curves () const {
    return match_subset_of_curves_;
  }  
  void set_match_subset_of_curves (const bool b) {
    match_subset_of_curves_ = b;
  }

  //: nid is the sorted id of nodes_[]
  //  sid is the unsorted id for medial scaffold nodes
  //  If nid == -1 (slack in matching), just return -1.
  int G_sid (const int nid) {
    if (nid==-1)
      return -1;
    vcl_map<int, int>::iterator it = G_sid_.find (nid);
    assert (it != G_sid_.end());
    return (*it).second;
  }
  dbsk3d_ms_node* G_ms_node (const int nid) {
    int sid = G_sid (nid);
    return (dbsk3d_ms_node*) ms_hypg_G_->vertexmap (sid);
  }

  void G_add_ga_node (dbasn_node* GN, const int sid) {
    G_sid_.insert (vcl_pair<int, int> (GN->nid(), sid));
    G_nid_.insert (vcl_pair<int, int> (sid, GN->nid()));
    ga_hypg_G()->_add_node (GN);
  }
  void g_add_ga_node (dbasn_node* GN, const int sid) {
    g_sid_.insert (vcl_pair<int, int> (GN->nid(), sid));
    g_nid_.insert (vcl_pair<int, int> (sid, GN->nid()));
    ga_hypg_g()->_add_node (GN);
  }
  
  int g_sid (const int nid) {
    if (nid==-1)
      return -1;
    vcl_map<int, int>::iterator it = g_sid_.find (nid);
    assert (it != g_sid_.end());
    return (*it).second;
  }
  dbsk3d_ms_node* g_ms_node (const int nid) {
    int sid = g_sid (nid);
    return (dbsk3d_ms_node*) ms_hypg_g_->vertexmap (sid);
  }
  
  int G_nid (const int sid) {
    vcl_map<int, int>::iterator it = G_nid_.find (sid);
    assert (it != G_nid_.end());
    return (*it).second;
  }
  int g_nid (const int sid) {
    vcl_map<int, int>::iterator it = g_nid_.find (sid);
    assert (it != g_nid_.end());
    return (*it).second;
  }
  
  vnl_matrix_fixed<double,3,3>& R() {
    return R_; 
  }
  vnl_vector_fixed<double,3>& Cf() {
    return Cf_; 
  }
  vnl_vector_fixed<double,3>& Cm() {
    return Cm_;
  }

  //###### Matching Setup Functions ######  
  //: allocate the ga_hypg_G from ms_hypg_G
  void alloc_ga_hypg_G (const int verbose = 1);
  //: allocate the ga_hypg_g from ms_hypg_g
  void alloc_ga_hypg_g (const int verbose = 1);
  
  //option 4: shock hypergraph matching using D.P. curve distance as compatibility.
  //option 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
  //option 6: option 5 + plus integration of the global R in node compatibility.
  void setup_G_g_dp_curve_dist (const int option, const int verbose = 1);

  //option 2: shock hypergraph matching using curve length as compatibility.
  //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
  //option 7: option 3 plus integration of the global (R, T) in node compatibility.
  void setup_G_g_curve_similarity (const int option, const int verbose = 1);

  void setup_G_g_corner_similarity (const int verbose = 1);

  void setup_G_g_corner_vl (const int verbose = 1);

  void update_corner_N (dbasnh_hypg_aug* H, dbsk3d_ms_node* NA, dbsk3d_ms_node* NB, 
                        const int nidA, const int nidB, dbsk3d_ms_curve* inputMC);

  void update_vl_corner_N (dbasnh_hypg_aug* H, dbsk3d_ms_node* NA, dbsk3d_ms_node* NV, 
                           const int nidA, const int nidV, dbsk3d_ms_curve* VMC);

  
  //: Debug print the final compatibility in tables.
  void print_C_ai_table ();
  void print_C_aibj_table ();
  void print_C_aibjck_table ();

  //###### Functions for matching ######

  // Return false if numerical problem occurs.
  bool compute_matching (const int verbose = 1);

  void print_match_results ();

  //###### Functions for shock curve D.P. matching ######
  
  //option 1 : test 3d curve matching.
  //option 2 : test 3d shock curve matching.
  void test_mc_dp_match (const int option, const int cid1, const int cid2, const bool flip,
                         vcl_vector< vcl_pair<int,int> >& alignment);

  //###### Query functions after shock matching ######

  //: Given sid in g, return the matching sid in G. Return -1 for no match.
  int matched_G_sid (int g_sid);

  //: Given dbsk3d_ms_curve in graph_1, return the matching dbsk3d_ms_curve in graph_2
  //  if there is no match (any ending vertex not matched), return NULL
  //  the flip is returned true if the matching is flipped.
  dbsk3d_ms_curve* matched_g_curve (dbsk3d_ms_curve* G_curve, bool& flip);

  //: Given dbsk3d_ms_curve in graph_2, return the matching dbsk3d_ms_curve in graph_1
  //  if there is no match (any ending vertex not matched), return NULL
  ///dbsk3d_ms_curve* get_graph_1_match_curve (dbsk3d_ms_curve* ms_graph_2_curve);

  //###### Modification functions ######
  void get_rigid_xform_matrices (const bool node_only = true, const int verbose = 1);

  double get_curve_align_avg_Eu_dist (dbmsh3d_curve* C1, dbmsh3d_curve* C2, 
                                      vcl_vector< vcl_pair<int,int> >& alignment,
                                      vcl_vector<vgl_point_3d<double> >& cor_movPS, 
                                      vcl_vector<vgl_point_3d<double> >& cor_fixPS);

  void transform_scaffold_graph_1_to_2 ();
  void transform_point_G_1_to_2 (dbmsh3d_mesh* M);
};

#endif
