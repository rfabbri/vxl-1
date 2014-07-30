// This is brcv/shp/dbsk2d/algo/dbsk2d_lagrangian_ishock_detector_prop.cxx

//:
// \file This file contains all the shock initialization functions
//       because the main file got too big.

#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>

//-------------------------------------------------------------------
// Shock propagation functions
//-------------------------------------------------------------------

//Go through the time sorted list of ACTIVE shock elements
//propagate each ACTIVE shock and move on to the NEXT ACTIVE one
//until all elements in the list are 'DEAD' or 'OUT OF THE SCREEN'
//'OUT OF THE SCREEN' means time > MAXRADIUS !
//return true if propagation is successful.
bool dbsk2d_lagrangian_ishock_detector::propagate_shocks ()
{
  PROPAGATION_TYPE ret=NO_PROPAGATION;
  #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout<< "\n===== Start Propagating Shocks =====" <<vcl_endl;
  #endif

  try {
    //MAIN LOOP OF PROPAGATION...
    while (ret != PROPAGATION_DONE && 
      ret != PROPAGATION_ERROR_DETECTED) //INVALID_JUNCT_REMOVED
    {
      ret = propagate_next_active_shock();

      #ifdef DEBUG_SHOCK_VERBOSE
      DebugPrintOnePropagation (0, ret);
      #endif

      //validate shocks at each step of the propagation
      #ifdef VALIDATE_SHOCKS_AT_EVERY_STEP
      if (!ValidateShockList()) {
        vcl_cout<< "ValidateShockList() error! " <<vcl_endl;
      }
      #endif
    }
  }
  catch (const dbsk2d_exception_topology_error &e)
  {
    vcl_cout << e.what() << vcl_endl;

    delete_the_active_selm_list();
    delete_the_candidate_src_list();

    //this shock is no longer valid so delete the shock graph completely
    //clear();
  }

  if (ret==PROPAGATION_DONE) return true;
  else  return false;
}

void dbsk2d_lagrangian_ishock_detector::propagate_a_bunch_of_shocks ()
{
  PROPAGATION_TYPE ret;

  //Propagate 25 steps at a time
  for (int i=0; i<25; i++)
  {
    ret = propagate_next_active_shock();

    #ifdef DEBUG_SHOCK_VERBOSE
    DebugPrintOnePropagation (0, ret);
    #endif

    //validate shocks at each step of the propagation
    #ifdef VALIDATE_SHOCKS_AT_EVERY_STEP
    if (!ValidateShockList()) {
      vcl_cout<< "ValidateShockList() error! " <<vcl_endl;
    }
    #endif

    if (ret == dbsk2d_lagrangian_ishock_detector::PROPAGATION_DONE )
      break;
  }
}

dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::propagate_next_active_shock()
{
  //1)If there are no shocks to propagate, return
  if (number_of_active_shocks()==0 && number_of_candidate_sources()==0)
  {
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout<< "No more active shocks!" <<vcl_endl;
    #endif

    finalize_propagation();
    return dbsk2d_lagrangian_ishock_detector::PROPAGATION_DONE;
  }

  //The shock to be propagated at this time
  dbsk2d_ishock_edge* curEdge = 0;

  // There are two events that are possible
  // 1. Either a candidate source gets validated or
  // 2. An active shock propagates
  // Normally it is decided by time of event. However if they are 
  // have the same sim time active edges get priority over
  // candidate sources

  //2) If there's a pre-determined shock to propagate...do it!
  if (_nextShockToPropagate) {
    curEdge = _nextShockToPropagate;
    _nextShockToPropagate = NULL;
  }
  else {
    //3)get the first active shock (first on the list)
    if (number_of_active_shocks()>0){
      curEdge = active_shocks_list.begin()->second;
    }
    //4)get the first candidate source
    dbsk2d_ishock_node* curSrc = 0;
    if (number_of_candidate_sources()>0){
      curSrc = candidate_src_list.begin()->second;
    }

    // 5)active edges get priority over candidate sources
    if (curSrc && curEdge){
      if (RisL(curSrc->simTime(), curEdge->simTime())){
        //update simtime
        _sim_time = curSrc->simTime();   

        #ifdef DEBUG_SHOCK_VERBOSE
        vcl_cout << "t= " << _sim_time << " " << "SN: " << curSrc->id() << ": ";
        #endif
        
        return validate_candidate_source(curSrc);
      }
    }
    else if (curSrc){
      //update simtime
      _sim_time = curSrc->simTime();

      #ifdef DEBUG_SHOCK_VERBOSE
      vcl_cout << "t= " << _sim_time << " " << "SN: " << curSrc->id() << ": ";
      #endif
      
      return validate_candidate_source(curSrc);
    }
  }

  //update simtime
  _sim_time = curEdge->simTime();

  #ifdef DEBUG_SHOCK_VERBOSE
  vcl_cout << "t= " << _sim_time << " " << "Sh: " << curEdge->id() << ": ";
  #endif

  //7)If not propagated, propagate it...
  if (!curEdge->hasPropagated()) { 
    return propagate_shock_to_the_nearest_junction (curEdge);
  }
  else {
    //9)If the current shock has already been propagated, 
    //it is about to form a junction

    JUNCTION_TYPE junction = get_junction_type (curEdge);

    switch (junction) 
    {
    case SINK_JUNCTION:
      return form_a_sink (curEdge);
    case DEGENERATE_JUNCTION:
    case LEFT_REGULAR_JUNCTION:
    case RIGHT_REGULAR_JUNCTION:
      return form_a_junction (curEdge);
    default:
      //10) No intersections with any other shock.
      //    Since we have lagrangian cell boundaries, just deactivate them
      deactivate_an_ishock_elm(curEdge);
      curEdge->setSimTime (curEdge->endTime());

      return dbsk2d_lagrangian_ishock_detector::PROPAGATION_TO_BND;
    }
  }
}

//: finalize shock propagation in this cell by assigning wavefronts to 
//  cell boundaries that do not have shock intersections
void dbsk2d_lagrangian_ishock_detector::finalize_propagation()
{
  // we need to assign wavefronts to the sides that don't get any shock intersections
  // from the shock intersections on the neighboring sides

  //1) if the left side does not have any shock intersections
  if ( _left_bnd->active_shocks_list().size()==0)
  {
    //get it either from the top or bottom boundary
    if (_top_bnd->active_shocks_list().size()>0)
      _left_bnd->set_swf(_top_bnd->active_shocks_list().begin()->second->lBElement());
    else if (_bottom_bnd->active_shocks_list().size()>0)
      _left_bnd->set_swf(_bottom_bnd->active_shocks_list().begin()->second->rBElement());
  }
  else
    _left_bnd->clear_swf();

  //2) if the right side does not have any shock intersections
  if ( _right_bnd->active_shocks_list().size()==0)
  {
    //get it either from the top or bottom boundary
    if (_top_bnd->active_shocks_list().size()>0)
      _right_bnd->set_swf(_top_bnd->active_shocks_list().rbegin()->second->rBElement());
    else if (_bottom_bnd->active_shocks_list().size()>0)
      _right_bnd->set_swf(_bottom_bnd->active_shocks_list().rbegin()->second->lBElement());
  }
  else
    _right_bnd->clear_swf();

  //3) if the top side does not have any shock intersections
  if ( _top_bnd->active_shocks_list().size()==0)
  {
    //get it either from the left or right boundary
    if (_left_bnd->active_shocks_list().size()>0)
      _top_bnd->set_swf(_left_bnd->active_shocks_list().rbegin()->second->rBElement());
    else if (_right_bnd->active_shocks_list().size()>0)
      _top_bnd->set_swf(_right_bnd->active_shocks_list().rbegin()->second->lBElement());
  }
  else
    _top_bnd->clear_swf();

  //4) if the bottom side does not have any shock intersections
  if ( _bottom_bnd->active_shocks_list().size()==0)
  {
    //get it either from the left or right boundary
    if (_left_bnd->active_shocks_list().size()>0)
      _bottom_bnd->set_swf(_left_bnd->active_shocks_list().begin()->second->lBElement());
    else if (_right_bnd->active_shocks_list().size()>0)
      _bottom_bnd->set_swf(_right_bnd->active_shocks_list().begin()->second->rBElement());
  }
  else
    _bottom_bnd->clear_swf();

  //Display wavefronts at the 
  #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "\n========================================================\n";
    vcl_cout << "Cell ID: (" << _bnd_cell->index().row << " " << _bnd_cell->index().col << ")\n\n";
    
    vcl_cout << "LEFT:: \n";
    if (_left_bnd->swf()) 
      vcl_cout << "SWF: {" << _left_bnd->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    vcl_cout << "Sh list: [ ";
    for (ordered_shock_list_iter s_it = _left_bnd->active_shocks_list().begin();
      s_it != _left_bnd->active_shocks_list().end(); s_it++)
      vcl_cout << s_it->second->id() << " ";
    vcl_cout << "]\n";
  
    vcl_cout << "RIGHT:: \n";
    if (_right_bnd->swf()) 
      vcl_cout << "SWF: {" << _right_bnd->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    vcl_cout << "Sh list: [ ";
    for (ordered_shock_list_iter s_it = _right_bnd->active_shocks_list().begin();
      s_it != _right_bnd->active_shocks_list().end(); s_it++)
      vcl_cout << s_it->second->id() << " ";
    vcl_cout << "]\n";

    vcl_cout << "TOP:: \n";
    if (_top_bnd->swf()) 
      vcl_cout << "SWF: {" << _top_bnd->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    for (ordered_shock_list_iter s_it = _top_bnd->active_shocks_list().begin();
      s_it != _top_bnd->active_shocks_list().end(); s_it++)
      vcl_cout << s_it->second->id() << " ";
    vcl_cout << "]\n";

    vcl_cout << "BOTTOM:: \n";
    if (_bottom_bnd->swf()) 
      vcl_cout << "SWF: {" << _bottom_bnd->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    for (ordered_shock_list_iter s_it = _bottom_bnd->active_shocks_list().begin();
      s_it != _bottom_bnd->active_shocks_list().end(); s_it++)
      vcl_cout << s_it->second->id() << " ";
    vcl_cout << "]\n";
  
    vcl_cout << "========================================================\n";
  #endif

}

