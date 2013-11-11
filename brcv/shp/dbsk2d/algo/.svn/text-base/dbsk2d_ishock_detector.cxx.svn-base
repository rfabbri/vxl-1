// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_detector.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_detector.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>

//: constructor
dbsk2d_ishock_detector::dbsk2d_ishock_detector(dbsk2d_boundary_sptr boundary)
{
  _boundary = boundary;

  //this boundary may already have an associated shock
  if (_boundary->ishock_graph()){
    _ishock_graph = _boundary->ishock_graph();
  }
  else //instantiate a new one
    _ishock_graph = new dbsk2d_ishock_graph(_boundary);

  //initialize merge schedule
  _cur_cell_row = 0;
  _cur_cell_col = 0;
  _next_cell_row = 0;
  _next_cell_col = 0;
  _merging_cols = true;
  _row_offset=1; 
  _col_offset=2;
}

//: \todo check this function
dbsk2d_ishock_detector::~dbsk2d_ishock_detector()
{
} 

//: \todo check this function
void dbsk2d_ishock_detector::clear ()
{
  //delete all the lagrangian cells
  _cells.clear();

  //clear the shock graph
  _ishock_graph->clear();

  //reset debug parameters
  _cur_cell_row = 0;
  _cur_cell_col = 0;
  _next_cell_row = 0;
  _next_cell_col = 0;
  _merging_cols = true;
  _row_offset=1; 
  _col_offset=2;
}

//-------------------------------------------------------------------
// Shock computation functions
//-------------------------------------------------------------------

void dbsk2d_ishock_detector::detect_shocks ()
{
  //first initialize the sources
  initialize_shocks();

  //now propagate the shocks
  propagate_shocks ();
}

void dbsk2d_ishock_detector::initialize_shocks ()
{
  //create the lagrangian cells from the boundary cells
  _cells.resize(_boundary->num_rows(), _boundary->num_cols());

  //now go over each lagrangian cell and assign it the proper shock graph
  //and boundary object
  for (int i=0; i<_boundary->num_rows(); ++i){
    for (int j=0; j<_boundary->num_cols(); ++j){
      _cells(i, j).set_boundary(_boundary);
      _cells(i, j).set_bnd_cell(_boundary->cell(i, j));
      _cells(i, j).set_ishock_graph(_ishock_graph);
      
      // temporarily commented out to avoid dealing with shocks of arcs
     _cells(i, j).initialize_shocks();
    }
  }
}

void dbsk2d_ishock_detector::propagate_shocks_without_merge()
{
  // propagate the shocks in each lagrangian cell
  for (int i=0; i<_boundary->num_rows(); ++i){
    for (int j=0; j<_boundary->num_cols(); ++j){
      _cells(i, j).propagate_shocks();
    }
  }
}

void dbsk2d_ishock_detector::propagate_shocks ()
{
  //1) detect shocks in each lagrangian cell
  propagate_shocks_without_merge();

  //2) Start merging neighboring lagrangian cells until there is a single cell
  //   Merge Schedule:
  //     a) merge pairs in rows first
  //     b) merge pairs in columns
  _merging_cols = true;
  _row_offset=1;
  _col_offset=2;
  while (_row_offset < 2*_boundary->num_rows() || 
         _col_offset < 2*_boundary->num_cols())
  { 
    for (_cur_cell_row=0; 
         _cur_cell_row<_boundary->num_rows(); 
         _cur_cell_row+=_row_offset)
    {
      for (_cur_cell_col=0; 
           _cur_cell_col<_boundary->num_cols(); 
           _cur_cell_col+=_col_offset)
      {
        int neigh_row, neigh_col;
        if (_merging_cols){
          neigh_row = _cur_cell_row;
          neigh_col = _cur_cell_col+_col_offset/2;
        }
        else {
          neigh_row = _cur_cell_row+_row_offset/2;
          neigh_col = _cur_cell_col;
        }

        //if these are valid cells, merge
        if (neigh_row < _boundary->num_rows() && 
            neigh_col < _boundary->num_cols())
        {
          #ifdef DEBUG_SHOCK_VERBOSE
            vcl_cout << "\n=========================================================\n";
            vcl_cout << "Merging Cells: (" << 
              _cur_cell_row << ", " << _cur_cell_col << ")-(" << 
              neigh_row << ", " << neigh_col << ") " << vcl_endl;
          #endif

          //now merge the cells
          merge_cells(_cur_cell_row, _cur_cell_col, neigh_row, neigh_col);

          #ifdef DEBUG_SHOCK_VERBOSE
            vcl_cout << "\n=========================================================\n";
          #endif

          //3) propagate all activated and newly formed shocks
          _cells(_cur_cell_row, _cur_cell_col).propagate_shocks();
        }
      }
    }

    if (_merging_cols){
      _row_offset*=2; //increment row offset
      _merging_cols = false;
    }
    else {
      _col_offset*=2;//increment column offset
      _merging_cols = true;
    }
  }
  
  //Note: after this merging schedule only cell(0,0) is valid
}
void dbsk2d_ishock_detector::
reactivate_shocks_at_cell_bnd(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                              dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  //reactivate all the shocks coming to the boundary
  while (bnd->active_shocks_list().size()>0)
    cur_cell.reactivate_an_ishock_elm(bnd->active_shocks_list().begin()->second);
}

