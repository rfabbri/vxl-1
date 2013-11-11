//: Aug 19, 2005 MingChing Chang
//  

#include <dbgdt3d/dbgdt3d_edge.h>
#include <dbgdt3d/dbgdt3d_path.h>

dbmsh3d_gdt_edge::~dbmsh3d_gdt_edge () 
{
  //: go through each interval and disconnect its prev-next connectivity
  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  for (; it != interval_section_.I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    assert (!I->is_dummy());

    if (I->prev_flag())
      untie_prevI_nextI (I->prevI(), I);

    vcl_vector<gdt_interval*>::iterator nit = I->nextIs().begin();
    while (nit != I->nextIs().end()) {
      gdt_interval* nI = *nit;
      
      //untie I-nI
      nI->set_prev_flag (false);
      I->nextIs().erase (nit);

      nit = I->nextIs().begin();
    }
  }

  //: go through the interval_section again and 
  //  detach and delete each interval
  it = interval_section_.I_map()->begin();
  while (it != interval_section_.I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    interval_section_.I_map()->erase (it);

    #if GDT_ALGO_F
    I->_set_attach_to_edge (false);
    #endif

    delete I;

    it = interval_section_.I_map()->begin();
  }
}

dbmsh3d_halfedge* dbmsh3d_gdt_edge::get_he_not_completely_propagated ()
{
  dbmsh3d_halfedge* cur_he = halfedge_;
  //: if there's no associated halfedge 
  if (cur_he == NULL)
    return NULL;

  //: if there's only one associated halfedge (no loop)
  if (cur_he->pair() == NULL) {
    if (!_is_face_completely_propagated (cur_he->face()))
      return cur_he;
    else
      return NULL;
  }

  //: the last case, the associated halfedges form a circular list
  do {
    if (!_is_face_completely_propagated (cur_he->face()))
      return cur_he;
    cur_he = cur_he->pair();
  }
  while (cur_he != halfedge_);

  return NULL;
}

//: loop through halfedges and try to find one valid face to propagate.
//  Note that in general there always exists such next face for an interval to propagate.
dbmsh3d_halfedge* dbmsh3d_gdt_edge::get_he_to_propagate (dbmsh3d_halfedge* cur_from_he)
{
  dbmsh3d_halfedge* cur_he = halfedge_;

  assert (cur_he != NULL);
  //: if there's only one associated halfedge (no loop)
  if (cur_he->pair() == NULL)
    return NULL;

  //: the last case, the associated halfedges form a circular list
  do {
    if (cur_he != cur_from_he) { //skip if it is the 'from' halfedge.
      dbmsh3d_face* cur_face = cur_he->face();
      bool b_face_to_prop = is_face_to_propagate (cur_face);
      if (b_face_to_prop)
        return cur_he;
    }
    cur_he = cur_he->pair();
  }
  while (cur_he != halfedge_);

  return NULL;
}

//: Fix the edge-interval both-way pointers and
//  the prevI-next both-way pointers for the edge's intervals.
//  Important: only regular intervals are in the prevI-next tree structure.
//  Degenerate intervals are not in it! Degenerate I's prev_ is reserved for other use!
void dbmsh3d_gdt_edge::attach_IS () 
{
  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  for (; it != interval_section_.I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;

#if GDT_ALGO_F
    //: Note that the I is already in the edge's interval_section, just set the flag.
    I->_set_attach_to_edge (true);

    //: If I is non-degenerate and I is not in the prev_next structure, add I into it.
    if (!I->is_dege())
      if (!I->prev_flag())
        if (I->prevI())
          tie_prevI_nextI (I->prevI(), I);
#endif

    double dist = I->_get_min_dist ();
    if (dist < dist_)
      dist_ = dist;

  }
}

//: Fix the edge-interval both-way pointers and
//  the prevI-next both-way pointers for the edge's intervals.
//  Important: only regular intervals are in the prevI-next tree structure.
//  Degenerate intervals are not in it! Degenerate I's prev_ is reserved for other use!
void dbmsh3d_gdt_edge::attach_IS (gdt_interval_section* IS)
{
  assert (interval_section_.I_map()->size()==0);
  vcl_map<double, gdt_ibase*>::iterator it = IS->I_map()->begin();
  while (it != IS->I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    assert (I->b_attach_to_edge() == false);
    I->_set_attach_to_edge (true);

    double dist = I->_get_min_dist ();
    if (dist < dist_)
      dist_ = dist;

    //: If I is non-degenerate and I is not in the prev_next structure, add I into it.
    if (!I->is_dege())
      if (!I->prev_flag())
        if (I->prevI())
          tie_prevI_nextI (I->prevI(), I);

    IS->I_map()->erase (it);
    interval_section_.I_map()->insert (vcl_pair<double, gdt_ibase*>(I->stau(), I));

    it = IS->I_map()->begin();
  }
}

