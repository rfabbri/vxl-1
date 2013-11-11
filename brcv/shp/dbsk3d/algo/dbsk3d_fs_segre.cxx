//: dbsk3d_fs_segre.cxx
//: MingChing Chang
//  May 05, 2005

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <dbgl/dbgl_barycentric.h>

#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>
#include <dbsk3d/algo/dbsk3d_fs_segre.h>

//: Perform surface segregation.
//  Return number of surface interpolant created.
//
//  Greedy iteration of segregation on A13-shock-links.
//  The shock segregation is a greedy iterative process of 3D gap transforms.
//  At each step, the next A13-shock-elm is removed and
//  its corresponding surface polygon (triangle) is created to fill the gap.
//  The process is performed in a greedy (gradient descent) manner until
//  all relevant surface triangles are considered.
//
void dbsk3d_fs_segre::run_surface_segre (const float max_sample_ratio, const int topo_option,
                                         const float percentage, const bool reasgn_lost_genes)
{
  //0) Initialize shock queue.
  seg_init_shock_queue (max_sample_ratio);  
  bnd_mesh()->clear_faces(); //clear all existing mesh faces.
  bnd_mesh()->clear_edges(); //clear all existing mesh edges.

  topo_option_ = (SEG_TOPO_OPTION) topo_option;
  greedy_option_ = 1;
  vul_timer total_timer;

  //1) First greedy iteration of segregation on A13-2 shock links.  
  assert (bnd_mesh()->facemap().size() == 0);
  unsigned int n_tri_1st_iteration = first_greedy_iteration (percentage);

  if (percentage != 1.0f) {
    vul_printf (vcl_cout, "\t%u interpolants created in the first greedy iterative process.\n", n_tri_1st_iteration);
    vul_printf (vcl_cout, "\t%u total surface interpolants created (specified percentage %3.0f%%).\n", 
                 n_tri_1st_iteration, percentage*100);
    vul_printf (vcl_cout, "\t%u total surface interpolants in the facemap.\n", 
                 bnd_mesh()->facemap().size());
  }
  else {
    greedy_option_ = 2; //Now all operations using second shock queue.

    //2) First boundary mesh topology fixing.
    unsigned int n_tri_1st_fix = first_topo_fixing ();

    if (topo_option_ == STO_NON2MANIFOLD)
      add_bnd_links_to_queue ();

    //3) Second greedy iteration of non-A13-2 shock-links to fill the holes.
    unsigned int n_tri_2nd_iteration = second_greedy_iteration ();

    //4) Second boundary mesh topology fixing.
    unsigned int n_tri_2nd_fix = 0;
    if (topo_option_ != STO_NON2MANIFOLD)
      n_tri_2nd_fix = second_topo_fixing ();

    //5) Final oversize triangle filling.
    unsigned int n_oversize_fill = final_oversize_fill ();

    F_L_vector_.clear ();

    vcl_cerr << "\nTotal surface reconstruction time: " << total_timer.real() << " milliseconds.\n";

    vul_printf (vcl_cout, "\nResult of surface segregation:\n");
    vul_printf (vcl_cout, "\t%u triangles created in the 1st greedy iteration.\n", n_tri_1st_iteration);
    vul_printf (vcl_cout, "\t%u triangles fixed in the 1st topology fixing.\n", n_tri_1st_fix);
    vul_printf (vcl_cout, "\t%u triangles created in the 2nd greedy iteration.\n", n_tri_2nd_iteration);
    vul_printf (vcl_cout, "\t%u triangles fixed in the 2nd topology fixing.\n", n_tri_2nd_fix);
    vul_printf (vcl_cout, "\t%u oversize triangles filled in holes.\n", n_oversize_fill);

    unsigned int n_total_tri = n_tri_1st_iteration + n_tri_1st_fix + n_tri_2nd_iteration + n_tri_2nd_fix + n_oversize_fill;
    vul_printf (vcl_cout, "\t%u total surface triangles processed (specified percentage %3.0f%%).\n", 
                 n_total_tri, percentage*100);
    vul_printf (vcl_cout, "\t%u total surface triangles in the facemap.\n", 
                 bnd_mesh()->facemap().size());

    //6) Fix surface interpolant orientations.
    dbmsh3d_face* seedF = bnd_mesh()->facemap(0);
    unify_mesh_orientation (bnd_mesh(), seedF);
  }

  //Release mem.
  reset_mesh_bnd_chains ();
  F_L_vector_.clear ();
  L_1st_queue_.clear();
  L_2nd_queue_.clear();

  bnd_mesh()->print_topo_summary ();

  if (reasgn_lost_genes) {
    //Check the association of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    bool result = fs_mesh_->check_all_G_asgn (unasgn_genes);
  }
}