void dbsk2d_ishock_detector::
find_interacting_wavefronts_at_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd1, 
                                   dbsk2d_lagrangian_cell_bnd_sptr bnd2, 
                                   wf_pairs_list& interacting_wf_list)
{
  #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Interacting wavefronts.....\n\n";

    vcl_cout << "Bnd1: \n";
    vcl_cout << "==================================\n";
    if (bnd1->swf()) 
      vcl_cout << "SWF: {" << bnd1->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    for (ordered_shock_list_iter s_it = bnd1->active_shocks_list().begin();
       s_it != bnd1->active_shocks_list().end(); ++s_it)
      vcl_cout << s_it->first.first << " : " << s_it->second->id() << 
        " [" << s_it->second->rBElement()->id() << ", " << 
        s_it->second->lBElement()->id() << "]" << vcl_endl;
    vcl_cout << "==================================\n";
    vcl_cout << "Bnd2: \n";
    vcl_cout << "==================================\n";
    if (bnd2->swf()) 
      vcl_cout << "SWF: {" << bnd2->swf()->id() << "}\n";
    else
      vcl_cout << "SWF: {}\n";
    for (ordered_shock_list_iter s_it = bnd2->active_shocks_list().begin();
       s_it != bnd2->active_shocks_list().end(); ++s_it)
      vcl_cout << s_it->first.first << " : " << s_it->second->id() << 
        " [" << s_it->second->rBElement()->id() << ", " << 
        s_it->second->lBElement()->id() << "]" << vcl_endl;
    vcl_cout << "==================================\n";

  #endif

  // Make a list of all interacting wavefronts at the boundary
  // Note:
  //   This will require simultaneously traversing the active shocks lists from the 
  //   two boundaries
  ordered_shock_list_iter s_it1 = bnd1->active_shocks_list().begin();
  ordered_shock_list_iter s_it2 = bnd2->active_shocks_list().begin();
  
  //flags to keep track of whether we're at the end of the list
  //explicit boolean variable used to speed up loop
  bool eof_bnd1_list = (s_it1 == bnd1->active_shocks_list().end());
  bool eof_bnd2_list = (s_it2 == bnd2->active_shocks_list().end());

  //Special cases:
  //A) The cell is empty
  //B) There're a few bnd elements in the cell that don't interact
  //C) none of the shocks in the cell intersect with the current bnd

  dbsk2d_ishock_belm *belm1, *belm2;

  // if there are no shock intersections on this bnd, just let the 
  // special wavefronts interact
  if (eof_bnd1_list && eof_bnd2_list &&
      bnd1->swf() && bnd2->swf())
  {
    record_wavefront_interaction_between(bnd1->swf(), bnd2->swf(), bnd1, interacting_wf_list);
    return;
  }

  // Let the wavefronts from the shocks and/or SWFs interact
  // traverse both lists simultaneously
  while ( !(eof_bnd1_list && eof_bnd2_list))
  {
    //wavefront selection rules
    if (eof_bnd1_list){
      if (bnd1->swf())
        belm1 = bnd1->swf(); //only have the SWF 
      else if (bnd1->active_shocks_list().size()>0)
        belm1 = bnd1->active_shocks_list().rbegin()->second->lBElement();
      else
        belm1 = 0;  //there are no wavefronts from this cell
    }
    else 
      belm1 = s_it1->second->rBElement();      

    if (eof_bnd2_list){
      if (bnd2->swf())
        belm2 = bnd2->swf();  //only have the SWF
      else if (bnd2->active_shocks_list().size()>0)
        belm2 = bnd2->active_shocks_list().rbegin()->second->rBElement();
      else
        belm2 = 0;  //there are no wavefronts from this cell
    }
    else
      belm2 = s_it2->second->lBElement();
      
    //make sure there are some wavefronts that can interact
    if (!belm1 || !belm2)
      return;

    //list traversal rules
    if (eof_bnd1_list){
      s_it2++;
    }
    else if (eof_bnd2_list){
      s_it1++;
    }
    //increment the iterator that is lagging behind
    else if (RisL(s_it1->first.first, s_it2->first.first)){
      s_it1++;  
    }
    else if (RisL(s_it2->first.first, s_it1->first.first)){
      s_it2++;
    }
    else {
      //when it's difficult to judge the correct wavefront interaction
      //because they are too close, simply consider both possibilities
      record_wavefront_interaction_between(s_it1->second->lBElement(), 
        s_it2->second->lBElement(), bnd1, interacting_wf_list);
      record_wavefront_interaction_between(s_it1->second->rBElement(), 
        s_it2->second->rBElement(), bnd1, interacting_wf_list);

      s_it1++;
      s_it2++;
    }

    //let these two wavefronts interact
    record_wavefront_interaction_between(belm1, belm2, bnd1, interacting_wf_list);
    
    //after updating the iterators, update these flags
    eof_bnd1_list = (s_it1 == bnd1->active_shocks_list().end());
    eof_bnd2_list = (s_it2 == bnd2->active_shocks_list().end());

    //final interaction
    if (eof_bnd1_list && eof_bnd2_list)
    {
      if (bnd1->swf())
        belm1 = bnd1->swf(); //only have the SWF 
      else
        belm1 = bnd1->active_shocks_list().rbegin()->second->lBElement();

      if (bnd2->swf())
        belm2 = bnd2->swf(); //only have the SWF 
      else
        belm2 = bnd2->active_shocks_list().rbegin()->second->rBElement();

      record_wavefront_interaction_between(belm1, belm2, bnd1, interacting_wf_list);
    }
  }
}

