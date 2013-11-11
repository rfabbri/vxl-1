// This is brcv/shp/dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.cxx

//:
// \file This file contains all the shock initialization functions
//       because the other file got too big.

#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>

//-------------------------------------------------------------------
// Shock initialization functions
//-------------------------------------------------------------------

void dbsk2d_lagrangian_ishock_detector::initialize_shocks ()
{
  try {
    //1)if this cell only has a singular point, asign this belm as a SWF
    //  on all the boundaries
    if (_bnd_cell->num_belms()==1){
      dbsk2d_ishock_belm* swf_belm = _bnd_cell->belms().front();
      _left_bnd->set_swf(swf_belm);
      _right_bnd->set_swf(swf_belm);
      _top_bnd->set_swf(swf_belm);
      _bottom_bnd->set_swf(swf_belm);
    }

    //2)Initialize contacts and A3 shocks
    initialize_contacts_and_A3s();

    //3)Setup array of BElements for loop speedup.
    dbsk2d_ishock_belm** BElmArray = new dbsk2d_ishock_belm*[_bnd_cell->num_belms()];

    int sizeBElmArray=0;
    for (dbsk2d_bnd_cell::belm_iterator bit = _bnd_cell->belms().begin();
         bit != _bnd_cell->belms().end(); ++bit)
    {
      //only put singular points within the list for init loop
      if ((*bit)->is_a_point()){
        if (((dbsk2d_ishock_bpoint*)(*bit))->is_a_free_point()){
          BElmArray[sizeBElmArray] = *bit;
          sizeBElmArray++;
        }
      }
      else if ((*bit)->is_a_GUIelm()){
        BElmArray[sizeBElmArray] = *bit; //all  GUIelms can go into the list
        sizeBElmArray++;
      }
    }

    //4) Set up arrays of belms for initialization loops
    
    //  Depending on the InitType, the appropriate list of boundary elements is picked.
    //
    //  ArrayA :: List of all new elements
    //                1)For first initialization: all BElements
    //                2)Dynamic adding: all new elements
    //
    //  ArrayB :: List of all possible paired elements
    //                1)Brute-Force: all BElements
    dbsk2d_ishock_belm **ArrayA=0, **ArrayB=0;
    int sizeArrayA=0, sizeArrayB=0;

    //O(n^2) All candidate sources Initilization
    ArrayA = BElmArray;
    ArrayB = BElmArray;
    sizeArrayA = sizeArrayB = sizeBElmArray;
    
    //5) C(N,2), initialize candidate sources between all pairs of generators
    for (int a=sizeArrayA-1; a>=0; a--) 
      for (int b = a-1; b>=0; b--) 
        init_cand_src_between(ArrayA[a], ArrayB[b]); //initialize a candidate source between this pair

    //delete the temporary array of belements
    delete []BElmArray;

    #ifdef DEBUG_SHOCK_VERBOSE
      vcl_cout << candidate_src_list.size() << " cand. srcs!" << vcl_endl;
    #endif
  }

  catch (const dbsk2d_exception_topology_error &e)
  {
    vcl_cout << e.what() << vcl_endl;
    
    //this shock is no longer valid so delete the shock graph completely
    clear();
  }
}

//: Initialize all contact shocks and A3 shock nodes
//  EPSILONISSUE (in determining is lines are collinear)
void dbsk2d_lagrangian_ishock_detector::initialize_contacts_and_A3s()
{
  //go through all the line segments and arc segments
  //Initialize A3s if there is a corner (tangent discontinuity is negative)
  //else initialize contacts

  //Note:
  //   Due to the design of the shock map data structure at a bpoint
  //   the right contacts have to be initialized first to set the 
  //   reference vectors correctly.
  //   Hence the 2 pass initialization (RIGHT side first)

  // 1) First pass (right side)
  for (dbsk2d_bnd_cell::belm_iterator bit = _bnd_cell->belms().begin();
    bit != _bnd_cell->belms().end(); ++bit)
  {
    if ((*bit)->is_a_curve()){
      dbsk2d_ishock_bcurve* lbcurve = (dbsk2d_ishock_bcurve*)(*bit);

      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(lbcurve->e_pt()->pt()))
        continue;

      //Now look at the element connected to this curve
      if (lbcurve->e_pt()->is_an_end_point()){
        form_a_contact_shock(lbcurve, lbcurve->e_pt());
      }
      else {
        dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)
          lbcurve->e_pt()->getElmToTheRightOf(lbcurve);

        //get the tangents of the elements at this junction
        double left_tangent, right_tangent;
        if (lbcurve->is_a_line())
          left_tangent = ((dbsk2d_ishock_bline*)lbcurve)->u();
        else
          left_tangent = ((dbsk2d_ishock_barc*)lbcurve)->OutTangentAtEndPt();

        if (rbcurve->is_a_line())
          right_tangent = ((dbsk2d_ishock_bline*)rbcurve)->u();
        else
          right_tangent = ((dbsk2d_ishock_barc*)rbcurve)->InTangentAtStartPt();

        //decide whether a contact or A3 needs to be created at this junction
        double theta = CCW(left_tangent, right_tangent);
        if (_isEqAngle(theta, 0, CONTACT_EPSILON))
        {
          //the lines are colinear: initialize a line-line contact
          form_a_contact_shock(lbcurve, rbcurve);
        }
        //EPSILONISSUE (co-tangent curves: when is it valid to create an A3?)
        else if (AisL(theta,vnl_math::pi))
        {
          //form an A3 shock
          form_a_corner_a3(lbcurve, rbcurve);
        }
        else { 
          //form contact shocks for both curves
          form_a_contact_shock(lbcurve, lbcurve->e_pt());
          dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
          if (lcontact){
            //Need to actively set the max_eta (wavefront limit) 
            //for the visible portion of the endpoint
            rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
          }
        }
      }
    }
  }

  // 2) Second pass (left side)
  for (dbsk2d_bnd_cell::belm_iterator bit = _bnd_cell->belms().begin();
    bit != _bnd_cell->belms().end(); ++bit)
  {
    if ((*bit)->is_a_curve())
    {
      dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)(*bit);

      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(rbcurve->start()))
        continue;

      if (rbcurve->s_pt()->is_an_end_point()){
        dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
        if (lcontact){
          //Again, we need to actively set the max_eta (wavefront limit) 
          //for the visible portion of the endpoint (Assumes that the min_eta has
          //already been set in the first pass)
          rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
        }
      }
      else {
        //yet another task assigned for this second pass
        //is to decide if junction points are invisible
        //Condition: if this is a junction point and reference vec (_vref) 
        //           of the point is still not set, then this point is invisible
        if (rbcurve->s_pt()->vref()==-1.0)
          rbcurve->s_pt()->set_visibility(false);
      }
    }
  }

}


