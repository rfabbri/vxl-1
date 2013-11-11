//: Aug 19, 2005 MingChing Chang
//  

#include <gdt/gdt_mesh.h>

void dbmsh3d_gdt_mesh::compute_edge_lengths ()
{
  vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    dbmsh3d_gdt_edge* E = (dbmsh3d_gdt_edge*) (*it).second;
    E->interval_section()->set_len (E->length());
  }
}

void dbmsh3d_gdt_mesh::compute_vertex_sum_angles ()
{
  vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.begin();
  for (; it != vertexmap_.end(); it++) {
    dbmsh3d_gdt_vertex_3d* V = (dbmsh3d_gdt_vertex_3d*) (*it).second;
    if (V->get_1st_bnd_HE()) {
      V->set_interior (false);
      V->set_sumtheta (GDT_INVALID_HUGE);
    }
    else {
      V->set_interior (true);
      V->set_sumtheta (V->m2_sum_theta ());
    }
  }
}



