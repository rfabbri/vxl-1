#ifndef dbmsh3d_sheet_h_
#define dbmsh3d_sheet_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_sheet.h
//:
// \file
// \brief Basic 3d surface (more complex than a face)
//
//
// \author
//  MingChing Chang  July 05, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_face.h>

class dbmsh3d_node;
class dbmsh3d_curve;
class dbmsh3d_mesh;
class dbmsh3d_hypg;

class dbmsh3d_sheet : public dbmsh3d_face
{
protected:
  //: Link list of heads of interval curve chains.
  dbmsh3d_ptr_node* icurve_chain_list_;

  //: Dynamic mesh face data structure.
  vcl_map<int, dbmsh3d_face*> facemap_;

  //: link list to store shared edge elements.
  dbmsh3d_ptr_node* shared_F_list_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_sheet (int id) : dbmsh3d_face (id) {
    icurve_chain_list_ = NULL;
    shared_F_list_ = NULL;
  }
  virtual ~dbmsh3d_sheet () {
    //: make sure that all halfedges are deleted before the destructor.
    //  User should use dbmsh3d_hypg::_del_sheet() to delete a sheet.
    assert (icurve_chain_list_ == NULL);
    facemap_.clear();
    assert (shared_F_list_ == NULL);
    clear_shared_F_list ();
  }

  //###### Data access functions ######  
  dbmsh3d_ptr_node* icurve_chain_list() const {
    return icurve_chain_list_;
  }
  void set_icurve_chain_list (dbmsh3d_ptr_node* icurve_chain_list) {
    icurve_chain_list_ = icurve_chain_list;
  }

  vcl_map<int, dbmsh3d_face*>& facemap() {
    return facemap_;
  }
  dbmsh3d_face* facemap (const int i) {
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.find (i);
    if (it == facemap_.end())
      return NULL;
    return (*it).second;
  }

  //###### Mesh Face Query Functions ######
  bool contain_F (const int fid) {
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.find (fid);
    return it != facemap_.end();
  }  
  void get_F_set (vcl_set<dbmsh3d_face*>& F_set, const bool skip_shared_F);

  void get_fine_scale_vertices (vcl_map<int, dbmsh3d_vertex*>& V_map);

  //: Return the first found F of this sheet which is incident to E.
  //  Note that there can exist multiple possible F's incident to E.
  dbmsh3d_face* get_1st_F_incident_E (const dbmsh3d_edge* E);
  
  //: Return the first found F not in inputF_set of this sheet which is incident to E.
  //  Note that there can exist multiple possible F's incident to E.
  dbmsh3d_face* get_1st_F_incident_E2 (const dbmsh3d_edge* E, 
                                       vcl_set<dbmsh3d_face*>& inputF_set);

  //: Check if all fine-scale F's are connected.
  //  Start with a non-shared_F as seed.
  //  For efficiency, first check for edge-connected components,
  //  If failed, check for also vertex-connected components.
  bool check_non_shared_Fs_connected ();

  dbmsh3d_face* get_1st_non_shared_F ();

  //: Check if all fine-scale F's are connected.
  //  Skip checking all shared_F's.
  //  If v_conn specified, also consider vertex-connected neighbors.
  //  Otherwise only edge-connected neighbors are traversed.
  bool non_shared_Fs_connected (dbmsh3d_face* seedF, const bool v_conn);

  //###### Mesh Face Modification Functions ######
  void remove_F (dbmsh3d_face* F) {
    facemap_.erase (F->id());
  }
  void add_F (const dbmsh3d_face* F) {
    facemap_.insert (vcl_pair<int, dbmsh3d_face*>(F->id(), (dbmsh3d_face*) F));
  }
  void reset_traverse_F (); 