//: validate candidate source using wavefront information
// if valid, create a source node from it
dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE   
dbsk2d_lagrangian_ishock_detector::validate_candidate_source(dbsk2d_ishock_node* candsrc)
{
  //1)This candidate source needs to be removed from the candidate sources list
  //  whether it is validated or not

  //  we know that this element is always at the beginning of the list
  dbsk2d_assert(candidate_src_list.begin()->second == candsrc); //make sure
  candidate_src_list.erase(candidate_src_list.begin());

  #ifdef DEBUG_SHOCK_VERBOSE
  vcl_cout << "Validate Src=( "; 
  for (ishock_node_belm_list_iter curB = candsrc->bndList().begin();
       curB != candsrc->bndList().end(); ++curB)
    vcl_cout << (curB->second)->id() << " ";
  vcl_cout << "):" ;
  #endif

  //2) then check to see if all the wavefronts are still alive at the current time
  ishock_node_belm_list_iter curB = candsrc->bndList().begin();
  for ( ; curB != candsrc->bndList().end(); ++curB){
    if (! (curB->second)->is_wavefront_alive(curB->first, candsrc->startTime()))
    {
      delete candsrc; //delete this candidate source
      return dbsk2d_lagrangian_ishock_detector::INVALID_CANDIDATE_SOURCE;
    }
  }
  
  //3) update the etas of this source (for all points(ONLY))
  update_point_etas(candsrc);

  //4) update the status of this candidate source into a fully active source
  //   i.e., add it to the shock graph
  add_an_ishock_node (candsrc);

  //5)Instantiate the two child shocks from this source
  return propagate_from_a_source(candsrc);
}

//: if any of the boundary elements causing it are points
//  their etas need to be updated before propagation because during 
//  init only the raw vectors were recorded
//
// Note:
//  But due to the 0=2pi issue, when the vector is equal to  vref, 
//  if the point is on the left, it's eta needs to be set to max_eta
void dbsk2d_lagrangian_ishock_detector::update_point_etas(dbsk2d_ishock_node* cand_src)
{
  dbsk2d_ishock_belm* lbelm = cand_src->bndList().front().second;
  dbsk2d_ishock_belm* rbelm = cand_src->bndList().back().second; 

  double leta = cand_src->bndList().front().first;
  double reta = cand_src->bndList().back().first;

  if (lbelm->is_a_point())
    cand_src->bndList().front().first = ((dbsk2d_ishock_bpoint*)lbelm)->vec_to_eta(leta);

  if (rbelm->is_a_point())
      cand_src->bndList().back().first = ((dbsk2d_ishock_bpoint*)rbelm)->vec_to_eta(reta);
}

//: propagate shock branches from 2nd order sources
dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::propagate_from_a_source (dbsk2d_ishock_node* source)
{
  //1) get the wavefronts involved in forming this source
  dbsk2d_ishock_belm* belm1 = source->bndList().front().second;
  dbsk2d_ishock_belm* belm2 = source->bndList().back().second; 

  double eta1 = source->bndList().front().first;
  double eta2 = source->bndList().back().first;

  //2) now propagate the two children
  dbsk2d_ishock_edge *childShock1 = propagate_from_a_node(source, NULL, NULL, belm1, belm2, eta1, eta2);
  dbsk2d_ishock_edge *childShock2 = propagate_from_a_node(source, NULL, NULL, belm2, belm1, eta2, eta1);

  //There might not be a childshock to propagate from this source because, due to degeneracy, 
  //the current source node might have transitioned into a junction or a sink node
  if (childShock1){
    if (!childShock1->isValid()){
      //dbsk2d_assert(false);
      // The children are not yet in the shock list so we can just delete them
      delete childShock1; 
      childShock1 = 0;
    }
  }

  if (childShock2){
    if (!childShock2->isValid()){
      //dbsk2d_assert(false);
      // The children are not yet in the shock list so we can just delete them
      delete childShock2; 
      childShock2=0;
    }
  }

  //3) Despite all the effort, the source may yet turn out to be invalid, 
  // This is signalled by either of the children being invalid and the node 
  // not having morphed into another type due to transitions
  if ((!childShock1 || !childShock2) && source->indeg()==0)
  {
    //remove the source from the shock graph
    _ishock_graph->remove_vertex(source); 
    return dbsk2d_lagrangian_ishock_detector::INVALID_CANDIDATE_SOURCE;
  }

  //4) connect source to children set the propagated flag
  //   Note: sometimes, due to shock transitions, the shock branches are removed
  if (childShock1){
    source->set_cShock(childShock1);
    add_an_ishock_edge(childShock1);
  }

  if (childShock2) {
    if (source->cShock())
      source->set_cShock2(childShock2);
    else  
      source->set_cShock(childShock2);
    
    add_an_ishock_edge(childShock2);
  }

  //update the propagated flag
  source->setPropagated (true);

  return dbsk2d_lagrangian_ishock_detector::NEW_SHOCKS_FROM_SOURCE;
}

dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::propagate_from_a_junction(dbsk2d_ishock_node* cur_node)
{
  //determine the leftmost shock and the rightmost shock coming into this junction
  dbsk2d_ishock_edge* lshock = cur_node->pShocks().front();
  dbsk2d_ishock_edge* rshock = cur_node->pShocks().back();

  //Determine the leftmost and the rightmost boundary elements of the junction.
  dbsk2d_ishock_belm* lbelm = cur_node->bndList().front().second;
  dbsk2d_ishock_belm* rbelm = cur_node->bndList().back().second;
  dbsk2d_assert (lbelm != rbelm);

  //find the left eta and right eta
  double lseta = cur_node->bndList().front().first;
  double rseta = cur_node->bndList().back().first;

  //Now form the new shock edge
  dbsk2d_ishock_edge* newShock = propagate_from_a_node(
    cur_node, lshock, rshock, lbelm, rbelm, lseta, rseta);

  //this junction could turn out to be a sink when attempting to propagate
  //if this happens, do nothing, the sink has formed already.
  if (!newShock)
    return dbsk2d_lagrangian_ishock_detector::SINK_FORMATION;

  //Dynamic Error Recovery during Shock Propagation
  if (!newShock->isValid()) //this shock should not have formed!!!
  {
    //dbsk2d_assert(false);

    //1)  it is not yet in the shock list so we can just delete it
    delete newShock;

    //2)The junction is invalid as well. 
    //  But do not Reactivate the edges coming into it. They will be 
    //  activated by future propagation. Just delete the child node ptrs.
    for (ishock_edge_list_iter e_it = cur_node->pShocks().begin();
         e_it != cur_node->pShocks().end(); e_it++)
      (*e_it)->set_cSNode(NULL);

    //remove it from the shock graph
    _ishock_graph->remove_vertex(cur_node); 
    
    //3)junction deleted:
    //  The shock from this source was probably invalid because
    //  this junction should really be a sink but the other shock
    //  never got initialized.
    //  This is likely to happen while repropagating shocks from an
    //  earlier sim time. The proper course of action then is to 
    //  initialize a candidate source between the elements
    if ( local_shock_)
    {
        init_cand_src_between (lbelm, rbelm);
    }

    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Inv. Junct. : ";
    #endif

    return NEW_CAND_SRC;
  }

  #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Junct formed. : ";
  #endif

  //successful initialization of a new shock from this junction
  add_an_ishock_edge(newShock);
  cur_node->set_cShock (newShock);
  cur_node->setPropagated (true);

  return dbsk2d_lagrangian_ishock_detector::NEW_SHOCK_FROM_JUNCT;
}

