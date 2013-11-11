//: Aug 19, 2005 MingChing Chang
//  

#include <gdt/gdt_interval.h>
#include <gdt/gdt_vertex.h>
#include <gdt/gdt_edge.h>
#include <gdt/gdt_path.h>

//: return any interval I that starts at this vertex
//  and I's pseudo-source != the current vertex itself.
//  if no such I found, return NULL (shouldn't happen)
gdt_interval* dbmsh3d_gdt_vertex_3d::get_adjacent_interval ()
{
  //: loop through every propagated incident edge
  //  check the endpoints and return the right I.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur->ptr();

    if (!cur_edge->is_propagated())
      continue;
    gdt_ibase* I = NULL;

    //: the interval_section on cur_edge should be a coverage
    if (cur_edge->sV() == this) {
      vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
      I = (*it).second;
      assert (I->stau() == 0);
    }
    else {
      vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
      I = (*rit).second;
      double len = cur_edge->length ();
      assert (I->etau() == len);
    }

    switch (I->type()) {
    case ITYPE_PSRC:
    case ITYPE_DEGE:
    {
      gdt_interval* II = (gdt_interval*) I;
      if (II->psrc()->is_real_source())
        return II;
      if (II->psrc() != this) 
        return II;
    }
    break;
    default:
    break;
    }

  }

  return NULL;
}

//: return the whole geodesic path to the closest source.
dbmsh3d_gdt_vertex_3d* dbmsh3d_gdt_vertex_3d::V_get_gdt_path (gdt_path* gdt_path)
{
  //: find the I to begin with back-tracking
  gdt_interval* I = get_adjacent_interval ();

  if (I->edge()->sV() == this)
    I->I_get_prev_path_till_psource (0, gdt_path);
  else
    I->I_get_prev_path_till_psource (I->edge()->len(), gdt_path);

  return I->psrc ();
}

//: now the query_s is from the pseudo-source.
int dbmsh3d_gdt_vertex_3d::V_iter_to_source (double query_s, double& local_s)
{
  //: find the I to begin with
  gdt_interval* I = get_adjacent_interval ();
  double len = I->edge()->length ();

  int iter;
  if (I->edge()->sV() == this)
    iter = I->I_iter_to_source (0, query_s, local_s);
  else
    iter = I->I_iter_to_source (len, query_s, local_s);

  return iter;
}

#if GDT_ALGO_WS

gdt_interval* dbmsh3d_gdt_vertex_3d::_find_childI_on_edge (const dbmsh3d_edge* edge) 
{
  vcl_set<gdt_interval*>::iterator it = childIs_.begin();
  for (; it != childIs_.end(); it++) {
    gdt_interval* I = (*it);
    if (I->edge() == edge)
      return I;
  }
  return NULL;
}

#endif



