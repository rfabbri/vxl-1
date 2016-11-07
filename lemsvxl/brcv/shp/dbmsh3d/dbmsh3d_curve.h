#ifndef dbmsh3d_curve_h_
#define dbmsh3d_curve_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_curve.h
//:
// \file
// \brief Basic 3D curve (polyline) to describe e.g. mesh boundary.
//
//
// \author
//  MingChing Chang  Oct. 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_node.h>

class dbmsh3d_mesh;
class dbmsh3d_hypg;

//: Digital curve (polyline) data type.
#define C_DATA_TYPE         char
#define C_DATA_TYPE_EDGE    'E'
#define C_DATA_TYPE_VERTEX  'V'

//: Medial scaffold shock curve type
#define C_TYPE              char
#define C_TYPE_BOGUS        '?'
#define C_TYPE_RIB          'R'   //A3
#define C_TYPE_AXIAL        'A'
#define C_TYPE_DEGE_AXIAL   'D'
#define C_TYPE_VIRTUAL      'V'

class dbmsh3d_curve : public dbmsh3d_edge
{
protected:
  //: Vector of ordered edge elements.
  vcl_vector<dbmsh3d_edge*>     E_vec_;
  
  //: Vector of ordered vertex elements.
  vcl_vector<dbmsh3d_vertex*>   V_vec_;

  //: link list to store shared edge elements.
  dbmsh3d_ptr_node* shared_E_list_;

  //: Digital curve (polyline) data type.
  //  'L': store each dbmsh3d_edge.
  //  'N': store each dbmsh3d_vertex.
  C_DATA_TYPE  data_type_;

  C_TYPE      c_type_;

public:  
  //###### Constructor/Destructor ######
  dbmsh3d_curve (dbmsh3d_vertex* s_V, dbmsh3d_vertex* e_V, int id) :
      dbmsh3d_edge (s_V, e_V, id)
  {
    shared_E_list_ = NULL;
    data_type_  = C_DATA_TYPE_EDGE;
    c_type_ = C_TYPE_BOGUS;
  }
  virtual ~dbmsh3d_curve () {
    for (unsigned int i=0; i<E_vec_.size(); i++) {
      dbmsh3d_edge* E = E_vec_[i];
      if (E->e_type() == E_TYPE_SPECIAL)
        E->compute_e_type ();
    }

    E_vec_.clear();
    V_vec_.clear();
    assert (shared_E_list_ == NULL);
    clear_shared_E_list ();
  }

  //###### Data access functions ######  
  const dbmsh3d_node* s_N() const {
    return (dbmsh3d_node*) vertices_[0];
  }
  dbmsh3d_node* s_N() {
    return (dbmsh3d_node*) vertices_[0];
  }
  const dbmsh3d_node* e_N() const {
    return (dbmsh3d_node*) vertices_[1];
  }
  dbmsh3d_node* e_N() {
    return (dbmsh3d_node*) vertices_[1];
  }

  const C_DATA_TYPE data_type() const {
    return data_type_;
  }
  void set_data_type (const C_DATA_TYPE dtype) {
    data_type_ = dtype;
  }
  const C_TYPE c_type() const {
    return c_type_;
  }
  void set_c_type (const C_TYPE c_type) {
    c_type_ = c_type;
  }  

  const vcl_vector<dbmsh3d_edge*>& E_vec() const {
    return E_vec_;
  }
  vcl_vector<dbmsh3d_edge*>& E_vec() {
    return E_vec_;
  }
  dbmsh3d_edge* E_vec (const unsigned int i) const {
    return E_vec_[i];
  }
  dbmsh3d_edge* E_vec (const unsigned int i) {
    return E_vec_[i];
  }
  dbmsh3d_edge* E_vec_last () const {
    assert (E_vec_.size() > 0);
    return E_vec_[ E_vec_.size() - 1 ];
  }
  dbmsh3d_edge* E_vec_last () {
    assert (E_vec_.size() > 0);
    return E_vec_[ E_vec_.size() - 1 ];
  }

  const vcl_vector<dbmsh3d_vertex*>& V_vec() const {
    return V_vec_;
  }
  const dbmsh3d_vertex* V_vec (const unsigned int i) const {
    return V_vec_[i];
  }
  dbmsh3d_vertex* V_vec (const unsigned int i) {
    return V_vec_[i];
  }
  unsigned int n_Nvec () const {
    return V_vec_.size();
  }
  
