//: Aug 19, 2005 MingChing Chang
//  Goedesic Shock Propagation Manager

#ifndef gdt_shock_prop_h_
#define gdt_shock_prop_h_

#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <gdt/gdt_shock.h>
#include <gdt/gdt_shock_section.h>

class gdt_shock_prop
{
protected:
  //: The shock queue is a map of <(dist, id), shock>.
  vcl_map<vcl_pair<double, int>, gdt_shock*> shock_queue_;

  dbmsh3d_face*     cur_face_;

  dbmsh3d_gdt_edge* cur_edge_;
  dbmsh3d_gdt_edge* left_edge_;
  dbmsh3d_gdt_edge* right_edge_;

  gdt_shock_section* shocks_to_cur_edge_;
  gdt_shock_section* shocks_to_left_edge_;
  gdt_shock_section* shocks_to_right_edge_;

  //: Variables for current computation
  //  Note that these alpha angles are w.r.t. to the current shock!
  double s_alpha_cl_;
  double s_alpha_cr_;
  double s_alpha_lr_;

  int n_verbose_;

public:
  //: ====== Constructor/Destructor ======
  gdt_shock_prop (dbmsh3d_face* cur_face, 
                  dbmsh3d_gdt_edge* cur_edge,
                  dbmsh3d_gdt_edge* left_edge,
                  dbmsh3d_gdt_edge* right_edge,
                  gdt_shock_section* shocks_to_cur_edge,
                  gdt_shock_section* shocks_to_left_edge,
                  gdt_shock_section* shocks_to_right_edge,
                  int n_verbose) {
    cur_face_ = cur_face;

    cur_edge_             = cur_edge;
    left_edge_            = left_edge;
    right_edge_           = right_edge;
    shocks_to_cur_edge_   = shocks_to_cur_edge;
    shocks_to_left_edge_  = shocks_to_left_edge;
    shocks_to_right_edge_ = shocks_to_right_edge;

    n_verbose_ = n_verbose;
  }

  ~gdt_shock_prop () {
  }

  //: ====== Data access functions ======
  vcl_map<vcl_pair<double, int>, gdt_shock*>* shock_queue() {
    return &shock_queue_;
  }
  void _add_shock (gdt_shock* S) {
    vcl_pair<double, int> key (S->simT(), S->id()); //simtime??
    shock_queue_.insert (vcl_pair<vcl_pair<double, int>, gdt_shock*>(key, S));
  }

  //: ====== Shock propagation functions ======
  //: Shock propagation should be linear.
  void propagate_gdt_shocks ();

  //  For each shock, try to intersect with 
  //  left_neighbor, right_neighbor, and the two other edges.
  void propagate_gdt_shock_one_step ();

  void _propagate_shock_from_edge (gdt_shock* S, 
                                   dbmsh3d_gdt_edge* s_cur_edge,
                                   dbmsh3d_gdt_edge* s_left_edge,
                                   dbmsh3d_gdt_edge* s_right_edge,
                                   gdt_shock_section* shocks_to_s_cur_edge,
                                   gdt_shock_section* shocks_to_s_left_edge,
                                   gdt_shock_section* shocks_to_s_right_edge);

  void _propagate_shock_from_vertex (gdt_shock* S, 
                                     dbmsh3d_gdt_edge* s_from_left_edge,
                                     dbmsh3d_gdt_edge* s_from_right_edge,
                                     dbmsh3d_gdt_edge* s_dest_edge,
                                     gdt_shock_section* shocks_to_s_dest_edge);

};

#endif