void dbmsh3d_gdt_edge::detach_IS_to (gdt_interval_section* IS)
{
  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  while (it != interval_section_.I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    //detach I from the prev
    if (!I->is_dege() && I->prevI())
      untie_prevI_nextI (I->prevI(), I);

    //the nexts[] should be empty!
    assert (I->nextIs().size() == 0);

    //add to the destination interval_section
    IS->_add_interval (I);

    //detach I from the edge    
    I->_set_attach_to_edge (false);
    interval_section_.I_map()->erase (it);

    it = interval_section_.I_map()->begin();
  }

  //: reset this edge's distance_.
  dist_ = GDT_HUGE;
}

// ######################################################################

//: Loop through interval_section_[] find the I containing input_tau, 
//  and also compute the geodesic distance.
//  return the I containing input_tau
//  return in geodesic_dist the distance.
gdt_interval* dbmsh3d_gdt_edge::get_dist_at_tau (const double& input_tau, double& geodesic_dist)
{
  //: loop through interval_section and find the I containing input_tau
  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  for (; it != interval_section_.I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    if (input_tau < I->stau() || input_tau > I->etau())
      continue;

    //: compute the geodesic distance via I
    geodesic_dist = I->get_dist_at_tau (input_tau);
    return I;
  }

  //: set the dist to be INVALID_HUGE
  geodesic_dist = GDT_INVALID_HUGE;
  return NULL;
}

void dbmsh3d_gdt_edge::get_min_max_distance (double& min_dist, double& max_dist)
{
  //: loop through all intervals and find the ones with min/max distance
  min_dist = GDT_HUGE;
  max_dist = 0;

  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  for (; it != interval_section_.I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    double min, max;
    I->get_min_max_dist (min, max);

    if (min < min_dist)
      min_dist = min;
    if (max > max_dist)
      max_dist = max;
  }
}

//: Given the query_dist, there exists at almost two points with the given geodesic distance
void dbmsh3d_gdt_edge::get_gdt_points (const double gdt_dist, 
                                       vcl_pair<gdt_interval*, double>& gdt_point_1,
                                       vcl_pair<gdt_interval*, double>& gdt_point_2)
{
  //: go through all intervals and query tau from given distance
  vcl_map<double, gdt_ibase*>::iterator it = interval_section_.I_map()->begin();
  for (; it != interval_section_.I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    //: -1 means not valid
    double tau1 = -1;
    double tau2 = -1;

    I->query_taus_from_dist (gdt_dist, tau1, tau2);

    if (tau1 != -1) {
      //: should be possible?
      assert (gdt_point_1.second = -1);
      gdt_point_1.first = I;
      gdt_point_1.second = tau1;
    }
    if (tau2 != -1) {
      assert (gdt_point_2.second = -1);
      gdt_point_2.first = I;
      gdt_point_2.second = tau2;
    }
  }
}

// #########################################################

#if GDT_ALGO_WS



#endif

// #########################################################

bool _is_face_completely_propagated (dbmsh3d_face* cur_face)
{
  //: the starting halfedge is cur_face's pointing halfedge
  dbmsh3d_halfedge* cur_he = cur_face->halfedge();

  //: Becareful if the next is NULL.
  assert (cur_he->next() != NULL);

  //: traverse through the circular list of halfedges,
  //  if anyone is unpropagated, return false.
  do {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
    if (!cur_edge->intervals_a_coverage())
      return false;
    
    cur_he = cur_he->next();
  }
  while (cur_he != cur_face->halfedge());

  return true;
}

void _find_min_max_dist_face (dbmsh3d_face* cur_face,
                              double& min_dist, double& max_dist)
{
  min_dist = GDT_HUGE;
  max_dist = 0;

  dbmsh3d_halfedge* cur_he = cur_face->halfedge();
  assert (cur_he->next() != NULL);

  do {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
    double min, max;
    cur_edge->get_min_max_distance (min, max);

    if (min < min_dist)
      min_dist = min;
    if (max > max_dist)
      max_dist = max;

    cur_he = cur_he->next();
  }
  while (cur_he != cur_face->halfedge());  
}