//: Initialize all contact shocks and A3 shock nodes
//  EPSILONISSUE (in determining is lines are collinear)
void dbsk2d_lagrangian_ishock_detector::initialize_contacts_and_A3s(
    vcl_vector<dbsk2d_ishock_belm*> belm_list)
{
  //go through all the line segments and arc segments
  //Initialize A3s if there is a corner (tangent discontinuity is negative)
  //else initialize contacts

  //Note:
  //   Due to the design of the shock map data structure at a bpoint
  //   the right contacts have to be initialized first to set the 
  //   reference vectors correctly.
  //   Hence the 2 pass initialization (RIGHT side first)

  // 1) First pass (right side)
  vcl_vector<dbsk2d_ishock_belm*>::iterator bit;
  bit = belm_list.begin();
  for ( ; bit != belm_list.end() ; ++bit)
  {
    if ((*bit)->is_a_curve()){
      dbsk2d_ishock_bcurve* lbcurve = (dbsk2d_ishock_bcurve*)(*bit);
     
      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(lbcurve->e_pt()->pt()))
          continue;

      //Now look at the element connected to this curve
      if (lbcurve->e_pt()->is_an_end_point()){
        form_a_contact_shock(lbcurve, lbcurve->e_pt());
      }
      else {
        dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)
          lbcurve->e_pt()->getElmToTheRightOf(lbcurve);

        //get the tangents of the elements at this junction
        double left_tangent, right_tangent;
        if (lbcurve->is_a_line())
          left_tangent = ((dbsk2d_ishock_bline*)lbcurve)->u();
        else
          left_tangent = ((dbsk2d_ishock_barc*)lbcurve)->OutTangentAtEndPt();

        if (rbcurve->is_a_line())
          right_tangent = ((dbsk2d_ishock_bline*)rbcurve)->u();
        else
          right_tangent = ((dbsk2d_ishock_barc*)rbcurve)->InTangentAtStartPt();

        //decide whether a contact or A3 needs to be created at this junction
        double theta = CCW(left_tangent, right_tangent);
        if (_isEqAngle(theta, 0, CONTACT_EPSILON))
        {
          //the lines are colinear: initialize a line-line contact
          form_a_contact_shock(lbcurve, rbcurve);
        }
        //EPSILONISSUE (co-tangent curves: when is it valid to create an A3?)
        else if (AisL(theta,vnl_math::pi))
        {
          //form an A3 shock
          form_a_corner_a3(lbcurve, rbcurve);
        }
        else { 
          //form contact shocks for both curves
          form_a_contact_shock(lbcurve, lbcurve->e_pt());
          dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
          if (lcontact){
            //Need to actively set the max_eta (wavefront limit) 
            //for the visible portion of the endpoint
            rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
          }
        }
      }
    }
  }

  // 2) Second pass (left side)
  bit = belm_list.begin();
  for ( ; bit != belm_list.end() ; ++bit)
  {
    if ((*bit)->is_a_curve())
    {
      dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)(*bit);

      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(rbcurve->start()))
          continue;

      if (rbcurve->s_pt()->is_an_end_point()){
        dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
        if (lcontact){
          //Again, we need to actively set the max_eta (wavefront limit) 
          //for the visible portion of the endpoint (Assumes that the min_eta has
          //already been set in the first pass)
          rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
        }
      }
      else {
        //yet another task assigned for this second pass
        //is to decide if junction points are invisible
        //Condition: if this is a junction point and reference vec (_vref) 
        //           of the point is still not set, then this point is invisible
        if (rbcurve->s_pt()->vref()==-1.0)
          rbcurve->s_pt()->set_visibility(false);
      }
    }
  }

}