  //###### Handle the internal curve chains ######  
  unsigned int get_icurve_chains (vcl_set<void*>& icurve_chains) const {
    return get_all_ptrs (icurve_chain_list_, icurve_chains);
  }
  unsigned int n_icurve_chains() const {
    return count_all_ptrs (icurve_chain_list_);
  }
  unsigned int clear_icurve_chains () {
    return clear_ptr_list (icurve_chain_list_);
  }
  bool have_icurve_chain() const {
    return icurve_chain_list_!=NULL;
  }
  bool is_HE_in_icurve_chain (const dbmsh3d_halfedge* HE) const {
    return is_in_ptr_list (icurve_chain_list_, HE);
  }
  void add_icurve_chain_HE (const dbmsh3d_halfedge* HE) {
    add_ptr_to_list (icurve_chain_list_, HE); //add_ptr_check
  }
  bool check_add_icurve_chain_HE (const dbmsh3d_halfedge* HE) {
    return check_add_ptr (icurve_chain_list_, HE);
  }
  bool del_icurve_chain_HE (const dbmsh3d_halfedge* HE) {
    return del_ptr (icurve_chain_list_, HE);
  }

  //###### Handle the shared faces ######   
  dbmsh3d_ptr_node* shared_F_list() const {
    return shared_F_list_;
  }
  void set_shared_F_list (dbmsh3d_ptr_node* shared_F_list) {
    shared_F_list_ = shared_F_list;
  }
  
  unsigned int get_shared_Fs (vcl_set<void*>& shared_Fs) const {
    return get_all_ptrs (shared_F_list_, shared_Fs);
  }
  unsigned int n_shared_Fs() const {
    return count_all_ptrs (shared_F_list_);
  }
  unsigned int clear_shared_F_list () {
    return clear_ptr_list (shared_F_list_);
  }
  bool have_shared_Fs() const {
    return shared_F_list_!=NULL;
  }
  bool is_F_shared (const dbmsh3d_face* F) const {
    return is_in_ptr_list (shared_F_list_, F);
  }
  void add_shared_F (const dbmsh3d_face* F) {
    add_ptr_to_list (shared_F_list_, F);
  }
  bool check_add_shared_F (const dbmsh3d_face* F) {
    return check_add_ptr (shared_F_list_, F);
  }
  bool del_shared_F (const dbmsh3d_face* F) {
    return del_ptr (shared_F_list_, F);
  }

  //###### Connectivity Query Functions ######   
  unsigned int n_IC_pairs () const;
  unsigned int n_IC_loops () const;
  dbmsh3d_ptr_node* _find_1st_icurve_loop () const;

  unsigned int total_HEs_icurve_chains () const;

  bool is_HE_in_icurves (const dbmsh3d_halfedge* inputHE) const;
  bool is_E_in_icurves (const dbmsh3d_edge* inputE) const;
  bool is_E_in_icurve_pair (const dbmsh3d_edge* E) const;

  //: Find the bnd or internal-curve HE of the given edge.
  //  Return 1 if found in bnd chain.
  //  Return 2 if found in i-curve pair.
  //  Return 3 if found in i-curve loop.
  //  Return 0 if not found.
  int find_bnd_IC_HE (const dbmsh3d_edge* inputE, dbmsh3d_halfedge*& foundHE) const;

  void get_incident_Es (vcl_set<dbmsh3d_edge*>& incident_Es) const;

  void get_incident_Vs (vcl_set<dbmsh3d_vertex*>& incident_Vs) const;

  void get_incident_Sset_via_C_N (vcl_set<dbmsh3d_sheet*>& incident_Sset) const;

  dbmsh3d_halfedge* get_1st_HE_incident_V (const dbmsh3d_halfedge* startHE, 
                                           dbmsh3d_vertex* V) const;

  dbmsh3d_halfedge* get_1st_bnd_HE_incident_V (const dbmsh3d_halfedge* startHE, 
                                               dbmsh3d_vertex* V) const;

  int n_bnd_IC_chain_E_inc_V (const dbmsh3d_vertex* inputV) const;

  //: get the set of i-curve pair curves.
  void get_icurve_pairs (vcl_set<dbmsh3d_edge*>& icurve_pairs) const;

