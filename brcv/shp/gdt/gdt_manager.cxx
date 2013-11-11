//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <gdt/gdt_manager.h>
#include <gdt/gdt_mesh.h>

// ###########################################################

void gdt_manager::run_gdt (unsigned int n_total_iter)
{
  gdt_init ();

  gdt_propagation (n_total_iter);
}

void gdt_manager::_compute_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, const gdt_interval* I) const
{
  double distance;

  if (psrc == I->sV()) {
    distance = I->get_dist_at_tau (0);
  }
  else {
    assert (psrc == I->eV());
    distance = I->get_dist_at_tau (I->edge()->len());
  }

  psrc->set_dist (distance);
}

void gdt_manager::_compute_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, dbmsh3d_gdt_edge* cur_edge) const
{
  double distance;
  assert (cur_edge->interval_section()->size());

  if (psrc == cur_edge->sV()) {
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
    gdt_interval* I = (gdt_interval*) (*it).second;
    distance = I->get_dist_at_tau (0);
  }
  else {
    vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
    gdt_interval* I = (gdt_interval*) (*rit).second;
    distance = I->get_dist_at_tau (cur_edge->len());
  }

  psrc->set_dist (distance);
}

double gdt_manager::get_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, const gdt_interval* I) const
{
  if (psrc->dist() == INVALID_DIST)
    _compute_psrc_dist (psrc, I);
  return psrc->dist ();
}

double gdt_manager::get_psrc_dist (dbmsh3d_gdt_vertex_3d* psrc, dbmsh3d_gdt_edge* cur_edge) const
{
  if (psrc->dist() == INVALID_DIST)
    _compute_psrc_dist (psrc, cur_edge);
  return psrc->dist ();
}

// ###########################################################

//: for the given psrc, get the farthest vertex connected to it with a shortest path
gdt_interval* gdt_manager::find_farthest_vertex (dbmsh3d_gdt_vertex_3d* input_psource,
                                                 double& farthest_tau)
{
  double farthest_dist = 0;
  gdt_interval* farthest_I = NULL;
  dbmsh3d_gdt_vertex_3d* farthest_vertex = NULL;

  //: loop through all mesh vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator it = gdt_mesh_->vertexmap().begin();
  for (; it != gdt_mesh_->vertexmap().end(); it++) {
    dbmsh3d_gdt_vertex_3d* V = (dbmsh3d_gdt_vertex_3d*) (*it).second;

    //: we can only query distance from the edge now
    //  so select one edge to query the distance
    //  if all connecting edges are not propagated, just ignore this vertex.
    for (dbmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur->ptr();

      if (cur_edge->is_propagated() == 0)
        continue;

      //: either this vertex is the sV or eV of some edge.
      if (V == cur_edge->sV()) {
        double dist;
        gdt_interval* I = cur_edge->get_dist_at_tau (0, dist);

        if (I->is_dege())
          continue;

        //: ignore the invalid distance
        if (dist != GDT_INVALID_HUGE && dist > farthest_dist) {
          farthest_dist = dist;
          farthest_I = I;
          farthest_tau = 0;
          farthest_vertex = V;
          break;
        }
      }
      else {
        double dist;
        gdt_interval* I = cur_edge->get_dist_at_tau (cur_edge->len(), dist);

        if (I->is_dege())
          continue;

        if (dist != GDT_INVALID_HUGE && dist > farthest_dist) {
          farthest_dist = dist;
          farthest_I = I;
          farthest_tau = cur_edge->len();
          farthest_vertex = V;
          break;
        }
      }
    }//end for edge
  }

#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cerr, "\nThe farthest vertex %d on edge %d, tau: %lf, dist: %lf\n", 
                 farthest_vertex->id(), farthest_I->edge()->id(), farthest_tau, farthest_dist);
#endif

  return farthest_I;
}