//: Initialize all contact shocks and A3 shock nodes
//  EPSILONISSUE (in determining is lines are collinear)
void dbsk2d_lagrangian_ishock_detector::initialize_contacts_and_A3s_recompute()
{
  //go through all the line segments and arc segments
  //Initialize A3s if there is a corner (tangent discontinuity is negative)
  //else initialize contacts

  //Note:
  //   Due to the design of the shock map data structure at a bpoint
  //   the right contacts have to be initialized first to set the 
  //   reference vectors correctly.
  //   Hence the 2 pass initialization (RIGHT side first)

  // 1) First pass (right side)
  for (dbsk2d_bnd_cell::belm_iterator bit = _bnd_cell->belms().begin();
    bit != _bnd_cell->belms().end(); ++bit)
  {
    if (_boundary->belm_off((*bit)->id()))
    {
        continue;
    }

    if ((*bit)->is_a_curve()){
      dbsk2d_ishock_bcurve* lbcurve = (dbsk2d_ishock_bcurve*)(*bit);

      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(lbcurve->e_pt()->pt()))
        continue;

      //Now look at the element connected to this curve
      if (lbcurve->e_pt()->is_an_end_point()){
        form_a_contact_shock(lbcurve, lbcurve->e_pt());
      }
      else {
        dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)
          lbcurve->e_pt()->getElmToTheRightOf(lbcurve);

        //get the tangents of the elements at this junction
        double left_tangent, right_tangent;
        if (lbcurve->is_a_line())
          left_tangent = ((dbsk2d_ishock_bline*)lbcurve)->u();
        else
          left_tangent = ((dbsk2d_ishock_barc*)lbcurve)->OutTangentAtEndPt();

        if (rbcurve->is_a_line())
          right_tangent = ((dbsk2d_ishock_bline*)rbcurve)->u();
        else
          right_tangent = ((dbsk2d_ishock_barc*)rbcurve)->InTangentAtStartPt();

        //decide whether a contact or A3 needs to be created at this junction
        double theta = CCW(left_tangent, right_tangent);
        if (_isEqAngle(theta, 0, CONTACT_EPSILON))
        {
          //the lines are colinear: initialize a line-line contact
          form_a_contact_shock(lbcurve, rbcurve);
        }
        //EPSILONISSUE (co-tangent curves: when is it valid to create an A3?)
        else if (AisL(theta,vnl_math::pi))
        {
          //form an A3 shock
          form_a_corner_a3(lbcurve, rbcurve);
        }
        else { 
          //form contact shocks for both curves
          form_a_contact_shock(lbcurve, lbcurve->e_pt());
          dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
          if (lcontact){
            //Need to actively set the max_eta (wavefront limit) 
            //for the visible portion of the endpoint
            rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
          }
        }
      }
    }
  }

  // 2) Second pass (left side)
  for (dbsk2d_bnd_cell::belm_iterator bit = _bnd_cell->belms().begin();
    bit != _bnd_cell->belms().end(); ++bit)
  {

    if (_boundary->belm_off((*bit)->id()))
    {
        continue;
    }

    if ((*bit)->is_a_curve())
    {
      dbsk2d_ishock_bcurve* rbcurve = (dbsk2d_ishock_bcurve*)(*bit);

      //ignore this point if it is not inside the current cell
      if (!this->is_point_inside_the_cell(rbcurve->start()))
        continue;

      if (rbcurve->s_pt()->is_an_end_point()){
        dbsk2d_ishock_contact* lcontact = form_a_contact_shock(rbcurve->s_pt(), rbcurve);
        if (lcontact){
          //Again, we need to actively set the max_eta (wavefront limit) 
          //for the visible portion of the endpoint (Assumes that the min_eta has
          //already been set in the first pass)
          rbcurve->s_pt()->set_max_eta(lcontact->LsEta());
        }
      }
      else {
        //yet another task assigned for this second pass
        //is to decide if junction points are invisible
        //Condition: if this is a junction point and reference vec (_vref) 
        //           of the point is still not set, then this point is invisible
        if (rbcurve->s_pt()->vref()==-1.0)
          rbcurve->s_pt()->set_visibility(false);
      }
    }
  }

}

//: Initialize candidate source between these two generators
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_belm* belm1, 
                                                         dbsk2d_ishock_belm* belm2)
{
  if (belm1==belm2) //nonsensical wavefront interaction
    return; 

  switch (belm1->type()) 
  {
  case BPOINT: 
    switch (belm2->type()) 
    {
    case BPOINT: 
      init_cand_src_between((dbsk2d_ishock_bpoint*)belm1, (dbsk2d_ishock_bpoint*)belm2);
      return;
    case BLINE: 
      init_cand_src_between((dbsk2d_ishock_bpoint*)belm1, (dbsk2d_ishock_bline*)belm2);
      return;
    case BARC:
      init_cand_src_between((dbsk2d_ishock_bpoint*)belm1, (dbsk2d_ishock_barc*)belm2);
      return;
    }
  case BLINE: 
    switch (belm2->type()) 
    {
    case BPOINT:
      init_cand_src_between((dbsk2d_ishock_bpoint*)belm2, (dbsk2d_ishock_bline*)belm1);              
      return;
    case BLINE:
      init_cand_src_between((dbsk2d_ishock_bline*)belm1, (dbsk2d_ishock_bline*)belm2);              
      return;
    case BARC:
      init_cand_src_between((dbsk2d_ishock_bline*)belm1, (dbsk2d_ishock_barc*)belm2);
      return;
    }
  case BARC: 
    switch (belm2->type()) 
    {
    case BPOINT:
      init_cand_src_between((dbsk2d_ishock_bpoint*)belm2, (dbsk2d_ishock_barc*)belm1);              
      return;
    case BLINE:  
      init_cand_src_between((dbsk2d_ishock_bline*)belm2, (dbsk2d_ishock_barc*)belm1);
      return;
    case BARC:
      init_cand_src_between((dbsk2d_ishock_barc*)belm1, (dbsk2d_ishock_barc*)belm2);          
      return;
    }       
  }
}

//: initialize candidate source between two points
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_bpoint* bp1, 
                                                         dbsk2d_ishock_bpoint* bp2)
{
  //1) compute the etas (actually just vectors)
  double eta1 = _vPointPoint (bp1->pt(), bp2->pt());
  double eta2 = angle0To2Pi(eta1+vnl_math::pi);

  vgl_point_2d<double> midPoint = _midPointPoint (bp1->pt(), bp2->pt());
  double start_time = _distPointPoint (bp1->pt(), bp2->pt())/2;

  //2) initialize candidate source
  dbsk2d_ishock_node* cand_src = new dbsk2d_ishock_node(
                                      ishock_graph()->nextAvailableID(),
                                      start_time, midPoint);

  //3) put the boundary elements into the list
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, bp1));
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, bp2));

  //4) insert the candidate source into the list
  candidate_src_list.insert(time_src_pair(start_time, cand_src));
}

