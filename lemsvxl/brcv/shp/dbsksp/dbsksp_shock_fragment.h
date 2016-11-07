// This is shp/dbsksp/dbsksp_shock_fragment.h
#ifndef dbsksp_shock_fragment_h_
#define dbsksp_shock_fragment_h_

//:
// \file
// \brief A class for shock fragment
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Sep 30, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>

#include <dbgl/algo/dbgl_conic_arc.h>

#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_bnd_arc_sptr.h>
#include <dbsksp/dbsksp_bnd_arc.h>
#include <dbsksp/dbsksp_shapelet.h>

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_shock_fragment : public vbl_ref_count
{
public:
  // =============== CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ===============
  //: constructor
  dbsksp_shock_fragment();

  //: destructor
  virtual ~dbsksp_shock_fragment(){};

  
  // ===============  DATA ACCESS =====================================
  
  //: Get and set the shock edge
  dbsksp_shock_edge_sptr shock_edge() const {return this->shock_edge_; }

  void set_shock_edge(const dbsksp_shock_edge_sptr& shock_edge)
  { this->shock_edge_ = shock_edge; }


  //: Get and set the left circular arc boundary 
  dbsksp_bnd_arc_sptr left_bnd() const {return this->bnd_[0]; }
  
  void set_left_bnd(const dbsksp_bnd_arc_sptr& left_bnd)
  { this->bnd_[0] = left_bnd; }
  
  

  //: Get and set the right circular arc boundary 
  dbsksp_bnd_arc_sptr right_bnd() const { return this->bnd_[1]; }
  //{ return this->right_bnd_; }
  
  void set_right_bnd(const dbsksp_bnd_arc_sptr& right_bnd)
  { this->bnd_[1] = right_bnd; }
  //{ this->right_bnd_ = right_bnd; }

  //: Get and Set a boundary by its index
  dbsksp_bnd_arc_sptr bnd(int i) const {return this->bnd_[i]; }
  void set_bnd(int i, const dbsksp_bnd_arc_sptr& arc)
  {this->bnd_[i] = arc; }

  //: Get and set the shock geometry (a conic)
  dbgl_conic_arc shock_geom() const
  { return this->shock_geom_; }
  
  void set_shock_geom(const dbgl_conic_arc& conic_arc)
  { this->shock_geom_ = conic_arc; }
  
  

  // ============= UTILITY FUNCTIONS ===============================

  //: Build `this' shock fragment using info from a shock edge
  void build_from(const dbsksp_shock_edge_sptr& edge);

  //: Get the shapelet of this fragment
  dbsksp_shapelet_sptr get_shapelet() const;

  //: Update parameters on the edge and its nodes to match with those on the shapelet
  void update_edge_with_shapelet(const dbsksp_shapelet_sptr& shapelet, 
    const dbsksp_shock_node_sptr& starting_node = 0,
    bool maintain_phi_sum_equal_pi = true);

  




  // =============== MISCELLANEOUS =====================================

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

  // =========== SUPPORT FUNCTIONS =======================================

protected:
  //: Build this fragment from a regular edge (source != target)
  void build_from_regular_edge(const dbsksp_shock_edge_sptr& edge);

  //: Build this fragment from a degenerate edge (source == target)
  void build_from_degenerate_edge(const dbsksp_shock_edge_sptr& edge);

  // ============= MEMBER VARIABLES =====================================
protected:
  //: the shock edge this fragment is associated with
  dbsksp_shock_edge_sptr shock_edge_;

  //: left and right circular arc boundary, w.r.t. the direction from source 
  // to target of the edge
  // bnd_[0] is left boundary
  // bnd_[1] is right boundary
  dbsksp_bnd_arc_sptr bnd_[2];

  //: geometry of the shock of the fragment, starting from the source node 
  // of the fragment
  dbgl_conic_arc shock_geom_;
};
#endif // shp/dbsksp/dbsksp_shock_fragment.h


