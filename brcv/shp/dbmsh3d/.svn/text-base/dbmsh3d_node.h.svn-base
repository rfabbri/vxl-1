#ifndef dbmsh3d_node_h_
#define dbmsh3d_node_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_node.h
//:
// \file
// \brief Basic 3d point sample on a mesh
//
//
// \author
//  MingChing Chang  Oct 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>

class dbmsh3d_curve;
class dbmsh3d_mesh;

#define N_TYPE                  char
#define N_TYPE_BOGUS            '?'
#define N_TYPE_RIB_END          '3'   //A1A3
#define N_TYPE_AXIAL_END        '4'   //A14
#define N_TYPE_DEGE_AXIAL_END   'D'   //Degenerate A1N (N>4)
#define N_TYPE_DEGE_RIB_END     'E'   //Degenerate A1MA3 (M>1) or (A1A3)_k or A1A5.
#define N_TYPE_LOOP_END         'L'   //Loop End

class dbmsh3d_node : public dbmsh3d_vertex
{
protected:
  dbmsh3d_vertex*     V_;

public:
  //####### Constructor/Destructor #######
  dbmsh3d_node (int id) : dbmsh3d_vertex (id) {
    V_ = NULL;
  }
  virtual ~dbmsh3d_node () {
  }

  //###### Data access functions ######
  dbmsh3d_vertex* V() const {
    return V_;
  }
  dbmsh3d_vertex* V() {
    return V_;
  }
  void set_V (const dbmsh3d_vertex* V) {
    V_ = (dbmsh3d_vertex*) V;
    V_->set_vid (id_);
  }

  //###### Connectivity query functions ######
  
  //: Return # of incident loop curves.
  int count_incident_Cs (int& nRib, int& nAxial, int& nDege, int& nVirtual) const;

  bool has_rib_C () const;

  bool is_dummy_node (dbmsh3d_curve* C1, dbmsh3d_curve* C2) const;

  dbmsh3d_curve* find_C_containing_E (const dbmsh3d_edge* E) const;

  //###### Modification Functions ######
  void compute_n_type ();

  //###### Other functions ######
  virtual bool check_integrity ();

  //: The clone function clones everything except the connectivity
  //  when cloning a graph, need to build the connectivity separetely.
  virtual dbmsh3d_vertex* clone () const;
  virtual dbmsh3d_node* clone (dbmsh3d_mesh* M) const;

  virtual void getInfo (vcl_ostringstream& ostrm);

  //###### Handle Incident Virtual Curves ######
  unsigned int n_incident_Cs_nv () const;
  unsigned int n_E_incidence_nv () const;

  dbmsh3d_curve* find_C_containing_E_nv (const dbmsh3d_edge* E) const;

  void get_incident_Es_nv (vcl_set<void*>& incident_Es) const;

  bool all_incident_Es_in_set_nv (vcl_set<dbmsh3d_edge*>& E_set) const;

};


bool dbmsh3d_get_canonical_type (const int nA3, const int nA13, const int nDege, 
                                 int& m, int& n, int& f);

//###### Handle Incident Virtual Curves ######

dbmsh3d_curve* N_find_other_C_nv (const dbmsh3d_node* N, const dbmsh3d_curve* inputC);

#endif

