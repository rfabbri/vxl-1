//: This is dbsk3d/dbsk3d_ms_curve.h
//  MingChing Chang
//  Nov 23, 2004.

#ifndef dbsk3d_ms_curve_h_
#define dbsk3d_ms_curve_h_

#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class dbsk3d_ms_node;
class dbsk3d_ms_sheet;
class dbsk3d_ms_hypg;
class dbsk3d_sg_sa;

//###############################################################
//       dbsk3d_ms_curve
//###############################################################


class dbsk3d_ms_curve : public dbmsh3d_curve
{
protected:
  float         length_;

  //: temp: the underlie supporting curves of a virtual curve
  vcl_vector<dbmsh3d_curve*> sup_curves_;
  
  bool          selected_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_curve (dbmsh3d_vertex* s_V, dbmsh3d_vertex* e_V, int id) :
      dbmsh3d_curve (s_V, e_V, id)
  {
    length_     = 0.0f;
    selected_   = false;
  }
  virtual ~dbsk3d_ms_curve () {
  }

  //###### Data access functions ######
  const float length() const {
    return length_;
  }
  void set_length (const float len) {
    length_ = (float) len;
  }
  const bool selected() const {
    return selected_;
  }
  void set_selected (const bool s) {
    selected_ = bool(s);
  }

  const dbsk3d_ms_node* s_MN() const {
    return (dbsk3d_ms_node*) vertices_[0];
  }
  dbsk3d_ms_node* s_MN() {
    return (dbsk3d_ms_node*) vertices_[0];
  }
  const dbsk3d_ms_node* e_MN() const {
    return (dbsk3d_ms_node*) vertices_[1];
  }
  dbsk3d_ms_node* e_MN() {
    return (dbsk3d_ms_node*) vertices_[1];
  }
  
  const bool self_loop_on_LV () const;

  vcl_vector<dbmsh3d_curve*>& sup_curves () {
    return sup_curves_;
  }
  dbmsh3d_curve* sup_curves (const int i) {
    return sup_curves_[i];
  }
  void add_sup_curve (dbmsh3d_curve* C) {
    sup_curves_.push_back (C);
  }

  //###### Query Functions ######
  double get_sum_node_radius () const;

  //###### Modification Functions ######
  virtual void add_E_to_back (const dbmsh3d_edge* E) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (E);
    if (E_vec_.size() > 0)
      assert (Es_sharing_V (E_vec_[E_vec_.size()-1], FE));
    E_vec_.push_back (FE);
    FE->set_s_value (id_);
  }

  //####### Generator handling functions #######
  void get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_Gs) const;  
  void get_asso_Gs_incld_FFs (vcl_map<int, dbmsh3d_vertex*>& asso_Gs) const;

  //###### Other functions ######
  virtual bool check_integrity ();

  //: The clone function clones everything except the connectivity
  //  when cloning a graph, need to build the connectivity separetely.
  virtual dbmsh3d_edge* clone () const;
  virtual dbsk3d_ms_curve* clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2) const;

  virtual void getInfo (vcl_ostringstream& ostrm);

  //: if length_ != 0, no need to re-compute.
  double get_length ();

  void setup_virtual_curve ();
  C_TYPE get_C_VC_c_type() const;
};

inline void _swap_ms_curve (dbsk3d_ms_curve*& MC1, dbsk3d_ms_curve*& MC2)
{  
  dbsk3d_ms_curve* tmp = MC1;
  MC1 = MC2;
  MC2 = tmp;
}

void clone_ms_curve_sg_sa (dbsk3d_ms_curve* targetSC, dbsk3d_ms_curve* inputSC,
                           dbsk3d_sg_sa* targetSG);

dbsk3d_fs_vertex* closest_MN_MC (dbsk3d_ms_node* MN, dbsk3d_ms_curve* MC);

double closest_MC_MC (dbsk3d_ms_curve* A13_curve, dbsk3d_ms_curve* A3_curve, 
                      dbsk3d_fs_vertex* & closest_A13_node, dbsk3d_fs_vertex* & closest_A3_node);

//###############################################################
//       dbsk3d_ms_curve TEXT FILE I/O FUNCTIONS
//###############################################################

//: File I/O for FS/SHG
void mc_save_text_file (vcl_FILE* fp, dbsk3d_ms_curve* MC);
void mc_load_text_file (vcl_FILE* fp, dbsk3d_ms_curve* MC, 
                        dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_hypg* ms_hypg);

#endif
