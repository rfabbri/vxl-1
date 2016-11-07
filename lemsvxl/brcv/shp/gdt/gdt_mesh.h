//: Aug 19, 2005 MingChing Chang
//  Goedesic Distance Transform (DT) Wavefront Propagation mesh
//  Extented definition of a mesh

#ifndef gdt_mesh_h_
#define gdt_mesh_h_

#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <gdt/gdt_vertex.h>
#include <gdt/gdt_edge.h>

class dbmsh3d_gdt_mesh : public dbmsh3d_mesh
{
public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_gdt_mesh () {
  }
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbmsh3d_gdt_vertex_3d (vertex_id_counter_++);
  }
  virtual void _del_vertex (dbmsh3d_vertex* v) {
    dbmsh3d_gdt_vertex_3d* vertex = (dbmsh3d_gdt_vertex_3d*) v;
    delete vertex;
  }

  //: ====== Data access functions ======
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sV,
                                   dbmsh3d_vertex* eV) {
    return new dbmsh3d_gdt_edge (sV, eV, edge_id_counter_++);
  }
  virtual void _del_edge (dbmsh3d_edge* edge) {
    dbmsh3d_gdt_edge* gdt_edge = (dbmsh3d_gdt_edge*) edge;
    delete gdt_edge;
  }

  //: ====== Modification functions ======
  void compute_edge_lengths ();
  void compute_vertex_sum_angles ();
};

#endif



