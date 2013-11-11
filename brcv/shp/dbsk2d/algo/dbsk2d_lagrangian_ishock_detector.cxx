// This is brcv/shp/dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>

//: Default constructor
dbsk2d_lagrangian_ishock_detector::dbsk2d_lagrangian_ishock_detector():
  _ishock_graph(NULL), 
  _boundary(NULL), 
  _bnd_cell(NULL),
  _left_bnd(NULL),
  _right_bnd(NULL),
  _top_bnd(NULL),
  _bottom_bnd(NULL),
  active_shocks_list(), 
  candidate_src_list(), 
  _nextShockToPropagate(NULL),
  _sim_time(-1.0),
  _bValid(true),
  local_shock_(false)
{  
}

//: constructor
dbsk2d_lagrangian_ishock_detector::dbsk2d_lagrangian_ishock_detector(dbsk2d_boundary_sptr boundary):
  _ishock_graph(NULL), 
  _boundary(boundary),
  _bnd_cell(boundary->cell(0,0)),//assume only one cell
  _left_bnd(NULL),
  _right_bnd(NULL),
  _top_bnd(NULL),
  _bottom_bnd(NULL),
  active_shocks_list(), 
  candidate_src_list(), 
  _nextShockToPropagate(NULL),
  _sim_time(-1.0),
  _bValid(true),
  local_shock_(false)
{
  //this boundary may already have an associated shock
  if (_boundary->ishock_graph()){
    _ishock_graph = _boundary->ishock_graph();
    compile_the_active_selm_list();
  }
  else
    _ishock_graph = new dbsk2d_ishock_graph(_boundary);
}

//: Destructor
dbsk2d_lagrangian_ishock_detector::~dbsk2d_lagrangian_ishock_detector()
{
  delete_the_active_selm_list();
  delete_the_candidate_src_list();
} 

//: set the initial bnd_cell for this lagrangian cell
// This will also set up the lagrangian cell boundaries correctly
void dbsk2d_lagrangian_ishock_detector::set_bnd_cell(dbsk2d_bnd_cell_sptr bnd_cell) 
{ 
  _bnd_cell = bnd_cell; 

  //set the extrinsic parameters of the cell boundaries
  _left_bnd = new dbsk2d_lagrangian_cell_bnd(dbsk2d_lagrangian_cell_bnd::LEFT, bnd_cell->box().min_x());
  _right_bnd = new dbsk2d_lagrangian_cell_bnd(dbsk2d_lagrangian_cell_bnd::RIGHT, bnd_cell->box().max_x());
  _top_bnd = new dbsk2d_lagrangian_cell_bnd(dbsk2d_lagrangian_cell_bnd::TOP, bnd_cell->box().max_y());
  _bottom_bnd = new dbsk2d_lagrangian_cell_bnd(dbsk2d_lagrangian_cell_bnd::BOTTOM, bnd_cell->box().min_y());
}

//: is this point inside this cell
bool dbsk2d_lagrangian_ishock_detector::
is_point_inside_the_cell(vgl_point_2d<double> pt)
{
  return RisGEq(pt.x(), _left_bnd->loc()) &&
         RisLEq(pt.x(), _right_bnd->loc()) &&
         RisGEq(pt.y(), _bottom_bnd->loc()) &&
         RisLEq(pt.y(), _top_bnd->loc());
}

void dbsk2d_lagrangian_ishock_detector::clear ()
{
  delete_the_active_selm_list();
  delete_the_candidate_src_list();

  //clear the shock graph
  _ishock_graph->clear();
}

//-------------------------------------------------------------------
// functions to manage the ordered shock elements list
//-------------------------------------------------------------------

//: delete all the candidate sources
void dbsk2d_lagrangian_ishock_detector::delete_the_candidate_src_list()
{
  ordered_src_list_iter curS = candidate_src_list.begin();
  for (; curS!=candidate_src_list.end(); curS++){
    dbsk2d_ishock_node* curSrc = (curS->second);
    delete curSrc;
  }
  //clean out the list which is by now just pointers to the 
  //deleted items on the list
  candidate_src_list.clear();
}