//: Given the iso-contour of query_dist on cur_face,
//  find the closet immediate intersection on the other 2 edges of the cur_face.
//  in additional to the points, also return the edges the intersection is on.
void gdt_manager::get_iso_contour_s_e_gdt_points (
                            vgl_point_3d<double>* start_pt,
                            dbmsh3d_face* cur_face, double gdt_dist,
                            vcl_vector<vcl_pair<gdt_interval*, double> >* iso_contour_points,
                            dbmsh3d_gdt_edge** iso_contour_s_edge,
                            vcl_pair<gdt_interval*, double>& iso_contour_s_gdt_point, 
                            dbmsh3d_gdt_edge** iso_contour_e_edge,
                            vcl_pair<gdt_interval*, double>& iso_contour_e_gdt_point)
{
  iso_contour_s_gdt_point.first = NULL;
  iso_contour_e_gdt_point.first = NULL;

  //: loop through each edge of the cur_face
  dbmsh3d_halfedge* cur_he = cur_face->halfedge();
  do {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();

    //: There exists at almost two points on the cur_edge with the gdt_dist
    vcl_pair<gdt_interval*, double> point1, point2;
    cur_edge->get_gdt_points (gdt_dist, point1, point2);
    
    //: We want the closer point to the start_pt
    if (point1.first != NULL) { //1) if point1 is valid      
      if (iso_contour_s_gdt_point.first == NULL) {
        *iso_contour_s_edge = cur_edge;
        iso_contour_s_gdt_point = point1;
      }
      else {
        *iso_contour_e_edge = cur_edge;
        iso_contour_e_gdt_point = point1;
      }
    }
    else if (point2.first != NULL) { //2) if point2 is valid
      if (iso_contour_s_gdt_point.first == NULL) {
        *iso_contour_s_edge = cur_edge;
        iso_contour_s_gdt_point = point2;
      }
      else {
        *iso_contour_e_edge = cur_edge;
        iso_contour_e_gdt_point = point2;
      }
    }

    cur_he = cur_he->next();
  }
  while (cur_he != cur_face->halfedge());
}

//: For each I, there exists at almost two points with the given geodesic distance
//  But for each edge, there might be more than 2 iso-distance intersection points !
void gdt_manager::next_iso_contour_point (double gdt_dist,
                                          dbmsh3d_gdt_edge* input_edge, 
                                          vcl_pair<gdt_interval*, double>& input_point,
                                          dbmsh3d_face* nextF, 
                                          dbmsh3d_gdt_edge** nextE, 
                                          vcl_pair<gdt_interval*, double>& next_point)
{
  //: loop through each edge of the cur_face
  dbmsh3d_halfedge* cur_he = nextF->halfedge();
  do {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
    if (cur_edge != input_edge) {
      //: For each I, there exists at almost two points with the given geodesic distance
      vcl_pair<gdt_interval*, double> point1, point2;
      cur_edge->get_gdt_points (gdt_dist, point1, point2);
    
      //: !! for now, only consider one of them here
      if (point1.first != NULL) {
        *nextE = cur_edge;
        next_point = point1;
        return;
      }
      else if (point2.first != NULL) {
        *nextE = cur_edge;
        next_point = point2;
        return;
      }
    }
    cur_he = cur_he->next();
  }
  while (cur_he != nextF->halfedge());
}

