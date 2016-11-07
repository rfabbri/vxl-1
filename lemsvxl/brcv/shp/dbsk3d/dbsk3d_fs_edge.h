//: This is dbsk3d_fs_edge.h
//  MingChing Chang
//  Nov 14, 2004      Creation.

#ifndef dbsk3d_fs_edge_h_
#define dbsk3d_fs_edge_h_

#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>

class dbsk3d_fs_face;
class dbsk3d_fs_vertex;

//###############################################################
//       dbsk3d_fs_edge
//###############################################################

//Shock link types for segregation.
#define FE_SEG_TYPE         char
#define FE_SEG_BOGUS        '?'
#define FE_SEG_MESHED       'm'   //has already meshed face (dual)
#define FE_SEG_ACUTE        'a'   //acute_tri, acute_polygon
#define FE_SEG_OBTUSE       'o'   //right_tri, obtuse_tri, non_acute_polygon
#define FE_SEG_NO_A132      '2'
#define FE_SEG_OVERSIZED    's'

//The flow type of shock curve element
#define FE_FLOW_TYPE        char
#define FE_FT_BOGUS         '?'
#define FE_FT_I_A13_2_ACUTE '1'   //Init. from an A13-2 source pt.
#define FE_FT_II_A13_3_OBT  '2'   //Init. from an A13-3 relay pt.
#define FE_FT_III_A14_ACUTE '3'   //Init. from an A14 and the triangle is acute.
#define FE_FT_IV_A14_OBT    '4'   //Init. from an A14 and the triangle is obtuse.
#define FE_FT_INF           'I'   //For shock curve at infinity.

//: pre-defined constant shock link gap costs.
#define SEG_OVERSIZE_COST     2e+38F
#define SEG_NON_A13_2_COST    10000

class dbsk3d_fs_edge : public dbmsh3d_edge
{
protected:
  dbmsh3d_face*     bnd_face_;

  //: Saliency measure for SurfaceTriangles: Max side length
  float             cost_;

  //: link list to assigned generator of this fs_vertex 
  dbmsh3d_ptr_node* asgn_G_list_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_edge (dbmsh3d_vertex* s_FV, dbmsh3d_vertex* e_FV, int id) : 
    dbmsh3d_edge (s_FV, e_FV, id)
  {
    bnd_face_     = NULL;
    cost_         = FLT_MAX;
    asgn_G_list_  = NULL;
  }

  virtual ~dbsk3d_fs_edge () {
    assert (asgn_G_list_ == NULL);
  }

  //####### Data access functions #######  
  dbmsh3d_ptr_node* asgn_G_list() {
    return asgn_G_list_;
  }
  void set_asgn_G_list (dbmsh3d_ptr_node* ptr_list) {
    asgn_G_list_ = ptr_list;
  }

  const short s_value () const {
    return s_value_;
  }
  void set_s_value (const short s) {
    s_value_ = s;
  }

  const short counter() const {
    return s_value_;
  }
  void increase_counter () {
    s_value_++;
  }
  void set_counter (const short c) {
    s_value_ = c;
  }

  const FE_SEG_TYPE seg_type() const {
    return flow_type_; ///c_value_;
  }
  void set_seg_type (const FE_SEG_TYPE type) {
    flow_type_ = type;
  }
  FE_FLOW_TYPE flow_type () const {
    return flow_type_;
  }
  void set_flow_type (const FE_FLOW_TYPE ft) {
    flow_type_ = ft;
  }

  const dbmsh3d_face* bnd_face() const {
    return bnd_face_;
  }
  void set_bnd_face (const dbmsh3d_face* face) {
    bnd_face_ = (dbmsh3d_face*) face;
  }

  const float cost() const {
    return cost_;
  }
  void set_cost (const double cost) {
    cost_ = (float) cost;
  }

  //####### Graph connectivity query functions #######
  const dbsk3d_fs_vertex* s_FV () const {
    return (const dbsk3d_fs_vertex*) vertices_[0];
  }
  dbsk3d_fs_vertex* s_FV () {
    return (dbsk3d_fs_vertex*) vertices_[0];
  }
  const dbsk3d_fs_vertex* e_FV () const {
    return (const dbsk3d_fs_vertex*) vertices_[1];
  }
  dbsk3d_fs_vertex* e_FV () {
    return (dbsk3d_fs_vertex*) vertices_[1];
  }

  const dbmsh3d_vertex* _get_one_G_via_FF () const;

  dbsk3d_fs_face* other_FF_of_G (const dbsk3d_fs_face* inputFF, 
                                 const dbmsh3d_vertex* G) const;

  const dbmsh3d_vertex* A13_opposite_G (const dbsk3d_fs_face* FF) const;

  bool get_ordered_Gs_via_FF (vcl_vector<dbmsh3d_vertex*>& genes) const;
  bool get_ordered_Gs_via_FF (vcl_set<dbmsh3d_vertex*>& genes) const;
  void get_3_Gs_via_FF (dbmsh3d_vertex** G0, dbmsh3d_vertex** G1, dbmsh3d_vertex** G2) const;
  
