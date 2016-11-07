//: Aug 19, 2005 MingChing Chang
//  Goedesic Distance Transform (DT) Wavefront Propagation mesh
//  Extented definition of a mesh

#ifndef gdt_vertex_h_
#define gdt_vertex_h_

#include <vcl_set.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbgdt3d/dbgdt3d_numerical.h>

class gdt_interval;
class gdt_path;

//: the 'virtual' source
//  See the note for 'splitting' of wavefront into intervals.
//  A virtual source S is associated with each split.
//  The virtual source contains info about
//    - the shortest path to the parent virtual source S'
//    - the distance to the original true source S0
class dbmsh3d_gdt_vertex_3d : public dbmsh3d_vertex
{
protected:
  //: the distance to the original true source S0.
  //  We store it for quick reference.
  double dist_;

  bool b_interior_;

  //: for an interior vertex only
  double sumtheta_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_gdt_vertex_3d (int id) : dbmsh3d_vertex (id) {
    dist_ = INVALID_DIST;
    b_interior_ = false;
    
    #if GDT_ALGO_WS
    b_propagated_ = false;
    #endif
  }
  //: ====== Data access functions ======
  double dist () const {
    return dist_;
  }
  void set_dist (const double d) {
    dist_ = (double) d;
  }

  bool is_interior () const {
    return b_interior_;
  }
  void set_interior (const bool b) {
    b_interior_ = b;
  }

  double sumtheta () const {
    return sumtheta_;
  }
  void set_sumtheta (const double t) {
    sumtheta_ = (double) t;
  }
  bool is_hyperbolic () {
    assert (b_interior_ == true);
    return _lessM (vnl_math::pi*2, sumtheta_); //_lessT
  }
  bool is_elliptic () {
    assert (b_interior_ == true);
    return _lessM (sumtheta_, vnl_math::pi*2); //_lessT
  }

  //: ====== Query functions ======
  bool is_real_source () {
    return dist_ == 0.0;
  }
  //: return one interval adjacent to this vertex
  gdt_interval* get_adjacent_interval ();

  //: return the whole geodesic path to the closest source.
  dbmsh3d_gdt_vertex_3d* V_get_gdt_path (gdt_path* gdt_path);

  //: get the iterations required to cover the query_s
  int V_iter_to_source (double query_s, double& local_s);

  
#if GDT_ALGO_WS // =============================================
protected:
  vcl_set<gdt_interval*> childIs_;

  //: true if the shock from it has already been launched.
  bool b_propagated_;

public:
  vcl_set<gdt_interval*>& childIs() {
    return childIs_;
  }
  void _add_childI (const gdt_interval* I) {
    childIs_.insert ((gdt_interval*) I);
  }
  void _del_childI (const gdt_interval* I) {
    childIs_.erase ((gdt_interval*) I);
  }
  gdt_interval* _find_childI_on_edge (const dbmsh3d_edge* edge);

  bool b_propagated() {
    return b_propagated_;
  }
  void set_propagated (const bool b) {
    b_propagated_ = b;
  };
#endif // =============================================
};

#endif