void dbsk3d_fs_segre::seg_init_shock_queue (const float max_sample_ratio)
{
  vul_printf (vcl_cout, "\nseg_init_shock_queue(): totally %u fs_edges.\n",
              fs_mesh_->edgemap().size());
  L_1st_queue_.clear();
  L_2nd_queue_.clear();

  unsigned int n_meshed_links = 0;
  unsigned int n_inf_links = 0;
  unsigned int n_acute_links = 0;
  unsigned int n_obtuse_links = 0;
  unsigned int n_nonA132_links = 0;
  unsigned int n_oversize_links = 0;

  fs_mesh_->compute_median_A122_dist ();
  max_sample_dist_ = fs_mesh_->median_A122_dist() * max_sample_ratio;  
  vul_printf (vcl_cout, "\td_median %f msr = %.2f * estimated max_sample_dist %f\n", 
              fs_mesh_->median_A122_dist(), max_sample_ratio, max_sample_dist_);

  const bool has_already_meshed_F = bnd_mesh()->facemap().size() != 0;

  //Go through all shock link elements and insert relevant ones to queue.
  vul_printf (vcl_cout, "  Inserting shock links to Q: ");
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh_->edgemap().begin();
  int prev_per = 0;
  for (int count=0; lit != fs_mesh_->edgemap().end(); lit++, count++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;

    float percentage = (float) (count * 100.0 / fs_mesh_->edgemap().size());
    int per = static_cast<int>(vcl_ceil (percentage));
    if (per % 10 ==0 && per != prev_per)
      vul_printf (vcl_cout, "%2d%% ", per);
    prev_per = per;

    FE->set_counter (0); //reset FE's counter
    if (FE->b_inf()) {
      n_inf_links++;
      continue;
    }

    if (has_already_meshed_F && L_find_already_meshed_dual_F (FE) != NULL) {
      //If FE's tri is already meshed in the input, set its cost to 0.
      n_meshed_links++;
      FE->set_cost (0);
      FE->set_seg_type (FE_SEG_MESHED);
      _add_to_L_1st_queue (FE);
    }
    else {
      L_compute_gap_cost (FE, 1);    

      switch (FE->seg_type()) {
      case FE_SEG_ACUTE:        
        _add_to_L_1st_queue (FE);
        n_acute_links++;
      break;
      case FE_SEG_OBTUSE: 
        _add_to_L_1st_queue (FE);
        n_obtuse_links++;
      break;
      case FE_SEG_NO_A132:
        _add_to_L_2nd_queue (FE);
        n_nonA132_links++;
      break;
      case FE_SEG_OVERSIZED:
        n_oversize_links++;
      break;
      }        
    }
  }

  vul_printf (vcl_cout, "\t%u already meshed shock links.\n", n_meshed_links);
  vul_printf (vcl_cout, "\t%u fs_edges at infinity.\n", n_inf_links);
  vul_printf (vcl_cout, "\t%u non-A13-2 fs_edges.\n", n_nonA132_links);
  vul_printf (vcl_cout, "\t%u fs_edges of oversize triangles.\n", n_oversize_links);
  vul_printf (vcl_cout, "\t%u fs_edges of acute triangles.\n", n_acute_links);
  vul_printf (vcl_cout, "\t%u fs_edges of obtuse triangles.\n", n_obtuse_links);
  vul_printf (vcl_cout, "\tFirst fs_edge queue size: %u.\n", L_1st_queue_.size());

  //Print initiail A13-2 link cost summary.
  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_1st_queue_.begin();
  dbsk3d_fs_edge* minL = (*it).second;
  it = L_1st_queue_.end();
  it--;
  dbsk3d_fs_edge* maxL = (*it).second;
  vul_printf (vcl_cout, "\t  cost min = %f, max= %f.\n", minL->cost(), maxL->cost());  
  vul_printf (vcl_cout, "\t  cost median = %f, 10%% = %f, 90%% = %f\n", 
               get_Q1_median_cost(), get_Q1_percent_cost (0.1f), get_Q1_percent_cost (0.9f));
}

#define V_ONLY_INCIDENCE_MAX_DELAY      5     //0: disable such delay.
#define V_ONLY_INCIDENCE_COST_INCREASE  1.2f  //1.2: increase cost by 20%
#define V_ONLY_INCIDENCE_COST_I0        1.0f

unsigned int dbsk3d_fs_segre::first_greedy_iteration (const float percentage)
{
  vul_printf (vcl_cout, "\nfirst_greedy_iteration (%2d%%)\n", (int) (percentage*100));
  vul_printf (vcl_cout, "\tFirst fs_edge queue size: %d\n", L_1st_queue_.size());
  vul_printf (vcl_cout, "\tsurface interpolants (triangles) created: ");

  unsigned int n_tri_created = 0;  
  unsigned int n_fix_topo = 0;
  stop_rth_ = (float) get_Q1_percent_cost (percentage); //Determine the stopping threshold

  //Loop through shock links in the L_1st_queue_ until finish.
  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_1st_queue_.begin();
  while (it != L_1st_queue_.end()) {
    dbsk3d_fs_edge* FE = (*it).second;
    L_1st_queue_.erase (it); //remove FE from queue.
    assert (FE->seg_type() == FE_SEG_MESHED || 
            FE->seg_type() == FE_SEG_ACUTE || 
            FE->seg_type() == FE_SEG_OBTUSE);

    #if DBMSH3D_DEBUG>5
    print_Q1_file ();
    #endif
  
    //Thresholding: stop the iteration if current cost greater than th.
    if (FE->cost() > stop_rth_ && percentage != 1.0f)
      break;

    //Perform a greedy step.
    if (first_greedy_step (FE) == true) { 
      //Has done a successful 3D gap-transform.
      n_tri_created++;

      //Fixing the one-ring-nbr topology for each vertex.
      if (first_greedy_fix_vtopo (FE) == true)
        n_fix_topo++;      

      if (n_tri_created % 1000 == 0) //Show processing progress.
        vul_printf (vcl_cout, "%uk ", n_tri_created/1000);
    }

    it = L_1st_queue_.begin(); //the next iteration.
  }

  vul_printf (vcl_cout, "\n\t%u surface triangles created.\n", n_tri_created);
  vul_printf (vcl_cout, "\t%u vertex 1-ring-nbrhood topology fixed.\n", n_fix_topo);  
  vul_printf (vcl_cout, "\t%u remaining in the first shock link queue.\n", L_1st_queue_.size());
  assert (L_1st_queue_.size() == 0);
  return n_tri_created;
}

bool dbsk3d_fs_segre::first_greedy_step (dbsk3d_fs_edge* FE)
{
  //Check if insertion of the surface interpolant of the current link 
  //violates the 2-manifold topological assumption.
  vcl_set<dbsk3d_fs_edge*> undo_L_set;
  SURF_TOPO_TYPE topo_type = check_L_topology_1 (FE, undo_L_set);

  switch (topo_type) {
  case SURF_TOPO_OK:
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\tgap xform on link %d with cost %f.\n", FE->id(), FE->cost());
    #endif
    L_3d_gap_transform (FE);
    L_modify_nbrs_cost (FE);
    return true;
  break;
  case SURF_TOPO_V_ONLY_INCIDENCE: 
    //Delay such surface triangles: double its cost and re-insert to the queue.
    //Only delay for 5 times, after it just perform the gap transform.
    //Use the link's s_value_ for the counter.
    if (FE->counter() >= V_ONLY_INCIDENCE_MAX_DELAY) {
      #if DBMSH3D_DEBUG>2
      vul_printf (vcl_cout, "\tafter delaying, gap xform on FE %d with cost %f.\n", FE->id(), FE->cost());
      #endif
      L_3d_gap_transform (FE);
      L_modify_nbrs_cost (FE);
      return true;
    }
    else {
      #if DBMSH3D_DEBUG>2
      vul_printf (vcl_cout, "\tdelay link %d for vertex-only-incidence, cost=%f.\n", 
                   FE->id(), FE->cost());
      #endif
      //For cost 0, should perform it without delaying.
      if (FE->cost() == 0)
        FE->set_cost (V_ONLY_INCIDENCE_COST_I0);
      else
        FE->set_cost ((float) (FE->cost() * V_ONLY_INCIDENCE_COST_INCREASE));
      FE->increase_counter ();
      _add_to_L_1st_queue (FE);
    }
  break;
  case SURF_TOPO_NON_2_MANIFOLD:
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\tskip gap xform on link %d for 2-manifold.\n", FE->id());
    #endif
    //for STO_NON2MANIFOLD, move to 2nd queue
    L_compute_gap_cost (FE, 2);
    _add_to_L_2nd_queue (FE);
  break;
  case SURF_TOPO_V_1_RING_NBR:
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\tskip gap xform on link %d for v-incidence at 1-ring-nbrhood.\n", FE->id());
    #endif
  break;
  default:
    assert (0);
  break;
  }

  
  if (undo_L_set.size() != 0) {
    //If unconfident shock list is non-empty, add FE to 2nd queue.
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\n  Unconfident shock link %d detected and moved to 2nd queue.\n", FE->id());
    #endif

    L_compute_gap_cost (FE, 2);
    _add_to_L_2nd_queue (FE);
  }

  //Undo the unconfident shock transforms and put them to the 2nd queue.
  vcl_set<dbsk3d_fs_edge*>::iterator it = undo_L_set.begin();
  for (; it != undo_L_set.end(); it++) {
    dbsk3d_fs_edge* Lo = (*it);

    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "  Undo gap xform of link %d and move to 2nd queue.\n", Lo->id());
    #endif

    //Perform an inverse 3D gap transform on Lo
    undo_L_3d_gap_transform (Lo);
    undo_L_modify_nbrs_cost (Lo);

    L_compute_gap_cost (Lo, 2);
    _add_to_L_2nd_queue (Lo);
  }  

  return false;
}

