//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbgdt3d/dbgdt3d_manager.h>

//: initialize the geodesic DT (for each source S_i)
//  assume triangular mesh
//  For each point source (vertex of the mesh) s_i of S,
//    For each adjacent mesh face f_j of s_i,
//      Initialize a point-source interval I_j with an epsilon simtime with
//      s_i as the pseudo-source on the opposite edge e_j of the face f_j,
//
//      - I_j = create_ratefaction_interval (s_i, e_j, 0, len(e_j))
//      - Add <simtime, I_j> into the wavefront queue Qw.
//
//      Initialize a degenerate interval I_d with an epsilon simtime with
//      s_i as the pseudo-source on the adjacent edge e_d.
//
//      - I_d = create_dege_interval (e, s_i)
//      - Add <simtime, I_d> into the wavefront queue Qw.  
//    End
//  End
//
void gdt_ws_manager::gdt_init ()
{
  gdt_welm* W;

  //: Reset the face traversing.
  gdt_mesh_->reset_traverse ();

  // Initialize the 4 queues to be empty.
  Qw_.clear();
  Qv_.clear();
  Qs_.clear();

  if (n_verbose_>1)
    vul_printf (vcl_cout, "Initialization for %d sources:\n", (int) source_vertices_.size());

  //: For each point source (vertex of the mesh) s_i of S, initialize a wavefront circle.
  for (unsigned int i=0; i<source_vertices_.size(); i++) {
    dbmsh3d_gdt_vertex_3d* src_vertex = source_vertices_[i];
    src_vertex->set_dist (0);

    vcl_vector<dbmsh3d_halfedge*> src_ordered_halfedges;
    src_ordered_halfedges.clear();

    dbmsh3d_halfedge* last_he = src_vertex->m2_get_ordered_HEs (src_ordered_halfedges);

    //: create the last initial degenerate interval.
    //  This happens when the init. source is at the manifold boundary
    if (last_he) {
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) last_he->edge();

      W = create_degeW (last_he, src_vertex == edge_d->sV());
      add_to_Qw (W);
      
#if GDT_DEBUG_MSG
      if (n_verbose_>1)
        vul_printf (vcl_cout, "    dege_I on edge %d initialized.\n", edge_d->id());
#endif
    }

    //: for each edge_j on face_j against the src_vertex, create an init. interval.
    for (unsigned int j=0; j<src_ordered_halfedges.size(); j++) {

      //: create the initial degenerate interval
      dbmsh3d_halfedge* he_d = src_ordered_halfedges[j];
      dbmsh3d_gdt_edge* edge_d = (dbmsh3d_gdt_edge*) he_d->edge();

      W = create_degeW (he_d, src_vertex == edge_d->sV());
      add_to_Qw (W);

      //: create the init. wavefront edge on edge_j
      dbmsh3d_face* face_j = he_d->face();
      dbmsh3d_halfedge* he_j = face_j->m2t_halfedge_against_vertex (src_vertex);
      dbmsh3d_gdt_edge* edge_j = (dbmsh3d_gdt_edge*) he_j->edge();

      W = create_RF (he_j, src_vertex, 0, edge_j->len());
      add_to_Qw (W);

#if GDT_DEBUG_MSG
      if (n_verbose_>1) {
        vul_printf (vcl_cout, "    psrc_I on edge %d initialized.\n", edge_j->id());
        vul_printf (vcl_cout, "    dege_I on edge %d initialized.\n", edge_d->id());
        vul_printf (vcl_cout, "  wavefront arc on face %d initialized.\n\n", face_j->id());
      }
#endif
    }
  }

}