//: add all the active shock edges to the active shocks list
void dbsk2d_lagrangian_ishock_detector::compile_the_active_selm_list()
{
  dbsk2d_ishock_graph::edge_iterator curE = _ishock_graph->all_edges().begin();
  for (; curE != _ishock_graph->all_edges().end(); curE++){
    dbsk2d_ishock_edge* Sedge = (*curE);

    //if active, add the element to the active shock list
    if (Sedge->isActive())
      active_shocks_list.insert(key_selm_pair(r_id_pair(Sedge->simTime(), Sedge->id()), Sedge));
  }
}

//: delete all the shocks from the active shocks list
void dbsk2d_lagrangian_ishock_detector::delete_the_active_selm_list()
{
  active_shocks_list.clear();
}

//: insert a new edge into the time ordered active shock list
void dbsk2d_lagrangian_ishock_detector::add_an_ishock_edge (dbsk2d_ishock_edge* Sedge)
{
  // add it to the shock graph
  _ishock_graph->add_edge(Sedge);

  // and add the element to the active shock list
  active_shocks_list.insert(key_selm_pair(r_id_pair(Sedge->simTime(), Sedge->id()), Sedge));
}

//: insert a new node into the shock graph
void dbsk2d_lagrangian_ishock_detector::add_an_ishock_node (dbsk2d_ishock_node* Snode)
{
  // add it to the shock graph
  _ishock_graph->add_vertex(Snode);

  // don't need to add it to the active shock list
  // because nodes propagate into edges as soon as they are initialized
}

void dbsk2d_lagrangian_ishock_detector::delete_an_ishock_elm (dbsk2d_ishock_elm* selm)
{
  //Now you also need to update connectivity information
  //You can't do it from the destructors of the elements
  //because the shock list needs to be updated too

  if (!selm) {
    //This happens when we delete a A3, the child is NULL. dbsk2d_assert (0);
    return;
  }
  dbsk2d_assert (selm->id()>0);

  if (selm->is_a_link())
  {
    dbsk2d_ishock_edge* cur_edge = (dbsk2d_ishock_edge*)selm;
    init_cand_src_between(cur_edge->lBElement(),cur_edge->rBElement());
    deleted_bnd_elements_[cur_edge->lBElement()->id()]=cur_edge->lBElement();
    deleted_bnd_elements_[cur_edge->rBElement()->id()]=cur_edge->rBElement();
    
    //First, update all the connectivity information 
    if (cur_edge->lNeighbor())
      cur_edge->lNeighbor()->clear_rNeighbor();
    if (cur_edge->rNeighbor())
      cur_edge->rNeighbor()->clear_lNeighbor();

    if (cur_edge->pSNode()){
      if (cur_edge->pSNode()->cShock() == cur_edge)
        cur_edge->pSNode()->clear_cShock();
      else
        cur_edge->pSNode()->clear_cShock2();
    }

    if ( cur_edge->cSNode())
      cur_edge->cSNode()->remove_pShock(cur_edge);

    //delete all future(children) shocks (until the day when we can truly 
    //do local shock computation with error detection)
    
    //CAUTION: THIS WILL MAKE THIS FUNCTION RECURSIVE!!!!
    if ( cur_edge->cSNode())
      delete_an_ishock_elm(cur_edge->cSNode());

    //remove it from the active shocks list
    active_shocks_list.erase(r_id_pair(cur_edge->simTime(),cur_edge->id()));

    //also remove it from the cell boundary
    if (cur_edge->cell_bnd())
      cur_edge->cell_bnd()->delete_shock_edge(cur_edge);

    //remove it from the shock graph
    _ishock_graph->remove_edge(cur_edge);
  }
  else {
    //go through the list of parents of this node
    //by this time only the shocks that belong to neighboring
    //boundaries should be remaining

    //what about the children?
    //the children can't exist without these nodes so get rid of them too!
    dbsk2d_ishock_node* cur_node = (dbsk2d_ishock_node*)selm;

    //reactivate all the parent shocks
    for(ishock_edge_list::iterator curS = cur_node->pShocks().begin();
        curS != cur_node->pShocks().end(); ++curS)
      reactivate_an_ishock_elm (*curS); //very important!

    //remove children if any
    if (cur_node->cShock())
        delete_an_ishock_elm(cur_node->cShock());
    if (cur_node->cShock2())
        delete_an_ishock_elm(cur_node->cShock2());

    //remove it from the shock graph
    _ishock_graph->remove_vertex(cur_node);
  } 
}