//: force a shock to propagate to a node (for handling degeneracies) to a given tau
void dbsk2d_lagrangian_ishock_detector::force_propagate_a_shock_to_a_junction(
     dbsk2d_ishock_edge* neighbor, dbsk2d_ishock_node* cur_node, double tau, DIRECTION dir)
{
  if (dir==LEFT)
  {
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Force Prop. LN=" << neighbor->id() << " to degen. node";
    #endif

    double cur_time = neighbor->rFromRTau(tau);

    //1) If there is a neighbor on the left side of the left neighbor that is 
    //   intersecting at a greater time than the current time, delete ties with it, 
    if (neighbor->lNeighbor()){
      if (RisL(cur_time, neighbor->lNeighbor()->endTime())){
        neighbor->lNeighbor()->clear_rNeighbor();
        neighbor->clear_lNeighbor();
      }
    }

    //2) Remove any cell boundary intersections if they exist
    if (neighbor->cell_bnd())
      neighbor->cell_bnd()->delete_shock_edge(neighbor);

    //3) Update current's intrinsic parameters to the current time
    neighbor->setSimTime(cur_time);
    neighbor->setLeTau (neighbor->LTau(tau));
    neighbor->setReTau (tau);
    
    //4) if this node is already a junction, update intersection info as well
    if (cur_node->indeg()>0) {
      dbsk2d_ishock_edge* cur_shock = cur_node->pShocks().front();
      neighbor->set_rNeighbor (cur_shock);
      cur_shock->set_lNeighbor (neighbor);
    }
  }
  else if (dir==RIGHT)
  {
    
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Force Prop. RN=" << neighbor->id() << " to degen. node";
    #endif

    double cur_time = neighbor->rFromLTau(tau);

    //1) If there is a neighbor on the right side of the right neighbor that is 
    //   intersecting at a greater time than the current time, delete ties with it, 
    if (neighbor->rNeighbor()){
      if (RisL(cur_time, neighbor->rNeighbor()->endTime())){
        neighbor->rNeighbor()->clear_lNeighbor();
        neighbor->clear_rNeighbor();
      }
    }

    //2) Remove any cell boundary intersections if they exist
    if (neighbor->cell_bnd())
      neighbor->cell_bnd()->delete_shock_edge(neighbor);

    //3) Update current's intrinsic parameters to the current time
    neighbor->setSimTime(cur_time);
    neighbor->setLeTau (tau);
    neighbor->setReTau (neighbor->RTau(tau));
    
    //4) if this node is already a junction, update intersection info as well
    if (cur_node->indeg()>0) {
      dbsk2d_ishock_edge* cur_shock = cur_node->pShocks().back();
      neighbor->set_lNeighbor (cur_shock);
      cur_shock->set_rNeighbor (neighbor);
    }
  }
}

//: force a shock to propagate to a node (for handling degeneracies)
void dbsk2d_lagrangian_ishock_detector::force_propagate_a_shock_to_a_junction(
     dbsk2d_ishock_edge* neighbor, dbsk2d_ishock_node* cur_node, DIRECTION dir)
{
  //the time to update the shock to
  double cur_time = cur_node->simTime();

  if (dir==LEFT)
  {
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Force Prop. LN=" << neighbor->id() << " to degen. node";
    #endif

    //1) If there is a neighbor on the left side of the left neighbor that is 
    //   intersecting at a greater time than the current time, delete ties with it, 
    if (neighbor->lNeighbor()){
      if (RisL(cur_time, neighbor->lNeighbor()->endTime())){
        neighbor->lNeighbor()->clear_rNeighbor();
        neighbor->clear_lNeighbor();
      }
    }

    //2) Remove any cell boundary intersections if they exist
    if (neighbor->cell_bnd())
      neighbor->cell_bnd()->delete_shock_edge(neighbor);

    //3) Update current's intrinsic parameters to the current time
    neighbor->setSimTime(cur_time);
    neighbor->setLeTau (neighbor->getLTauFromTime(cur_time));
    neighbor->setReTau (neighbor->getRTauFromTime(cur_time));
    
    //4) if this node is already a junction, update intersection info as well
    if (cur_node->indeg()>0) {
      dbsk2d_ishock_edge* cur_shock = cur_node->pShocks().front();
      neighbor->set_rNeighbor (cur_shock);
      cur_shock->set_lNeighbor (neighbor);
    }
  }
  else if (dir==RIGHT)
  {
    
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Force Prop. RN=" << neighbor->id() << " to degen. node";
    #endif

    //1) If there is a neighbor on the right side of the right neighbor that is 
    //   intersecting at a greater time than the current time, delete ties with it, 
    if (neighbor->rNeighbor()){
      if (RisL(cur_time, neighbor->rNeighbor()->endTime())){
        neighbor->rNeighbor()->clear_lNeighbor();
        neighbor->clear_rNeighbor();
      }
    }

    //2) Remove any cell boundary intersections if they exist
    if (neighbor->cell_bnd())
      neighbor->cell_bnd()->delete_shock_edge(neighbor);

    //3) Update current's intrinsic parameters to the current time
    neighbor->setSimTime(cur_time);
    neighbor->setLeTau (neighbor->getLTauFromTime(cur_time));
    neighbor->setReTau (neighbor->getRTauFromTime(cur_time));
    
    //4) if this node is already a junction, update intersection info as well
    if (cur_node->indeg()>0) {
      dbsk2d_ishock_edge* cur_shock = cur_node->pShocks().back();
      neighbor->set_lNeighbor (cur_shock);
      cur_shock->set_rNeighbor (neighbor);
    }
  }
}

