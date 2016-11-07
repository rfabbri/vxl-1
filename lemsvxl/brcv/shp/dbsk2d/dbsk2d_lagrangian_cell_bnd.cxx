// This is brcv/shp/dbsk2d/dbsk2d_lagrangian_cell_bnd.cxx

//:
// \file

#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>

//: constructor
dbsk2d_lagrangian_cell_bnd::dbsk2d_lagrangian_cell_bnd(bnd_type type, double loc):
  _type(type), _loc(loc)
{
  _swf = 0;
}

//: destructor
dbsk2d_lagrangian_cell_bnd::~dbsk2d_lagrangian_cell_bnd()
{
  _active_shocks_list.clear();
  _swf = 0;
} 

void dbsk2d_lagrangian_cell_bnd::add_shock_edge (dbsk2d_ishock_edge* Sedge)
{
  // and add the element to the active shock list
  _active_shocks_list.insert(key_selm_pair(r_id_pair(Sedge->bnd_intersect_pos(), Sedge->id()), Sedge));

  //put a pointer from the shock edge to this bnd
  Sedge->set_cell_bnd(this);
}

void dbsk2d_lagrangian_cell_bnd::delete_shock_edge (dbsk2d_ishock_edge* Sedge)
{
  //delete the element from the active shock list
  _active_shocks_list.erase(r_id_pair(Sedge->bnd_intersect_pos(), Sedge->id()));

  //remove the pointer from the shock edge
  Sedge->set_cell_bnd(0);
}

void dbsk2d_lagrangian_cell_bnd::clear ()
{
  _active_shocks_list.clear();
  _swf = 0;
}

//: merge another cell's boundary with this one
void dbsk2d_lagrangian_cell_bnd::merge_with(dbsk2d_lagrangian_cell_bnd_sptr other)
{
  //if both bnds have a SWF then it will not survive
  //however, if only one has it then it may survive
  if (_swf && other->swf())
    _swf = 0;
  else if (other->swf())
    _swf = other->swf();
  
  //merge the active shocks list of the two boundaries
  ordered_shock_list_iter s_it = other->active_shocks_list().begin();
  for ( ; s_it != other->active_shocks_list().end(); ++s_it)
    add_shock_edge(s_it->second);

}

//: function to check if a given point is on this bnd
bool dbsk2d_lagrangian_cell_bnd::is_pt_on_bnd(vgl_point_2d<double> pt)
{
  return ((LisEq(pt.x(), _loc) && is_vert() ) ||
          (LisEq(pt.y(), _loc) && is_horiz())   );
}