void dbsk2d_lagrangian_ishock_detector::update_an_ishock_elms_simtime(dbsk2d_ishock_edge* selm, double stime)
{
  //1) First remove the shock element from the active shocks list if active
  if (selm->isActive())
    active_shocks_list.erase(r_id_pair(selm->simTime(), selm->id())); 

  //2) update the shock's simulation time and reactivate it
  selm->setSimTime (stime);
  selm->setActive(true);

  //3) then reinsert it into the appropriate place
  active_shocks_list.insert(key_selm_pair(r_id_pair(selm->simTime(), selm->id()), selm));
}

void dbsk2d_lagrangian_ishock_detector::deactivate_an_ishock_elm (dbsk2d_ishock_elm* selm)
{
  //1) Remove the shock element from the active shocks list
  active_shocks_list.erase(r_id_pair(selm->simTime(),selm->id())); 

  //2) update the shock's bActive status
  selm->setActive (false);
}

void dbsk2d_lagrangian_ishock_detector::reactivate_an_ishock_elm (dbsk2d_ishock_elm* selm)
{
  //reactivate this element for propagation
  //should involve moving the shock element to its start time
  //and resetting all the other variables

  //only the shock links will be reactivated
  dbsk2d_assert(selm->is_a_link());
  dbsk2d_ishock_edge* curEdge = (dbsk2d_ishock_edge*)selm;

  //If the shock is active, remove the shock element from the active shocks list
  if (curEdge->isActive())
    active_shocks_list.erase(r_id_pair(curEdge->simTime(),curEdge->id())); 

  //delete all events after the start_time of this shock
  
  //reset this shock as if it was just init'd
  curEdge->reset_shock();

  // remove any cell boundary intersections if they exist
  if (curEdge->cell_bnd())
    curEdge->cell_bnd()->delete_shock_edge(curEdge);

  //then reinsert it into the appropriate place
  active_shocks_list.insert(key_selm_pair(r_id_pair(curEdge->simTime(),curEdge->id()), curEdge));

  //after reactivation recompute intersections with the cell boundaries to redefine uppper limits
  //on propagation
  intersect_with_cell_boundaries(curEdge);
}