//: Return the topology change of introducing the input surface interpolant:
//  Also detect unconfident shock links.
//    - SURF_TOPO_OK: do not violate any topological assumption.
//    - SURF_TOPO_V_ONLY_INCIDENCE: produce the vertex-only incidence.
//    - SURF_TOPO_NON_2_MANIFOLD: violates the 2-manifold assumption, a triple junction.
//    - SURF_TOPO_V_1_RING_NBR: violates the vertex-on-one-ring-neighborhood.
//
SURF_TOPO_TYPE dbsk3d_fs_segre::check_L_topology_1 (const dbsk3d_fs_edge* FE,
                                                    vcl_set<dbsk3d_fs_edge*>& undo_L_set)
{
  // Check the mesh edge topology
  //   Loop through each incident patch (bnd_edge) of this link,
  //   check if any bnd_edge already has two incident bnd_faces.
  //   return false if any triple junction found.
  dbmsh3d_halfedge* HE = FE->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;

  do {
    //Check the mesh edge topology: about non-2-manifold.
    if (FF->both_sides_meshed()) {
      //For non-2-manifold, compare the costs of candidates and existing shock link cost.

      //Find the other existing shock link Lo
      vcl_set<dbsk3d_fs_edge*> FE_with_bnd_F_set;
      FF->get_FEs_with_bnd_F (FE_with_bnd_F_set);

      //Loop through all Lo's and compare the cost to FE.
      vcl_set<dbsk3d_fs_edge*>::iterator it = FE_with_bnd_F_set.begin();
      assert (FE_with_bnd_F_set.size() < 3);
      for (; it != FE_with_bnd_F_set.end(); it++) {
        dbsk3d_fs_edge* Lo = (*it);
        assert (Lo != FE);

        if (FE->cost() < Lo->cost()) { 
          //Add Lo to the undo-gap-xform list.
          undo_L_set.insert (Lo);

          #if DBMSH3D_DEBUG>3
          vul_printf (vcl_cout, "FE %d cost %f < Lo %d cost %f\n",
                       FE->id(), FE->cost(), Lo->id(), Lo->cost());
          #endif
        }
      }

      return SURF_TOPO_NON_2_MANIFOLD;
    }

    //Check the mesh vertex topology: one-ring-neighborhood.
    VTOPO_TYPE type = G->detect_vtopo_type();

    assert (type != VTOPO_EDGE_ONLY);
    assert (type != VTOPO_EDGE_JUNCTION);
    assert (type != VTOPO_NON_MANIFOLD_1RING);
    if (type == VTOPO_2_MANIFOLD_1RING)
      return SURF_TOPO_V_1_RING_NBR;

    //the next FF that connected to G
    G = FF->other_G (G);
    FF = FE->other_FF_of_G (FF, G);
  }
  while (G != startG && FF != NULL);

  //Loop through all generators to check the vertex-only-incidence
  G = startG;
  FF = (dbsk3d_fs_face*) HE->face();
  do {
    //the next FF that connected to G
    dbmsh3d_vertex* nextG = FF->other_G (G);
    dbsk3d_fs_face* nextP = FE->other_FF_of_G (FF, nextG);

    if (is_F_V_incidence (nextG, G, nextP->other_G (nextG)))
      return SURF_TOPO_V_ONLY_INCIDENCE;

    G = nextG;
    FF = nextP;
  }
  while (G != startG && FF != NULL);

  return SURF_TOPO_OK;
}