dbsk2d_ishock_edge* 
dbsk2d_lagrangian_ishock_detector::propagate_from_a_node(dbsk2d_ishock_node* cur_node,                                                         
                                                         dbsk2d_ishock_edge* lshock, dbsk2d_ishock_edge* rshock,
                                                         dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                         double lEta, double rEta)
{
  double cur_time = cur_node->simTime(); //the current simtime

  //---------------------------------
  //  LEFT SIDE
  //---------------------------------
  
  dbsk2d_ishock_edge* lNshock=0;
  dbsk2d_ishock_intersection_data leftI;

  if (lshock && lshock->lNeighbor())  //if already intersecting with another shock
  {
    lNshock = lshock->lNeighbor();
              
    //terminate the neighbor at this node
    //a) deactivate the neighbor
    deactivate_an_ishock_elm(lNshock);
    lNshock->setPropagated(true);
  }
  else {
    //get the left neighboring shock from the wavefront data structure
    lNshock = lbelm->get_left_neighboring_shock_at(lEta);

    if (lNshock){
      //now compute the location of the degenerate intersection of the current node with the left neighboring shock
      leftI = compute_degenerate_intersection( cur_node, lbelm, rbelm, lEta, rEta, lNshock, LEFT);
      
      //determine if it satisfies the wavefront and radius degeneracy conditions
      if ( leftI.R != ISHOCK_DIST_HUGE && //if this degerate transition is possible
           _isEq(leftI.LSRtau, lEta, D_EPSILON) && 
           _isEq(leftI.RSRtau, rEta, D_EPSILON) &&
           RisLEq (leftI.R, cur_time)
         )
      {
        //terminate the neighbor at this node
        //a) deactivate this shock
        deactivate_an_ishock_elm(lNshock);
        lNshock->setPropagated(true);

        //b) force propagate it to form a junction with the current node
        force_propagate_a_shock_to_a_junction(lNshock, cur_node, LEFT);
      }
      else
        lNshock = 0; //No intersection possible
    }
  }

  //---------------------------------
  //  RIGHT SIDE
  //---------------------------------
  
  dbsk2d_ishock_edge* rNshock=0;
  dbsk2d_ishock_intersection_data rightI;

  if (rshock && rshock->rNeighbor())  //if already intersecting with another shock
  {
    rNshock = rshock->rNeighbor();
              
    //terminate the neighbor at this node
    //a) deactivate the neighbor
    deactivate_an_ishock_elm(rNshock);
    rNshock->setPropagated(true);
  }
  else {
    //get the right neighboring shock from the wavefront data structure
    rNshock = rbelm->get_right_neighboring_shock_at(rEta);

    if (rNshock){
      //now compute the location of the degenerate intersection of the current node with the right neighboring shock
      rightI = compute_degenerate_intersection( cur_node, lbelm, rbelm, lEta, rEta, rNshock, RIGHT);
      
      //determine if it satisfies the wavefront and radius degeneracy conditions
      if ( rightI.R != ISHOCK_DIST_HUGE && //if this degerate transition is possible
           _isEq(rightI.LSLtau, lEta, D_EPSILON) && 
           _isEq(rightI.RSLtau, rEta, D_EPSILON) &&
           RisLEq (rightI.R, cur_time)
         )
      {
        //terminate the neighbor at this node
        //a) deactivate this shock
        deactivate_an_ishock_elm(rNshock);
        rNshock->setPropagated(true);

        //b) force propagate it to form a junction with the current node
        force_propagate_a_shock_to_a_junction(rNshock, cur_node, RIGHT);
      }
      else
        rNshock = 0; //No intersection possible
    }
  }

  //Now that we have determined whether degenerate intersections are possible
  //update this junction with the new intersections

  if (lNshock) //if the neighbor is degenerately intersecting, link it with the node
  {
    // update the shock topology of the current node
    lNshock->set_cSNode(cur_node);
    cur_node->pShocks().push_front(lNshock);

    // update the wavefront info(bnd_list) at this node
    lbelm = lNshock->lBElement(); //the new wavefront getting to this junction
    lEta = lNshock->LeEta();
    cur_node->bndList().push_front(vcl_pair<double, dbsk2d_ishock_belm*>(lEta, lbelm));

    lshock = lNshock; //update the leftmost shock at the junction
  }

  if (rNshock) //if the neighbor is degenerately intersecting, link it with the node
  {
    // update the shock topology of the current node
    rNshock->set_cSNode(cur_node);
    cur_node->pShocks().push_back(rNshock);

    // update the wavefront info(bnd_list) at this node
    rbelm = rNshock->rBElement(); //the new wavefront getting to this junction
    rEta = rNshock->ReEta();
    cur_node->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(rEta, rbelm));

    rshock = rNshock; //update the rightmost shock at the junction
  }

  //if (leftI.R != ISHOCK_DIST_HUGE && 
  //    lNshock && !rNshock) //update the other eta from the intersection
  //  rEta = leftI.RSRtau;

  //if (rightI.R != ISHOCK_DIST_HUGE &&
  //    !lNshock && rNshock)//update the other eta from the intersection
  //  lEta = rightI.LSLtau;

  // If this turned out to be a sink node after the transitions,
  // we don't need to do anything more
  if (lbelm==rbelm) //check for sink formation
    return 0;
  
  //2) If this turned out to be a near degenerate junction, and we already performed the 
  //   necessary shock transition, we can try to propagate from this node again by 
  //   recursing into this function. (this will allow for dealing with higher order degeneracies)
  //
  //   Sometimes, the addition of more shocks to the degenerate node causes other neighboring shocks
  //   to become degenerate to it. The recursion will handle all these scenarios
  //
  if (lNshock || rNshock){
    //fix the etas first
    fix_etas_at_a_degen_node(cur_node, lshock, rshock, lbelm, rbelm, lEta, rEta);
    //now look for more degeneracies
    return propagate_from_a_node(cur_node, lshock, rshock, lbelm, rbelm, lEta, rEta);
  }
  
  //3) Else form a child shock from this non-degenerate node
  return form_a_child_shock_from_a_node(cur_node, lbelm, rbelm, lEta, rEta);
}



////: Propagate a shock path due to a pair of wavefronts, from a given shock node 
////  EPSILONISSUE: need to detect if the current node is approximately degenerate
////  EPSILONISSUE: needs to determe whether a third order shock is to be formed
////
//// Note: 
////  A shock node can be approximately degenerate wrt the child shock it spawns, i.e., the
////  child shock of this node can be of a very small length after which it goes into another node.
////  This function therefore, in keeping with our philosophy, must swallow this child shock into the 
////  current node (effectively increasing the order of contact of the node). Then it must
////  terminate the neighboring shock at this node and form a higher order node.
////
////  Finally, this method has to determine the type of shock to propagate from the interacting
////  wavefronts. 
//dbsk2d_ishock_edge* 
//dbsk2d_lagrangian_ishock_detector::propagate_from_a_node(dbsk2d_ishock_node* cur_node,                                                         
//                                                         dbsk2d_ishock_edge* lshock, dbsk2d_ishock_edge* rshock,
//                                                         dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
//                                                         double lEta, double rEta)
//{
//
//  //1) The first order of business is to determine if this node is near degeneracy of some kind
//  //   If it is, it should transition to degeneracy by creating a hole in the wavefront
//  bool left_degenerate_node = false;
//  double cur_time = cur_node->simTime(); //the current simtime
//
//  //-----------------------------------------------------------------------------------------------
//  //LEFT SIDE
//
//  dbsk2d_ishock_edge* lNshock=0;
//  if (lshock && lshock->lNeighbor())  //if already intersecting with another shock
//  {
//    left_degenerate_node = true;
//    lNshock = lshock->lNeighbor();
//              
//    //terminate the neighbor at this node
//    //a) deactivate the neighbor
//    deactivate_an_ishock_elm(lNshock);
//    lNshock->setPropagated(true);
//  }
//  else {
//    //get the left neighboring shock from the wavefront data structure
//    lNshock = lbelm->get_left_neighboring_shock_at(lEta);
//
//    if (lNshock)
//    {
//      dbsk2d_assert(lNshock->rBElement()==lbelm);
//
//      //First determine if the neighbor is already intersecting with this node:
//      // this can happen at degenerate junctions of higher order where several near
//      // degenerate shocks have been forced into a degenerate junction
//      //
//      // check the radius of the neighbor at the current eta:
//      // if the radius is less than the cur_time, it is degenerate
//      //double lN_rtau_lEta = lNshock->REtaToRTau(lEta, UNCONSTRAINED);
//
//      ////this tau is not guaranteed to be valid so check it
//      //if (lNshock->isRTauValid_MinMax(lN_rtau_lEta))
//      //{
//      //  double lN_R_lEta = lNshock->rFromRTau(lN_rtau_lEta);
//      //  if (RisLEq(lN_R_lEta, cur_time))
//      //  {
//      //    left_degenerate_node = true;             //form a degenerate junction
//
//      //    //a) deactivate this shock
//      //    deactivate_an_ishock_elm(lNshock);
//      //    lNshock->setPropagated(true);
//
//      //    //b) force propagate it to form a junction with the current node
//      //    force_propagate_a_shock_to_a_junction(lNshock, cur_node, lN_rtau_lEta, LEFT);
//      //  }
//      //}
//
//      //Next determine if this neighbor is approximately degenerate:
//      //  compute the eta of the neighbor at the current radius 
//      //  and compare it to the eta of the current node
//      double lNreTau = lNshock->getRTauFromTime(cur_time);
//
//      //this tau is not guaranteed to be valid so check it
//      if (lNshock->isRTauValid_MinMax(lNreTau))
//      {
//        double lNeEta = lNshock->RTauToREta(lNreTau);
//        if (_isEq(lEta, lNeEta, D_EPSILON))   //if approximately intersecting
//        {  
//          left_degenerate_node = true;             //form the degenerate junction
//
//          //a) deactivate this shock
//          deactivate_an_ishock_elm(lNshock);
//          lNshock->setPropagated(true);
//
//          //b) force propagate it to form a junction with the current node
//          force_propagate_a_shock_to_a_junction(lNshock, cur_node, LEFT);
//        }
//      }
//    
//      if (!left_degenerate_node)
//        lNshock = 0;
//    }
//  }
//
//  if (lNshock) //if the neighbor is degenerately intersecting, link it with the node
//  {
//    // update the shock topology of the current node
//    lNshock->set_cSNode(cur_node);
//    cur_node->pShocks().push_front(lNshock);
//
//    // update the wavefront info(bnd_list) at this node
//    lbelm = lNshock->lBElement(); //the new wavefront getting to this junction
//    lEta = lNshock->LeEta();
//    cur_node->bndList().push_front(vcl_pair<double, dbsk2d_ishock_belm*>(lEta, lbelm));
//  }
//  else
//    lNshock = lshock; //the current leftmost shock is still the leftmost shock
//
//  // If this turns out to be a sink node after the transitions,
//  // we don't need to do anything more
//  if (lbelm==rbelm) //check for sink formation
//    return 0;
//
//  //-----------------------------------------------------------------------------------------------
//  //RIGHT SIDE
//
//  bool right_degenerate_node = false;
//  dbsk2d_ishock_edge* rNshock=0;
//  if (rshock && rshock->rNeighbor())  //if already intersecting with another shock
//  {
//    right_degenerate_node = true;
//    rNshock = rshock->rNeighbor();
//              
//    //terminate the neighbor at this node
//    //a) deactivate the neighbor
//    deactivate_an_ishock_elm(rNshock);
//    rNshock->setPropagated(true);
//  }
//  else {
//
//    //get the right neighboring shock from the wavefront data structure
//    rNshock = rbelm->get_right_neighboring_shock_at(rEta);
//
//    if (rNshock)
//    {
//      dbsk2d_assert(rNshock->lBElement()==rbelm);
//
//      //First determine if the neighbor is already intersecting with this node:
//      // this can happen at degenerate junctions of higher order where several near
//      // degenerate shocks have been forced into a degenerate junction
//      //
//      // check the radius of the neighbor at the current eta:
//      // if the radius is less than the cur_time, it is degenerate
//      //double rN_ltau_rEta = rNshock->LEtaToLTau(rEta, UNCONSTRAINED);
//
//      ////this tau is not guaranteed to be valid so check it
//      //if (rNshock->isLTauValid_MinMax(rN_ltau_rEta))
//      //{
//      //  double rN_R_rEta = rNshock->rFromLTau(rN_ltau_rEta);
//      //  if (RisLEq(rN_R_rEta, cur_time))
//      //  {
//      //    right_degenerate_node = true;             //form a degenerate junction
//
//      //    //a) deactivate this shock
//      //    deactivate_an_ishock_elm(rNshock);
//      //    rNshock->setPropagated(true);
//
//      //    //b) force propagate it to form a junction with the current node
//      //    force_propagate_a_shock_to_a_junction(rNshock, cur_node, rN_ltau_rEta, RIGHT);
//      //  }
//      //}
//
//      //Next determine if this neighbor is approximately degenerate:
//      //  compute the eta of the neighbor at the current radius 
//      //  and compare it to the eta of the current node
//      double rNleTau = rNshock->getLTauFromTime(cur_time);
//      //this tau is not guaranteed to be valid so check it
//      if (rNshock->isLTauValid_MinMax(rNleTau))
//      {
//        double rNeEta = rNshock->LTauToLEta(rNleTau);
//
//        if (_isEq(rEta, rNeEta, D_EPSILON))   //if approximately intersecting
//        {  
//          right_degenerate_node = true;       //form a degenerate junction
//
//          //a) deactivate this shock
//          deactivate_an_ishock_elm(rNshock);
//          rNshock->setPropagated(true);
//
//          //b) force propagate it to form a junction with the current node
//          force_propagate_a_shock_to_a_junction(rNshock, cur_node, RIGHT);
//        }
//      }
//      
//      if (!right_degenerate_node)
//        rNshock = 0;
//    }
//  }
//
//  if (rNshock) //if the neighbor is degenerately intersecting, link it with the node
//  {
//    // update the shock topology of the current node
//    rNshock->set_cSNode(cur_node);
//    cur_node->pShocks().push_back(rNshock);
//
//    // update the wavefront info(bnd_list) at this node
//    rbelm = rNshock->rBElement(); //the new wavefront getting to this junction
//    rEta = rNshock->ReEta();
//    cur_node->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(rEta, rbelm));
//  }
//  else
//    rNshock = rshock; //the current righmost shock is still the rightmost shock
//
//  //-----------------------------------------------------------------------------------------------
//
//  // If this turned out to be a sink node after the transitions,
//  // we don't need to do anything more
//  if (lbelm==rbelm) //check for sink formation
//    return 0;
//  
//  //2) If this turned out to be a near degenerate junction, and we already performed the 
//  //   necessary shock transition, we can try to propagate from this node again by 
//  //   recursing into this function. (this will allow for dealing with higher order degeneracies)
//  //
//  //   Sometimes, the addition of more shocks to the degenerate node causes other neighboring shocks
//  //   to become degenerate to it. The recursion will handle all these scenarios
//  //
//  if (left_degenerate_node || right_degenerate_node){
//    //fix the etas first
//    fix_etas_at_a_degen_node(cur_node, lNshock, rNshock, lbelm, rbelm, lEta, rEta);
//    //now look for more degeneracies
//    return propagate_from_a_node(cur_node, lNshock, rNshock, lbelm, rbelm, lEta, rEta);
//  }
//  
//  return form_a_child_shock_from_a_node(cur_node, lbelm, rbelm, lEta, rEta);
//}