//: Initialize candidate source between a point and a line
// EPSILONISSUE: near degeneracy check using the eta parameter
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_bpoint* bp, 
                                                         dbsk2d_ishock_bline* bl)
{
  //if the point is an endpoint of the line it can only create a contact shock,
  //so no sources need to be initialized
  if (bp==bl->e_pt() || bp==bl->s_pt())
    return;

  if (!bp->is_visible()) //when this is used 
    return;

  //Note: 
  //candidate sources for the twin line is initialized too 
  //since only the GUIElms are on the list

  dbsk2d_ishock_belm *belm1, *belm2;
  double eta1, eta2;
  vgl_point_2d<double> midPoint;
  double start_time;

  //1) determine the half line the point is interacting with
  dbsk2d_ishock_bline* hl;
  if (_isPointAboveLine(bp->pt(), bl->start(), bl->end()))
    hl = bl; //point_is_above_GUI_line
  else
    hl = bl->twinLine();

  //2) compute the projection of the point on to the line
  eta2 = _deltaPointLine (bp->pt(), hl->start(), hl->end(), hl->l());
  
  //2-1) if the point projects outside the line closer to the start point
  if (LisL(eta2, 0))
  {
    //   .        
    //                  
    //     .--->--   
    //

    //compute the Lsp->P vector
    double pp_vec = _vPointPoint (hl->start(), bp->pt());

    //compute the angle between the pp_vec and the contact
    double lsp_angle = CCW(hl->n(), pp_vec);

    midPoint = _midPointPoint (bp->pt(), hl->start());
    start_time = _distPointPoint (bp->pt(), hl->start())/2;

    //if PP is not degenerate,  form a PP source else form a PL source
    if (_isG(lsp_angle, 0, D_EPSILON))
    {
      //PP is not possible if Lsp is not visible
      if (!hl->s_pt()->is_visible())
        return;

      //prepare to form a PP source
      belm1 = bp;
      belm2 = hl->s_pt();
      eta1 = angle0To2Pi(pp_vec+vnl_math::pi);
      eta2 = pp_vec;
    }
    else {
      //prepare to form a PL source
      belm1 = bp;
      belm2 = hl;
      //will need to correct eta1 and starttime at some point (for PL)
      eta1 = angle0To2Pi(pp_vec+vnl_math::pi);
      eta2 = 0;
    }
  }
  //2-2) if the point projects outside the line closer to the end point
  else if (LisG(eta2, bl->l()))
  {
    //            .        
    //                  
    //   --->--.   
    //

    //compute the Lep->P vector
    double pp_vec = _vPointPoint (hl->end(), bp->pt());
    
    //compute the angle between the pp_vec and the contact
    double lep_angle = CCW(pp_vec, hl->n());

    midPoint = _midPointPoint (bp->pt(), hl->end());
    start_time = _distPointPoint (bp->pt(), hl->end())/2;

    //if PP is not degenerate,  form a PP source else form a PL source
    if (_isG(lep_angle, 0, D_EPSILON))
    {
      //PP is not possible if Lep is not visible
      if (!hl->e_pt()->is_visible())
        return;

      //prepare to form a PP source
      belm1 = bp;
      belm2 = hl->e_pt();
      eta1 = angle0To2Pi(pp_vec+vnl_math::pi);
      eta2 = pp_vec;
    }
    else {
      //prepare to form a PL source
      belm1 = bp;
      belm2 = hl;
      //will need to correct eta1 and starttime at some point (for PL)
      eta1 = angle0To2Pi(pp_vec+vnl_math::pi);
      eta2 = hl->l();
    }
  }
  //2-3) the point projects to the valid portion of the line
  else 
  {
    //      .        
    //                  
    //   .--->--.   
    //

    vgl_point_2d<double> footPt = _getFootPt (bp->pt(), hl->start(), hl->end());

    midPoint = _midPointPoint (bp->pt(), footPt);
    start_time = _distPointLine (bp->pt(), hl->start(), hl->end())/2;

    //prepare to form a PL source
    belm1 = bp;
    belm2 = hl;
    eta1 = _vPointLine (bp->pt(), hl->start(), hl->end());
    if (eta2<0)
      eta2 = 0;
    if (eta2>hl->l())
      eta2 = hl->l();
  }

  //for the line endpoint, we can do a wavefront check
  if (belm2->is_a_point()){
    if (!belm2->is_wavefront_alive(eta2, 0))
      return;
  }

  //3-a)Initialize the new candidate source
  dbsk2d_ishock_node* cand_src = new dbsk2d_ishock_node (
                  ishock_graph()->nextAvailableID(), 
                  start_time, midPoint);
  
  //3-b) put the boundary elements into the list
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, belm1));
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, belm2));
  
  //4) insert the candidate source into the list
  candidate_src_list.insert(time_src_pair(start_time, cand_src));
}