//: Topology fixing step: removing vertex-only incidence around 
//  the one-ring-neighborhood which violates the 2-manifold topology.
//  Return true if such situation is detected and fixed.
bool dbsk3d_fs_segre::first_greedy_fix_vtopo (dbsk3d_fs_edge* FE)
{
  vcl_vector<dbmsh3d_vertex*> genes;
  bool result = FE->get_ordered_Gs_via_FF (genes);
  assert (result);

  for (unsigned int i=0; i<genes.size(); i++) {
    dbmsh3d_vertex* G = genes[i];

    //For each G, check the vertex-topology.
    VTOPO_TYPE type = G->detect_vtopo_type ();

    if (type == VTOPO_NON_MANIFOLD_1RING) {
      //Need a while loop to make the vertex topology one-ring.
      do {
        //Locate the extra face.
        dbmsh3d_face* Fe = get_non_manifold_1ring_extra_Fs (G);
        assert (Fe);
        
        //Find the corresponding shock link for this extra face.
        dbsk3d_fs_edge* Le = find_L_of_F (Fe);
        assert (Le);

        //undo the 3D gap transform of Fe and Le.
        undo_L_3d_gap_transform (Le);
        undo_L_modify_nbrs_cost (Le);    
        
        #if DBMSH3D_DEBUG>2
        vul_printf (vcl_cout, "  Undo gap_xform of shock_link %d bnd_face %d gene %d.\n", 
                     FE->id(), FE->bnd_face()->id(), G->id());
        #endif

        type = G->detect_vtopo_type ();
      }
      while (type != VTOPO_2_MANIFOLD_1RING);
    }
  }

  return false;
}
unsigned int dbsk3d_fs_segre::first_topo_fixing ()
{
  vul_printf (vcl_cout, "\nfirst_topo_fixing()\n");
  vcl_vector<dbsk3d_fs_edge*> undo_Ls;

  //Go through all shock links with bnd_face on mesh boundary.  
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh_->edgemap().begin();
  for (; lit != fs_mesh_->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    if (FE->b_inf())
      continue;

    //Add FE to undo_list if its not both_sides_meshed.
    if (FE->bnd_face())
      if (FE->bnd_face()->tri_get_topo_type() == TRIFACE_111 ||
          FE->bnd_face()->tri_get_topo_type() == TRIFACE_112)
        undo_Ls.push_back (FE);
  }

  vul_printf (vcl_cout, "\tUndo %u gap xform, move to 2nd queue of shock links on mesh boundary.\n", undo_Ls.size());
  vcl_vector<dbsk3d_fs_edge*>::iterator it = undo_Ls.begin();
  for (; it != undo_Ls.end(); it++) {
    dbsk3d_fs_edge* FE = (*it);

    undo_L_3d_gap_transform (FE);
    undo_L_modify_nbrs_cost (FE);

    L_compute_gap_cost (FE, 2);
    _add_to_L_2nd_queue (FE);
  }

  unsigned int sz = undo_Ls.size();
  undo_Ls.clear();
  return sz;
}
void dbsk3d_fs_segre::add_bnd_links_to_queue ()
{
  //: loop through all shock sheets FF with one side meshed.
  //  add all other unmeshed FE into 2nd_queue.
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->one_side_meshed()) {
      //Loop through all FE's of FF and add all non-meshed and non-infinity FE into queue2.
      dbmsh3d_halfedge* HE = FF->halfedge();
        do {
          dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();  
          if (FE->b_inf() == false && FE->bnd_face() == false) {
            L_compute_gap_cost (FE, 2);
            if (FE->cost() != SEG_OVERSIZE_COST)
              _add_to_L_2nd_queue (FE);
          }
          HE = HE->next();
        }
        while (HE != FF->halfedge());
    }
  }
}

unsigned int dbsk3d_fs_segre::second_greedy_iteration ()
{
  vul_printf (vcl_cout, "\nsecond_greedy_iteration()\n");
  vul_printf (vcl_cout, "\tSecond shock link queue size: %d\n", L_2nd_queue_.size());
  vul_printf (vcl_cout, "\tsurface interpolants created: ");

  unsigned int n_tri_created = 0;  
  unsigned int n_fix_topo = 0;

  //Loop through shock links in the L_2nd_queue_ until finish.
  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_2nd_queue_.begin();
  while (it != L_2nd_queue_.end()) {
    dbsk3d_fs_edge* FE = (*it).second;
    L_2nd_queue_.erase (it); //remove FE from queue.

    #if DBMSH3D_DEBUG>4
    print_Q1_file ();
    #endif

    //Perform a greedy step
    if (second_greedy_step (FE) == true) { 
      //Has done a successful 3D gap-transform.
      n_tri_created++;

      //Fixing the one-ring-nbr topology for each vertex.
      if (second_greedy_fix_vtopo (FE) == true)
        n_fix_topo++;      

      if (n_tri_created % 1000 == 0) //Show processing progress.
        vul_printf (vcl_cout, "%uk ", n_tri_created/1000);
    }

    it = L_2nd_queue_.begin(); //the next iteration.
  }

  vul_printf (vcl_cout, "\n\t%u surface triangles created.\n", n_tri_created);
  vul_printf (vcl_cout, "\t%u vertex 1-ring-nbrhood topology fixed.\n", n_fix_topo);  

  L_2nd_queue_.clear();
  return n_tri_created;
}

//: Perform the 2nd part of segregation according to the topo_option.
bool dbsk3d_fs_segre::second_greedy_step (dbsk3d_fs_edge* FE)
{
  //Check if insertion of the surface interpolant of the current link 
  //violates the 2-manifold topological assumption.
  dbsk3d_fs_edge* L_ungap = NULL;
  SURF_TOPO_TYPE topo_type = check_L_topology_2 (FE, &L_ungap);

  //If FE is better than L_ungap, redo gap transform on L_ungap and 
  //check again until no change needs to be made.
  while (L_ungap &&
         (topo_type == SURF_TOPO_OK || topo_type == SURF_TOPO_V_ONLY_INCIDENCE)) {
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "  Undo gap xform of link %d.\n", L_ungap->id());
    #endif

    //Perform an inverse 3D gap transform on L_ungap
    undo_L_3d_gap_transform (L_ungap);
    undo_L_modify_nbrs_cost (L_ungap);    

    L_ungap = NULL;
    topo_type = check_L_topology_2 (FE, &L_ungap);
  }
  
  switch (topo_type) {
  case SURF_TOPO_OK:
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\tgap xform on link %d with cost %f.\n", FE->id(), FE->cost());
    #endif
    L_3d_gap_transform (FE);
    L_modify_nbrs_cost (FE);
    return true;
  break;
  case SURF_TOPO_V_ONLY_INCIDENCE: 
    //Delay such surface triangles: double its cost and re-insert to the queue.
    //Only delay for 5 times, after it just perform the gap transform.
    //Use the link's s_value_ for the counter.
    if (FE->counter() >= V_ONLY_INCIDENCE_MAX_DELAY) {
      #if DBMSH3D_DEBUG>2
      vul_printf (vcl_cout, "\tafter delaying, gap xform on FE %d with cost %f.\n", FE->id(), FE->cost());
      #endif
      L_3d_gap_transform (FE);
      L_modify_nbrs_cost (FE);
      return true;
    }
    else {
      #if DBMSH3D_DEBUG>2
      vul_printf (vcl_cout, "\tdelay link %d for vertex-only-incidence, cost=%f.\n", 
                   FE->id(), FE->cost());
      #endif
      //For cost 0, should perform it without delaying.
      if (FE->cost() == 0)
        FE->set_cost (V_ONLY_INCIDENCE_COST_I0);
      else
        FE->set_cost ((float) (FE->cost() * V_ONLY_INCIDENCE_COST_INCREASE));
      FE->increase_counter ();
      _add_to_L_1st_queue (FE);
    }
  break;
  case SURF_TOPO_NON_2_MANIFOLD:
    if (topo_option_ == STO_2MANIFOLD) {
      #if DBMSH3D_DEBUG>2
      vul_printf (vcl_cout, "\tskip gap xform on link %d for 2-manifold.\n", FE->id());
      #endif
    }
    else if (topo_option_ == STO_NON2MANIFOLD) {
      //if the angle check of triple junction is passed, perform gap-xform.
      if (check_multi_jun_angles (FE)) {
        L_3d_gap_transform (FE);
        L_modify_nbrs_cost (FE);
      }
      return true;
    }
  break;
  case SURF_TOPO_V_1_RING_NBR:
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "\tskip gap xform on link %d for v-incidence at 1-ring-nbrhood.\n", FE->id());
    #endif
  break;
  default:
    assert (0);
  break;
  }
  return false;
}

