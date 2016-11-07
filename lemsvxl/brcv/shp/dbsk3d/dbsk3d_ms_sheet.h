//: This is dbsk3d_ms_sheet.h
//  MingChing Chang
//  Nov 23, 2004.

#ifndef dbsk3d_ms_sheet_h_
#define dbsk3d_ms_sheet_h_

#include <vcl_cassert.h>

#include <dbmsh3d/dbmsh3d_sheet.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_edge.h>

class dbsk3d_ms_node;
class dbsk3d_ms_curve;
class dbsk3d_fs_mesh;
class dbsk3d_ms_hypg;

//###############################################################
//       dbsk3d_ms_sheet
//###############################################################

class dbsk3d_ms_sheet : public dbmsh3d_sheet
{
protected:
  float         cost_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_sheet (int id) : dbmsh3d_sheet (id) {
    cost_     = 0.0f;
  }
  ~dbsk3d_ms_sheet () {
  }

  //###### Data access functions ######
  const float cost() const {
    return cost_;
  }
  void set_cost (const float cost) {
    cost_ = (float) cost;
  }

  //###### Connectivity Query Functions ######
  bool has_incident_A3rib () const;
  bool has_bnd_A3rib () const;

  void get_incident_A3ribs (vcl_set<dbmsh3d_edge*>& A3_MCs) const;
  void get_bnd_A3ribs (vcl_set<dbmsh3d_edge*>& A3_MCs) const;

  void get_incident_FEs (vcl_set<dbmsh3d_edge*>& incident_FEs) const;
  void get_incident_FVs (vcl_set<dbmsh3d_vertex*>& incident_FVs) const;
  
  //: Get all associated generators of this MS.
  int get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_genes,
                   const bool remove_A12_A3_Gs = false);

  //: Get the associated boundary mesh faces (triangles) of this fs_sheet.
  void get_bnd_mesh_Fs (vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                        vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1);

  //: Get the set of incident axials of this ms_sheet.
  void get_axial (vcl_set<dbmsh3d_edge*>& axial_set) const;

  //: Get the set of boundary axials of this ms_sheet.
  int get_axial_bnd (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;

  //: Get the set of i-curve (pairs and loops) axials of this ms_sheet.
  int get_axial_icurve (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;

  //: Get the set of non-swallowtail incident axial_set of this ms_sheet.
  void get_axial_nonsw (vcl_set<dbmsh3d_edge*>& axial_set) const;

  //: Get the set of non-swallowtail boundary axials of this ms_sheet.
  int get_axial_nonsw_bnd (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;

  //: Get the set of non-swallowtail i-curve (pairs and loops) axials of this ms_sheet.
  int get_axial_nonsw_icurve (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;

    int _get_axial_chain (const dbmsh3d_halfedge* headHE, 
                          vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;
    int _get_axial_nonsw_chain (const dbmsh3d_halfedge* headHE, 
                                vcl_vector<dbsk3d_ms_curve*>& axial_vec) const;
    
  //: Get the set of incident axials of this ms_sheet incident to N.
  void get_axial_inc_N (const dbmsh3d_vertex* V, 
                        vcl_set<dbmsh3d_edge*>& axial_N_set) const;
  
  void get_bndN_inc_axial (vcl_set<dbmsh3d_vertex*>& bnd_N_axial_set) const;

  void find_MCs_sharing_E (dbsk3d_ms_curve* inputMC, vcl_vector<dbsk3d_ms_curve*>& shared_MC_set) const;

  const dbsk3d_fs_vertex* get_middle_FV () {    
    //bad-- just return the FF with the median id.
    unsigned int mid = (unsigned int) facemap_.size()/2;
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
    for (unsigned int i=0; i<mid; i++)
      it++;
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) (*it).second;
    dbmsh3d_halfedge* HE = FE->halfedge();
    return (dbsk3d_fs_vertex*) HE->edge()->sV();
  }

  //###### Modification Functions ######

  void mark_all_FFs_invalid ();
  
  //: prune and pass associated generators to remaining shock curves.
  bool prune_pass_Gs (vcl_set<dbmsh3d_vertex*>& asso_genes);
  
  //: Re-organize this MS's bnd_HE_chain and icurve_HE_chain such that
  //  there are no duplicate HE in bnd_HE_chain and icurve_loop_chain.
  virtual void canonicalization ();

  bool move_rib_to_bnd_chain ();
  
  double compute_cost ();
  //: if length_ != 0, no need to re-compute.
  double get_cost ();

  //###### Other functions ######  
  virtual dbmsh3d_face* clone () const;
  virtual dbsk3d_ms_sheet* clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2);

  virtual void getInfo (vcl_ostringstream& ostrm);
};
  
//###############################################################
//       dbsk3d_ms_sheet TEXT vcl_FILE I/O FUNCTIONS
//###############################################################

void ms_save_text_file (vcl_FILE* fp, dbsk3d_ms_sheet* MS);
void ms_load_text_file (vcl_FILE* fp, dbsk3d_ms_sheet* MS, 
                        dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_hypg* ms_hypg);

//###############################################################
//       OTHER FUNCTIONS
//###############################################################

bool MC_chain_contains_rib (const dbmsh3d_halfedge* headHE);

int n_incidence_MC_MS (const dbsk3d_ms_curve* MC, dbsk3d_ms_sheet* MS);

bool _insert_bnd_loop (dbsk3d_ms_sheet* MS, const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es);

#endif
