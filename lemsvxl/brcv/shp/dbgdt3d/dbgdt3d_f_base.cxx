//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_mesh.h>

// ###########################################################

void gdt_f_manager_base::add_wavefront_he (dbmsh3d_halfedge* cur_he) 
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  double dist = cur_edge->dist();

#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cout, "    Add edge %d(%f) to the front.\n", cur_edge->id(), dist);
#endif

  assert (cur_edge->intervals_a_coverage());
  //: check that cur_he is not in the front with another dist value.
  ///assert (_brute_force_find_in_wavefront (cur_he) == wavefront_.end());

  wavefront_.insert (vcl_pair<double, dbmsh3d_halfedge*> (dist, cur_he));
}

bool gdt_f_manager_base::remove_wavefront_he (dbmsh3d_halfedge* cur_he) 
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();

  vcl_multimap<double, dbmsh3d_halfedge*>::iterator it = _find_in_wavefront (cur_he);
  if (it != wavefront_.end()) {
#if GDT_DEBUG_MSG
    if (n_verbose_>2) {
      double dist = (*it).first;
      vul_printf (vcl_cout, "    Remove edge %d(%f) from the front.\n", cur_edge->id(), dist);
    }
#endif
    wavefront_.erase (it);
    return true;
  }
  else
    return false;
}

//: Remove all halfedges of the input_edge from the wavefront.
bool gdt_f_manager_base::remove_wavefront_edge (dbmsh3d_gdt_edge* input_edge) 
{
  dbmsh3d_halfedge* cur_he = input_edge->halfedge();

  assert (cur_he != NULL);
  //: if there's only one associated halfedge (no loop)
  if (cur_he->pair() == NULL) {
    return remove_wavefront_he (cur_he);
  }

  //: the last case, the associated halfedges form a circular list
  bool b_success = false;
  do {
    b_success |= remove_wavefront_he (cur_he);
    cur_he = cur_he->pair();
  }
  while (cur_he != input_edge->halfedge());

  return b_success;
}

//: For visualization, not about computation.
void gdt_f_manager_base::mark_front_edges_visited ()
{
  gdt_mesh_->init_traverse();

  //: go through the front elements and mark each edge.
  vcl_multimap<double, dbmsh3d_halfedge*>::iterator wit = wavefront_.begin();
  for (; wit != wavefront_.end(); wit++) {
    dbmsh3d_halfedge* cur_he = (*wit).second; 
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();

    cur_edge->set_i_visited (gdt_mesh_->i_traverse_flag());
  }
}