void dbsk2d_lagrangian_ishock_detector::print_ordered_selm_list (bool bPrintAll)
{
  if (bPrintAll!=true)
    return;

  vcl_cout<< "ShockList: " <<vcl_endl;
  ordered_shock_list_iter elmPtr = active_shocks_list.begin();
  for (; elmPtr != active_shocks_list.end(); elmPtr++) {
    dbsk2d_ishock_elm* current = elmPtr->second;

    switch (current->type()) {
    case dbsk2d_ishock_elm::SNODE:                vcl_cout<< "N"; break;
    case dbsk2d_ishock_elm::POINTPOINT:           vcl_cout<< "P-P"; break;
    case dbsk2d_ishock_elm::POINTLINE:            vcl_cout<< "P-L"; break;
    case dbsk2d_ishock_elm::POINTARC:             vcl_cout<< "P-A"; break;
    case dbsk2d_ishock_elm::LINELINE:             vcl_cout<< "L-L"; break;
    case dbsk2d_ishock_elm::LINEARC:              vcl_cout<< "L-A"; break;
    case dbsk2d_ishock_elm::ARCARC:               vcl_cout<< "A-A"; break;
    case dbsk2d_ishock_elm::CONTACTSHOCK:         vcl_cout<< "C"; break;
    case dbsk2d_ishock_elm::LINELINE_THIRDORDER:  vcl_cout<< "LL-TO"; break;
    case dbsk2d_ishock_elm::POINTARC_THIRDORDER:  vcl_cout<< "PA-TO"; break;
    case dbsk2d_ishock_elm::ARCARC_THIRDORDER:    vcl_cout<< "AA-TO"; break;
    default:                                      vcl_cout<< "ERROR"; break;
    }
    vcl_cout<< ", Sid: "<< current->id();
    vcl_cout<< ", simTime: "<< current->simTime();
    if (current->isActive()) {
      if (current->simTime() < MAX_RADIUS)
        vcl_cout<< ", Active.";
      else
        vcl_cout<< ", OutOfRange.";
    }
    else
      vcl_cout<< ", Dead.";
    if (current->is_a_link())
      if (current->hasPropagated()) 
        vcl_cout<< " Propagated.";
      else
        vcl_cout<< " Unpropagated.";

    vcl_cout<< vcl_endl;
  }

  vcl_cout<< "------------------" <<vcl_endl;
}

void dbsk2d_lagrangian_ishock_detector::print_selm_info_from_id(int id)
{
  ordered_shock_list_iter elmPtr = active_shocks_list.begin();
  for (; elmPtr != active_shocks_list.end(); elmPtr++) {
    dbsk2d_ishock_elm* current = elmPtr->second;

    if (current->id() == id){
      //display info
      current->getInfo(vcl_cout);
      return;
    }      
  }
  vcl_cout <<"INVALID SHOCK ID: "<<id<<vcl_endl;
}

//-------------------------------------------------------------------
// functions to manage dynamic changes in boundary
//-------------------------------------------------------------------
  
void dbsk2d_lagrangian_ishock_detector::delete_a_belm(dbsk2d_ishock_belm* belm)
{
  //go through the shockmap and delete all shocks created by this belm
  while ( belm->shock_map().size()>0){
    bnd_ishock_map_iter curS = belm->shock_map().begin();
    dbsk2d_ishock_edge* selm = curS->second;
    //delete the parent node along with the edge because the parent node
    //must also be caused by this belm by definition
    if (selm->pSNode())
      delete_an_ishock_elm(selm->pSNode());
    else
      delete_an_ishock_elm(selm); //contacts don't have psnodes
  }

  //if this belm is a SWF in any cell boundary, clear it
  if (_left_bnd->swf()==belm)
    _left_bnd->clear_swf();
  if (_right_bnd->swf()==belm)
    _right_bnd->clear_swf();
  if (_top_bnd->swf()==belm)
    _top_bnd->clear_swf();
  if (_bottom_bnd->swf()==belm)
    _bottom_bnd->clear_swf();
  
  //finally delete the belm
  delete belm;
}

//-------------------------------------------------------------------
// Shock computation functions
//-------------------------------------------------------------------

bool dbsk2d_lagrangian_ishock_detector::detect_shocks ()
{
  //first initialize the sources
  if (number_of_active_shocks()==0)
    initialize_shocks ();

  //now propagate the shocks
  return propagate_shocks ();
}

//-------------------------------------------------------------------
// DEBUG functions
//-----------------------------------------------------------------
//: validate the computation
void dbsk2d_lagrangian_ishock_detector::validate_shocks()
{
  //first step in validation is to see that the wavefront information
  //exists and is correct
  for ( dbsk2d_ishock_graph::edge_iterator curE = _ishock_graph->all_edges().begin();
        curE != _ishock_graph->all_edges().end();
        curE++ ) 
  {
    dbsk2d_ishock_edge* selm = (*curE);

    dbsk2d_assert(selm->lShock());
    dbsk2d_assert(selm->rShock());
  }
  #ifdef DEBUG_SHOCK_VERBOSE
  vcl_cout << "shocks validated!" << vcl_endl;
  #endif
}