//: record all wavefront interactions between these two wavefronts
//Note:
// If either of these two elements(wavefronts) are virtual,
// we have to interact with the elements(wavefronts) they interact with.
void dbsk2d_ishock_detector::
record_wavefront_interaction_between(dbsk2d_ishock_belm* belm1, 
                                     dbsk2d_ishock_belm* belm2, 
                                     dbsk2d_lagrangian_cell_bnd_sptr bnd,
                                     wf_pairs_list& interacting_wf_list)
{
  //first determine if either belm1 or belm2 is virtual
  bool belm1_virtual = is_belm_virtual_on_bnd(belm1, bnd);
  bool belm2_virtual = is_belm_virtual_on_bnd(belm2, bnd);

  //if both wavefronts are virtual, we don't need them to interact
  if (belm1_virtual && belm2_virtual)
    return;

  //if both are non-virtual, there is just a simple interaction
  if (!belm1_virtual && !belm2_virtual){
    interacting_wf_list.insert(wf_pair(belm1, belm2));
    return;
  }

  //However if one of them is virtual, we need to interact the other wavefront with 
  //all the wavefronts the virtual wavefront interacted with
  belm_list blist;
  dbsk2d_ishock_belm* regular_belm;

  if (belm1_virtual){
    regular_belm = belm2;
    belm1->get_interacting_belements(blist);
  }
  else {
    regular_belm = belm1;
    belm2->get_interacting_belements(blist);
  }

  //go over all the wavefronts but weed out virtual ones
  for (belm_list_iter b_it=blist.begin(); b_it!=blist.end(); b_it++){
    if (!is_belm_virtual_on_bnd((*b_it), bnd))
      interacting_wf_list.insert(wf_pair(regular_belm, (*b_it)));
  }
}