  //###### Handle the shared edges ######   
  dbmsh3d_ptr_node* shared_E_list() const {
    return shared_E_list_;
  }
  void set_shared_E_list (dbmsh3d_ptr_node* shared_E_list) {
    shared_E_list_ = shared_E_list;
  }
  
  unsigned int get_shared_Es (vcl_set<void*>& shared_Es) const {
    return get_all_ptrs (shared_E_list_, shared_Es);
  }
  unsigned int n_shared_Es() const {
    return count_all_ptrs (shared_E_list_);
  }
  unsigned int clear_shared_E_list () {
    return clear_ptr_list (shared_E_list_);
  }
  bool have_shared_Es() const {
    return shared_E_list_!=NULL;
  }
  bool is_E_shared (const dbmsh3d_edge* E) const {
    return is_in_ptr_list (shared_E_list_, E);
  }
  void add_shared_E (dbmsh3d_edge* E) {
    E->set_e_type (E_TYPE_SHARED);
    add_ptr_to_list (shared_E_list_, E); //add_ptr_check
  }
  bool check_add_shared_E (dbmsh3d_edge* E) {
    E->set_e_type (E_TYPE_SHARED);
    return check_add_ptr (shared_E_list_, E);
  }
  bool del_shared_E (const dbmsh3d_edge* E) {
    //Need to change the E_TYPE_SHARED in other places.
    return del_ptr (shared_E_list_, E);
  }

  //###### Fine-scale edge (E_vec) Query Functions ######
  bool contain_E (const dbmsh3d_edge* inputE) const;
  void get_Evec (vcl_vector<dbmsh3d_edge*>& Evec) const;
  void get_Eset (vcl_set<dbmsh3d_edge*>& Eset) const;
  bool all_Es_in_vec (const vcl_vector<dbmsh3d_edge*>& Evec) const;

  dbmsh3d_edge* get_E_incident_N (const dbmsh3d_node* N) const;
  void get_Evec_incident_F (const dbmsh3d_face* F, const dbmsh3d_node* N, vcl_vector<dbmsh3d_edge*>& Evec) const;

  void get_Eset_S_M (const dbmsh3d_vertex* M, vcl_set<dbmsh3d_edge*>& Eset) const;
  void get_Eset_M_E (const dbmsh3d_vertex* M, vcl_set<dbmsh3d_edge*>& Eset) const;
  
  double compute_length_Es () const;
  
  dbmsh3d_edge* get_1st_non_shared_E () const;

  //###### Fine-scale edge (E_vec) Modification Functions ######
  virtual void add_E_to_back (const dbmsh3d_edge* E) {
    if (E_vec_.size() != 0)
      assert (E_vec_[E_vec_.size()-1] != E);
    E_vec_.push_back ((dbmsh3d_edge*) E);
  }
  void add_E_vec_to_front (const vcl_vector<dbmsh3d_edge*>& E_vec);
  void check_add_E_vec_to_front (vcl_vector<dbmsh3d_edge*>& E_vec);

  void add_E_vec_to_back (const vcl_vector<dbmsh3d_edge*>& E_vec);
  void check_add_E_vec_to_back (vcl_vector<dbmsh3d_edge*>& E_vec);

  bool del_from_E_vec (const dbmsh3d_edge* E) {
    vcl_vector<dbmsh3d_edge*>::iterator it = E_vec_.begin();
    for (; it != E_vec_.end(); it++) {
      if ((*it) == E) {
        E_vec_.erase (it);
        return true;
      }
    }
    return false;
  }

  void _clear_E_vec () {
    E_vec_.clear();
  }
  void _reverse_E_vec() {
    assert (data_type_ == C_DATA_TYPE_EDGE);
    vcl_reverse (E_vec_.begin(), E_vec_.end());
  }
  void _del_E_vec_i_to_end (const unsigned int i) {
    assert (data_type_ == C_DATA_TYPE_EDGE);
    vcl_vector<dbmsh3d_edge*>::iterator it = E_vec_.begin();
    it = it + i;
    E_vec_.erase (it, E_vec_.end());
  }

  //###### Fine-scale vertex (V_vec) Query Functions ######
  void get_V_vec (vcl_vector<dbmsh3d_vertex*>& V_vec) const;
  void get_V_set (vcl_set<dbmsh3d_vertex*>& Vset) const;
  bool is_V_on_C (const dbmsh3d_vertex* inputV) const;

  void get_V_set_S_M (const dbmsh3d_vertex* M, vcl_set<dbmsh3d_vertex*>& Vset) const;  
  void get_V_set_M_E (const dbmsh3d_vertex* M, vcl_set<dbmsh3d_vertex*>& Vset) const;

