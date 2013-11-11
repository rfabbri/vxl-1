//: This is dbsk3d_fs_sheet.h
//  MingChing Chang
//  Nov 30, 2006.

#ifndef dbsk3d_fs_sheet_h_
#define dbsk3d_fs_sheet_h_

#include <vcl_list.h>
#include <dbmsh3d/dbmsh3d_utils.h>

#include <dbsk3d/dbsk3d_fs_face.h>

#define FF_UNVISITED_SID      -1
#define FF_INVALID_SID        -2

#define FS_TYPE               char
#define FS_TYPE_BOGUS         '?'
#define FS_TYPE_TAB           'T'
#define FS_TYPE_INTERIOR      'I'
#define FS_TYPE_UNBOUNDED     'U'

#define FS_COST_UNDEFINED     (-1.0f)

//: Shock sheet (component) of a set of dbsk3d_fs_faces.
class dbsk3d_fs_sheet : public vispt_elm
{
protected:
  //: The set of fs_faces of this shock sheet.
  dbmsh3d_ptr_node*   FF_list_;
  int         id_;
  float       cost_;
  FS_TYPE     type_;
  bool        b_visited_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_sheet (int id) { 
    FF_list_     = NULL;
    id_         = id;
    cost_       = FS_COST_UNDEFINED;
    type_       = FS_TYPE_BOGUS;
    b_visited_  = false;
  }
  virtual ~dbsk3d_fs_sheet () {
    clear_ptr_list (FF_list_);
  }

  //####### Data Access #######
  dbmsh3d_ptr_node* FF_list() const {
    return FF_list_;
  }
  void set_FF_list (dbmsh3d_ptr_node* ptr_list) {
    FF_list_ = ptr_list;
  }
  const int id() const {
    return id_;
  }
  const bool b_visited () const {
    return b_visited_;
  }
  void set_visited (bool b) {
    b_visited_ = b;
  }
  const float cost () const {
    return cost_;
  }
  void set_cost (const float cost) {
    cost_ = cost;
  }
  const FS_TYPE type() const {
    return type_;
  }
  void set_type (const FS_TYPE type) {
    type_ = type;
  }
  vcl_string type_string() {
    switch (type_) {
    case FS_TYPE_TAB:
      return vcl_string ("TAB");
    break;      
    case FS_TYPE_INTERIOR:
      return vcl_string ("INTERIOR");      
    break;
    case FS_TYPE_UNBOUNDED:
      return vcl_string ("UNBOUNDED");
    break;
    default:
      return vcl_string ("BOGUS");
    break;
    }
  }

  //###### Handle the fs_faces ######  
  unsigned int get_FFs (vcl_set<void*>& FF_set) const {
    return get_all_ptrs (FF_list_, FF_set);
  }
  unsigned int n_FFs() const {
    return count_all_ptrs (FF_list_);
  }
  unsigned int clear_FF_list () {
    return clear_ptr_list (FF_list_);
  }
  bool have_FFs() const {
    return FF_list_!=NULL;
  }
  bool contain_FF (const dbsk3d_fs_face* FF) const {
    return is_in_ptr_list (FF_list_, FF);
  }
  void add_FF (const dbsk3d_fs_face* FF) {
    add_ptr_to_list (FF_list_, FF); //add_ptr_check
  }
  bool check_add_FF (const dbsk3d_fs_face* FF) {
    return check_add_ptr (FF_list_, FF);
  }
  bool del_FF (const dbsk3d_fs_face* FF) {
    return del_ptr (FF_list_, FF);
  }

  //: Add FF to S and also set FF->sid() to this sheet id.
  void add_FF_to_S (dbsk3d_fs_face* FF) {
    assert (FF->b_valid());
    add_FF (FF);
    if (FF->b_finite() == false)
      type_ = FS_TYPE_UNBOUNDED;
    FF->set_sid (id_);
  }
  //: Remove FF from S and also set FF->sid() to -1.
  void del_FF_from_S (dbsk3d_fs_face* FF) {
    del_FF (FF);
    FF->set_sid (FF_UNVISITED_SID);
  }

  //####### Connectivity Query Functions #######
  //: Get all associated generators of this fs_sheet.
  void get_asso_Gs (vcl_set<dbmsh3d_vertex*>& Gset, const bool remove_G_from_L_N);

  //: Get the associated boundary mesh faces (triangles) of this fs_sheet.
  void get_bnd_mesh_Fs (vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                        vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1);

  //: Collect all boundary A3 and A13 (and higher order) fs_edges.
  void get_bnd_FEs (vcl_set<dbsk3d_fs_edge*>& A3_fs_edges, 
                   vcl_set<dbsk3d_fs_edge*>& A13_fs_edges);
  
  //####### Modification Functions #######
  void set_all_FFs_sid ();
  void set_all_FFs_invalid ();

  void mark_all_FFs_valid ();
  void mark_all_FFs_invalid ();

  void S_pass_Gs (vcl_vector<dbsk3d_fs_edge*>& C_Lset);

  void clear_incident_LN_asgn_Gs ();

  void compute_splice_cost ();

  //####### Other functions #######
  virtual bool check_integrity ();
  virtual void getInfo (vcl_ostringstream& ostrm);
};

void get_ifs_faces_pts (vcl_set<dbmsh3d_face*>& Gfaces, vcl_set<dbmsh3d_vertex*>& Gset);

void get_closest_L_from_G (const dbmsh3d_vertex* G,
                           const vcl_vector<dbsk3d_fs_edge*>& C_Ls,
                           dbsk3d_fs_edge** closestL,
                           dbsk3d_fs_vertex** closest_FV);

#endif