//: fix etas at a degenerate node
void 
dbsk2d_lagrangian_ishock_detector::fix_etas_at_a_degen_node(dbsk2d_ishock_node* cur_node, 
                                                            dbsk2d_ishock_edge* lshock, dbsk2d_ishock_edge* rshock,
                                                            dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                            double &lsEta, double &rsEta)
{
  // Note:
  // 
  // This method is responsible for fixing etas if the shock is starting
  // from a degenerate node.
  //
  // The main idea is to form a child shock from this degen node and use its constructor to fix the etas
  // but we don't propagate these shocks, we kill them and swallow them into the degenerate junction

  dbsk2d_ishock_edge* degen_edge = form_a_child_shock_from_a_node(cur_node, lbelm, rbelm, lsEta, rsEta, CONSTRAINED, true);//temp shock

  //make sure that the shock is valid, if it is not valid, the etas will not be valid either
  if (degen_edge->isValid()){
    //get the starting etas from this edge
    lsEta = degen_edge->LsEta();
    rsEta = degen_edge->RsEta();

    //compute the radius at the starting point and set it as the node's start time
    //double cur_time = degen_edge->rFromLTau(degen_edge->LsTau());
    //cur_node->setSimTime(cur_time);
  }

  //then delete this shock edge
  delete degen_edge;
}

//: Look at the possibility of an approximate degenerate junction arising at a node (due to degeneracy transitions)
dbsk2d_ishock_intersection_data 
dbsk2d_lagrangian_ishock_detector::compute_degenerate_intersection(dbsk2d_ishock_node* cur_node,
                                                                   dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                                   double lsEta, double rsEta, 
                                                                   dbsk2d_ishock_edge* Nshock,
                                                                   DIRECTION dir)
{
  //Note:
  //
  // The goal is to compute an approximate intersection with the neighbor
  //
  // To do this we need to determine the extent of the neighbor at the current time 
  // This will give us an idea of the wavefront quench point for the entire junction.
  // Unfortunately, this only gives us the information on one side of the intersection. 
  // In order to determine the wavefront quench point on the other side we have to instantiate a shock
  // and refer to its parameter at the starting point

  dbsk2d_ishock_intersection_data intersection;
  double cur_time = cur_node->simTime();

  if (dir==LEFT)
  {
    double lNreTau = Nshock->getRTauFromTime(cur_time);

    //this tau is not guaranteed to be valid so check it
    if (!Nshock->isRTauValid_MinMax(lNreTau))
      return intersection; //intersection not possible

    double lNreEta = Nshock->RTauToREta(lNreTau);
    double lNleEta = Nshock->LTauToLEta(Nshock->LTau(lNreTau));

    dbsk2d_ishock_edge* child_edge = form_a_child_shock_from_a_node(cur_node, Nshock->lBElement(), rbelm, lNleEta, rsEta, CONSTRAINED, true); //temp shock

    //make sure that the shock is valid
    if (!child_edge->isValid()){
      delete child_edge;
      return intersection; //intersection is not valid
    }

    //now get the intersection information from the starting etas of the child edge
    //Note: these parameters are actually etas (not taus)
    intersection.LSLtau = child_edge->LsEta();
    intersection.LSRtau = lNreEta;
    intersection.RSLtau = rsEta;
    intersection.RSRtau = child_edge->RsEta();
    intersection.R = child_edge->rFromLTau(child_edge->LsTau());

    //now that we have the intersection information, we can delete the shock
    delete child_edge;
  }
  else  //dir==RIGHT
  {
    double rNleTau = Nshock->getLTauFromTime(cur_time);

    //this tau is not guaranteed to be valid so check it
    if (!Nshock->isLTauValid_MinMax(rNleTau))
      return intersection; //intersection not possible

    double rNleEta = Nshock->LTauToLEta(rNleTau);
    double rNreEta = Nshock->RTauToREta(Nshock->RTau(rNleTau));

    dbsk2d_ishock_edge* child_edge = form_a_child_shock_from_a_node(cur_node, lbelm, Nshock->rBElement(), lsEta, rNreEta, CONSTRAINED);

    //make sure that the shock is valid
    if (!child_edge->isValid()){
      delete child_edge;
      return intersection; //intersection is not valid
    }

    //now get the intersection information from the starting etas of the child edge
    //Note: these parameters are actually etas (not taus)
    intersection.LSLtau = child_edge->LsEta();
    intersection.LSRtau = lsEta;
    intersection.RSLtau = rNleEta;
    intersection.RSRtau = child_edge->RsEta();
    intersection.R = child_edge->rFromLTau(child_edge->LsTau());

    //now that we have the intersection information, we can delete the shock
    delete child_edge;
  }

  return intersection;
}