  int get_nE_of_V_from_N (const dbmsh3d_vertex* inputV, const dbmsh3d_node* N) const;
  
  dbmsh3d_vertex* get_middle_V() const;
  double compute_length_Vs () const;

  //###### Fine-scale vertex (V_vec) Modification Functions ######
  void add_V_to_front (const dbmsh3d_vertex* V) {
    V_vec_.insert (V_vec_.begin(), (dbmsh3d_vertex*) V);
  }
  void add_V_to_back (const dbmsh3d_vertex* V) {
    V_vec_.push_back ((dbmsh3d_vertex*) V);
  }
  void add_V_to_pos (const unsigned int i, const dbmsh3d_vertex* V) {
    V_vec_.insert (V_vec_.begin()+i, (dbmsh3d_vertex*) V);
  }

  //: Remove the first vertex.
  void remove_first_V () {    
    vcl_vector<dbmsh3d_vertex*>::iterator it = V_vec_.begin();
    V_vec_.erase (it);
  }
  //: Remove the last vertex.
  void remove_last_V () {    
    vcl_vector<dbmsh3d_vertex*>::iterator it = V_vec_.begin() + V_vec_.size()-1;
    V_vec_.erase (it);
  }
  
  bool del_from_V_vec (const dbmsh3d_vertex* V) {
    vcl_vector<dbmsh3d_vertex*>::iterator it = V_vec_.begin();
    for (; it != V_vec_.end(); it++) {
      if ((*it) == V) {
        V_vec_.erase (it);
        return true;
      }
    }
    return false;
  }

  void _reverse_V_vec();

  //: insert the vertices from curve C to the current curve
  //  the insertion includes the starting point, but does not include the end point.
  void insert_Vs_of_C (dbmsh3d_curve* C, dbmsh3d_node* N);
  
  //###### Connectivity Query Functions ######
  bool contain_V (const dbmsh3d_vertex* inputV) const;
  bool all_Es_shared () const;
  bool shared_E_incident_to_V (const dbmsh3d_vertex* V) const;

  //: Return true if all shared_E[] are with one of the given set of C's.
  bool shared_E_with_Cset (vcl_set<dbmsh3d_curve*>& shared_E_Cset) const;

  //###### Connectivity Modification Functions ######
  virtual void switch_sV_eV ();

  void add_shared_Es_to_C (dbmsh3d_curve* C);
  void pass_shared_Es_to_C (dbmsh3d_curve* C);

  void check_add_all_Es_to_C (dbmsh3d_curve* C);

  void compute_c_type ();

  //###### Other functions ######
  virtual bool check_integrity ();
  
  virtual dbmsh3d_edge* clone () const;
  void _clone_C_EV_conn (dbmsh3d_curve* C2, dbmsh3d_mesh* M2) const;
  virtual dbmsh3d_curve* clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2) const;
  
  virtual void getInfo (vcl_ostringstream& ostrm);

  double compute_length () const;
};

inline void add_Cs_sharing_E (dbmsh3d_curve* C1, dbmsh3d_curve* C2, dbmsh3d_edge* E) {
  C1->add_shared_E (E);
  C2->add_shared_E (E);
}

inline void check_add_Cs_sharing_E (dbmsh3d_curve* C1, dbmsh3d_curve* C2, dbmsh3d_edge* E) {
  C1->check_add_shared_E (E);
  C2->check_add_shared_E (E);
}

inline void remove_Cs_sharing_E (dbmsh3d_curve* C1, dbmsh3d_curve* C2, dbmsh3d_edge* E) {
  C1->del_shared_E (E);
  C2->del_shared_E (E);
}

bool Cs_sharing_E (const dbmsh3d_curve* C1, const dbmsh3d_curve* C2);

void remove_Cs_sharing_E (dbmsh3d_curve* C1, dbmsh3d_curve* C2);

//: For the case the input curve C has 3 incident sheets, 
//  where 2 of them are identical (2-incidence), e.g. (S1, S2, S2).
//  Get the non-duplicated sheet S1 and the duplicated sheet S2, and return true.
//  Return false if the local config is not in such case.
bool C_get_non_dup_S (dbmsh3d_curve* C, dbmsh3d_sheet* & tabS, dbmsh3d_sheet* & baseS);

void merge_C1_C2_Es (dbmsh3d_curve* C1, dbmsh3d_curve* C2, dbmsh3d_node* N, const bool make_shared_E); 

#endif

