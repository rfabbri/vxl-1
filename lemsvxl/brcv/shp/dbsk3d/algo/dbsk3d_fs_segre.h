//: dbsk3d/dbsk3d_fs_segre.h
//  MingChing Chang
//  May 05, 2005        Creation.

#ifndef dbsk3d_fs_segre_h_
#define dbsk3d_fs_segre_h_

#include <vcl_queue.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>

#include <dbsk3d/algo/dbsk3d_mesh_bnd.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//##################### Surface Segregation #####################

//: Option for the segregation process.
typedef enum {
  STO_BOGUS         = -1,
  STO_2MANIFOLD     = 0,
  STO_NON2MANIFOLD  = 1,
} SEG_TOPO_OPTION;

typedef enum {
  SURF_TOPO_OK                = 0,
  SURF_TOPO_V_ONLY_INCIDENCE  = 1,
  SURF_TOPO_NON_2_MANIFOLD    = 2,
  SURF_TOPO_V_1_RING_NBR      = 3,
} SURF_TOPO_TYPE;

#define TOPO_FIX_HOLE_MAX_EDGES     5
#define TOPO_CHAIN_MAX_EDGES        9

//: class for shock segregation and surface meshing
class dbsk3d_fs_segre
{
protected:
  dbsk3d_fs_mesh* fs_mesh_;

  //: The parameter of maximum allowable surface interpolant size.
  float           max_sample_dist_;

  SEG_TOPO_OPTION topo_option_;
  int             greedy_option_;

  //: The classified priority queue of shock link elements.  
  vcl_multimap<double, dbsk3d_fs_edge*>  L_1st_queue_;
  vcl_multimap<double, dbsk3d_fs_edge*>  L_2nd_queue_;

  //: Variable to store the stopping cost threshold
  float           stop_rth_;

  //: A vector to store the backward association of bnd_face to shock_link.
  vcl_vector<dbsk3d_fs_edge*>   F_L_vector_;

  dbsk3d_bnd_chain_set*  mesh_bnd_chains_; 

public:
  //###### Constructor & Destructor ######
  dbsk3d_fs_segre (dbsk3d_fs_mesh* fs) {
    fs_mesh_          = fs;
    topo_option_      = STO_BOGUS;
    greedy_option_    = -1;
    max_sample_dist_  = -1.0f;     //value will be determined in the algorithm.
    mesh_bnd_chains_ = new dbsk3d_bnd_chain_set (fs_mesh_);
  }
  ~dbsk3d_fs_segre () {
    L_1st_queue_.clear();
    L_2nd_queue_.clear();
    F_L_vector_.clear();
    delete mesh_bnd_chains_;
  }
  float stop_rth () const {
    return stop_rth_;
  }

  //###### Data Accessing ######
  dbmsh3d_mesh* bnd_mesh() {
    return fs_mesh_->bnd_mesh();
  }
  void set_fs_mesh (dbsk3d_fs_mesh* fs_mesh) {
    fs_mesh_ = fs_mesh;
  }
  const double median_A122_dist () {
    return fs_mesh_->median_A122_dist();
  }

