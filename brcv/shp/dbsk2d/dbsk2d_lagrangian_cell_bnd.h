// This is brcv/shp/dbsk2d/dbsk2d_lagrangian_cell_bnd.h
#ifndef dbsk2d_lagrangian_cell_bnd_h_
#define dbsk2d_lagrangian_cell_bnd_h_
//:
// \file
// \brief The boundaries of the lagrangian cells to keep track of wavefronts
// \author Amir Tamrakar
// \date 09/29/05
// 
// \verbatim
//  Modifications
//
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <dbsk2d/dbsk2d_ishock_utils.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd_sptr.h>

//: This class contains an ordered list of shocks intersecting with this extrinsic 
//  boundary of a lagrangian cells. This information is used when cells are merged
//  in the Eulerian-lagrangian computation.
//
class dbsk2d_lagrangian_cell_bnd : public vbl_ref_count
{
public:
  enum bnd_type
  {
    LEFT = 0,   //left vertical bnd
    RIGHT = 1,  //right vertical bnd
    TOP = 2,    //top horizontal bnd 
    BOTTOM = 3, //bottom horizontal bnd
  };

protected:
  //extrinsic parameters
  bnd_type _type; ///> type of bnd: horiz or vert
  double _loc;    ///> extrinsic location(coordinate) of this bnd

  //: ordered list of shocks that intersect with this boundary
  ordered_shock_list _active_shocks_list;

  //: special wavefront
  //  This is the belm that did not form shocks that intersected with this 
  //  cell bnd but have been inferred from shock intersections in adjacent bnds.
  dbsk2d_ishock_belm* _swf;

public:

  //: Constructor
  dbsk2d_lagrangian_cell_bnd(bnd_type type, double loc);

  //: Destructor
  virtual ~dbsk2d_lagrangian_cell_bnd();

  //-------------------------------------------------------------------
  // functions to access and set protected members
  //-------------------------------------------------------------------
  bnd_type type() { return _type; }
  bool is_horiz() { return _type>RIGHT; }
  bool is_vert() { return _type<TOP; }
  bool is_left() { return _type==LEFT; }
  bool is_right() { return _type==RIGHT; }
  bool is_top() { return _type==TOP; }
  bool is_bottom() { return _type==BOTTOM; }

  double loc() { return _loc; }

  //: set the extrinsic parameters of this boundary
  void set_ex_params(bnd_type type, double loc) { _type=type; _loc=loc; }

  //: return the list of shocks that intersected with this boundary
  ordered_shock_list & active_shocks_list() { return _active_shocks_list; }

  //: return the special wavefront
  dbsk2d_ishock_belm* swf() { return _swf; }

  //: add a shock to the active shocks list
  void add_shock_edge (dbsk2d_ishock_edge* Sedge);
  
  //: delete a shock from the active shocks list because it has terminated at an earlier point
  void delete_shock_edge (dbsk2d_ishock_edge* Sedge);

  //: set this belm as the SWF
  void set_swf (dbsk2d_ishock_belm* swf_belm) {_swf = swf_belm; }

  //: remove the SWF
  void clear_swf() { _swf = 0; }

  //: merge another cell's boundary with this one (same extrinsic location, just longer)
  void merge_with(dbsk2d_lagrangian_cell_bnd_sptr other);

  //: delete everything (all the shocks and any other addons)
  void clear();  
  
  //-------------------------------------------------------------------
  // utility functions
  //-------------------------------------------------------------------

  //: function to check if a given point is on this bnd
  bool is_pt_on_bnd(vgl_point_2d<double> pt);
};

#endif //dbsk2d_lagrangian_cell_bnd_h_