  //####### Generator handling functions #######  
  unsigned int get_asgn_Gs (vcl_set<void*>& asgn_genes) const {
    return get_all_ptrs (asgn_G_list_, asgn_genes);
  }
  unsigned int n_asgn_Gs() const {
    return count_all_ptrs (asgn_G_list_);
  }
  unsigned int clear_asgn_G_list () {
    return clear_ptr_list (asgn_G_list_);
  }
  bool have_asgn_Gs() const {
    return asgn_G_list_!=NULL;
  }
  bool is_G_asgn (const dbmsh3d_vertex* G) const {
    return is_in_ptr_list (asgn_G_list_, G);
  }
  void add_asgn_G (const dbmsh3d_vertex* G) {
    add_ptr_to_list (asgn_G_list_, G); //add_ptr_check
  }
  bool check_add_asgn_G (const dbmsh3d_vertex* G) {
    return check_add_ptr (asgn_G_list_, G);
  }
  bool del_asgn_G (const dbmsh3d_vertex* G) {
    return del_ptr (asgn_G_list_, G);
  }

  void check_add_asgn_Gs_to_set (vcl_map<int, dbmsh3d_vertex*>& asso_genes,
                                 const dbsk3d_fs_face* fromP);

  bool _is_G_asgn_in_FF (const dbmsh3d_vertex* G, 
                         const dbsk3d_fs_face* fromFF) const;
  
  //: prune and pass associated generators to remaining shock curves.
  bool prune_pass_Gs_to_FV (vcl_map<int, dbmsh3d_vertex*>& asso_genes,
                            dbsk3d_fs_vertex* FV);

  //: Get all associated generators of this FE.
  void get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_genes);
  
  //: Get all associated generators of this FE and incident FE's.
  void get_asso_Gs_incld_FFs (vcl_map<int, dbmsh3d_vertex*>& asso_genes);
  
  unsigned int get_pass_Gs (vcl_set<dbmsh3d_vertex*>& pass_genes) const;

  //####### Flow type computation functions #######
  
  //: Compute circum center from the associated generators.
  //  return false if this can't be done (on boundary shock curves).
  bool compute_circum_cen (vgl_point_3d<double>& C) const;

  //: compute the circum center time 
  double circum_cen_time () const;
  double circum_cen_time (const vgl_point_3d<double>& C) const;

  //: Check if the flow is bi-directional, i.e. containing the circumcenter.
  //  return 0: uni-directional
  //         1: center C fuzzily on boundary
  //         2: bi-directional
  int is_flow_bidirect ();

  int is_flow_bidirect (const vgl_point_3d<double>& C);

  //: Detect the flow type of this fs_edge
  FE_FLOW_TYPE detect_flow_type ();
  
  //: type 1: A13-2 point included and inside dual Delaunay triangle.
  //  type 2: A13-2 point included and outside dual Delaunay triengle.
  //  type 3: A13-2 point not included.
  FE_FLOW_TYPE detect_flow_type (const vgl_point_3d<double>& C); 

  ///bool contain_A13_2_src () const;

  dbsk3d_fs_face* type_II_get_outgoing_P () const;

  //####### Surface meshing functions #######  

  bool get_tri_params (double& max_side, double& perimeter, unsigned int& nG, 
                       double* Side, dbmsh3d_vertex** Gene) const;
  
  double compute_tri_compactness (const double nG, double* Side);

  double compute_min_radius (const int nG, dbmsh3d_vertex** Gene) const;

  bool dual_DT_contains_A13_2 (const int nG, dbmsh3d_vertex** Gene) const;
    bool _dual_polygon_contains_A13_2 (const vcl_vector<dbmsh3d_vertex*>& Genes) const;

  bool face_on_mesh_bnd () const;
  
  void update_validity_FFs ();
  
  bool check_fit_hole (const dbmsh3d_vertex* Vo, const dbmsh3d_edge* E) const;

  //####### Shock transform (pruning) functions #######

  //####### Other Functions #######
  virtual dbmsh3d_edge* clone () const;
  void _clone_FE_G_conn (dbsk3d_fs_edge* FE2, dbmsh3d_pt_set* BND2) const;
  virtual dbsk3d_fs_edge* clone (dbmsh3d_pt_set* PS2, dbmsh3d_pt_set* BND2);

  virtual void getInfo (vcl_ostringstream& ostrm);
};

//####### dbsk3d_fs_vertex TEXT FILE I/O FUNCTIONS #######

void fe_save_text_file (vcl_FILE* fp, dbsk3d_fs_edge* FE);
void fe_load_text_file (vcl_FILE* fp, dbsk3d_fs_edge* FE, 
                        vcl_map <int, dbmsh3d_vertex*>& vertexmap,
                        vcl_map <int, dbmsh3d_vertex*>& genemap);

#endif
