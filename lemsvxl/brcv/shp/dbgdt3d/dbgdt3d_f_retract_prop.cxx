//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_mesh.h>

// #######################################################################
//        Wavefront Handling Functions

//: Retract one step of propagation, error correction.
void gdt_f_manager::retract_edge_propagation_other (dbmsh3d_halfedge* input_he)
{
  //: The input_halfedge corresponds to the cur_face that decides the retraction.
  //  The actual retraction should happens at the other halfedge, the proped_he.
  dbmsh3d_gdt_edge* input_edge = (dbmsh3d_gdt_edge*) input_he->edge();
  dbmsh3d_halfedge* proped_he = input_edge->m2_other_HE (input_he);
  
  if (proped_he)
    retract_edge_propagation (proped_he);
  else
    //: !! proped_he can be NULL for some degenerate case (ex: mid_plane_hole -s 6)
    //  in this case, remove the propagation of input_he
    remove_E_propagation_on_edge (input_edge);
}

//: Retract one step of propagation, error correction.
//  - The error-correction of the Wavefront-Interval method is different from the
//    original Interval-based method. To maintain integrity,
//    once an interval of an edge is retracted, all intervals on that edge are to be retracted.
//    Moreover, edges of all children intervals are to be retracted.
//  - For all three edges of the face, remove all halfedges from the wavefront.
//  - Check the other two edges, edge[0], edge[1], if any interval of them is from input_edge, 
//    retact the whole subtree of it.
//  - Add new candidate to the wavefront.
void gdt_f_manager::retract_edge_propagation (dbmsh3d_halfedge* input_he)
{
  dbmsh3d_gdt_edge* input_edge = (dbmsh3d_gdt_edge*) input_he->edge();

  dbmsh3d_face* prop_face = input_he->face();

#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cout, "    Retract propagation of edge %d at face %d\n", input_edge->id(), prop_face->id());
#endif
  
  dbmsh3d_gdt_edge* edge[2];
  edge[0] = (dbmsh3d_gdt_edge*) prop_face->find_next_bnd_E (input_edge->sV(), input_edge);
  edge[1] = (dbmsh3d_gdt_edge*) prop_face->find_next_bnd_E (input_edge->eV(), input_edge);

  //: For all three edges of the face, remove all halfedges from the front.
  remove_wavefront_edge (input_edge);
  remove_wavefront_edge (edge[0]);
  remove_wavefront_edge (edge[1]);

  //edges to remove propagation
  vcl_set<dbmsh3d_gdt_edge*> edges_to_remove_prop; 
  edges_to_remove_prop.insert (input_edge);

  gdt_mesh_->init_traverse();

  //: Check the other edges of the prop_face
  for (unsigned int i=0; i<2; i++) {    
    //: add edges of edge[i]'s subtree of intervals to the 'remove' set.
    vcl_map<double, gdt_ibase*>::iterator it = edge[i]->interval_section()->I_map()->begin();
    for (; it != edge[i]->interval_section()->I_map()->end(); it++) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      //: If I are from the input_edge, I is not valid.
      if (I->is_from_edge (input_edge))
        _get_edges_subtree (I, &edges_to_remove_prop);
    }
  }

  _remove_intervals_on_edgeset (&edges_to_remove_prop);

  //: Add possible edges to the wavefront
  //  If edge[i] still contains valid intervals, 
  //  and there exists other faces to propagate, add it to the front.
  for (unsigned int i=0; i<2; i++) {
    dbmsh3d_halfedge* he = edge[i]->get_propagated_halfedge ();
    if (he && edge[i]->m2_other_HE(he))
      add_wavefront_he (he);
  }
}

//: Remove propagation (and all subsequent propagations) on the input_edge.
//  - Once an interval of an edge is retracted, all intervals on that edge are to be retracted.
//    Moreover, edges of all children intervals are to be retracted.
//  - For all such edges to be retracted, remove them from the wavefront.
void gdt_f_manager::remove_E_propagation_on_edge (dbmsh3d_gdt_edge* input_edge)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cout, "    Remove propagation of edge %d.\n", input_edge->id());
#endif

  //edges to remove propagation
  vcl_set<dbmsh3d_gdt_edge*> edges_to_remove_prop;

  gdt_mesh_->init_traverse();

  //: add edges of its subtree of intervals to the 'remove' set.
  vcl_map<double, gdt_ibase*>::iterator it = input_edge->interval_section()->I_map()->begin();
  for (; it != input_edge->interval_section()->I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    _get_edges_subtree (I, &edges_to_remove_prop);
  }

  _remove_intervals_on_edgeset (&edges_to_remove_prop);
}

//: Remove subsequent propagations but leave the edge's propagation.
void gdt_f_manager::remove_E_subseq_prop_on_edge (dbmsh3d_gdt_edge* input_edge)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cout, "    Remove subseq. propagation of edge %d.\n", input_edge->id());
#endif

  //edges to remove propagation
  vcl_set<dbmsh3d_gdt_edge*> edges_to_remove_prop;

  gdt_mesh_->init_traverse();

  //: add edges of each next_I's subtree of intervals to the 'remove' set.
  vcl_map<double, gdt_ibase*>::iterator iit = input_edge->interval_section()->I_map()->begin();
  for (; iit != input_edge->interval_section()->I_map()->end(); iit++) {
    gdt_interval* II = (gdt_interval*) (*iit).second;

    vcl_vector<gdt_interval*>::iterator it = II->nextIs().begin();
    for (; it != II->nextIs().end(); it++) {
      gdt_interval* nI = (gdt_interval*) (*it);
      assert (nI->prev_flag()==true);
    
      _get_edges_subtree (nI, &edges_to_remove_prop);
    }
  }

  _remove_intervals_on_edgeset (&edges_to_remove_prop);
}