//: form a child shock from a node
dbsk2d_ishock_edge* 
dbsk2d_lagrangian_ishock_detector::form_a_child_shock_from_a_node(dbsk2d_ishock_node* pNode, 
                                                                  dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                                  double lEta, double rEta,
                                                                  bool constrained,
                                                                  bool temp_shock) //temp shocks for degeneracy computation
{
  //propagate a shock branch from this node from the given etas
  dbsk2d_ishock_edge* newShock = 0;
  double cur_time = pNode->simTime(); //the current simtime

  int new_id = -1; //temp_id 
  if (!temp_shock) //get a real id
    new_id = ishock_graph()->nextAvailableID();

  switch (lbelm->type()) 
  {
  case BPOINT:
    switch (rbelm->type()) 
    {
      case BPOINT:
        newShock = new dbsk2d_ishock_pointpoint (
          new_id, cur_time,
          pNode, lbelm, rbelm, 
          lEta, rEta, constrained);
        break;
      case BLINE:
        newShock =  new dbsk2d_ishock_pointline (
          new_id, cur_time,
          pNode, lbelm, rbelm, 
          lEta, rEta, constrained);
        break;
      case BARC:
        //EPSILONISSUE: whether two points are close enough to create a thirdorder shock
        if (_BisEqPoint (((dbsk2d_ishock_bpoint*)lbelm)->pt(), ((dbsk2d_ishock_barc*)rbelm)->center()))
          newShock =  new dbsk2d_ishock_pointarc_thirdorder (
            new_id, cur_time,
            pNode, lbelm, rbelm, 
            lEta, rEta);
        else
          newShock =  new dbsk2d_ishock_pointarc (
            new_id, cur_time,
            pNode, lbelm, rbelm, 
            lEta, rEta);
        break;
    }
    break;
  case BLINE:
    switch (rbelm->type()) 
    {
      case BPOINT:
        newShock =  new dbsk2d_ishock_pointline (
          ishock_graph()->nextAvailableID(), cur_time,
          pNode, lbelm, rbelm,  
          lEta, rEta, constrained);
        break;
      case BLINE: {
        dbsk2d_ishock_bline* lbline = (dbsk2d_ishock_bline*) lbelm;
        dbsk2d_ishock_bline* rbline = (dbsk2d_ishock_bline*) rbelm;

        double theta = CCW(lbline->u(), rbline->u());
        ////EPSILONISSUE: whether two lines are parallel to create a thirdorder shock
        //if (_isEq(theta,vnl_math::pi, TO_EPSILON))
        //{
        //  //ThirdOrder
        //  newShock = new dbsk2d_ishock_lineline_thirdorder (
        //    new_id, cur_time,
        //    pNode, lbelm, rbelm,
        //    lEta, rEta);
        //}
        //else {
          newShock = new dbsk2d_ishock_lineline (
            new_id, cur_time,
            pNode, lbelm, rbelm, 
            lEta, rEta, constrained);
        //}
        break; }
      case BARC:
        newShock = new dbsk2d_ishock_linearc (
          new_id, cur_time,
          pNode, lbelm, rbelm, 
          lEta, rEta);
        break;
    }
    break;
  case BARC:
    switch (rbelm->type()) 
    {
      case BPOINT:
        //EPSILONISSUE: whether two points are close enough to create a thirdorder shock
        if (_BisEqPoint (((dbsk2d_ishock_barc*)lbelm)->center(), ((dbsk2d_ishock_bpoint*)rbelm)->pt()))
          newShock = new dbsk2d_ishock_pointarc_thirdorder (
          new_id, cur_time, 
          pNode, lbelm, rbelm, 
          lEta, rEta);
        else
          newShock = new dbsk2d_ishock_pointarc (
          new_id, cur_time,
          pNode, lbelm, rbelm, 
          lEta, rEta);
        break; 
      case BLINE:
        newShock = new dbsk2d_ishock_linearc (
          new_id, cur_time,
          pNode, lbelm, rbelm, 
          lEta, rEta);
        break;
      case BARC:
        //EPSILONISSUE: whether two points are close enough to create a thirdorder shock
        if (_BisEqPoint (((dbsk2d_ishock_barc*)lbelm)->center(), ((dbsk2d_ishock_barc*)rbelm)->center()))
          newShock = new dbsk2d_ishock_arcarc_thirdorder (
            new_id, cur_time,
            pNode, lbelm, rbelm, 
            lEta, rEta);
        else
          newShock = new dbsk2d_ishock_arcarc (
            new_id, cur_time,
            pNode, lbelm, rbelm, 
            lEta, rEta);
        break;
    }
    break;
  } //end switch

  //5) Compute intersections of this shock branch with the boundaries of this 
  //   lagrangian cell immediately to determine the upper propagation limit. 

  if (temp_shock) //if temp do not interact with the boundaries
    return newShock;

  if (!newShock->isValid()) //if it is not valid there is no point
    return newShock;
  else
    intersect_with_cell_boundaries(newShock); //intersect with all four boundaries

  return newShock;
}

//: If the current shockwave is at a junction, return JUNCTION_TYPE
//  else return NO_JUNCTION
dbsk2d_lagrangian_ishock_detector::JUNCTION_TYPE 
dbsk2d_lagrangian_ishock_detector::get_junction_type (dbsk2d_ishock_edge* sedge)
{
  if (sedge->lNeighbor()==NULL && sedge->rNeighbor()==NULL)
    return NO_JUNCTION;

  if (sedge->rNeighbor() == sedge->lNeighbor())
    return SINK_JUNCTION;

  dbsk2d_ishock_edge *rshock = sedge;
  while (rshock->rNeighbor() !=NULL && rshock->rNeighbor() != sedge)
    rshock = rshock->rNeighbor();

  if (rshock->rNeighbor() == sedge)
    return SINK_JUNCTION;

  dbsk2d_ishock_edge *lshock = sedge;
  while (lshock->lNeighbor() !=NULL && lshock->lNeighbor() != sedge)
    lshock = lshock->lNeighbor();

  if (lshock->lNeighbor() == sedge)
    return SINK_JUNCTION;

  //!!!!! Special case of SINK: shouldn't happen,
  //But it does happen when numerical epsilon of R is introduced.
  //( see sink-bug1.bnd)
  if (lshock->lBElement() == rshock->rBElement()) {
  //vcl_cout<< vcl_endl<<"WARNING: Numerical Issue--Special case of SINK." <<vcl_endl;
    lshock->set_lNeighbor (rshock);
    rshock->set_rNeighbor (lshock);
    return SINK_JUNCTION;
  }

  if (rshock != sedge && lshock != sedge)
    return DEGENERATE_JUNCTION;

  if (rshock != sedge && rshock != sedge->rNeighbor())
    return DEGENERATE_JUNCTION;

  if (lshock != sedge && lshock != sedge->lNeighbor())
    return DEGENERATE_JUNCTION;

  if (lshock == sedge && rshock == sedge->rNeighbor())
    return RIGHT_REGULAR_JUNCTION;

  if (rshock == sedge && lshock == sedge->lNeighbor()) 
    return LEFT_REGULAR_JUNCTION;

  return NO_JUNCTION;
}