//: Return the topology change of introducing the input surface interpolant:
//  Return the most unwanted existing shock link L_ungap (with highest cost) to undo gap xform.
//
//    - SURF_TOPO_OK: do not violate any topological assumption.
//    - SURF_TOPO_V_ONLY_INCIDENCE: produce the vertex-only incidence.
//    - SURF_TOPO_NON_2_MANIFOLD: violates the 2-manifold assumption, a triple junction.
//    - SURF_TOPO_V_1_RING_NBR: violates the vertex-on-one-ring-neighborhood.
//
SURF_TOPO_TYPE dbsk3d_fs_segre::check_L_topology_2 (const dbsk3d_fs_edge* FE,
                                                    dbsk3d_fs_edge** L_ungap)
{
  // Check the mesh edge topology
  //   Loop through each incident patch (bnd_edge) of this link,
  //   check if any bnd_edge already has two incident bnd_faces.
  //   return false if any triple junction found.
  dbmsh3d_halfedge* HE = FE->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;

  do {
    //Check the mesh edge topology: about non-2-manifold.
    if (FF->both_sides_meshed()) {
      //For non-2-manifold, compare the costs of candidates and existing shock link cost.

      //Find the other existing shock link Lo
      vcl_set<dbsk3d_fs_edge*> FE_with_bnd_F_set;
      FF->get_FEs_with_bnd_F (FE_with_bnd_F_set);
      assert (topo_option_==STO_NON2MANIFOLD || FE_with_bnd_F_set.size() < 3);

      //Loop through all Lo's and compare the cost to FE.
      vcl_set<dbsk3d_fs_edge*>::iterator it = FE_with_bnd_F_set.begin();
      for (; it != FE_with_bnd_F_set.end(); it++) {
        dbsk3d_fs_edge* Lo = (*it);
        ///assert (Lo != FE);
        if (Lo == FE)
          continue; ////!!

        if (FE->cost() < Lo->cost()) { 
          //Replace the L_ungap
          if (*L_ungap == NULL || (*L_ungap)->cost() < Lo->cost())
            *L_ungap = Lo;

          #if DBMSH3D_DEBUG>2
          vul_printf (vcl_cout, "FE %d cost %f < Lo %d cost %f\n",
                       FE->id(), FE->cost(), Lo->id(), Lo->cost());
          #endif
        }
      }

      return SURF_TOPO_NON_2_MANIFOLD;
    }

    //Check the mesh vertex topology: one-ring-neighborhood.
    VTOPO_TYPE type = G->detect_vtopo_type();

    assert (type != VTOPO_EDGE_ONLY);
    assert (topo_option_==STO_NON2MANIFOLD || type != VTOPO_EDGE_JUNCTION);
    ///assert (type != VTOPO_NON_MANIFOLD_1RING); !!
    if (type == VTOPO_2_MANIFOLD_1RING)
      return SURF_TOPO_V_1_RING_NBR;

    //the next FF that connected to G
    G = FF->other_G (G);
    FF = FE->other_FF_of_G (FF, G);
  }
  while (G != startG && FF != NULL);

  //Loop through all generators to check the vertex-only-incidence
  G = startG;
  FF = (dbsk3d_fs_face*) HE->face();
  do {
    //the next FF that connected to G
    dbmsh3d_vertex* nextG = FF->other_G (G);
    dbsk3d_fs_face* nextP = FE->other_FF_of_G (FF, nextG);

    if (is_F_V_incidence (nextG, G, nextP->other_G (nextG)))
      return SURF_TOPO_V_ONLY_INCIDENCE;

    G = nextG;
    FF = nextP;
  }
  while (G != startG && FF != NULL);

  return SURF_TOPO_OK;
}

//: return true if the angle checking is passed.
bool dbsk3d_fs_segre::check_multi_jun_angles (const dbsk3d_fs_edge* inputL)
{
  //Loop through each shock sheet FF and find the multi-junction.  
  dbmsh3d_halfedge* HE = inputL->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;

  do {
    dbsk3d_fs_edge *L1, *L2;
    if (FF->both_sides_meshed(&L1, &L2)) { //Find the non-2-manifold.
      //Check the angles of between two triangles of inputL-L1 and inputL-L2.
      double dihedral1 = compute_bending_angle (inputL, FF, L1);
      double dihedral2 = compute_bending_angle (inputL, FF, L2);

      if (dihedral1 > 0.2 && dihedral2 > 0.2 &&
          dihedral1 < 2.9 && dihedral2 < 2.9)
        return true;
      else //fail on angle checking.
        return false;
    }

    //the next FF that connected to G
    G = FF->other_G (G);
    FF = inputL->other_FF_of_G (FF, G);
  }
  while (G != startG && FF != NULL);

  return true;
}

//: Topology fixing step: removing vertex-only incidence around 
//  the one-ring-neighborhood which violates the 2-manifold topology.
//  Return true if such situation is detected and fixed.
bool dbsk3d_fs_segre::second_greedy_fix_vtopo (dbsk3d_fs_edge* FE)
{
  vcl_vector<dbmsh3d_vertex*> genes;
  bool result = FE->get_ordered_Gs_via_FF (genes);
  assert (result);

  for (unsigned int i=0; i<genes.size(); i++) {
    dbmsh3d_vertex* G = genes[i];

    //For each G, check the vertex-topology.
    VTOPO_TYPE type = G->detect_vtopo_type ();

    if (type == VTOPO_NON_MANIFOLD_1RING) {
      //Need a while loop to make the vertex topology one-ring.
      do {
        //Locate the extra face.
        dbmsh3d_face* Fe = get_non_manifold_1ring_extra_Fs (G);
        assert (Fe);
        
        //Find the corresponding shock link for this extra face.
        dbsk3d_fs_edge* Le = find_L_of_F (Fe);
        assert (Le);

        //undo the 3D gap transform of Fe and Le.
        undo_L_3d_gap_transform (Le);
        undo_L_modify_nbrs_cost (Le);    
        
        #if DBMSH3D_DEBUG>2
        vul_printf (vcl_cout, "  Undo gap_xform of shock_link %d bnd_face %d gene %d.\n", 
                     FE->id(), FE->bnd_face()->id(), G->id());
        #endif

        type = G->detect_vtopo_type ();
      }
      while (type != VTOPO_2_MANIFOLD_1RING);
    }
  }

  return false;
}