void dbsk2d_lagrangian_ishock_detector::DebugPrintOnePropagation (int id, PROPAGATION_TYPE action)
{
  //vcl_cout<< "---> Shock "<< id;
  switch (action) {
    case BOGUS_PROPAGATION_TYPE: dbsk2d_assert (0); break;
    case NO_PROPAGATION: vcl_cout<< " NO_PROPAGATION"; break;
    case PROPAGATION_DONE: vcl_cout<< " PROPAGATION_DONE"; break;
    case PROPAGATION_TO_INFINITY: vcl_cout<< " PROPAGATION_TO_INFINITY"; break;
    case PROPAGATION_TO_BND: vcl_cout<< " PROPAGATION_TO_BND"; break;
    case INVALID_CANDIDATE_SOURCE: vcl_cout<< " INVALID_CANDIDATE_SOURCE"; break;
    case A3_FORMATION: vcl_cout<< " A3_FORMATION"; break;
    case REGULAR_JUNCT: vcl_cout<< " REGULAR_JUNCT"; break;
    case DEGENERATE_JUNCT: vcl_cout<< " DEGENERATE_JUNCT"; break;
    case SINK_FORMATION: vcl_cout<< " SINK_FORMATION"; break;
    case NEW_SHOCK_FROM_A3: vcl_cout<< " NEW_SHOCK_FROM_A3"; break;
    case NEW_SHOCKS_FROM_SOURCE: vcl_cout<< " NEW_SHOCKS_FROM_SOURCE"; break;
    case NEW_SHOCK_FROM_JUNCT: vcl_cout<< " NEW_SHOCK_FROM_JUNCT"; break;
    case NEW_CAND_SRC: vcl_cout<< "NEW_CAND_SRC"; break;
    case LEFT_INTERSECTION: vcl_cout<< " LEFT_INTERSECTION"; break;
    case RIGHT_INTERSECTION: vcl_cout<< " RIGHT_INTERSECTION"; break;
    case BOTH_INTERSECTION: vcl_cout<< " BOTH_INTERSECTION"; break;
    case THIRD_ORDER_FORMATION: vcl_cout<< " THIRD_ORDER_FORMATION"; break;
    case ARC_THIRD_ORDER_FORMATION: vcl_cout<< " ARC_THIRD_ORDER_FORMATION"; break;  
    case PROPAGATION_ERROR_DETECTED: vcl_cout << "PROPAGATION_ERROR_DETECTED"; break;
  }
  vcl_cout<< vcl_endl;
}