//: propagate the current shock to the nearest junction.
//  This is achieved by computing intersections with the left and
//  right neighbors and propagating to the one with the smaller radius.
//  If the radius is equal for both intersections, it is to form a degenerate
//  junction.
//
// Note:
//  As per the new philosophy of handling shock transitions during computation,
//  I have changed this function to be based on the wavefront parameters (eta)
//  as well as time -- if a shock advances the wavefront less than
//  D_EPSILON, it is to be transitioned into the maximal degenerate config.
//
dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::
propagate_shock_to_the_nearest_junction (dbsk2d_ishock_edge* current)
{
  dbsk2d_ishock_edge *candidate=0;
  dbsk2d_ishock_edge *l_neighbor=0, *r_neighbor=0;
  dbsk2d_ishock_intersection_data leftI, rightI;
  double lILeta=0, lIReta, rILeta, rIReta=0;

  //------------------------------------------------------------------
  //LEFT SIDE
  candidate = get_neighboring_shock(current, LEFT);

  //intersect with the left neighboring shock (if available)
  leftI = dbsk2d_ishock_compute_intersection (candidate, current);
  
  if (leftI.R != ISHOCK_DIST_HUGE) //if this intersection is possible
  {
    //find the intersection point (in terms of eta) (i.e., wavefront quench point)
    lILeta = current->LTauToLEta(leftI.RSLtau, false);

    //determine if this intersection is legal
    if (_isLEq(lILeta, candidate->ReEta(), D_EPSILON) && 
        _isGEq(lILeta, current->LeEta(), D_EPSILON) &&
        RisLEq (leftI.R, candidate->endTime()) &&
        RisLEq (leftI.R, current->endTime()))
      l_neighbor = candidate;
  }

  //------------------------------------------------------------------
  //RIGHT SIDE
  candidate = get_neighboring_shock(current, RIGHT);

  //intersect with the right neighboring shock (if available)
  rightI = dbsk2d_ishock_compute_intersection (current, candidate);

  if (rightI.R != ISHOCK_DIST_HUGE) //if this intersection is possible
  {
    //find the intersection point (in terms of eta) (i.e., wavefront quench point)
    rIReta = current->RTauToREta(rightI.LSRtau, false);

    //determine if this intersection is legal
    if (_isGEq(rIReta, candidate->LeEta(), D_EPSILON) && 
        _isLEq(rIReta, current->ReEta(), D_EPSILON) &&
        RisLEq (rightI.R, candidate->endTime()) &&
        RisLEq (rightI.R, current->endTime()))
      r_neighbor = candidate;
  }

  //------------------------------------------------------------------

  //mark the current shock as having propagated
  current->setPropagated (true);

  //========= Decide on the best intersection point to propagate to =========
  
  if (l_neighbor && r_neighbor) //both intersections legal
  {
    //this could be a simple or a degenerate intersection
    lIReta = current->RTauToREta(leftI.RSRtau, false);
    rILeta = current->LTauToLEta(rightI.LSLtau, false);

    //using the etas alone (for near colinear cases) or time alone (for near parallel cases) 
    //is not sufficient to make this decision
    if (_isEq(lILeta, rILeta, D_EPSILON) && //compare the eta parameters 
        _isEq(lIReta, rIReta, D_EPSILON) && //compare the other side eta (redundant but safe)
        RisEq(leftI.R, rightI.R))           //also compare the intersection times (another layer of redundancy)
    {
      //advocate a degenerate junction (could be a sink)

      //since both the following methods update the current shock's endtime, make sure
      //the smaller time is the one that is registered (even though they are epsilon equal)
      if (leftI.R < rightI.R){
        propagate_shock_to_a_right_junction (current, r_neighbor, rightI);
        propagate_shock_to_a_left_junction (l_neighbor, current, leftI);
      }
      else {
        propagate_shock_to_a_left_junction (l_neighbor, current, leftI);
        propagate_shock_to_a_right_junction (current, r_neighbor, rightI);
      }
      return dbsk2d_lagrangian_ishock_detector::BOTH_INTERSECTION;
    }
    //Since not all the conditions for a degenerate junction were met,
    //treat near parallel intersections differently from near collinear intersections
    else if (RisEq(leftI.R, rightI.R))
    {
      if (_isG(lILeta, rILeta, D_EPSILON)){ //only left intersection legal
        propagate_shock_to_a_left_junction (l_neighbor, current, leftI);
        return dbsk2d_lagrangian_ishock_detector::LEFT_INTERSECTION;
      }
      else {                                //only right intersection legal
        propagate_shock_to_a_right_junction (current, r_neighbor, rightI);
        return dbsk2d_lagrangian_ishock_detector::RIGHT_INTERSECTION;
      }
    }
    //if its not a near parallel situation, just form a regular junction on the basis 
    //of intersection time. If this intersection is indeed degenerate, 
    //it will be caught while propagating from the junction formed.
    else if(leftI.R < rightI.R)
    {  
      //left intersection happens earlier
      propagate_shock_to_a_left_junction (l_neighbor, current, leftI);
      return dbsk2d_lagrangian_ishock_detector::LEFT_INTERSECTION;
    }
    else {
      //right intersection happens earlier
      propagate_shock_to_a_right_junction (current, r_neighbor, rightI);
      return dbsk2d_lagrangian_ishock_detector::RIGHT_INTERSECTION;
    }
  }
  else if (l_neighbor) { //only left intersection legal
    propagate_shock_to_a_left_junction (l_neighbor, current, leftI);
    return dbsk2d_lagrangian_ishock_detector::LEFT_INTERSECTION;
  }
  else if (r_neighbor) { //only right intersection legal
    propagate_shock_to_a_right_junction (current, r_neighbor, rightI);
    return dbsk2d_lagrangian_ishock_detector::RIGHT_INTERSECTION;
  }
  else { //no intersection possible at this time!
    //since we have lagrangian cell boundaries, 
    //just propagate them to the bnd and deactivate them
    deactivate_an_ishock_elm(current);
    current->setSimTime (current->endTime());

    if (current->cell_bnd())
      return dbsk2d_lagrangian_ishock_detector::PROPAGATION_TO_BND;
    else
      return dbsk2d_lagrangian_ishock_detector::NO_PROPAGATION;
  }
}

void dbsk2d_lagrangian_ishock_detector::
propagate_shock_to_infinity (dbsk2d_ishock_edge* elm)
{
  update_an_ishock_elms_simtime(elm, ISHOCK_DIST_HUGE);
}

//when we get to this function, it means that at the current time
//it's guarrenteed that the current and left neighbor are gonna intersect.
void dbsk2d_lagrangian_ishock_detector::
propagate_shock_to_a_left_junction (dbsk2d_ishock_edge* l_neighbor, 
                                    dbsk2d_ishock_edge* current, 
                                    dbsk2d_ishock_intersection_data intersection)
{
  //1) If this shock already had a neighbor on the left side, we need to delete ties
  //   with it because the current intersection is happening at an earlier time. The
  //   wavefront must have been broken up by a new source thatn just formed.
  if (current->lNeighbor() && current->lNeighbor()!= l_neighbor){
    current->lNeighbor()->clear_rNeighbor();
    current->clear_lNeighbor();
  }

  //2) If there is a neighbor on the right side that is intersecting at a greater
  //   time than the current intersection, delete ties with it, because it's now 
  //   going to intersect with a different shock at some point in the future.
  if (current->rNeighbor()){
    if (RisL(intersection.R, current->rNeighbor()->endTime())){
      current->rNeighbor()->clear_lNeighbor();
      current->clear_rNeighbor();
    }
  }

  //3) Remove any cell boundary intersections if they exist
  if (current->cell_bnd())
    current->cell_bnd()->delete_shock_edge(current);

  //4) If the left neighbor has any children shocks
  //   we need to remove them too
  if (l_neighbor->cSNode())
    delete_an_ishock_elm(l_neighbor->cSNode());

  //5) Update current's intrinsic parameters to this intersection
  current->set_lNeighbor (l_neighbor);
  update_intrinsic_parameters_of_a_shock (current, intersection.R, intersection.RSLtau, intersection.RSRtau);

  //6) Update left neighbor but careful not to set it into an infinite loop
  if (l_neighbor->rNeighbor() != current)
    propagate_shock_to_a_right_junction (l_neighbor, current, intersection);
}

void dbsk2d_lagrangian_ishock_detector::
propagate_shock_to_a_right_junction (dbsk2d_ishock_edge* current, 
                                     dbsk2d_ishock_edge* r_neighbor, 
                                     dbsk2d_ishock_intersection_data intersection)
{
  //1) If this shock already had a neighbor on the right side, we need to delete ties
  //   with it because the current intersection is happening at an earlier time. The
  //   wavefront must have been broken up by a new source thatn just formed.
  if (current->rNeighbor() && current->rNeighbor()!= r_neighbor){
    current->rNeighbor()->clear_lNeighbor();
    current->clear_rNeighbor();
  }

  //2) If there is a neighbor on the left side that is intersecting at a greater
  //   time than the current intersection, delete ties with it, because it's now 
  //   going to intersect with a different shock at some point in the future.
  if (current->lNeighbor()){
    if (RisL(intersection.R, current->lNeighbor()->endTime())){
      current->lNeighbor()->clear_rNeighbor();
      current->clear_lNeighbor();
    }
  }

  //3) Remove any cell boundary intersections if they exist
  if (current->cell_bnd())
    current->cell_bnd()->delete_shock_edge(current);

  //4) If the right neighbor has any children shocks
  //   we need to remove them too
  if (r_neighbor->cSNode())
    delete_an_ishock_elm(r_neighbor->cSNode());

  //5) Update current's intrinsic parameters to this intersection
  current->set_rNeighbor (r_neighbor);
  update_intrinsic_parameters_of_a_shock (current, intersection.R, intersection.LSLtau, intersection.LSRtau);

  //6) Update right neighbor but careful not to set it into an infinite loop
  if (r_neighbor->lNeighbor() != current)
    propagate_shock_to_a_left_junction (current, r_neighbor, intersection);
}

