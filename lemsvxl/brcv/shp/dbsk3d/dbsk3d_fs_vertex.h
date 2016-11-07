//: This is dbsk3d_fs_vertex.h
//  MingChing Chang
//  Nov 14, 2004.

#ifndef dbsk3d_fs_vertex_h_
#define dbsk3d_fs_vertex_h_

#include <vcl_map.h>
#include <dbmsh3d/dbmsh3d_node.h>

class dbsk3d_fs_edge;
class dbsk3d_fs_face;

//###############################################################
//       dbsk3d_fs_vertex
//###############################################################

//Different than the above SHOCK_NODE_TYPE, this is SHOCK_FLOW_TYPE.
//Refer to Siersma's paper for definitions.
#define FV_FLOW_TYPE        char
#define FV_FT_BOGUS         '?'
#define FV_FT_4641          '1'
#define FV_FT_4631          '2'
#define FV_FT_4531          '3'
#define FV_FT_4520          '4'
#define FV_FT_4421_O        '5'
#define FV_FT_4410_O        '6'
#define FV_FT_4410_P        '7'
#define FV_FT_4300_T        '8'
#define FV_FT_4300_L        '9'
#define FV_FT_UNCLASSIFIED  'U'
#define FV_FT_INF           'I'   // For shock vertex at infinity.

class dbsk3d_fs_vertex : public dbmsh3d_vertex
{
protected:
  //: link list to assigned generator of this fs_vertex 
  dbmsh3d_ptr_node* asgn_G_list_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_vertex (int id) : dbmsh3d_vertex (id) {
    asgn_G_list_ = NULL;
  }
  virtual ~dbsk3d_fs_vertex () {
    assert (asgn_G_list_ == NULL);
  }

  //####### Data access functions #######
  bool b_inf () const {
    return c_value_ == V_TYPE_INF;
  }
  void set_inf () {
    c_value_ = V_TYPE_INF;
  }

  FV_FLOW_TYPE flow_type() const {
    return flow_type_;
  }
  void set_flow_type (const FV_FLOW_TYPE ft) {
    flow_type_ = ft;
  }
  
  dbmsh3d_ptr_node* asgn_G_list() const {
    return asgn_G_list_;
  }
  void set_asgn_G_list (dbmsh3d_ptr_node* ptr_list) {
    asgn_G_list_ = ptr_list;
  }

  //####### Graph connectivity query functions #######
  //: traverse to incident patchElms to see if it's valid
  bool is_valid_via_FF ();

  //: fs_vertex that belongs to finite (bounded) fs_face
  bool is_valid_finite_via_FF ();

  vcl_set<dbmsh3d_vertex*> get_Gs_from_FFs ();
  int n_Gs_from_FFs ();
  
  unsigned int count_valid_FEs () const;
  unsigned int count_valid_FFs () const;
  unsigned int count_valid_FFs (vcl_set<dbsk3d_fs_face*>& Pset) const;

  //####### Flow type computation functions #######
  double compute_time();
  double compute_time_ve ();

  bool is_inf_from_FE() const;
  int n_out_flow_FEs () const;  

  FV_FLOW_TYPE detect_flow_type ();
  FV_FLOW_TYPE detect_flow_type_A14 ();
  FV_FLOW_TYPE detect_flow_type_A1n ();

  int compute_s1 (vcl_set<dbmsh3d_face*>& face_set);
  int compute_s2 ();
  bool get_2_non_Gabriel_FFs (vcl_set<dbmsh3d_face*>& FF_set,
                              dbsk3d_fs_face*& FF1, dbsk3d_fs_face*& FF2);
  bool get_3_Gabriel_edges (vcl_set<dbmsh3d_face*>& FF_set,
                            dbsk3d_fs_face*& FF1, dbsk3d_fs_face*& FF2, dbsk3d_fs_face*& FF3);

  //###### Handle the assigned generators ######  
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

  //: Get all current associated G's excluding the inputFE and FF's incident ot FE.
  void get_asso_Gs_excld_FE (const dbsk3d_fs_edge* inputFE, 
                             vcl_map<int, dbmsh3d_vertex*>& asso_genes);
  
  //: After undoing a gap xform, need to delete genes asgn. to FF's valid FE's.
  void del_asgn_G_of_FE (const dbsk3d_fs_edge* FE);

  //: Return true if this fs_vertex is still valid after gap xform on its incident FE.
  bool valid_after_gap_xform_FE (const dbsk3d_fs_edge* FE);

  //####### Other functions #######

  //: The clone function clones everything except the connectivity
  //  when cloning a graph, need to build the connectivity separetely.
  virtual dbmsh3d_vertex* clone () const;
  void _clone_FV_G_conn (dbsk3d_fs_vertex* FV2, dbmsh3d_pt_set* BND2) const;
  virtual dbsk3d_fs_vertex* clone (dbmsh3d_pt_set* BND2) const;

  virtual void getInfo (vcl_ostringstream& ostrm);
  
  void get_incident_elms (vcl_set<dbsk3d_fs_edge*>& FE_set,
                          vcl_set<dbsk3d_fs_face*>& FF_set);
};

//####### dbsk3d_fs_vertex TEXT FILE I/O FUNCTIONS #######

void fv_save_text_file (vcl_FILE* fp, const dbsk3d_fs_vertex* FV);
void fv_load_text_file (vcl_FILE* fp, dbsk3d_fs_vertex* FV, 
                        vcl_map <int, dbmsh3d_vertex*>& genemap);

void fv_load_text_file_sg (vcl_FILE* fp, dbsk3d_fs_vertex* FV);
void fv_load_text_file_sg_old (vcl_FILE* fp, dbsk3d_fs_vertex* FV);

#endif