  //: get the set of i-curve pair and loops.
  void get_icurves (vcl_set<dbmsh3d_edge*>& icurves) const;

  //: get the set of fine-scale vertices in i-curve chains.
  void get_icurve_Vset (vcl_set<dbmsh3d_vertex*>& Vset) const;
  
  //: Return the set of boundary nodes incident to any i-curves chain (pairs or loops).
  void get_bnd_Ns_inc_ICchain (vcl_set<dbmsh3d_vertex*>& bndN_icurve) const;

  //: Return the set of boundary and icurve-loop nodes incident to any i-curves.
  void get_bnd_iloop_N_icurve (vcl_set<dbmsh3d_vertex*>& bndN_icurve) const;

  bool Es_in_same_loop (dbmsh3d_edge* E1, dbmsh3d_edge* E2) const;

  //: get the set of i-curve pair curves inciden to N.
  void get_ICpairs_inc_N (const dbmsh3d_vertex* N, vcl_set<dbmsh3d_edge*>& ICpairs) const;

  void get_ICchains_inc_N (const dbmsh3d_vertex* N, vcl_set<dbmsh3d_edge*>& ICchains) const;

  dbmsh3d_curve* get_incident_C_via_E (dbmsh3d_edge* E) const;

  //: Get all incident MC's shared_Es.
  bool get_incident_C_shared_Es (vcl_set<dbmsh3d_edge*>& shared_Es) const;

  dbmsh3d_curve* find_next_bnd_C (const dbmsh3d_node* inputN, 
                                  const dbmsh3d_curve* inputC) const;

  void get_bnd_icurveloop_Ns (vcl_set<dbmsh3d_vertex*>& vertices) const;

  //Find any duplicate E from icurve-pair in bnd-chain or icurve-loop.
  dbmsh3d_halfedge* _find_dup_Es_icurve_pair () const;  

  //: Return the first found otherC of this sheet incident to N via F.
  bool get_otherC_via_F (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                         dbmsh3d_curve*& otherC, dbmsh3d_face*& F);

  dbmsh3d_face* get_corner_F (const dbmsh3d_curve* C1, const dbmsh3d_node* N,
                              const dbmsh3d_curve* C2);

  //: Return the first found otherC of this sheet incident to N via F 
  //  with a constraint that F != inputF in the input set.
  dbmsh3d_curve* get_1st_other_C_via_F2 (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                                         vcl_set<dbmsh3d_face*>& inputF_set);

  //: Check if any of E's incident F belongs to this sheet.
  bool fine_scale_E_incident (const dbmsh3d_edge* E);
  
  bool find_bnd_chain (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV, 
                       vcl_vector<dbmsh3d_halfedge*>& HEvec);

  //: Return true if all shared_F[] are with one of the given set of S's.
  bool shared_F_with_Sset (vcl_set<dbmsh3d_sheet*>& shared_F_Sset) const;
  
  //###### Connectivity modification functions ######

  //: Set the referencing headHE to the next if its equal to inputHE.
  void _set_headHE_to_next (dbmsh3d_halfedge* inputHE);

  bool _set_headHE_to_E (const dbmsh3d_edge* inputE);

  //: add a inputHE to an existing internal curve halfedge chain.
  void add_HE_to_icurve_chain (dbmsh3d_halfedge* headHE, dbmsh3d_halfedge* inputHE) const {
    add_HE_chain_end (headHE, inputHE);
  }

  //: Disconnect HE from this sheet: either from the bnd chain or the i-curve chains.
  bool _disconnect_HE (dbmsh3d_halfedge* inputHE);

  //: Disconnect HE from the i-curve chain.
  bool _disconnect_HE_icurve (dbmsh3d_halfedge* inputHE);