void dbsk2d_lagrangian_ishock_detector::MessageOutDetectionResults (int wndid)
{
  int nTotalShocks=0, nTotalContacts=0;
  int nTotalSNodes=0, nTotalSLinks=0;
  int nA3=0, nSO=0, nSJunct=0, nSink=0;
  int nPP=0, nPL=0, nPA=0;
  int nLL=0, nLA=0, nAA=0;
  int nC=0;
  int nTO=0;

  int nActiveShocks=0, nDeadShocks=0;

  ordered_shock_list_iter curS = active_shocks_list.begin();
  for (; curS!=active_shocks_list.end(); curS++){
    dbsk2d_ishock_elm* current = (curS->second);
    if (current->isActive())
      nActiveShocks++;

    switch (current->type()) {
      case dbsk2d_ishock_elm::SNODE:           nA3++;    break;
      //case dbsk2d_ishock_elm::SOURCE:          nSO++;    break;
      //case dbsk2d_ishock_elm::JUNCT:           nSJunct++;  break;
      //case dbsk2d_ishock_elm::SINK:            nSink++;    break;
      case dbsk2d_ishock_elm::POINTPOINT:      nPP++;    break;
      case dbsk2d_ishock_elm::POINTLINE:       nPL++;    break;
      case dbsk2d_ishock_elm::POINTARC:        nPA++;    break;
      case dbsk2d_ishock_elm::LINELINE:        nLL++;    break;
      case dbsk2d_ishock_elm::LINEARC:         nLA++;    break;
      case dbsk2d_ishock_elm::ARCARC:          nAA++;    break;
      case dbsk2d_ishock_elm::CONTACTSHOCK:    nC++;    break;
      case dbsk2d_ishock_elm::LINELINE_THIRDORDER:  nTO++;    break;
      case dbsk2d_ishock_elm::POINTARC_THIRDORDER:  nTO++;    break;
      case dbsk2d_ishock_elm::ARCARC_THIRDORDER:    nTO++;   break;
      default: break;
    }
  }

  nTotalShocks = number_of_active_shocks();
  nTotalSNodes = nA3 + nSO + nSJunct + nSink;
  nTotalSLinks = nPP + nPL + nPA + nLL + nLA + nAA + nTO;
  nTotalContacts = nC;
  nDeadShocks = nTotalShocks - nActiveShocks;

  dbsk2d_assert (nTotalShocks == nTotalSNodes + nTotalSLinks + nTotalContacts);


  #ifdef DEBUG_SHOCK_VERBOSE
  vcl_cout<< "\n===== Shock Detection Results ====="<<vcl_endl;
  //if (_ShockAlgoType==LAGRANGIAN)
  //   vcl_cout<<"ShockAlgorithm: Lagrangian: "<<vcl_endl;
  //else if (_ShockAlgoType==DYN_VAL)
  //   vcl_cout<<"ShockAlgorithm: Dynamic Validation: "<<vcl_endl;

  vcl_cout<<"Bnd Elements: "<< boundary()->belm_list().size()<<vcl_endl;
  vcl_cout<<"Total Shock Elements: "<< nTotalShocks <<vcl_endl<<vcl_endl;

  /*if (MessageOption == MSG_VERBOSE) {
  vcl_cout <<"SNodes: "<< nTotalSNodes <<vcl_endl; 
  vcl_cout <<"SLinks: "<< nTotalSLinks <<vcl_endl;
  vcl_cout <<"Contacts: "<< nTotalContacts <<vcl_endl;
  vcl_cout <<"A3s: "<< nA3  <<vcl_endl;
  vcl_cout <<"SO Source: "<< nSO <<vcl_endl;
  vcl_cout <<"Junctions: "<< nSJunct <<vcl_endl;
  vcl_cout <<"Sinks: " << nSink <<vcl_endl;
  vcl_cout <<"Point-Point: "<< nPP  <<vcl_endl;
  vcl_cout <<"Point-Line: "<< nPL <<vcl_endl;
  vcl_cout <<"Point-Arc: "<< nPA <<vcl_endl;
  vcl_cout <<"Line-Line: "<< nLL  <<vcl_endl;
  vcl_cout <<"Line-Arc: "<< nLA <<vcl_endl;
  vcl_cout <<"Arc-Arc: "<< nAA <<vcl_endl;
  vcl_cout <<"PointLineContact: "<< nPLC <<vcl_endl;
  vcl_cout <<"PointArcContact: "<< nPAC<<vcl_endl;
  vcl_cout <<"LLC: "<< nLLC <<vcl_endl;
  vcl_cout <<"LAC: "<< nLAC <<vcl_endl;
  vcl_cout <<"AAC: "<< nAAC <<vcl_endl;
  vcl_cout <<"ThirdOrder: "<< nTO  <<vcl_endl;
  vcl_cout <<"ArcThirdOrder: "<< nATO <<vcl_endl;
  }*/
  #endif
}
