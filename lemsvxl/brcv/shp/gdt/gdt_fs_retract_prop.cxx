//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>

#include <gdt/gdt_manager.h>
#include <gdt/gdt_mesh.h>

#if GDT_ALGO_FS

//: Retract one step of propagation, the retract_he is the one from the event face.
void gdt_fs_manager::fs_retract_edge_other (dbmsh3d_halfedge* input_he)
{
  //: The input_he corresponds to the cur_face that decides the retraction.
  //  The actual retraction should happens at the other halfedge, the proped_he.
  dbmsh3d_gdt_edge* input_edge = (dbmsh3d_gdt_edge*) input_he->edge();
  dbmsh3d_halfedge* proped_he = input_edge->m2_other_HE (input_he);
  
  if (proped_he)
    ws_retract_edge (proped_he);
  else
    //: !! proped_he can be NULL for some degenerate case (ex: mid_plane_hole -s 6)
    //  in this case, remove the propagation of input_he
    assert (0);
}

void gdt_fs_manager::ws_retract_edge (dbmsh3d_halfedge* prop_he)
{
  dbmsh3d_gdt_edge* prop_edge = (dbmsh3d_gdt_edge*) prop_he->edge();
  dbmsh3d_face* prop_face = prop_he->face();

#if GDT_DEBUG_MSG
  if (n_verbose_>2)
    vul_printf (vcl_cerr, "    Retract propagation of edge %d at face %d\n", prop_edge->id(), prop_face->id());
#endif

  //: For all three edges of the face, remove all halfedges from the front.
  dbmsh3d_gdt_edge* edge[2];
  edge[0] = (dbmsh3d_gdt_edge*) prop_face->find_other_edge (prop_edge->sV(), prop_edge);
  edge[1] = (dbmsh3d_gdt_edge*) prop_face->find_other_edge (prop_edge->eV(), prop_edge);
  remove_wavefront_edge (prop_edge);
  prop_edge->set_dist (GDT_HUGE);
  remove_wavefront_edge (edge[0]);
  remove_wavefront_edge (edge[1]);

  //: retract each shock on the prop_edge
  _ws_retract_shock_on_edge (prop_edge, edge[0], edge[1]);

  //: delete each interval on the prop_edge
  _ws_delete_interval_on_edge (prop_edge, edge[0], edge[1]);
  assert (!prop_edge->is_propagated());

  for (unsigned int i=0; i<2; i++) {
    //: for each edge in edge[2], for each shock, only retract if it is from the prop_face
    _ws_retract_shock_on_edge (edge[i], prop_edge, edge[1-i]);

    //: for each edge in edge[2], for each interval, only retract if it is from the prop_face
    _ws_delete_interval_on_edge (edge[i], prop_edge, edge[1-i]);
  }
}

void gdt_fs_manager::_ws_retract_shock_on_edge (dbmsh3d_gdt_edge* input_edge,
                                                dbmsh3d_gdt_edge* from_edge1,
                                                dbmsh3d_gdt_edge* from_edge2)
{
  vcl_map<double, gdt_shock*>::iterator sit = input_edge->shock_section()->S_map()->begin();
  for (; sit != input_edge->shock_section()->S_map()->end(); sit++) {
    gdt_shock* S = (*sit).second;

  }
}

void gdt_fs_manager::_ws_delete_interval_on_edge (dbmsh3d_gdt_edge* input_edge,
                                                  dbmsh3d_gdt_edge* from_edge1,
                                                  dbmsh3d_gdt_edge* from_edge2)
{
  vcl_set<gdt_ibase*> intervals_to_detele;

  vcl_map<double, gdt_ibase*>::iterator it = input_edge->interval_section()->I_map()->begin();
  while (it != input_edge->interval_section()->I_map()->end()) {
    gdt_ibase* II = (*it).second;

    assert (II->type() != ITYPE_DUMMY);

    if (II->is_dege()) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      if (I->is_from_edge (from_edge1) || I->is_from_edge (from_edge2)) {
        //break the edge connection
        if (I->b_attach_to_edge()) {
          input_edge->_detach_interval (I);
          I->_set_attach_to_edge (false);        
        }

        intervals_to_detele.insert (I);
        it = input_edge->interval_section()->I_map()->upper_bound (I->stau());
      }
      else
        it++;
    }
    else if (II->is_psrc()) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      if (I->is_from_edge (from_edge1) || I->is_from_edge (from_edge2)) {
        //break the prev connection
        if (I->prev_flag()) {
          I->prev()->_del_next (I);
          I->set_b_prev_connectivity (false);
        }
        //break the edge connection
        if (I->b_attach_to_edge()) {
          input_edge->_detach_interval (I);
          I->_set_attach_to_edge (false);        
        }

        intervals_to_detele.insert (I);
        it = input_edge->interval_section()->I_map()->upper_bound (I->stau());
      }
      else
        it++;
    }
    else
      assert (0);

    //: bug !!
    //the element has been removed from the map, next is the begin().
    ///it = input_edge->interval_section()->I_map()->begin();
  }

  //: !! SLOW !! delete all intervals in intervals_to_detele[]
  vcl_set<gdt_ibase*>::iterator iit = intervals_to_detele.begin();
  while (iit != intervals_to_detele.end()) {
    gdt_ibase* II = *iit;

    if (II->is_dege()) {
      gdt_interval* I = (gdt_interval*) (*iit);
      delete I;
    }
    else if (II->is_psrc()) {
      gdt_interval* I = (gdt_interval*) (*iit);
      delete I;
    }
    else
      assert (0);

    intervals_to_detele.erase (iit);
    iit = intervals_to_detele.begin();
  }
}

#endif