  //: Connect a sheet and a boundary edge in a tri-incidence.
  //  An example of the shock A5 swallowtail case.
  void connect_bnd3_E_end (dbmsh3d_edge* E) {
    //Create the first halfedge of incidence
    dbmsh3d_halfedge* he1 = new dbmsh3d_halfedge (E, this);    
    _connect_bnd_HE_end (he1);
    E->_connect_HE_end (he1);

    //Produce a second halfedge of incidence.
    dbmsh3d_halfedge* he2 = new dbmsh3d_halfedge (E, this);
    _connect_bnd_HE_end (he2);
    E->_connect_HE_end (he2);
    
    //Produce a third halfedge of incidence.
    dbmsh3d_halfedge* he3 = new dbmsh3d_halfedge (E, this);
    _connect_bnd_HE_end (he3);
    E->_connect_HE_end (he3);
  }

  bool connect_adj_HE (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn);

  virtual bool disconnect_bnd_E (dbmsh3d_edge* E);

  //: Disconnect a sheet and a boundary edge in a tri-incidence.
  bool disconnect_bnd3_E (dbmsh3d_edge* E);

  //: Connect E as a internal break curve (2-incidence).
  dbmsh3d_halfedge* connect_icurve_pair_E (dbmsh3d_edge* E);

  //: Connect a loop of edges as a internal curve chain (1-incidence).
  void connect_icurve_loop_Es (vcl_vector<dbmsh3d_edge*>& icurve_loop_Es);
  
  //: Canonicalization: re-organize this SS's bnd_HE_chain and icurve_HE_chain such that
  //  there are no duplicate HE in bnd_HE_chain and icurve_loop_chain.
  virtual void canonicalization ();
  
  //: Fix the case the bnd_chain is a icurve-pair.
  bool _fix_bnd_icurve_pair ();
  
  //Fix any duplicate E from icurve-pair in bnd-chain or icurve-loop.
  void fix_dup_Es_icurve_pair (dbmsh3d_halfedge* inputHE); 

  bool detect_fix_2_suc_Es (const dbmsh3d_halfedge* headHE);
  bool detect_fix_2_suc_Es_loop3 (const dbmsh3d_halfedge* headHE);
  bool detect_fix_2_sep_Es (const dbmsh3d_halfedge* headHE);

  //: Detect and fix any self-intersection of the chain of a halfedge loop.
  bool detect_fix_loop_self_int (dbmsh3d_halfedge*& headHE);

  bool disconnect_icurve_pair_E (dbmsh3d_edge* E);

  void disconnect_all_icurve_Es ();
  
  //: disconnect all associated halfedges from their edges and delete them.
  void disconnect_all_incident_Es ();

  void _clear_all_Cs_shared_Es ();

  void add_shared_Fs_to_S (dbmsh3d_sheet* S);
  void pass_shared_Fs_to_S (dbmsh3d_sheet* S);

  //###### Other functions ######
  virtual bool check_integrity ();
    bool check_Es_in_C_chain (const dbmsh3d_halfedge* headHE);

  virtual dbmsh3d_face* clone () const;
  void _clone_S_C_conn (dbmsh3d_sheet* S2, dbmsh3d_hypg* HG2) const;
  void _clone_S_F_conn (dbmsh3d_sheet* S2, dbmsh3d_mesh* M2);
  virtual dbmsh3d_sheet* clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2);

  virtual void getInfo (vcl_ostringstream& ostrm);

  //###### Handle Incident Virtual Curves ######

  //: Return the first found otherC of this sheet incident to N via F.
  bool get_otherC_via_F_nv (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                            dbmsh3d_curve*& otherC, dbmsh3d_face*& F);
};

bool fix_headHE_3_incidence (dbmsh3d_halfedge*& headHE);

//: Look for the nextHE not pointing to the same edge.
dbmsh3d_halfedge* _find_nextE_HE (const dbmsh3d_halfedge* inputHE);

//: Look for the prevHE not pointing to the same edge.
dbmsh3d_halfedge* _find_prevE_HE (const dbmsh3d_halfedge* inputHE);

//: Return true if a consecutive pair of identical HEs is found in the chain.
bool _detect_2_suc_Es (const dbmsh3d_halfedge* headHE);