unsigned int dbsk3d_fs_segre::second_topo_fixing ()
{
  vul_printf (vcl_cout, "\nsecond_topo_fixing()\n");
  unsigned int n_tri_fixed = 0;

  //Initialize the boundary chain list.
  mesh_bnd_chains_->set_fs_mesh (fs_mesh_);

  //Skip if there are too many boundary chains.
  int skip_bnd_chain_th = 50;
  if (mesh_bnd_chains_->detect_bnd_chains_th (TOPO_CHAIN_MAX_EDGES, skip_bnd_chain_th) == false)
    return 0;

  //The first order fixing of boundary hole with a few edges.
  vcl_vector<dbmsh3d_bnd_chain*>::iterator it = mesh_bnd_chains_->chainset().begin();
  for (; it != mesh_bnd_chains_->chainset().end(); it++) {
    dbsk3d_bnd_chain* BCs = (dbsk3d_bnd_chain*) (*it);
    if (BCs->HE_list().size() <= TOPO_FIX_HOLE_MAX_EDGES) {
      fix_hole_1st_order (BCs);
    }
  }

  vul_printf (vcl_cout, "\t%u surface interpolant fixed.\n", n_tri_fixed);
  return n_tri_fixed;
}

bool dbsk3d_fs_segre::fix_hole_1st_order (dbsk3d_bnd_chain* BCs)
{
  //Loop through each boundary face F, find Vo, try to fill hole.
  unsigned sz = BCs->HE_list().size();
  for (unsigned int i=0; i<sz; i++) {
    dbmsh3d_halfedge* HE = BCs->HE_list (i);
    dbmsh3d_edge* E = HE->edge();
    dbmsh3d_face* F = HE->face();
    dbmsh3d_vertex* Vo = F->t_vertex_against_edge (E);

    //Go through the set of sz-1 opposite edges on the hole,
    //and try to fill the it.
    bool hole_fill_success = true;
    vcl_vector <dbsk3d_fs_edge*> hole_L_set;
    hole_L_set.clear();

    for (unsigned int j=1; j<sz; j++) {
      dbmsh3d_edge* Ej = BCs->HE_list ((i+j) % sz)->edge();

      //Search in the boundary chain's shock sheet set for 
      //the shock links corresponding to triangles (Vo-Ej).
      dbsk3d_fs_edge* Lj = BCs->find_L_for_hole (Vo, Ej);
      if (Lj)
        hole_L_set.push_back (Lj);
      else {
        hole_fill_success = false;
        break;
      }
    }

    if (hole_fill_success) {
      //Undo gap-xform of shock link Lo corresponding to face F.
      dbsk3d_fs_edge* Lo = find_L_of_F (F);
      undo_L_3d_gap_transform (Lo);
      undo_L_modify_nbrs_cost (Lo);

      //Perform gap-xform on the set of hole-filling shock links.
      for (unsigned int j=0; j<hole_L_set.size(); j++) {
        dbsk3d_fs_edge* Lj = hole_L_set[j];
        L_3d_gap_transform (Lj);
        L_modify_nbrs_cost (Lj);
      }
      return true;
    }
  }

  return false;
}

// ########################################################################

unsigned int dbsk3d_fs_segre::final_oversize_fill ()
{
  vul_printf (vcl_cout, "\tfinal_oversize_fill()\n");
  unsigned int n_oversize_links = 0;
  unsigned int n_tri_filled = 0;

  //Go through all remaining valid shock link elements and check 
  //if any of them fits a hole ignoring their size.
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh_->edgemap().begin();
  for (; lit != fs_mesh_->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;

    if (FE->seg_type() == FE_SEG_OVERSIZED) {
      n_oversize_links++; 
      if (L_check_fill_hole(FE)) { //mesh it if it just fills a hole.
        //Perform a greedy step.
        second_greedy_step (FE);
        n_tri_filled++;
      }
    }
  }

  vul_printf (vcl_cout, "\t%u holes filled from %u oversize surface interpolant.\n", 
               n_tri_filled, n_oversize_links);
  return n_tri_filled;
}

// ###########################################################################

double dbsk3d_fs_segre::get_Q1_percent_cost (const float percent)
{
  unsigned int idx = (unsigned int) (L_1st_queue_.size() * percent);
  idx = vcl_min (idx, static_cast<unsigned int>(L_1st_queue_.size()-1));

  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_1st_queue_.begin();
  for (unsigned int i=0; i<idx; i++)
    it++;

  dbsk3d_fs_edge* FE = (*it).second;
  assert (FE->cost() == (*it).first);

  return (*it).first;
}

double dbsk3d_fs_segre::get_Q1_median_cost ()
{
  return get_Q1_percent_cost (0.5f);
}