//: initialize candidate source between a point and an arc
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_bpoint* bp, 
                                                         dbsk2d_ishock_barc* ba)
{
  //if the point is an endpoint of the arc it can only create a contact shock,
  //so no sources need to be initialized
  if (bp==ba->e_pt() || bp==ba->s_pt())
    return;
  
  //if (!bp->is_visible()) //when this is used 
  //  return;

  //Note: 
  //candidate sources for the twin arc is initialized too 
  //since only the GUIElms are on the list

  dbsk2d_ishock_belm *belm1, *belm2;
  double eta1, eta2;
  vgl_point_2d<double> midPoint;
  double start_time;

  //1) determine the half arc the point is interacting with
  dbsk2d_ishock_barc* ha;
  double d = _distPointPoint(bp->pt(), ba->center());

  if (d>ba->R()) // interacting with the outer arc 
    ha = ba;     // GUIElm is always the one on the outside
  else
    ha = ba->twinArc();

  //1D) detect P-A degeneracy: when the point is at the center of the arc
  if (_isEq(d,0, D_EPSILON))
  {
    //need to instantiate a P-A-TO source here

    //1D-a) determine the correct etas
    //instantiate it from the center of the arc
    eta2 = (ha->min_eta()+ha->max_eta())/2;   
    eta1 = ha->eta_to_vec(eta2);

    if (!bp->is_wavefront_alive(eta1, 0))
      return;

    vgl_point_2d<double> footPt = _translatePoint (ha->center(), eta1, ha->R());
    midPoint = _midPointPoint (bp->pt(), footPt);
    start_time = _distPointPoint (bp->pt(), footPt)/2;

    //1D-b)Initialize the new candidate source
    dbsk2d_ishock_node* cand_src = new dbsk2d_ishock_node (
                    ishock_graph()->nextAvailableID(), 
                    start_time, midPoint);
    
    //1D-c) put the boundary elements into the list
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, bp));
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, ha));
    
    //1D-d) insert the candidate source into the list
    candidate_src_list.insert(time_src_pair(start_time, cand_src));

    return;
  }

  //2) compute the vector from the point to the arc
  eta1 = _vPointPoint (bp->pt(), ha->center());
  eta2 = ha->vec_to_eta(angle0To2Pi(eta1+vnl_math::pi));
  
  //2-1) the point projects to the valid portion of the arc segment
  if (AisGEq(eta2, 0) && AisLEq(eta2, ha->max_eta())) 
  {
    //      .        
    //                  
    //   .--->--.   
    //

    VECTOR_TYPE ap_vec = _vPointPoint(ha->center(), bp->pt());

    vgl_point_2d<double> footPt = _translatePoint (ha->center(), ap_vec, ha->R());
    midPoint = _midPointPoint (bp->pt(), footPt);
    start_time = _distPointPoint (bp->pt(), footPt)/2;

    //prepare to form a PA source
    belm1 = bp;
    belm2 = ha;

    //if the arc is the inner arc, eta1 needs to be reversed to point to the source
    if (ha->is_inner_arc())
      eta1 = angle0To2Pi(eta1+vnl_math::pi);

    //for the first point, do a wavefront check (if this is an endpoint of some bcurve, it might not be valid)
    if (!belm1->is_wavefront_alive(eta1, 0))
      return;

    //for the arc endpoint, we can do a wavefront check
    if (belm2->is_a_point()){
      if (!belm2->is_wavefront_alive(eta2, 0))
        return;
    }

    //2-1-a)Initialize the new candidate source
    dbsk2d_ishock_node* cand_src = new dbsk2d_ishock_node (
                    ishock_graph()->nextAvailableID(), 
                    start_time, midPoint);
    
    //2-1-b) put the boundary elements into the list
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, belm1));
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, belm2));
    
    //2-1-c) insert the candidate source into the list
    candidate_src_list.insert(time_src_pair(start_time, cand_src));
  }
  //2-2) if the point projects outside the arc segment, form a P-P source with both endpoints
  else {
    init_cand_src_between(bp, ha->s_pt());
    init_cand_src_between(bp, ha->e_pt());
  }
}

//: initialize candidate source between two line segments
void
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_bline* bl1, 
                                                         dbsk2d_ishock_bline* bl2)
{
  //1) if the lines are connected, only contact shocks can form,
  //so no sources need to be initialized
  if (bl1->s_pt()==bl2->s_pt() || bl1->s_pt()==bl2->e_pt() ||
      bl1->e_pt()==bl2->s_pt() || bl1->e_pt()==bl2->e_pt() )
      return;

  //Note: 
  //candidate sources for the twin line is initialized too 
  //since only the GUIElms are on the list

  //2) which two half lines are facing one another?
  dbsk2d_ishock_bline *hl1, *hl2;

  VECTOR_TYPE l1s_to_l2s = _vPointPoint(bl1->start(), bl2->start());
  if (CCW(l1s_to_l2s, bl1->u())<vnl_math::pi)
    hl1 = bl1->twinLine();
  else
    hl1 = bl1;

  if (CCW(l1s_to_l2s, bl2->u())<vnl_math::pi)
    hl2 = bl2;
  else
    hl2 = bl2->twinLine();
  
  //3) get candidate sources from all PL combinations

  //3-1) From line1->start to line2
  init_cand_src_between(hl1->s_pt(), hl2);

  //3-2) From line1->end to line2
  init_cand_src_between(hl1->e_pt(), hl2);

  //3-3) From line2->start to line1
  init_cand_src_between(hl2->s_pt(), hl1);
  
  //3-4) From line2->end to line1
  init_cand_src_between(hl2->e_pt(), hl1);

  //4) Create a Line-Line source if the lines are parallel

  //EPSILONISSUE: whether two lines are parallel to create a line-line source which will create third-order shocks
  double theta = CCW(hl1->u(), hl2->u());
  if (_isEq(theta,vnl_math::pi, TO_EPSILON))
  {
    dbsk2d_ishock_node* cand_src = 0;
    double eta1=0, eta2=0;
    vgl_point_2d<double> midPoint;
    double start_time=0;
    
    double Al_eta = _deltaPointLine (hl1->start(), hl2->start(), hl2->end(), hl2->l());
    double Bl_eta = _deltaPointLine (hl1->end(), hl2->start(), hl2->end(), hl2->l());

    if (Al_eta <0 || Bl_eta > hl2->l())
      return; //no overlap of parallel lines hence no shocks possible between them

    if (Al_eta > hl2->l())
      Al_eta = hl2->l();

    if (Bl_eta < 0)
      Bl_eta = 0;

    //Now initialize the candidate source at the middle of the overlapping region
    eta2 = (Al_eta + Bl_eta)/2.0;
    vgl_point_2d<double> footpt2 = _translatePoint(hl2->start(), hl2->u(), eta2);

    eta1 = _deltaPointLine (footpt2, hl1->start(), hl1->end(), hl1->l());
    vgl_point_2d<double> footpt1 = _translatePoint(hl1->start(), hl1->u(), eta1);

    midPoint = _midPointPoint(footpt1, footpt2);
    start_time = _distPointPoint(footpt1, footpt2)/2.0;

    //4-a)Initialize the new candidate source
    cand_src = new dbsk2d_ishock_node (
                    ishock_graph()->nextAvailableID(), 
                    start_time, midPoint);
    
    //4-b) put the boundary elements into the list
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, hl1));
    cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, hl2));
    
    //4-c) insert the candidate source into the list
    candidate_src_list.insert(time_src_pair(start_time, cand_src));
  }

}