//: For a given interval input_I, go through all sibling intervals of input_I's edge e,
//  get the set of edges of the subtree of intervals, including e itself.
//   - no deletion of elements is allowed in this loop.
//   - after an edge is visited, don't visit it again.
void gdt_f_manager::_get_edges_subtree (gdt_interval* input_I, 
                                               vcl_set<dbmsh3d_gdt_edge*>* edges_of_subtree)
{
  //: visit the edge only if it's not visited
  if (input_I->edge()->is_visited (gdt_mesh_->i_traverse_flag()))
    return;

  //: add itself into the set
  edges_of_subtree->insert (input_I->edge());

  //: go through each sibling interval on the edge.
  vcl_map<double, gdt_ibase*>::iterator iit = input_I->edge()->interval_section()->I_map()->begin();
  for (; iit != input_I->edge()->interval_section()->I_map()->end(); iit++) {
    gdt_interval* II = (gdt_interval*) (*iit).second;

    //: recursion on II's subtree.
    vcl_vector<gdt_interval*>::iterator it = II->nextIs().begin();
    for (; it != II->nextIs().end(); it++) {
      gdt_interval* nI = (gdt_interval*) (*it);
      assert (nI->prev_flag() == true);
  
      _get_edges_subtree (nI, edges_of_subtree);
    }
  }

  //: mark this edge as visited
  input_I->edge()->set_i_visited (gdt_mesh_->i_traverse_flag());
}

//: Go through the edges_to_remove_prop[] set and 
//  remove any edge on the wavefront
//  break all interval's prev connection
//  break all interval's edge connection
//  put all intervals into an intervals_to_detele[] set
//
void gdt_f_manager::_remove_intervals_on_edgeset (vcl_set<dbmsh3d_gdt_edge*>* edges_to_remove_prop)
{
  vcl_set<gdt_interval*> intervals_to_detele;

  vcl_set<dbmsh3d_gdt_edge*>::iterator eit = edges_to_remove_prop->begin();
  for (; eit != edges_to_remove_prop->end(); eit++) {
    dbmsh3d_gdt_edge* cur_edge = *eit;

    remove_wavefront_edge (cur_edge);
    cur_edge->set_dist (GDT_HUGE);

    vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
    while (it != cur_edge->interval_section()->I_map()->end()) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      //break the prev connection
      if (I->prev_flag()) {        
        I->prevI()->_remove_nextI (I);
        I->set_prev_flag (false);
      }

      //break the edge connection
      if (I->b_attach_to_edge()) {
        cur_edge->_detach_interval (I);
        I->_set_attach_to_edge (false);        
      }
      intervals_to_detele.insert (I);

      //the element has been removed from the map, next is the begin().
      it = cur_edge->interval_section()->I_map()->begin();
    }
  }

  //: delete all intervals in intervals_to_detele[]
  vcl_set<gdt_interval*>::iterator iit = intervals_to_detele.begin();
  while (iit != intervals_to_detele.end()) {
    gdt_interval* I = *iit;
    delete I;

    intervals_to_detele.erase (iit);
    iit = intervals_to_detele.begin();
  }
}

void gdt_f_manager::remove_subseq_intervals_on_edge (dbmsh3d_gdt_edge* input_edge)
{
#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cout, "    Remove subseq. intervals of edge %d.\n", input_edge->id());
#endif

  vcl_set<gdt_interval*> intervals_subtree;

  //: add each next_I's subtree of intervals to the 'remove' set.
  vcl_map<double, gdt_ibase*>::iterator iit = input_edge->interval_section()->I_map()->begin();
  for (; iit != input_edge->interval_section()->I_map()->end(); iit++) {
    gdt_interval* II = (gdt_interval*) (*iit).second;

    vcl_vector<gdt_interval*>::iterator it = II->nextIs().begin();
    for (; it != II->nextIs().end(); it++) {
      gdt_interval* nI = (*it);
      assert (nI->prev_flag()==true);
    
      _get_intervals_subtree (nI, &intervals_subtree);
    }
  }

  //: delete all intervals in intervals_subtree[]
  //  first break all the prev connection and edge connection.
  vcl_set<gdt_interval*>::iterator it = intervals_subtree.begin();
  for (; it != intervals_subtree.end(); it++) {
    gdt_interval* I = *it;

    //break the prev connection
    if (I->prev_flag()) {        
      I->prevI()->_remove_nextI (I);
      I->set_prev_flag (false);
    }

    //break the edge connection
    if (I->b_attach_to_edge()) {
      I->edge()->_detach_interval (I);
      I->_set_attach_to_edge (false);        
    }
  }

  //: delete all intervals
  it = intervals_subtree.begin();
  while (it != intervals_subtree.end()) {
    gdt_interval* I = (*it);
    delete I;

    intervals_subtree.erase (it);
    it = intervals_subtree.begin();
  }
}