  void _add_to_L_1st_queue (const dbsk3d_fs_edge* FE) {
    L_1st_queue_.insert (vcl_pair<double, dbsk3d_fs_edge*> (FE->cost(), (dbsk3d_fs_edge*) FE));
  }  
  void _add_to_L_1st_queue (const double cost, const dbsk3d_fs_edge* FE) {
    L_1st_queue_.insert (vcl_pair<double, dbsk3d_fs_edge*> (cost, (dbsk3d_fs_edge*) FE));
  }  
  bool _remove_from_L_1st_queue (const dbsk3d_fs_edge* FE) {
    double key = FE->cost();  
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator lower = L_1st_queue_.lower_bound(key);
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator upper = L_1st_queue_.upper_bound(key);
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == FE) {
        L_1st_queue_.erase (it);
        return true;
      }
    }
    return false;
  }
  vcl_multimap<double, dbsk3d_fs_edge*>& L_1st_queue () {
    return L_1st_queue_;
  }
  vcl_multimap<double, dbsk3d_fs_edge*>& L_2nd_queue () {
    return L_2nd_queue_;
  }
    
  void _add_to_L_2nd_queue (const dbsk3d_fs_edge* FE) {
    ///assert (FE->cost() != SEG_OVERSIZE_COST); can be already meshed tris ??
    //For type I and II shocks, cost == ?
    //For type III shocks, cost = FE's # genes passing through.
    double cost;
    if (FE->flow_type() == '1' || FE->flow_type() == '2') {
      cost = 0;
    }
    else {
      vcl_set<dbmsh3d_vertex*> pass_genes;
      cost = FE->get_pass_Gs (pass_genes);
    }

    //L_2nd_queue_.insert (vcl_pair<double, dbsk3d_fs_edge*> (FE->cost(), (dbsk3d_fs_edge*) FE));
    L_2nd_queue_.insert (vcl_pair<double, dbsk3d_fs_edge*> (cost, (dbsk3d_fs_edge*) FE));
  }  
  bool _remove_from_L_2nd_queue (const dbsk3d_fs_edge* FE) {
    double key = FE->cost();  
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator lower = L_2nd_queue_.lower_bound(key);
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator upper = L_2nd_queue_.upper_bound(key);
    vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == FE) {
        L_2nd_queue_.erase (it);
        return true;
      }
    }
    return false;
  }
  
  //: return 1 if FE is in A132_link_queue
  //  return 2 if FE is in Topo_link_queue
  //  return 0 otherwise.
  int remove_L_from_classified_queue (const dbsk3d_fs_edge* FE) {
    if (_remove_from_L_1st_queue (FE))
      return 1;
    if (_remove_from_L_2nd_queue (FE))
      return 2;
    return 0;
  }
  void add_L_to_classified_queue (const dbsk3d_fs_edge* FE, 
                                  const int option) {
    if (option == 1)
      _add_to_L_1st_queue (FE);
    else {
      assert (option == 2);
      _add_to_L_2nd_queue (FE);
    }
  }

  void _add_to_F_L_vector (const dbmsh3d_face* F, const dbsk3d_fs_edge* FE) {
    assert (FE->bnd_face() == F);
    assert (int(F_L_vector_.size()) == F->id());
    F_L_vector_.push_back ((dbsk3d_fs_edge*) FE);
  }
  void _del_from_F_L_vector (const dbmsh3d_face* F) {
    assert (F->id() < (int) F_L_vector_.size());
    dbsk3d_fs_edge* FE = F_L_vector_[F->id()];
    assert (FE->bnd_face() == F);
    F_L_vector_[F->id()] = NULL;
  }

  dbsk3d_fs_edge* find_L_of_F (dbmsh3d_face* F) {
    assert (F->id() < (int) F_L_vector_.size());
    return F_L_vector_[F->id()];
  }

  //###### Main Shock Segregation & Surface Meshing Functions ######

  //: Main segregation functions.
  void run_surface_segre (const float max_sample_ratio, const int topo_option,
                          const float percentage, const bool reasgn_lost_genes);

  void seg_init_shock_queue (const float max_sample_ratio);  

  unsigned int first_greedy_iteration (const float percentage);  
    bool first_greedy_step (dbsk3d_fs_edge* FE);
      SURF_TOPO_TYPE check_L_topology_1 (const dbsk3d_fs_edge* FE,
                                         vcl_set<dbsk3d_fs_edge*>& undo_L_set);
    bool first_greedy_fix_vtopo (dbsk3d_fs_edge* FE);

  unsigned int first_topo_fixing ();

  void add_bnd_links_to_queue ();

  unsigned int second_greedy_iteration (); 
    bool second_greedy_step (dbsk3d_fs_edge* FE);
      SURF_TOPO_TYPE check_L_topology_2 (const dbsk3d_fs_edge* FE,
                                         dbsk3d_fs_edge** L_ungap);
      bool check_multi_jun_angles (const dbsk3d_fs_edge* FE);
    bool second_greedy_fix_vtopo (dbsk3d_fs_edge* FE);

  unsigned int second_topo_fixing ();
    bool fix_hole_1st_order (dbsk3d_bnd_chain* BC);

  unsigned int final_oversize_fill ();

  //###### Shock Segregation Subroutines ######

  double get_Q1_percent_cost (const float percent);
  double get_Q1_median_cost ();
  void output_Q1_file ();
  void print_Q1_file ();
  double get_Q2_percent_cost (const float percent);

  //: Check if the shock link is already meshed in the input or not.
  dbmsh3d_face* L_find_already_meshed_dual_F (dbsk3d_fs_edge* FE);

  void L_compute_gap_cost (dbsk3d_fs_edge* FE, const int option);

  //: Perform 3D Gap Transform on FE.
  void L_3d_gap_transform (dbsk3d_fs_edge* FE);

  //: Modify continuity cost to all neighboring fs_edges.
  void L_modify_nbrs_cost (const dbsk3d_fs_edge* FE);    

    void L_add_nbr_cost_P (const dbsk3d_fs_edge* FE, 
                           const dbsk3d_fs_face* FF);
    void add_nbr_cost (const dbsk3d_fs_edge* inputL,
                       const dbsk3d_fs_face* FF,
                       dbsk3d_fs_edge* FE);

  //: Undo the 3D gap transform on FE.
  void undo_L_3d_gap_transform (dbsk3d_fs_edge* FE);

  //: Undo modifying the continuity cost to all neighbors.
  void undo_L_modify_nbrs_cost (dbsk3d_fs_edge* FE);

    void L_undo_nbr_cost_P (const dbsk3d_fs_edge* FE, 
                            const dbsk3d_fs_face* FF);
    void undo_nbr_cost (const dbsk3d_fs_edge* inputL,
                        const dbsk3d_fs_face* FF,
                        dbsk3d_fs_edge* FE);

  void reset_mesh_bnd_chains () {
    delete mesh_bnd_chains_;
    mesh_bnd_chains_ = new dbsk3d_bnd_chain_set (fs_mesh_);
  }

};

//###### Surface Segregation Subroutines ######

double compute_bending_angle (const dbsk3d_fs_edge* inputL,
                              const dbsk3d_fs_face* FF,
                              const dbsk3d_fs_edge* FE);

bool L_check_fill_hole (dbsk3d_fs_edge* FE);

//###### Surface Topo Fixing ######


#endif
