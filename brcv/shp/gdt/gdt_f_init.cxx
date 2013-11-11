//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <gdt/gdt_manager.h>

//: initialize the geodesic DT (for each source S_i)
//  assume triangular mesh
void gdt_f_manager::gdt_init ()
{
  gdt_interval* I;

  //: Reset the face traversing.
  gdt_mesh_->reset_traverse ();

  if (n_verbose_>1)
    vul_printf (vcl_cerr, "Initialization for %d sources:\n", (int)source_vertices_.size());

  //: Construct a wavefront for each src_vertex by putting its one-ring neighborhood edges surrounding it
  //  assume the sources are not adjacent !!
  for (unsigned int i=0; i<source_vertices_.size(); i++) {
    dbmsh3d_gdt_vertex_3d* src_vertex = source_vertices_[i];
    src_vertex->set_dist (0);

    vcl_vector<dbmsh3d_halfedge*> ordered_halfedges;
    ordered_halfedges.clear();

    dbmsh3d_halfedge* last_he = src_vertex->m2_get_ordered_HEs (ordered_halfedges);

    //: create the last init. internal propagated edge
    //  happens when the init. source is at the manifold boundary
    if (last_he) {
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) last_he->edge();
      I = create_dege_I (last_he, src_vertex == edge_d->sV());
      edge_d->_attach_interval (I);

#if GDT_DEBUG_MSG
      if (n_verbose_>1)
        vul_printf (vcl_cerr, "  init_dege_edge %d propagated\n", last_he->edge()->id());
#endif
    }

    for (unsigned int j=0; j<ordered_halfedges.size(); j++) {
      dbmsh3d_halfedge* he_d = ordered_halfedges[j];
      dbmsh3d_face* face_j = he_d->face();

      //: for m2t, use m2t_halfedge_against_vertex()
      //  for convex polygon, replace with a while loop.
      dbmsh3d_halfedge* he_j = face_j->m2t_halfedge_against_vertex (src_vertex);
      dbmsh3d_gdt_edge* edge_j = (dbmsh3d_gdt_edge*) he_j->edge();

      //: create the init. degenerate edge
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) he_d->edge();
      I = create_dege_I (he_d, src_vertex == edge_d->sV());
      edge_d->_attach_interval (I);

      //: create the init. wavefront edge
      I = create_rf_I (he_j, src_vertex, 0, edge_j->len());
      edge_j->_attach_interval (I);

      face_j->set_visited (true);

#if GDT_DEBUG_MSG
      if (n_verbose_>1) {
        vul_printf (vcl_cerr, "  init_wavefront_edge %d propagated\n", edge_j->id());
        vul_printf (vcl_cerr, "  init_dege_edge %d propagated\n", he_d->edge()->id());
        vul_printf (vcl_cerr, "    face %d propagated.\n", face_j->id());
      }
#endif

      //: if edge_j not on the boundary, add it to the frint
      if (edge_j->n_incident_Fs() > 1)
        add_wavefront_he (he_j);
    }
  }
}