//: initialize candidate source between a line segment and an arc segment
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_bline* bl, 
                                                         dbsk2d_ishock_barc* ba)
{
  //Note: 
  //candidate sources for the twin elements are initialized too 
  //since only the GUIElms are on the list

  //1) get candidate sources from all PA and PL combinations
  
  // Note: 
  // It does not matter which half arc is considered because 
  // the P-A and P_L initializations automatically pick the right ones

  init_cand_src_between(ba->s_pt(), bl);
  init_cand_src_between(ba->e_pt(), bl);
  init_cand_src_between(bl->s_pt(), ba);
  init_cand_src_between(bl->e_pt(), ba);

  //2) consider the direct interaction between the line and the arc

  vgl_point_2d<double> midPoint;
  double start_time;  
  dbsk2d_ishock_node* cand_src=0;
  double eta1=0, eta2=0;
  dbsk2d_ishock_barc* ha=0;
  dbsk2d_ishock_bline* hl=0; 

  //2a) There can be no sources between a CCW arc and a line segment
  ha = ba; //assuming GUIELm is CW

  //2b) There can only be a source if H>R
  double H = _distPointLine (ba->center(), bl->start(), bl->end());
  if (H < ba->R())
    return;

  //2c) determine the half line the arc is interacting with
  if (_isPointAboveLine(ba->center(), bl->start(), bl->end()))
    hl = bl; //point_is_above_GUI_line
  else
    hl = bl->twinLine();

  //3) compute the etas of the candidate source
  VECTOR_TYPE al_vec = angle0To2Pi(hl->u()-vnl_math::pi/2.0);
  eta1 = ha->vec_to_eta(al_vec);
  eta2 = _deltaPointLine (ha->center(), hl->start(), hl->end(), hl->l());

  if (AisL(eta1, ha->min_eta()) || AisG(eta1, ha->max_eta()) ||
      LisL(eta2, 0) || LisG(eta2, hl->l()))
    return; //wavefronts are not valid

  vgl_point_2d<double> footPtA = _translatePoint (ha->center(), al_vec, ha->R());
  vgl_point_2d<double> footPtL = _getFootPt (ha->center(), hl->start(), hl->end());
  
  midPoint = _midPointPoint (footPtA, footPtL);
  start_time = (H-ha->R())/2.0;

  //correct eta2 
  if (eta2<0)
    eta2 = 0;
  if (eta2>hl->l())
    eta2 = hl->l();

  //4a) Initialize the new candidate source
  cand_src = new dbsk2d_ishock_node (
                  ishock_graph()->nextAvailableID(), 
                  start_time, midPoint);
  
  //4b) put the boundary elements into the list
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, ha));
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, hl));
  
  //5) insert the candidate source into the list
  candidate_src_list.insert(time_src_pair(start_time, cand_src));
}

