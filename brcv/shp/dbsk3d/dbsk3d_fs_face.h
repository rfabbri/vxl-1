//: This is dbsk3d_fs_face.h
//  MingChing Chang
//  Nov 14, 2004.

#ifndef dbsk3d_fs_face_h_
#define dbsk3d_fs_face_h_

#include <vcl_set.h>
#include <vcl_map.h>
#include <vgl/vgl_point_3d.h>

#include <dbmsh3d/dbmsh3d_face.h>

class dbsk3d_fs_edge;
class dbsk3d_fs_vertex;

//###############################################################
//       dbsk3d_fs_face
//###############################################################

class dbsk3d_fs_face : public dbmsh3d_face
{
protected:
  //: Two generators associated with this dbsk3d_fs_face.
  dbmsh3d_vertex*   genes_[2];

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_face (int id) : dbmsh3d_face (id) { 
    genes_[0]     = NULL;
    genes_[1]     = NULL;
  }

  virtual ~dbsk3d_fs_face() {}

  virtual void assign_value (dbsk3d_fs_face* FF) {
    genes_[0]       = FF->genes(0);
    genes_[1]       = FF->genes(1);
  }

  //####### Data access functions #######  
  const dbmsh3d_vertex* genes (const int i) const {
    return genes_[i];
  }
  dbmsh3d_vertex* genes (const int i) {
    return genes_[i];
  }

  //####### Graph connectivity query functions #######
  void get_min_max_V_time (float& min_time, float& max_time);
  float max_time ();

  //: compute the geometric center point radius
  double compute_center_pt_time();

  bool is_elongate (const float ratio) const;
  
  //: Return true if this fs_face is incident to both A13 and A3 shock link elements.
  bool is_splice_cost_zero ();

  //####### Generator handling functions #######
  const dbmsh3d_vertex* other_G (const dbmsh3d_vertex* G) const {
    if (G == genes_[0])
      return genes_[1];
    else
      return genes_[0];
  }
  dbmsh3d_vertex* other_G (dbmsh3d_vertex* G) {
    if (G == genes_[0])
      return genes_[1];
    else
      return genes_[0];
  }
  const bool is_G_asso (const dbmsh3d_vertex* G) const {
    if (G == genes_[0])
      return true;
    else if (G == genes_[1])
      return true;
    else
      return false;
  }

  void set_G (const int i, const dbmsh3d_vertex* G) {
    //assert (i<2);
    genes_[i] = (dbmsh3d_vertex*) G;
  }
  bool remove_G (const dbmsh3d_vertex* G);

  void get_incoming_Gs (vcl_set<dbmsh3d_vertex*>& incomingG);

  //####### Flow type computation functions #######  
  const vgl_point_3d<double> mid_pt() const {
    return centre (genes_[0]->pt(), genes_[1]->pt());
  }
  double dist_Gs() const {
    return vgl_distance (genes_[0]->pt(), genes_[1]->pt());
  }
  const double mid_pt_time() const {
    return dist_Gs() / 2;
  }
  
  //: Return true if the fs_face contains the A12-2 source point.
  //  i.e., the edge of G0-G1 is a Gabriel edge,
  //  i.e., ball of G0-G1 does not contain any other generator.
  bool contain_A12_2 ();
  bool contain_A12_2 (const vcl_vector<dbmsh3d_vertex*>& vertices);

  //: Detect the flow type of this fs_face
  //  For type II fs_face, return the incoming fs_edge.
  //  For type III fs_face, return the incoming fs_vertex.
  FF_FLOW_TYPE detect_flow_type (vispt_elm*& elm);
  FF_FLOW_TYPE detect_flow_type ();

  //Find the incoming fs_edge Li containing the A13-3 relay.
  //Li is the bi-directional fs_edge with smallest A13-3 radius.
  dbsk3d_fs_edge* type_II_find_incoming_FE ();

  //####### Surface meshing functions #######
  dbmsh3d_edge* get_bnd_E () const {
    dbmsh3d_edge* E = E_sharing_2V (genes_[0], genes_[1]);
    //Check consistency during meshing: if such bnd_edge exists, b_valid_ has to be false.
    assert ((E!=NULL) == (!b_valid_));
    return E;
  }

  bool is_on_shock_bnd () const;

  //: The surface triangle(s) intersecting this fs_face.
  const unsigned int count_dual_surf_DT() const;
  const bool not_meshed() const;
  const bool one_side_meshed() const;
  const bool both_sides_meshed() const;
  const bool both_sides_meshed (dbsk3d_fs_edge** L1, dbsk3d_fs_edge** L2) const;

  //: get the set of boundary shock link with bnd_faces.
  void get_FEs_with_bnd_F (vcl_set<dbsk3d_fs_edge*>& FE_with_bnd_F_set) const;

  //: Add the boundary fs_edges associated with genes (if any) to the set.
  void get_bnd_FE_FV_with_Gs (vcl_set<dbsk3d_fs_edge*>& FE_with_G_set,
                              vcl_set<dbsk3d_fs_vertex*>& FV_with_G_set) const;

  //####### Reconstruction Functions #######
  virtual vgl_vector_3d<double> get_shock_flow (const vgl_point_3d<double>& center_pt);

  //: compute theta for a given point.
  double compute_theta (const vgl_point_3d<double>& pt);

  void reconstruct_two_gene (vgl_point_3d<double>& center_pt,
                             vgl_point_3d<double>& p1, 
                             vgl_point_3d<double>& p2);

  //####### Other Functions #######

  dbmsh3d_face* clone () const;
  void _clone_FF_G_conn (dbsk3d_fs_face* FF2, dbmsh3d_pt_set* BND2) const;
  virtual dbsk3d_fs_face* clone (dbmsh3d_mesh* M2, dbmsh3d_pt_set* BND2) const;

  virtual void getInfo (vcl_ostringstream& ostrm);

};

//####### The connectivity handling functions #######
inline const dbmsh3d_vertex* FFs_sharing_G (const dbsk3d_fs_face* P1, 
                                            const dbsk3d_fs_face* P2)
{
  if (P1->is_G_asso (P2->genes(0)))
    return P2->genes(0);

  if (P1->is_G_asso (P2->genes(1)))
    return P2->genes(1);

  return NULL;
}

inline bool is_FF_inf (dbsk3d_fs_face* FF, const int n_orig_gene) 
{
  if (FF->genes(0)->id() >= n_orig_gene || FF->genes(1)->id() >= n_orig_gene)
    return true;
  else
    return false;
}

//####### dbsk3d_fs_face TEXT FILE I/O FUNCTIONS #######

void ff_save_text_file (vcl_FILE* fp, const dbsk3d_fs_face* FF);
void ff_load_text_file (vcl_FILE* fp, dbsk3d_fs_face* FF, 
                        vcl_map<int, dbmsh3d_edge*>& edgemap,
                        vcl_map<int, dbmsh3d_vertex*>& vertexmap);
#endif