void dbsk2d_ishock_detector::
remove_virtual_endpoints(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                         dbsk2d_lagrangian_cell_bnd_sptr bnd1, 
                         dbsk2d_lagrangian_cell_bnd_sptr bnd2)
{
  vcl_set<dbsk2d_ishock_belm*> virtual_endpoints;

  for (ordered_shock_list_iter s_it1 = bnd1->active_shocks_list().begin();
       s_it1 != bnd1->active_shocks_list().end(); s_it1++)
  {
    dbsk2d_ishock_belm* belm = s_it1->second->rBElement();

    if (belm->is_a_point()){
      dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*) belm;
      dbsk2d_ishock_bpoint* real_endpt = bpoint->bnd_vertex()->bpoint();

      if (real_endpt != bpoint &&                //is a virtual endpoint
          bnd1->is_pt_on_bnd(real_endpt->pt()))   //is on the current bnd 
      {
        virtual_endpoints.insert(belm);

        // reconnect the curve to the real endpoint
        dbsk2d_ishock_bcurve* bcurve = (dbsk2d_ishock_bcurve*)
                                        bpoint->rContact()->rBElement();
        if (bcurve->s_pt()==bpoint){ 
          bcurve->set_s_pt(real_endpt);
          bcurve->twin_bcurve()->set_e_pt(real_endpt);
          real_endpt->connectTo(bcurve);
          real_endpt->connectTo(bcurve->twin_bcurve());
        }
      }
    }
  }

  //last element if it exists
  if (bnd1->active_shocks_list().size()>0){
    dbsk2d_ishock_belm* belm = bnd1->active_shocks_list().rbegin()->second->lBElement();
    if (belm->is_a_point()){
      dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*) belm;
      dbsk2d_ishock_bpoint* real_endpt = bpoint->bnd_vertex()->bpoint();

      if (real_endpt != bpoint &&                //is a virtual endpoint
          bnd1->is_pt_on_bnd(real_endpt->pt()))   //is on the current bnd 
      {
        virtual_endpoints.insert(belm);

        // reconnect the line to the real endpoint
        dbsk2d_ishock_bcurve* bcurve = (dbsk2d_ishock_bcurve*)
                                        bpoint->rContact()->rBElement();
        if (bcurve->s_pt()==bpoint){ 
          bcurve->set_s_pt(real_endpt);
          bcurve->twin_bcurve()->set_e_pt(real_endpt);
          real_endpt->connectTo(bcurve);
          real_endpt->connectTo(bcurve->twin_bcurve());
        }
      }
    }
  }

  //bnd2
  for (ordered_shock_list_iter s_it2 = bnd2->active_shocks_list().begin();
       s_it2 != bnd2->active_shocks_list().end(); s_it2++)
  {
    dbsk2d_ishock_belm* belm = s_it2->second->lBElement();

    if (belm->is_a_point()){
      dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*) belm;
      dbsk2d_ishock_bpoint* real_endpt = bpoint->bnd_vertex()->bpoint();

      if (real_endpt != bpoint &&                //is a virtual endpoint
          bnd2->is_pt_on_bnd(real_endpt->pt()))   //is on the current bnd 
      {
        virtual_endpoints.insert(belm);

        // reconnect the curve to the real endpoint
        dbsk2d_ishock_bcurve* bcurve = (dbsk2d_ishock_bcurve*)
                                        bpoint->lContact()->lBElement();
        if (bcurve->e_pt()==bpoint){
          bcurve->set_e_pt(real_endpt);
          bcurve->twin_bcurve()->set_s_pt(real_endpt);
          real_endpt->connectTo(bcurve);
          real_endpt->connectTo(bcurve->twin_bcurve());
        }
      }
    }
  }

  //last element if it exists
  if (bnd2->active_shocks_list().size()>0){
    dbsk2d_ishock_belm* belm = bnd2->active_shocks_list().rbegin()->second->rBElement();
    if (belm->is_a_point()){
      dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*) belm;
      dbsk2d_ishock_bpoint* real_endpt = bpoint->bnd_vertex()->bpoint();

      if (real_endpt != bpoint &&                //is a virtual endpoint
          bnd2->is_pt_on_bnd(real_endpt->pt()))   //is on the current bnd 
      {
        virtual_endpoints.insert(belm);

        // reconnect the line to the real endpoint
        dbsk2d_ishock_bcurve* bcurve = (dbsk2d_ishock_bcurve*)
                                        bpoint->lContact()->lBElement();
        if (bcurve->e_pt()==bpoint){
          bcurve->set_e_pt(real_endpt);
          bcurve->twin_bcurve()->set_s_pt(real_endpt);
          real_endpt->connectTo(bcurve);
          real_endpt->connectTo(bcurve->twin_bcurve());
        }
      }
    }
  }

  //now delete all the virtual endpoints
  for (vcl_set<dbsk2d_ishock_belm*>::iterator v_it = virtual_endpoints.begin();
       v_it != virtual_endpoints.end(); v_it++)
  {
    cur_cell.delete_a_belm(*v_it);
  }
  virtual_endpoints.clear();

}