//: Return true if [i,j] belongs to any conesecutive 3-incident edges.
bool _check_3_incidence (const vcl_vector<dbmsh3d_halfedge*>& HE_vec, 
                         const unsigned int i, const unsigned int j);

dbmsh3d_curve* _find_E_in_C_chain (const dbmsh3d_halfedge* headHE, dbmsh3d_edge* E);


//: Return true if the headHE next loop contains self intersection.
bool loop_self_int (const dbmsh3d_halfedge* headHE);

//: Given the inputHE and eV, find the next HE in the HE_loop_vec[].
//  Return true if exactly one such nextHE is found.
bool _find_nextHE_in_loop (const dbmsh3d_halfedge* inputHE, const dbmsh3d_vertex* eV, 
                           const vcl_vector<dbmsh3d_halfedge*>& HE_loop_vec,
                           dbmsh3d_halfedge*& nextHE);

//: Given the inputHE and eV, find the next HE in the HE_loop_vec[], considering 
//  the case of 3-incidence (should skip the 3-incidence).
//  Return the nextHE, and return the additional nextHE2 if it is found.
dbmsh3d_halfedge* _find_nextHE_in_loop_3inc (const dbmsh3d_halfedge* inputHE, 
                                             const dbmsh3d_vertex* eV, 
                                             const vcl_vector<dbmsh3d_halfedge*>& HEvet,
                                             dbmsh3d_halfedge*& HEnn2);

dbmsh3d_halfedge* _find_lastHE_in_chain (const dbmsh3d_halfedge* startHE, const dbmsh3d_vertex* startV, 
                                         const dbmsh3d_vertex* endV, const vcl_vector<dbmsh3d_halfedge*>& HEvec);

bool _check_loop_of_2_HEs (const dbmsh3d_halfedge* headHE);


bool _find_in_chain (const dbmsh3d_halfedge* headHE,
                     const dbmsh3d_vertex* sV, const dbmsh3d_vertex* eV, 
                     vcl_vector<dbmsh3d_halfedge*>& HEvec);

//: Go through each icurve HE and modify the face pointer.
void _modify_F_icurve_chain (dbmsh3d_halfedge* headHE, dbmsh3d_face* F);

//: Make sure that the loop_bnd_Es[] are problem free (for debugging).
bool _verify_loop_bnd_Es (const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es);

bool _find_V_reorder_Es (const dbmsh3d_vertex* Vc, const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es, 
                         vcl_vector<dbmsh3d_edge*>& reordered_Es);

void _get_chain_C_Evec (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_edge*>& Evec);

void _get_chain_C_shared_Es (const dbmsh3d_halfedge* headHE, vcl_set<dbmsh3d_edge*>& shared_Es);
void _get_C_shared_Es (dbmsh3d_curve* C, vcl_set<dbmsh3d_edge*>& shared_Es);

bool _disconnect_bnd3_E_chain (dbmsh3d_halfedge*& headHE, dbmsh3d_edge* E);

//###################################################################

inline void add_Ss_sharing_F (dbmsh3d_sheet* S1, dbmsh3d_sheet* S2, dbmsh3d_face* F) {
  S1->add_shared_F (F);
  S2->add_shared_F (F);
}

inline void check_add_Ss_sharing_F (dbmsh3d_sheet* S1, dbmsh3d_sheet* S2, dbmsh3d_face* F) {
  S1->check_add_shared_F (F);
  S2->check_add_shared_F (F);
}

inline void remove_Ss_sharing_F (dbmsh3d_sheet* S1, dbmsh3d_sheet* S2, dbmsh3d_face* F) {
  S1->del_shared_F (F);
  S2->del_shared_F (F);
}

bool _prop_label_Fs_e_conn (dbmsh3d_face* seedF, vcl_set<dbmsh3d_face*>& Fset);

void _prop_label_Fs_e_v_conn (dbmsh3d_face* seedF, vcl_set<dbmsh3d_face*>& Fset);

#endif