void dbsk2d_lagrangian_ishock_detector::
update_intrinsic_parameters_of_a_shock (dbsk2d_ishock_edge* elm, double time,
                                        double letau, double retau)
{
  //1) Update it's sim time if it has already propagated before 
  if (elm->hasPropagated())
    update_an_ishock_elms_simtime(elm, time);
  else
    elm->setEndTime(time);

  //2) Update Ltau, Rtau
  elm->setLeTau (letau);
  elm->setReTau (retau);
}

dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::form_a_sink(dbsk2d_ishock_edge* current)
{
  double sumTime=0;
  double sumCoordX=0, sumCoordY=0;
  int    n=0;

  dbsk2d_ishock_edge* cur = current;
  do 
  {
    deactivate_an_ishock_elm(cur);

    //average extrinsic coordinate and radius
    sumTime += cur->endTime();
    vgl_point_2d<double> endpt = cur->getEndPt();
    sumCoordX += endpt.x();
    sumCoordY += endpt.y();
    n++;

    cur = cur->rNeighbor();

  } while (cur != current);

  //Create a new Sink

  //1)Using current's endTime and EndPt
  //dbsk2d_ishock_node* newSink = new dbsk2d_ishock_node (nextAvailableID(), 
  //  current->endTime(), current->getEndPt());

  //2)Using average of all shock's endTime and EndPt;
  sumTime /= n;
  sumCoordX /= n;
  sumCoordY /= n;
  dbsk2d_ishock_node* newSink = new dbsk2d_ishock_node (
                                      ishock_graph()->nextAvailableID(),
                                      sumTime, 
                                      vgl_point_2d<double>(sumCoordX, sumCoordY));

  //Update the bnd_list and the _pShockList for this Sink
  //by traversing around the junction one more time
  cur = current;
  do 
  {
    newSink->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm* >(cur->LeEta(), cur->lBElement()));
    newSink->pShocks().push_back(cur);
    cur->set_cSNode(newSink);
    cur = cur->rNeighbor();
  } while (cur != current);

  //add it to the shock list
  add_an_ishock_node (newSink);

  return dbsk2d_lagrangian_ishock_detector::SINK_FORMATION;
}

dbsk2d_lagrangian_ishock_detector::PROPAGATION_TYPE 
dbsk2d_lagrangian_ishock_detector::form_a_junction(dbsk2d_ishock_edge* current)
{
  //1)Get lmostshock and rmostshock of the junction
  dbsk2d_ishock_edge *rshock = current;
  while (rshock->rNeighbor() != NULL)
    rshock = rshock->rNeighbor();
  
  //rshock is now the RIGHTMOST shock in the junction

  dbsk2d_ishock_edge *lshock = current;
  while (lshock->lNeighbor() != NULL)
    lshock = lshock->lNeighbor();
  
  //lshock is now the LEFTMOST shock in the junction

  //2) check for validity
  //Ming: degenerate cases!
  //if one of the intersected shock is not propagated,
  //we should propagate it next time!
  //The junction will form later!
  dbsk2d_ishock_edge* cur_shk = lshock;
  while (cur_shk) {
    if (!cur_shk->hasPropagated()) {
      _nextShockToPropagate = cur_shk;
      return dbsk2d_lagrangian_ishock_detector::NO_PROPAGATION;
    }
    cur_shk = cur_shk->rNeighbor();
  }

  //3)Initialize a new Junction
  dbsk2d_ishock_node* newJunction = new dbsk2d_ishock_node (
                                          ishock_graph()->nextAvailableID(),
                                          current->endTime(), current->getEndPt());

  //4)Update the belm_list and the _pShockList for this Junction
  //  By traversing from the leftmost to the rightmost shock
  dbsk2d_ishock_edge* cur = lshock;
  while (cur) {
    newJunction->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(cur->LeEta(), cur->lBElement()));
    newJunction->pShocks().push_back(cur);

    deactivate_an_ishock_elm(cur);
    cur->set_cSNode(newJunction);

    cur = cur->rNeighbor();
  }
  //rightmost boundary element
  newJunction->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(rshock->ReEta(), rshock->rBElement()));

  //add this junction on to the shock list
  add_an_ishock_node (newJunction);

  //Instatiate the child shock from this junction
  return propagate_from_a_junction(newJunction);
} 

//-------------------------------------------------------------------
// intersection functions
//-------------------------------------------------------------------

//: get the neighboring shock to intersect with on the side specified
//  Basically, return the shock sharing the same wavefront 
//
//  Delete any invalid neighbors (this is equivalent to pruning a 
//  branch on the wavefront tree when new shocks invalidate existing ones)
dbsk2d_ishock_edge* dbsk2d_lagrangian_ishock_detector::
get_neighboring_shock(dbsk2d_ishock_edge* current, DIRECTION dir)
{
  dbsk2d_ishock_edge* candidate = 0;
  dbsk2d_ishock_belm* common_belm=0;
  bool neighbor_found = false;

  if (dir==LEFT)
    common_belm = current->lBElement();
  else
    common_belm = current->rBElement();

  while (!neighbor_found)
  {
    if (dir==LEFT)
    {
      int id=current->lShock_id();
      if ( id < 0 || deleted_ishock_elements_.count(id))
      {
          candidate=0;
      }
      else
      {
          candidate = current->lShock();
      }
    } 
    else
    {
      int id=current->rShock_id();
      if ( id < 0 || deleted_ishock_elements_.count(id))
      {
          candidate=0;
      }
      else
      {
          candidate = current->rShock();
      }

    }

    //normally all shocks should have neighboring shocks (i.e. the other shock riding 
    //on the same wavefront), but due to compartmentalization, this is no longer true:
    if (!candidate)
      break;

    if ( candidate->type() == 0 )
    {
        break;
    }
    ////only interact with it if it also inside the current cell
    //if (!is_point_inside_the_cell(candidate->getStartPt())){
    //  candidate = 0;
    //  break;
    //}

    //make sure this is a valid wavefront interaction.
    //if not, delete the invalid candidate shock
    //(this could be expensive in the long run!)
    if (common_belm->shock_invalidates_neighbor(current, candidate))
      delete_an_ishock_elm(candidate->pSNode());
    else
      neighbor_found = true;
  }
  
  return candidate;
}

//: intersect this shock with all four cell boundaries
void dbsk2d_lagrangian_ishock_detector::intersect_with_cell_boundaries(dbsk2d_ishock_edge* sh)
{
  dbsk2d_ishock_intersection_data bnd_I, best_I;
  dbsk2d_lagrangian_cell_bnd_sptr best_bnd = 0;

  //left bnd
  best_I = dbsk2d_ishock_compute_intersection(sh, _left_bnd);
  best_bnd = _left_bnd;

  //right bnd
  bnd_I = dbsk2d_ishock_compute_intersection(sh, _right_bnd);
  if (bnd_I.R != ISHOCK_DIST_HUGE &&
      RisGEq (bnd_I.R, sh->startTime()) && 
      RisLEq (bnd_I.R, sh->endTime()) && 
      RisL(bnd_I.R, best_I.R))
  {
    best_I = bnd_I;
    best_bnd = _right_bnd;
  }

  //top bnd
  bnd_I = dbsk2d_ishock_compute_intersection(sh, _top_bnd);
  if (bnd_I.R != ISHOCK_DIST_HUGE &&
      RisGEq (bnd_I.R, sh->startTime()) && 
      RisLEq (bnd_I.R, sh->endTime()) && 
      RisL(bnd_I.R, best_I.R))
  {
    best_I = bnd_I;
    best_bnd = _top_bnd;
  }

  //bottom bnd
  bnd_I = dbsk2d_ishock_compute_intersection(sh, _bottom_bnd);
  if (bnd_I.R != ISHOCK_DIST_HUGE &&
      RisGEq (bnd_I.R, sh->startTime()) && 
      RisLEq (bnd_I.R, sh->endTime()) && 
      RisL(bnd_I.R, best_I.R))
  {
    best_I = bnd_I;
    best_bnd = _bottom_bnd;
  }
  
  //for shocks that do not have intersection with the boundary functions defined yet
  if (best_I.R == ISHOCK_DIST_HUGE)
    return;

  //has to have intersected with at least one boundary
  dbsk2d_assert(best_I.R != ISHOCK_DIST_HUGE);

  //1)update the parameters to the closest boundary intersection
  //update the endtime 
  sh->setEndTime(best_I.R);
  //update taus
  sh->setLeTau (best_I.LSLtau);
  sh->setReTau (best_I.LSRtau);

  //2) Also record this intersection on the shock edge and corresponding boundary
  if (best_bnd->is_horiz())
    sh->set_bnd_intersect_pos(sh->getEndPt().x());
  else
    sh->set_bnd_intersect_pos(sh->getEndPt().y());
  
  best_bnd->add_shock_edge(sh);
}