//: interact the wavefronts from the pairs stored in the list
//  some interactions create contact shocks and others produce
//  candidate sources
void dbsk2d_ishock_detector::
interact_wavefronts(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                    wf_pairs_list& interacting_wf_list)
{
  // Initialize candidate sources from pairs of interacting wavefronts
  // 1) first look for interactions creating colinear contact shocks
  for (wf_pairs_list::iterator wf_it = interacting_wf_list.begin();
       wf_it != interacting_wf_list.end(); wf_it++)
  {
    if (wf_it->first->is_a_curve() && wf_it->second->is_a_curve())
    {
      dbsk2d_ishock_bcurve* bcurve1 = (dbsk2d_ishock_bcurve*)wf_it->first;
      dbsk2d_ishock_bcurve* bcurve2 = (dbsk2d_ishock_bcurve*)wf_it->second;

      if (bcurve1->s_pt() == bcurve2->e_pt() && !bcurve1->lContact()){
        //curves are connected and need a contact:initialize a line-line contact
        cur_cell.form_a_contact_shock(bcurve2, bcurve1);
        //cur_cell.form_a_contact_shock(bcurve1->twin_bcurve(), bcurve2->twin_bcurve());
        bcurve1->s_pt()->set_visibility(false);
        continue;
      }

      if (bcurve1->e_pt() == bcurve2->s_pt() && !bcurve1->rContact()){
        //curves are connected and need a contact:initialize a line-line contact
        cur_cell.form_a_contact_shock(bcurve1, bcurve2);
        //cur_cell.form_a_contact_shock(bcurve2->twin_bcurve(), bcurve1->twin_bcurve());
        bcurve1->e_pt()->set_visibility(false);
      }
    }
  }

  // 2) form candidate sources from the rest of the interactions
  //    The line-line interactions that already formed contacts cannot form sources
  for (wf_pairs_list::iterator wf_it = interacting_wf_list.begin();
       wf_it != interacting_wf_list.end(); wf_it++)
  {
    cur_cell.init_cand_src_between(wf_it->first, wf_it->second);
    
    #ifdef DEBUG_SHOCK_VERBOSE
      vcl_cout << "Interaction between: " << wf_it->first->id() << " " << wf_it->second->id() << vcl_endl;
    #endif
  }
}