//: Also do a check here that all intervals are mutually exclusive
//  and form a coverage on all edges.
void gdt_manager::print_results (int method, const char* prefix, int i_source, bool b_assert_gap)
{
  vul_printf (vcl_cerr, "===========================================\n");
  vul_printf (vcl_cerr, "Summary of GDT on [%s] with source %d via\n  ", prefix, i_source);

  if (method == GDT_METHOD_I)
    vul_printf (vcl_cerr, "Method 1 (Interval Based)");
  else if (method == GDT_METHOD_F)
    vul_printf (vcl_cerr, "Method 2 (Face Based)");
  else if (method == GDT_METHOD_FS)
    vul_printf (vcl_cerr, "Method 3 (Face Based + Shock)");
  else if (method == GDT_METHOD_WS)
    vul_printf (vcl_cerr, "Method 4 (Wavefront + Shock)");

  vul_printf (vcl_cerr, "\n");

  //: loop through all faces
  int n_prop_faces = 0;

  //: loop through each edges and print intervals
  int n_prop_edges = 0;
  int n_total_intervals = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = gdt_mesh_->edgemap().begin();
  for (; eit != gdt_mesh_->edgemap().end(); eit++) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) (*eit).second;
    if (cur_edge->interval_section()->size() != 0)
      n_prop_edges++;

    #if GDT_DEBUG_MSG
    if (n_verbose_==3)
      vul_printf (vcl_cerr, "edge %d (s%d-e%d) (len: %lf) has %d intervals:\n", 
                  cur_edge->id(), cur_edge->sV()->id(), cur_edge->eV()->id(), 
                  cur_edge->length(), cur_edge->interval_section()->size());
    #endif

    //: A) print each intervals
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
    for (int i=0; it != cur_edge->interval_section()->I_map()->end(); it++, i++) {
      gdt_interval* I = (gdt_interval*) (*it).second;
      n_total_intervals++;

      #if GDT_DEBUG_MSG
      if (n_verbose_==3) {
        switch (I->type()) {
        case ITYPE_PSRC:
          vul_printf (vcl_cerr, "    regular I %d: (%lf - %lf) vs: %d\n", 
                      i, I->stau(), I->etau(), I->psrc()->id());
        break;
        case ITYPE_DEGE:
          vul_printf (vcl_cerr, "    dege I %d: (%lf - %lf) vs: %d\n", 
                      i, I->stau(), I->etau(), I->psrc()->id());
        break;
        default:
          break;
        }
      }
      #endif
      
      #if GDT_ALGO_WS
      if (method == 4 && b_assert_gap) {
        gdt_welm* W = (gdt_welm*) I;
        assert (W->is_final());
      }
      #endif
    }

    //: B) check if the interval_section form a coverage of this cur_edge.
    //     check if no gap between each I
    #if GDT_DEBUG_MSG
    if (method != 1 && b_assert_gap) { //  && method != 4
      bool result = cur_edge->interval_section()->assert_coverage_no_gap_overlap ();
      assert (result);
    }
    #endif
  }

  vcl_map<int, dbmsh3d_face*>::iterator itf = gdt_mesh_->facemap().begin();
  for (; itf != gdt_mesh_->facemap().end(); itf++) {
    dbmsh3d_face* face = (*itf).second;
    if (face->b_visited())
      n_prop_faces++;
  }
  
  vul_printf (vcl_cerr, "mesh #v: %d, #f: %d, #e: %d\n",
               (int) gdt_mesh_->vertexmap().size(), 
               (int) gdt_mesh_->facemap().size(), 
               (int) gdt_mesh_->edgemap().size());

  vul_printf (vcl_cerr, "# total iterations: %d\n", (int) n_prop_iter_);
  vul_printf (vcl_cerr, "# propagated faces: %d\n", n_prop_faces);
  vul_printf (vcl_cerr, "# propagated intervals: %d\n", n_total_intervals);
  vul_printf (vcl_cerr, "# propagated edges: %d\n", n_prop_edges);
  double d_I_per_edge = (double) n_total_intervals / n_prop_edges;
  vul_printf (vcl_cerr, "# average interval per edge: %f\n", d_I_per_edge);
}

void gdt_ws_manager::print_statistics ()
{
#if GDT_DEBUG_MSG
  vul_printf (vcl_cerr, "===========================================\n");
  vul_printf (vcl_cerr, "# 2nd order sources: %d\n", n_W_W_2nd_source_);
  vul_printf (vcl_cerr, "# shock junctions: %d\n", n_S_junct_);
  vul_printf (vcl_cerr, "# shock sinks: %d\n", n_S_sink_);
  vul_printf (vcl_cerr, "# W-E strike at FPT: %d\n", n_WE_FPT_);
  vul_printf (vcl_cerr, "# W-E strike at SV: %d\n", n_WE_SV_);
  vul_printf (vcl_cerr, "# W-E strike at EV: %d\n", n_WE_EV_);
  vul_printf (vcl_cerr, "# S-E intersection: %d\n", n_S_E_);
  vul_printf (vcl_cerr, "# W-V strikes: %d\n", n_W_V_strike_);
  vul_printf (vcl_cerr, "# V launch shocks: %d\n", n_V_launch_S_);
  vul_printf (vcl_cerr, "# interior V launch rarefactions: %d\n", n_V_interior_RF_);
  vul_printf (vcl_cerr, "# bnd V launch rarefactions: %d\n", n_V_bnd_RF_);
  vul_printf (vcl_cerr, "# V launch degeI (instead of S): %d\n", n_V_launch_degeI_);
  vul_printf (vcl_cerr, "# S on E: %d\n", n_S_on_edge_);  
  vul_printf (vcl_cerr, "# S terminate at V: %d\n", n_S_term_at_V_);
  vul_printf (vcl_cerr, "# W terminate at V: %d\n", n_W_term_at_V_);
#endif
}