void dbsk3d_fs_segre::output_Q1_file ()
{
  const char* filename = "a13cost.txt";
  FILE  *fp;
  if ((fp = fopen(filename, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file %s.\n", filename);
    return; 
  }

  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_1st_queue_.begin();
  for (; it != L_1st_queue_.end(); it++) {
    dbsk3d_fs_edge* FE = (*it).second;

    double max_side, perimeter;
    unsigned int nG;
    double Side[3];
    dbmsh3d_vertex* Gene[3];
    bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
    assert (result);
    vgl_point_3d<double> C = circum_center_3pts (Gene[0]->pt(), Gene[1]->pt(), Gene[2]->pt());

    ///assert ((*it).first == FE->cost());
    vcl_fprintf (fp, "%.16f\n", vgl_distance (C, Gene[0]->pt()));
  }
  fclose (fp);  
}

//: Print the shock link queue for debugging.
void dbsk3d_fs_segre::print_Q1_file ()
{
  vul_printf (vcl_cout, "\n  print_Q1_file(): %u shock links in queue.\n", L_1st_queue_.size());
  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_1st_queue_.begin();
  for (unsigned int i=0; it != L_1st_queue_.end(); it++, i++) {    
    dbsk3d_fs_edge* FE = (*it).second;
    vul_printf (vcl_cout, "FE %d (%f)  ", FE->id(), FE->cost());
    if (i % 3 == 2)
      vul_printf (vcl_cout, "\n");

  }
}

double dbsk3d_fs_segre::get_Q2_percent_cost (const float percent)
{
  unsigned int idx = (unsigned int) (L_2nd_queue_.size() * percent);
  idx = vcl_min (idx, static_cast<unsigned int>(L_2nd_queue_.size()-1));

  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = L_2nd_queue_.begin();
  for (unsigned int i=0; i<idx; i++)
    it++;

  dbsk3d_fs_edge* FE = (*it).second;
  assert (FE->cost() == (*it).first);

  return (*it).first;
}

// ###########################################################################

//: Return already meshed face dual to the input shock link FE.
//  If no such face found, return NULL.
dbmsh3d_face* dbsk3d_fs_segre::L_find_already_meshed_dual_F (dbsk3d_fs_edge* FE)
{
  //Go through each G of FE, and check all incident faces of G.
  //If any such face F happens to be the dual of FE, return true.
  vcl_vector<dbmsh3d_vertex*> genes;
  bool result = FE->get_ordered_Gs_via_FF (genes);
  assert (result);

  //Go through each G of FE.
  for (unsigned int i=0; i<genes.size(); i++) {
    dbmsh3d_vertex* G = genes[i];
    vcl_set<dbmsh3d_face*> incident_faces;
    G->get_incident_Fs (incident_faces);
    //Check all incident faces of G.
    vcl_set<dbmsh3d_face*>::iterator it = incident_faces.begin();
    for (; it != incident_faces.end(); it++) {
      dbmsh3d_face* F = (*it);
      //Check if F is dual to FE by checking if all genes[] incident to F.
      if (F->all_bnd_Vs_incident (genes))
        return F;
    }
  }

  return NULL;
}

//: Compute the gap cost of a shock link element.
//  For greedy_option_ == 1, compute cost for 1st greedy queue.
//    For A13-2 shock links (acute, obtuse)
//      cost = perimeter / Rmin / compactness^2
//    For non-A13-2 shock links
//      cost = perimeter / d_avg / compactness^2
//  For greedy_option_ == 2, compute cost for 2nd greedy queue.
//      cost = perimeter / d_avg / compactness^2
//
//    compactness = see Fre's thesis p.121.
//    Rmin = min of the two shock_radius.
//
//  Save FE_SEG_TYPE in c_value_ 
//    - FE_SEG_ACUTE for acute triangles.
//    - FE_SEG_OBTUSE for obtuse triangles.
//    - FE_SEG_NO_A132 for non-A13-2 links.
//    - FE_SEG_OVERSIZED for oversize triangles.
//
void dbsk3d_fs_segre::L_compute_gap_cost (dbsk3d_fs_edge* FE, const int option)
{
  assert (FE->b_inf() == false);

  //Compute the surface triangle parameters:
  double max_side, perimeter;
  unsigned int nG;
  double Side[3];
  dbmsh3d_vertex* Gene[3];
  bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
  ///assert (result);
  if (result == false) { //A TEMPORARY SOLUTION
    FE->set_seg_type (FE_SEG_OVERSIZED);
    FE->set_cost (SEG_OVERSIZE_COST);
    return;
  }

  //1) Assing huge cost for oversized links.
  if (max_side > ((double) max_sample_dist_)) {
    FE->set_seg_type (FE_SEG_OVERSIZED);
    FE->set_cost (SEG_OVERSIZE_COST);
    return;
  }
  
  //Compute compactness.
  double compactness = FE->compute_tri_compactness (nG, Side);

  //Compute the min shock radius Rmin.
  vgl_point_3d<double> C = circum_center_3pts (Gene[0]->pt(), Gene[1]->pt(), Gene[2]->pt());
  const double SC = vgl_distance (FE->s_FV()->pt(), C);
  const double EC = vgl_distance (FE->e_FV()->pt(), C);
  const double SE = vgl_distance (FE->s_FV()->pt(), FE->e_FV()->pt());

  double Rmin = vcl_min (SC, EC);

  if (dbgl_leq_m (SC, SE) == false || dbgl_leq_m (EC, SE) == false) {    
    //2) For non-A13-2 shock links, put to the 2nd shock queue.
    FE->set_seg_type (FE_SEG_NO_A132);
    FE->set_cost (perimeter / (median_A122_dist() * compactness * compactness));
  }
  else { 
    //3) For A13-2 acute and obtuse triangles.
    if (option == 1) { //For option 1 for the 1st shock queue.
      if (dbgl_eq_m (Rmin, 0))
        Rmin = DBGL_MID_EPSILON;
      FE->set_cost (perimeter / (Rmin * compactness * compactness));
    }
    else { //For option 2 for the 2nd shock queue.
      assert (option == 2);
      FE->set_cost (perimeter / (median_A122_dist() * compactness * compactness));
    }

    //Determine if this surface interpolant is acute or obtuse.
    if (FE->dual_DT_contains_A13_2 (nG, Gene))
      FE->set_seg_type (FE_SEG_ACUTE);
    else
      FE->set_seg_type (FE_SEG_OBTUSE);
  }

  assert (FE->cost() > 0);
}

// ###########################################################################

void dbsk3d_fs_segre::L_3d_gap_transform (dbsk3d_fs_edge* FE)
{
  dbmsh3d_face* F = L_3d_gap_xform (bnd_mesh(), FE);
  _add_to_F_L_vector (F, FE);
}

//: Modify the neighboring continuity cost to all adjacent fs_edges.
//    - for edge adjacent neighbor: reduce cost.
//    - for vertex adjacent neighbor: add cost.
//
void dbsk3d_fs_segre::L_modify_nbrs_cost (const dbsk3d_fs_edge* FE)
{
  //Loop through all incident shock sheet and add cost to each shock link.
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    //add neighboring continuity cost to adjacent fs_edges.
    L_add_nbr_cost_P (FE, FF);
    HE = HE->pair();
  }
  while (HE != FE->halfedge());
}

//: For the inputL and patch FF,
//  loop through each adjacent link and add the neighboring continuity cost.
//
void dbsk3d_fs_segre::L_add_nbr_cost_P (const dbsk3d_fs_edge* inputL, 
                                        const dbsk3d_fs_face* FF)
{
  //loop through each boundary link of this patch.
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    if (FE != inputL) //avoid itself
      add_nbr_cost (inputL, FF, FE);  
    HE = HE->next();
  }
  while (HE != FF->halfedge());
}