//: initialize candidate source between two arc segments
void 
dbsk2d_lagrangian_ishock_detector::init_cand_src_between(dbsk2d_ishock_barc* ba1, 
                                                         dbsk2d_ishock_barc* ba2)
{
  //Note: 
  //candidate sources for the twin arc is initialized too 
  //since only the GUIElms are on the list

  //1) get candidate sources from all PA combinations
  
  // Note: 
  // It does not matter which half arc is considered because 
  // the P-A initialization automatically picks the right ones

  init_cand_src_between(ba1->s_pt(), ba2);
  init_cand_src_between(ba1->e_pt(), ba2);
  init_cand_src_between(ba2->s_pt(), ba1);
  init_cand_src_between(ba2->e_pt(), ba1);

  //2) consider the direct interaction between the two arcs

  vgl_point_2d<double> midPoint;
  double start_time;  
  dbsk2d_ishock_node* cand_src=0;
  double eta1=0, eta2=0;
  dbsk2d_ishock_barc *ha1=0, *ha2=0; 

  double H = _distPointPoint (ba1->center(), ba2->center());

  //2a) circles are not intersecting and well separated
  if (LisG(H,ba1->R()+ba2->R()))
  {
    ha1 = ba1; dbsk2d_assert(ba1->nud() == ARC_NUD_CW); //GUIelm should be CW
    ha2 = ba2; dbsk2d_assert(ba2->nud() == ARC_NUD_CW); //GUIelm should be CW

    VECTOR_TYPE aa_vec = _vPointPoint (ha1->center(), ha2->center());

    eta1 = ha1->vec_to_eta(aa_vec);
    eta2 = ha2->vec_to_eta(angle0To2Pi(aa_vec+vnl_math::pi));

    if (AisL(eta1, ha1->min_eta()) || AisG(eta1, ha1->max_eta()) ||
        AisL(eta2, ha2->min_eta()) || AisG(eta2, ha2->max_eta()) )
      return; //wavefronts are not valid

    vgl_point_2d<double> footPt1 = _translatePoint (ha1->center(), aa_vec, ha1->R());
    vgl_point_2d<double> footPt2 = _translatePoint (ha2->center(), aa_vec+vnl_math::pi, ha2->R());
    start_time = _distPointPoint (footPt1, footPt2)/2;
    midPoint = _midPointPoint (footPt1, footPt2);
  }

  //2b) circles are not intersecting, but one is completely and well inside the other
  else if (LisL(H,vcl_fabs(ba1->R()-ba2->R())))
  {
    //make ha1 is the outer circle(inner half arc) and ha2 is the inner circle (outer half arc)
    if (ba1->R()>ba2->R()) { 
      ha1 = ba1->twinArc(); ha2 = ba2;
    }
    else {
      ha1 = ba2->twinArc(); ha2 = ba1;
    }
    dbsk2d_assert(ha1->nud()==ARC_NUD_CW && ha2->nud()==ARC_NUD_CCW);

    VECTOR_TYPE aa_vec = _vPointPoint (ha1->center(), ha2->center());

    eta1 = ha1->vec_to_eta(aa_vec);
    eta2 = ha2->vec_to_eta(aa_vec);

    if (AisL(eta1, ha1->min_eta()) || AisG(eta1, ha1->max_eta()) ||
        AisL(eta2, ha2->min_eta()) || AisG(eta2, ha2->max_eta()) )
      return; //wavefronts are not valid

    vgl_point_2d<double> footPt1 = _translatePoint (ha1->center(), aa_vec, ha1->R());
    vgl_point_2d<double> footPt2 = _translatePoint (ha2->center(), aa_vec, ha2->R());
    start_time = _distPointPoint (footPt1, footPt2)/2;
    midPoint = _midPointPoint (footPt1, footPt2);
  }

  //2c) arcs are intersecting or tangential, there can be no source between them
  else {
    return;
  }

  //3a) Initialize the new candidate source
  cand_src = new dbsk2d_ishock_node (
                  ishock_graph()->nextAvailableID(), 
                  start_time, midPoint);
  
  //3b) put the boundary elements into the list
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta1, ha1));
  cand_src->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(eta2, ha2));
  
  //4) insert the candidate source into the list
  candidate_src_list.insert(time_src_pair(start_time, cand_src));
}