//: merge these two lagrangian cells
void dbsk2d_ishock_detector::merge_cells(int row1, int col1, int row2, int col2)
{
  dbsk2d_lagrangian_cell_bnd_sptr bnd1, bnd2;

  dbsk2d_assert(row1==row2 || col1==col2);
  dbsk2d_assert(row1<row2 || col1<col2);

  //1a) find the common boundary of these cells
  //1b) Merge the boundaries adjacent to the common boundaries
  //1c) Use the second cell's remaining boundary as the new boundary 
  //    of the current larger cell [i.e., cell(row1, col1)]
  if (row1==row2)
  {
    //identify the common boundary
    bnd1 = _cells(row1, col1).right_bnd();
    bnd2 = _cells(row2, col2).left_bnd(); 
    //merge the adjacent boundaries
    _cells(row1, col1).top_bnd()->merge_with(_cells(row2, col2).top_bnd());
    _cells(row1, col1).bottom_bnd()->merge_with(_cells(row2, col2).bottom_bnd());
    //and replace the right bnd with the adjacent cell's right bnd
    _cells(row1, col1).set_right_bnd(_cells(row2, col2).right_bnd());
  }
  else if (col1==col2)
  {
    //identify the common boundary
    bnd1 = _cells(row2, col2).bottom_bnd();
    bnd2 = _cells(row1, col1).top_bnd();
    //merge the adjacent boundaries
    _cells(row1, col1).right_bnd()->merge_with(_cells(row2, col2).right_bnd());
    _cells(row1, col1).left_bnd()->merge_with(_cells(row2, col2).left_bnd());
    //and replace the top bnd with the adjacent cell's top bnd
    _cells(row1, col1).set_top_bnd(_cells(row2, col2).top_bnd());
  }
  else
    dbsk2d_assert(0); //can't merge these cells

  //label cell(row2, col2) as invalid
  _cells(row2, col2).make_invalid();

  //2) Look for all possible interactions of adjacent wavefronts 
  //   at the common boundary and put them on a list
  wf_pairs_list interacting_wf_list;
  find_interacting_wavefronts_at_bnd(bnd1, bnd2, interacting_wf_list);

  //3) Reconnect the lines to the real endpoints and
  //   Delete the virtual endpoints
  remove_virtual_endpoints(_cells(row1, col1), bnd1, bnd2);

  //4) reactivate all the shocks coming to the boundary
  reactivate_shocks_at_cell_bnd(_cells(row1, col1), bnd1);
  reactivate_shocks_at_cell_bnd(_cells(row1, col1), bnd2);

  //5) Initialize contact shocks and candidate sources from 
  //   the list of wavefront interactions at the bnd
  interact_wavefronts(_cells(row1, col1), interacting_wf_list);
}

//-------------------------------------------------------------------
// Utility functions
//-------------------------------------------------------------------

//: is this element a virtual belement on this bnd?
bool dbsk2d_ishock_detector::is_belm_virtual_on_bnd(dbsk2d_ishock_belm* belm,
                                                    dbsk2d_lagrangian_cell_bnd_sptr bnd)
{
  //only points can be virtual
  if (belm->is_a_point()){
    dbsk2d_ishock_bpoint* real_endpt = ((dbsk2d_ishock_bpoint*)belm)->bnd_vertex()->bpoint();
    //is it a valid virtual endpoint (is on the current bnd) ?
    if (real_endpt != belm && bnd->is_pt_on_bnd(real_endpt->pt()))   
      return true;
  }

  return false;
}

//-------------------------------------------------------------------
// DEBUG functions
//-------------------------------------------------------------------

//: single propagation step for detailed debugging
void dbsk2d_ishock_detector::propagate_next_active_shock()
{
  _cells(_cur_cell_row, _cur_cell_col).DebugPrintOnePropagation (0,
    _cells(_cur_cell_row, _cur_cell_col).propagate_next_active_shock());
}

//: A bunch of steps to speed up debugging
void dbsk2d_ishock_detector::propagate_a_bunch_of_shocks ()
{
  _cells(_cur_cell_row, _cur_cell_col).propagate_a_bunch_of_shocks();
}