#define NB_BEND_COST_C    1
#define NB_CONT_COST_C    1

//: For a meshed shock link inputL and
//  the neighboring shock link FE adjacent with shock patch FF,
//  add neighboring continuity cost to FE and 
//  re-insert FE back to the classified queue.
//
void dbsk3d_fs_segre::add_nbr_cost (const dbsk3d_fs_edge* inputL,
                                    const dbsk3d_fs_face* FF,
                                    dbsk3d_fs_edge* FE)
{
  if (FE->b_inf()) 
    return; //skip the shock-link-at-infinity.
  if (FE->cost() >= SEG_OVERSIZE_COST)
    return; //skip the oversize or non-A13-2 shocks.

  //Remove FE from the classified queue.
  int queue_option = remove_L_from_classified_queue (FE);

  double max_side, perimeter;
  unsigned int nG;
  double Side[3];
  dbmsh3d_vertex* Gene[3];
  bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
  assert (result);

  const double lenE = FF->dist_Gs ();

  //Compactness
  double compactness = FE->compute_tri_compactness (nG, Side);

  //theta: angle between the existing and candidate surface interpolants.
  double theta = compute_bending_angle (inputL, FF, FE);  

  //Compute the neighbor continuing cost.
  double exp_t_1 = vcl_exp (theta) - 1;
  double bending = exp_t_1 * exp_t_1 * NB_BEND_COST_C;
  double nbcont_cost;

  if (queue_option == 1 || (queue_option==0 && greedy_option_==1)) {
    double Rmin = FE->compute_min_radius (nG, Gene);
    if (dbgl_eq_m (Rmin, 0))
        Rmin = DBGL_MID_EPSILON;
    nbcont_cost = (bending - NB_CONT_COST_C) * lenE / (Rmin * compactness * compactness);
  }
  else {
    nbcont_cost = (bending - NB_CONT_COST_C) * lenE / (median_A122_dist() * compactness * compactness);
  }

  #if DBMSH3D_DEBUG>2
  vul_printf (vcl_cout, "\tnb_cont_cost: change cost of link %d (%f) by %f%%.\n", 
               FE->id(), FE->cost(), (double)(100.0 * nbcont_cost / FE->cost()));  
  #endif

  //Update the new cost.
  FE->set_cost ((float) (FE->cost() + nbcont_cost));  
  if (FE->cost() < 0) ///if (dbgl_eq_c (FE->cost(), 0)) /// debug aorte 
    FE->set_cost (0.0f);    
  assert (FE->cost() >= 0);

  //Re-insert link into the classified queue according to each case.
  if (queue_option != 0)
    add_L_to_classified_queue (FE, queue_option);
}

//    - Undo the neighboring continuity cost to each adjacent fs_edge.
//
void dbsk3d_fs_segre::undo_L_3d_gap_transform (dbsk3d_fs_edge* FE)
{
  //1) Delete the boundary face.
  _del_from_F_L_vector (FE->bnd_face());
  undo_L_3d_gap_xform (bnd_mesh(), FE);
}

//: Undo the modification of the continuity cost to all neighbors.
void dbsk3d_fs_segre::undo_L_modify_nbrs_cost (dbsk3d_fs_edge* FE)
{
  //Loop through all incident shock sheet and undo cost to each shock link.
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    //add neighboring continuity cost to adjacent fs_edges.
    L_undo_nbr_cost_P (FE, FF);
    HE = HE->pair();
  }
  while (HE != FE->halfedge());
}

//: For the inputL and patch FF,
//  loop through each adjacent link and undo the neighboring continuity cost.
//
void dbsk3d_fs_segre::L_undo_nbr_cost_P (const dbsk3d_fs_edge* inputL, 
                                         const dbsk3d_fs_face* FF)
{
  //loop through each boundary link of this patch.
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    if (FE != inputL) //avoid itself
      undo_nbr_cost (inputL, FF, FE);  
    HE = HE->next();
  }
  while (HE != FF->halfedge());
}

//: For shock link inputL under undoing gap-xform and
//  the neighboring shock link FE adjacent with shock patch FF,
//  undo neighboring continuity cost to FE and 
//  re-insert FE back to the classified queue.
//
void dbsk3d_fs_segre::undo_nbr_cost (const dbsk3d_fs_edge* inputL,
                                     const dbsk3d_fs_face* FF,
                                     dbsk3d_fs_edge* FE)
{
  if (FE->b_inf()) 
    return; //skip the shock-link-at-infinity.
  if (FE->cost() >= SEG_OVERSIZE_COST)
    return; //skip the oversize or non-A13-2 shocks.

  //Remove FE from the classified queue.
  int queue_option = remove_L_from_classified_queue (FE);

  double max_side, perimeter;
  unsigned int nG;
  double Side[3];
  dbmsh3d_vertex* Gene[3];
  bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
  assert (result);

  const double lenE = FF->dist_Gs ();

  //Compactness
  double compactness = FE->compute_tri_compactness (nG, Side);

  //theta: angle between the existing and candidate surface interpolants.
  double theta = compute_bending_angle (inputL, FF, FE);  

  //Compute the neighbor continuing cost.
  double exp_t_1 = vcl_exp (theta) - 1;
  double bending = exp_t_1 * exp_t_1 * NB_BEND_COST_C;
  double nbcont_cost;

  if (queue_option == 1 || (queue_option==0 && greedy_option_==1)) {
    double Rmin = FE->compute_min_radius (nG, Gene);
    ///assert (dbgl_eq_m (Rmin, 0) == false); !!
    nbcont_cost = (bending - NB_CONT_COST_C) * lenE / (Rmin * compactness * compactness);
  }
  else {
    nbcont_cost = (bending - NB_CONT_COST_C) * lenE / (median_A122_dist() * compactness * compactness);
  }

  #if DBMSH3D_DEBUG>2
  vul_printf (vcl_cout, "\tnb_cont_cost: change cost of link %d (%f) by %f%%.\n", 
               FE->id(), FE->cost(), (double)(100.0 * nbcont_cost / FE->cost()));  
  #endif

  //Update the new cost.
  FE->set_cost ((float) (FE->cost() - nbcont_cost));  
  if (FE->cost() < 0) ///if (dbgl_eq_c (FE->cost(), 0)) debug aorte
    FE->set_cost (0.0f);    
  assert (FE->cost() >= 0);

  //Re-insert link into the classified queue according to each case.
  if (queue_option != 0)
    add_L_to_classified_queue (FE, queue_option);
}