//: form a contact shock between the two elements
dbsk2d_ishock_contact* 
dbsk2d_lagrangian_ishock_detector::form_a_contact_shock (dbsk2d_ishock_belm* lbelm, 
                                                         dbsk2d_ishock_belm* rbelm)
{
  dbsk2d_ishock_contact *newContact=0;
  dbsk2d_ishock_bpoint  *bp1, *bp2;
  dbsk2d_ishock_bline   *bl1, *bl2;
  dbsk2d_ishock_barc    *ba1, *ba2;

  switch (lbelm->type())
  {
  case BPOINT:
    bp1 = (dbsk2d_ishock_bpoint*)lbelm; 
    switch (rbelm->type())
    {
    case BPOINT: //no contact possible
      break;
    case BLINE:  //regular contact
      bl2 = (dbsk2d_ishock_bline*)rbelm;
      
      if (bl2->lContact()) //contact already exists
        break;
      
      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bp1->pt()))
        break;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        bp1, bl2, bp1->pt(), bl2->n(),
                        bl2->n(), bl2->n(), //lstau, rstau
                        bp1->vec_to_eta(bl2->n()), 0); //lseta, rseta

      newContact->setSimTime(ISHOCK_DIST_HUGE);
      
      break;
    case BARC:  //regular contact
      ba2 = (dbsk2d_ishock_barc*)rbelm;

      if (ba2->lContact()) //contact already exists
        break;

      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bp1->pt()))
        break;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        bp1, ba2, bp1->pt(), ba2->StartNormalVector(),
                        ba2->StartNormalVector(), ba2->StartVector(), //lstau, rstau
                        bp1->vec_to_eta(ba2->StartNormalVector()), ba2->min_eta());

      if (ba2->is_inner_arc())
        newContact->setSimTime(ba2->R());
      else
        newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    }
    break;
  case BLINE:
    bl1 = (dbsk2d_ishock_bline*)lbelm; 
    switch (rbelm->type())
    {
    case BPOINT:  //regular contact
      bp2 = (dbsk2d_ishock_bpoint*)rbelm;

      if (bl1->rContact()) //contact already exists
        break;
      
      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bp2->pt()))
        break;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        bl1, bp2, bp2->pt(), bl1->n(), 
                        bl1->n(), bl1->n(), //lstau, rstau
                        bl1->l(), bp2->vec_to_eta(bl1->n()));

      newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    case BLINE:  //degenerate contact (EPSILONISSUE: contact tangent is not accurate) 
      bl2 = (dbsk2d_ishock_bline*)rbelm;

      if (bl2->lContact()) //contact already exists
        break;

      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bl1->end()))
        break;
      
      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        bl1, bl2, bl1->e_pt()->pt(), bl1->n(),
                        bl1->n(), bl2->n(), //lstau , rstau
                        bl1->l(), 0);//etas are crisp

      newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    case BARC:  //degenerate contact (EPSILONISSUE: contact tangent is not accurate)
      ba2 = (dbsk2d_ishock_barc*)rbelm;

      if (ba2->lContact()) //contact already exists
        break;

      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bl1->end()))
        break;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        bl1, ba2, bl1->e_pt()->pt(), bl1->n(), 
                        bl1->n(), ba2->StartVector(), //lstau, rstau
                        bl1->l(), ba2->min_eta());//etas are crisp

      if (ba2->is_inner_arc())
        newContact->setSimTime(ba2->R());
      else 
        newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    }
    break;
  case BARC:
    ba1 = (dbsk2d_ishock_barc*)lbelm; 
    switch (rbelm->type())
    {
    case BPOINT:  //regular contact
      bp2 = (dbsk2d_ishock_bpoint*)rbelm;

      if (ba1->rContact()) //contact already exists
        break;
      
      //shock starts outside the current cell
      if (!is_point_inside_the_cell(bp2->pt()))
        break;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        ba1, bp2, bp2->pt(), ba1->EndNormalVector(),
                        ba1->EndVector(), ba1->EndNormalVector(), //lstau, rstau
                        ba1->max_eta(), bp2->vec_to_eta(ba1->EndNormalVector()));

      if (ba1->is_inner_arc())
        newContact->setSimTime(ba1->R());
      else
        newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    case BLINE:  //degenerate contact (EPSILONISSUE: contact tangent is not accurate)
      bl2 = (dbsk2d_ishock_bline*)rbelm;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        ba1, bl2, bl2->s_pt()->pt(), bl2->n(), 
                        ba1->EndVector(), bl2->n(), //lstau, rstau
                        ba1->max_eta(), 0);//etas are crisp

      if (ba1->is_inner_arc())
        newContact->setSimTime(ba1->R());
      else 
        newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    case BARC:  //degenerate contact (EPSILONISSUE: contact tangent is not accurate)
      ba2 = (dbsk2d_ishock_barc*)rbelm;

      newContact = new dbsk2d_ishock_contact (ishock_graph()->nextAvailableID(), 
                        ba1, ba2, ba1->end(), ba1->EndNormalVector(), 
                        ba1->EndVector(), ba2->StartVector(), //lstau, rstau
                        ba1->max_eta(), ba2->min_eta());//etas are crisp

      if (ba1->is_inner_arc() && ba2->is_inner_arc())
        newContact->setSimTime(ba1->R()<ba2->R()? ba1->R(): ba2->R());
      else if (ba1->is_inner_arc())
        newContact->setSimTime(ba1->R());
      else if (ba2->is_inner_arc())
        newContact->setSimTime(ba2->R());
      else
        newContact->setSimTime(ISHOCK_DIST_HUGE);

      break;
    }
  }

  if (newContact){

    add_an_ishock_edge (newContact);

    //Note: 
    //The propagated flag is set to true for contacts. This is a very important 
    //step because it will never allow contacts to propagate on their own. 
    //
    //The most important effect of this is when a shock is being propagated
    //to the next intersection, etas are used to determine whether it is degenerate or not.
    //At this stage, since the etas on Contact shocks are always degenerate, this test is 
    //not sufficient. 
    newContact->setPropagated(true);

    //To deal with Arc-contact intersection issues, we need to make the two contacts
    //at the ends of inner arcs neighbors of each other so that they do not have to 
    //intersect later.

    if (newContact->lBElement()->is_an_arc()){
      dbsk2d_ishock_barc* ba = (dbsk2d_ishock_barc*) newContact->lBElement();
      
      if (ba->is_inner_arc()){
        if (ba->lContact()){ //make them neighbors of each other
          if (RisEq(ba->lContact()->endTime(), newContact->endTime())){
            ba->lContact()->set_rNeighbor(newContact);
            newContact->set_lNeighbor(ba->lContact());
          }
        }
      }
    }

    if (newContact->rBElement()->is_an_arc()){
      dbsk2d_ishock_barc* ba = (dbsk2d_ishock_barc*) newContact->rBElement();
      
      if (ba->is_inner_arc()){
        if (ba->rContact()){ 
          if (RisEq(ba->rContact()->endTime(), newContact->endTime())){
            //make them neighbors of each other
            ba->rContact()->set_lNeighbor(newContact);
            newContact->set_rNeighbor(ba->rContact());
          }
        }
      }
    }

    //intersect with all four boundaries
    intersect_with_cell_boundaries(newContact); 
  }
  return newContact;
}

//: Form and A3 node at the discontinuity between two curves
dbsk2d_ishock_node*
dbsk2d_lagrangian_ishock_detector::form_a_corner_a3(dbsk2d_ishock_bcurve* lbcurve, 
                                                    dbsk2d_ishock_bcurve* rbcurve)
{
  vgl_point_2d<double> origin = lbcurve->end();
  
  //if the shock starts outside the current cell, don't consider it
  if (!is_point_inside_the_cell(origin))
    return 0;

  //1) Create an A3 node
  dbsk2d_ishock_node* newA3 = new dbsk2d_ishock_node(
                                    ishock_graph()->nextAvailableID(),
                                    0, origin);

  double lsEta = lbcurve->max_eta();
  double rsEta = rbcurve->min_eta();

  //add the belements to the bnd list
  newA3->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(lsEta, lbcurve));
  newA3->bndList().push_back(vcl_pair<double, dbsk2d_ishock_belm*>(rsEta, rbcurve));

  //add this A3 node to the shock graph
  add_an_ishock_node (newA3);

  //Instatiate a child shock from this node
  dbsk2d_ishock_edge* newShock = propagate_from_a_node(newA3, NULL, NULL, lbcurve, rbcurve, lsEta, rsEta);

  //Due to degeneracy, there might be some issues in propagating form this A3 source
  //In this case, delete the shock 
  if (newShock && !newShock->isValid()){
    //dbsk2d_assert(false);
    // The children are not yet in the shock list so we can just delete them
    delete newShock; 
    newShock = 0;

    //there is no way to recover from this so might as well thrown an exception
    THROW_TOPOLOGY_EXCEPTION(false, "Propagation from an A3 source");
  }

  //successful initialization of a new shock from this junction
  add_an_ishock_edge(newShock);
  newA3->set_cShock (newShock);
  newA3->setPropagated (true);

  return newA3;
}