//: merge the next pair of cells in the merge schedule
void dbsk2d_ishock_detector::merge_next_scheduled_cells()
{
  // This function mimics the merge schedule but in single steps 
  // instead of a single loop
  if ( _row_offset < 2*_boundary->num_rows() || 
       _col_offset < 2*_boundary->num_cols())
  { 
    if (_next_cell_col<_boundary->num_cols())
    {
      int neigh_row, neigh_col;
      if (_merging_cols){
        neigh_row = _next_cell_row;
        neigh_col = _next_cell_col+_col_offset/2;
      }
      else {
        neigh_row = _next_cell_row+_row_offset/2;
        neigh_col = _next_cell_col;
      }

      //if these are valid cells, merge
      if (neigh_row < _boundary->num_rows() && 
          neigh_col < _boundary->num_cols())
      {
        _cur_cell_row = _next_cell_row;
        _cur_cell_col = _next_cell_col;

        // make sure the shocks in the two cells are propagated
        // before merging cells
        if (_cells(_cur_cell_row, _cur_cell_col).number_of_active_shocks()>0 ||
            _cells(_cur_cell_row, _cur_cell_col).number_of_candidate_sources()>0)
          _cells(_cur_cell_row, _cur_cell_col).propagate_shocks();

        if (_cells(neigh_row, neigh_col).number_of_active_shocks()>0 ||
            _cells(neigh_row, neigh_col).number_of_candidate_sources()>0)
          _cells(neigh_row, neigh_col).propagate_shocks();

        #ifdef DEBUG_SHOCK_VERBOSE
          vcl_cout << "\n=========================================================\n";
          vcl_cout << "Merging Cells: (" << 
            _cur_cell_row << ", " << _cur_cell_col << ")-(" << 
            neigh_row << ", " << neigh_col << ") " << vcl_endl;
        #endif

        //now merge the cells
        merge_cells(_cur_cell_row, _cur_cell_col, neigh_row, neigh_col);

        #ifdef DEBUG_SHOCK_VERBOSE
          vcl_cout << "\n=========================================================\n";
        #endif
      }
      else {
        //increment cur_col
        _next_cell_col+=_col_offset;

        merge_next_scheduled_cells(); //makes it recursive but should terminate
        return;
      }
      
      //increment cur_col
      _next_cell_col+=_col_offset;
    }
    else {
      _next_cell_col=0;
      _next_cell_row+=_row_offset; //increment cur row
      
      if (_next_cell_row>=_boundary->num_rows())
      {
        if (_merging_cols){
          _row_offset*=2; //increment row offset
          _merging_cols = false;
        }
        else {
          _col_offset*=2;//increment column offset
          _merging_cols = true;
        }
        _next_cell_row = 0;
      }

      merge_next_scheduled_cells(); //makes it recursive but should terminate
      return;
    }
  }
  else {
    #ifdef DEBUG_SHOCK_VERBOSE
    vcl_cout << "Only one cell left!" << vcl_endl;
    #endif
  }

}

//: validate the computation
bool dbsk2d_ishock_detector::validate_shocks()
{
  bool shocks_valid = true;

  //first step in validation is to see that the wavefront information
  //exists and is correct
  for ( dbsk2d_ishock_graph::edge_iterator curE = _ishock_graph->all_edges().begin();
        curE != _ishock_graph->all_edges().end();
        curE++ ) 
  {
    dbsk2d_ishock_edge* sedge = (*curE);

    //make sure that the wavefront structure is still valid
    if (!sedge->lShock() || !sedge->rShock()){
      shocks_valid = false;
      #ifdef DEBUG_SHOCK_VERBOSE
        vcl_cout << "S:" << sedge->id() << " wavefronts invalid. \n";
      #endif
      break;
    }

    //make sure that the edges terminate at an intersection
    //either at the cell boundary or another edge
    if (!(sedge->cSNode() || sedge->cell_bnd())){
      shocks_valid = false;
      #ifdef DEBUG_SHOCK_VERBOSE
        vcl_cout << "S:" << sedge->id() << "did not intersect. \n";
      #endif
    }
  }

  #ifdef DEBUG_SHOCK_VERBOSE
    if (shocks_valid)
      vcl_cout << "shocks validated!" << vcl_endl;
    else
      vcl_cout << "shocks invalid!" << vcl_endl;
  #endif

  dbsk2d_assert(shocks_valid);

  if (!shocks_valid){
    vcl_cout << "Shocks computation produced invalid shocks." <<vcl_endl;
    clear();
  }

  return shocks_valid;
}

