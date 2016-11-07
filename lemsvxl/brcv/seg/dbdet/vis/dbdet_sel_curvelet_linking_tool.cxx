#include "dbdet_sel_curvelet_linking_tool.h"

#include <vcl_limits.h>
#include <vcl_algorithm.h>
#include <vcl_queue.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <mbl/mbl_stats_1d.h>

#include <bvis1/bvis1_manager.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbdet/vis/dbdet_sel_tableau.h>

#include <dbgl/algo/dbgl_arc_algo.h>
#include <dbdet/algo/dbdet_sel.h>

#include "dbdet_sel_tableau_commands.h"


class dbdet_sel_curvelet_linking_tool_set_params_command : public vgui_command
{
 public:
  dbdet_sel_curvelet_linking_tool_set_params_command(dbdet_sel_tableau* tab, dbdet_sel_curvelet_linking_tool* cur_tool) : 
       sel_tableau(tab), tool(cur_tool) {}

  void execute() 
  { 
    vgui_dialog param_dlg("Set SEL Params");
    param_dlg.field("dx", tool->dx);
    param_dlg.field("dt", tool->dt);
    param_dlg.field("Token Length", tool->token_len);
    param_dlg.field("Neighborhood Radius", tool->rad);
    //param_dlg.field("Maximum size of a grouping", tool->maxN);
    if(!param_dlg.ask())
      return;
    
    //set the parameters on to the CM
    tool->CM->set_parameters(dbdet_curvelet_params(dbdet_curve_model::CC2, tool->rad, tool->dt, tool->dx, false, tool->token_len));

    //set the parameters to the linker
    tool->edge_linker = new dbdet_sel<dbdet_CC_curve_model_new>(tool->sel_storage_->EM(), tool->sel_storage_->CM(), 
                                                                tool->sel_storage_->ELG(), tool->sel_storage_->CFG(),  
                                                                dbdet_curvelet_params(dbdet_curve_model::CC2, 
                                                                                      tool->rad, tool->dt, tool->dx, false,
                                                                                      tool->token_len, 0.3, 0.01, false, false)
                                                                );

    //clear all the current curvelets in the map
    tool->CM->clear_all_curvelets();

    //also delete the current contour in the tool
    tool->clear_contour();
    tool->cur_edgel = 0;
    tool->cur_cvlet = 0;

    //clear all the HTs
    tool->edge_linker->clear_HTG();

    sel_tableau->post_redraw(); 
    sel_tableau->post_overlay_redraw();
  }

  dbdet_sel_tableau* sel_tableau;
  dbdet_sel_curvelet_linking_tool* tool;

};

class dbdet_sel_curvelet_linking_tool_misc_commands : public vgui_command
{
 public:
  dbdet_sel_curvelet_linking_tool_misc_commands(dbdet_sel_curvelet_linking_tool* cur_tool, int command) : 
    tool(cur_tool), command_id(command) {}

  void execute() 
  { 
    switch(command_id)
    {
      case 0: //print all HTs
        tool->edge_linker->print_all_trees();
        break;
    }
  }

  int command_id; 
  dbdet_sel_curvelet_linking_tool* tool;

};

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------

dbdet_sel_curvelet_linking_tool::dbdet_sel_curvelet_linking_tool():  
  sel_tab_(0), sel_storage_(0), EM(0), CM(0), ELG(0), CFG(0), edge_linker(0),
  rad(7.0), dt(15.0), dx(0.5), maxN(7), token_len(0.7),
  cur_edgel(0), cur_cvlet(0),
  quality_threshold(0.5), enforce_global_consistency(true), allow_C0(false),
  draw_bundle(false), draw_chain(false)
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  con_start = vgui_event_condition(vgui_key('s'), vgui_CTRL, false);
  con_next = vgui_event_condition(vgui_key('n'), vgui_CTRL, false);
  con_clear = vgui_event_condition(vgui_key('k'), vgui_CTRL, false);
  con_resolve = vgui_event_condition(vgui_key('r'), vgui_CTRL, false);
}


vcl_string dbdet_sel_curvelet_linking_tool::name() const
{
  return "SEL Explore Curvelet Linking";
}

bool dbdet_sel_curvelet_linking_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbdet_sel_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its an edgemap storage class
  if (storage->type() == "sel"){
    dbdet_sel_storage_sptr sel_storage;
    sel_storage.vertical_cast(storage);

    if (sel_storage!=sel_storage_){
      sel_storage_ = sel_storage;
      sel_tab_ = temp_tab;
      EM = sel_storage->EM(); 
      CM = &sel_storage->CM();
      ELG = &sel_storage->ELG();
      CFG = &sel_storage->CFG();
 
      //read parameters from the CM 
      rad = CM->params_.rad_;
      dx = CM->params_.dpos_;
      dt = CM->params_.dtheta_;
      token_len = CM->params_.token_len_;

      //initialize a sel linker
      edge_linker = new dbdet_sel<dbdet_CC_curve_model_new>(sel_storage_->EM(), sel_storage_->CM(), 
                                                            sel_storage_->ELG(), sel_storage_->CFG(),  
                                                            dbdet_curvelet_params(dbdet_curve_model::CC2, 
                                                                                  rad, dt, dx, false, 
                                                                                  token_len, 0.3, 0.01, false, false)
                                                            );

      //also clear existing contours
      clear_contour();
      cur_edgel = 0;
      cur_cvlet = 0;

      //reset the HTG
      edge_linker->clear_HTG();
    }
    return true;
  }
  return false;
}

bool dbdet_sel_curvelet_linking_tool::handle( const vgui_event & e, 
                                             const bvis1_view_tableau_sptr& view )
{
  //Functionality:
  //
  //  Left Mouse: Show all Curvelet anchored at the nearest edgel along with the properties (make this the cur_cvlet)
  //  Shift Mouse: Show one Curvelet anchored at the nearest edgel (randomly)

  //  Key commands:
  //
  //  CTRL + 's' : start the linking process from the current cvlet (i.e., put it into the contour fragment)
  //  CTRL + 'n' : add the next best cvlet to the end of the chain 

  if (gesture0_(e)) //left click
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    cur_edgel = sel_tab_->find_closest_edgel(ix, iy);
    
    //compute curvelets for this edgel with the current parameters if there are none here
    compute_local_curvelets(cur_edgel);

    //select the best curvelet
    cur_cvlet = random_curvelet(cur_edgel);//best_curvelet(cur_edgel);

    if (cur_edgel)
      sel_tab_->print_edgel_stats(cur_edgel);

    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (gesture1_(e)) //SHIFT-left click
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    cur_edgel = 0; //reset this so that only the cur_cvlet is drawn
    cur_cvlet = random_curvelet(sel_tab_->find_closest_edgel(ix, iy));

    if (cur_cvlet){
      double cost = 1000.0;

      //if (cur_cvlet->quality>quality_threshold && cur_CF.size()>0){
      //  //determine compatibility
      //  dbdet_curvelet_compatibility cv_comp = compute_compatibility(cur_CF.back(), cur_cvlet);
      //  cost = cv_comp.cost;

      //  //also update best curve
      //}

      print_cvlet_info(cur_cvlet, cost);
    }

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_start(e)){ //start a contour

    clear_contour();

    if (cur_cvlet){
      vcl_cout << "New Contour Started... " << vcl_endl;
      add_cvlet(cur_cvlet);
    }

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_next(e)){ //add next curvelet to the contour

    if (!add_next_curvelet())
      vcl_cout << "No legal curvelets available to extend contour." << vcl_endl;

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_clear(e)) //clear contour
  {
    clear_contour();

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
  }

  if ( e.type == vgui_DRAW ) 
  {
    //if (draw_chain) draw_edgel_chain(&cur_chain);
    draw_arc_spline();

    draw_HT();
  }

  if ( e.type == vgui_OVERLAY_DRAW ) 
  {
    if (cur_edgel) //if edgel selected, draw the curvelets anchored on it
    {
      //display all the groupings of the current edgel
      vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(cur_edgel->id).begin();
      for ( ; cv_it!=CM->curvelets(cur_edgel->id).end(); cv_it++)
        sel_tab_->draw_selected_cvlet((*cv_it));

      //also draw the reverse curvelets
      cv_it = CM->Rcurvelets(cur_edgel->id).begin();
      for ( ; cv_it!=CM->Rcurvelets(cur_edgel->id).end(); cv_it++)
        sel_tab_->draw_selected_cvlet((*cv_it));

      //mark the selected edgel
      glColor3f( 1.0 , 0.0 , 1.0 );
      glPointSize(8.0);
      //gl2psPointSize(8.0);
      glBegin( GL_POINTS );
      glVertex2f(cur_edgel->pt.x(), cur_edgel->pt.y());
      glEnd();

      //also draw a circle representing the size of the local neighborhood searched
      glColor3f( 1.0 , 0.0 , 1.0 );
      glLineWidth (1.0);
      glBegin( GL_LINE_STRIP );
      for (int th=0; th<=20; th++){
        double theta = cur_edgel->tangent - vnl_math::pi/2 + th*2*vnl_math::pi/20.0;
        glVertex2f(cur_edgel->pt.x() + rad*vcl_cos(theta), 
                   cur_edgel->pt.y() + rad*vcl_sin(theta));
      }
      //glVertex2f(cur_edgel->pt.x() + rad*vcl_cos(cur_edgel->tangent - vnl_math::pi/2), 
      //           cur_edgel->pt.y() + rad*vcl_sin(cur_edgel->tangent - vnl_math::pi/2));
      glEnd();

    }

     //if best curvelet has been selected only draw that one 
    if (cur_cvlet){
      sel_tab_->draw_selected_cvlet(cur_cvlet);
    }

    //draw_edgel_chain(&cur_chain);
    draw_arc_spline();

    draw_HT();
  }

  return false;
}

dbdet_curvelet* dbdet_sel_curvelet_linking_tool::best_curvelet(dbdet_edgel* e)
{
  if (!e) return 0;

  //compute local curvelet for this edge if there are none computed yet
  compute_local_curvelets(e);

  ////get the list of curvelets anchored at this site and pick the best one
  //if (CM->curvelets(e->id).size()>0){
  //  //pick the best one based on quality alone
  //  dbdet_curvelet* best_cvlet = 0;
  //  double best_val = -1;

  //  vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(e->id).begin();
  //  for ( ; cv_it!=CM->curvelets(e->id).end(); cv_it++)
  //  {
  //    if ((*cv_it)->quality>best_val){ 
  //      best_cvlet = (*cv_it);
  //      best_val = (*cv_it)->quality;
  //    }
  //  }

  //  //only return if quality better than threshold
  //  if (best_val>quality_threshold)
  //    return best_cvlet;
  //}

  //only return a curvelet if there is no ambiguity
  if (CM->curvelets(e->id).size()==1)
    return CM->curvelets(e->id).front();

  return 0;
}

dbdet_curvelet* dbdet_sel_curvelet_linking_tool::random_curvelet(dbdet_edgel* e)
{
  if (!e) return 0;

  //compute local curvelet for this edge if there are none computed yet
  compute_local_curvelets(e);

  //return a random curvelet anchored at this edgel
  if (rand()%2){
    if (CM->curvelets(e->id).size()>0){
      int rand_ind = rand() % CM->curvelets(e->id).size();

      vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(e->id).begin();
      for (int i=0; i<rand_ind; i++)
        cv_it++;

      return (*cv_it);
    }
  }
  else {
    if (CM->Rcurvelets(e->id).size()>0){
      int rand_ind = rand() % CM->Rcurvelets(e->id).size();

      vcl_list<dbdet_curvelet* >::iterator cv_it = CM->Rcurvelets(e->id).begin();
      for (int i=0; i<rand_ind; i++)
        cv_it++;

      return (*cv_it);
    }
  }
  return 0;
}

//: compute curvelets at the given edgel if there are none
void dbdet_sel_curvelet_linking_tool::compute_local_curvelets(dbdet_edgel* e)
{
  if (!e) return;

  //check to see if this edgel already has curvelets
  if (CM->curvelets(e->id).size()==0)
  {
    //build the curvelets for this edge
    edge_linker->build_curvelets_greedy_for_edge(e, 2*rad, false, true, false);//maxN, false, true
    //edge_linker->build_curvelets_greedy_for_edge(e, 2*rad, false, false, false);//maxN, false, false
  }
}

//-------------------------------------------------------------------------------------------
//  linking routines
//-------------------------------------------------------------------------------------------

void dbdet_sel_curvelet_linking_tool::clear_contour()
{
  HT.delete_tree();
}

//: initialize the HT from the current curvelet
void dbdet_sel_curvelet_linking_tool::add_cvlet(dbdet_curvelet* cvlet)
{
  //instantiate a HT node from the cvlet
  HT.root = new dbdet_hyp_tree_node ( new dbdet_curvelet(*cvlet), 
                                       -1, 
                                       true
                                     );

  ////debug
  //switch(comp.Corder)
  //{
  //  case 2: vcl_cout << "C2  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
  //  case 1: vcl_cout << "C1  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
  //  case 0: vcl_cout << "C0  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
  //}
}

bool dbdet_sel_curvelet_linking_tool::add_next_curvelet(dbdet_edgel* e)
{
  dbdet_hyp_tree_node* cur_node = 0;

  if (!e){ //if not specified, assume back of the chain
    dbdet_hyp_tree::iterator nit = HT.begin();
    for (; nit != HT.end(); nit++){
      cur_node = (*nit);
      if (cur_node->is_leaf())
        e = cur_node->cvlet->edgel_chain.back();
    }
  }

  if (!cur_node)//means that the HT has not been initialized yet
    return false;

  //compute local curvelet for this edge if there are none computed yet
  compute_local_curvelets(e);

  bool cvlet_added = false;

  vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(e->id).begin();
  for ( ; cv_it!=CM->curvelets(e->id).end(); cv_it++)
  {
    dbdet_curvelet* cvlet = (*cv_it);

    //only consider the curvelet if its quality better than threshold
    if (cvlet->quality<quality_threshold)
      continue;

    //make sure that this curvelet is C1 continuous with the terminal curvelet
    dbdet_curve_model* constrained_cb = edge_linker->C1_continuity_possible(cur_node->cvlet, cvlet);
    if (!constrained_cb)
      continue;

    //C1 possible: add this curvelet as a node in the current hyp tree
    dbdet_hyp_tree_node* new_node;
    if (enforce_global_consistency)
      new_node = cur_node->add_child(new dbdet_curvelet(*cvlet, constrained_cb));
    else {
      new_node = cur_node->add_child(new dbdet_curvelet(*cvlet));
      delete constrained_cb;
    }

    cvlet_added = true;
  }

  //This could potentially add multiple curvelets to the tree so the tree needs to be pruned
  if (cvlet_added){
    edge_linker->resolve_HT(&HT);
    return true;
  }
  else  //try pruning one edgel out of the curvelet and try again
  {
    //first shorten the curvelet
    unsigned N = cur_node->cvlet->edgel_chain.size();
    if (N<2)
      return false;
    
    dbdet_edgel* new_final_e = cur_node->cvlet->edgel_chain[N-2];
    dbdet_curvelet* chopped_cvlet = edge_linker->construct_chopped_cvlet(cur_node->cvlet, new_final_e);
    
    delete cur_node->cvlet;
    cur_node->cvlet = chopped_cvlet;

    if (cur_node->parent && enforce_global_consistency)
      chopped_cvlet->replace_curve_model(edge_linker->C1_continuity_possible(cur_node->parent->cvlet, chopped_cvlet));

    //then recursively call this function
    return add_next_curvelet(new_final_e);
  }
}

////: this function selects the next best curvelet to add from the choices available
//// using the compatibility heuristic
//bool dbdet_sel_curvelet_linking_tool::add_next_curvelet(dbdet_edgel* e)
//{
//  if (cur_CF.size()==0)
//    return false;
//
//  if (!e) //if not specified, assume back of the chain
//    e = cur_CF.back()->edgel_chain.back();//get the edgel at the end of the chain
//  
//  //compute local curvelet for this edge if there are none computed yet
//  compute_local_curvelets(e);
//
//  //time to select the best curvelet
//  dbdet_curvelet* best_cvlet = 0;
//  dbdet_curvelet_compatibility best_comp;
//
//  // Option A) pick the best one based on quality alone (simulating a schematic linking)
//  //if (!enforce_consistency){
//  //  best_cvlet = best_curvelet(e);
//  //  if (best_cvlet){
//  //    add_cvlet(best_cvlet, best_comp);
//  //    return true;
//  //  }
//  //  else
//  //    return false;
//  //}
//
//  // Option B) Check for compatibility 
//  //            - C2 > C1 > C0
//  //            - if order is the same
//  //               - compute difference in models (difference in k)
//  //               - compute bundle size
//  vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(e->id).begin();
//  for ( ; cv_it!=CM->curvelets(e->id).end(); cv_it++)
//  {
//    dbdet_curvelet* cvlet = (*cv_it);
//
//    //only consider the curvelet if its quality better than threshold
//    if (cvlet->quality<quality_threshold)
//      continue;
//
//    //determine compatibility
//    dbdet_curvelet_compatibility cv_comp = compute_compatibility(cur_CF.back(), cvlet);
//
//    if (cv_comp.Corder==-1) continue; //ignore incompatible transitions
//
//    //do not allow C0 if the flag is up
//    if (!allow_C0 && cv_comp.Corder==0)
//      continue;
//
//    //if (cv_comp.Corder>best_comp.Corder){ //prioritize continuity
//    //  best_cvlet = cvlet;
//    //  best_comp = cv_comp;
//    //}
//    //else if (cv_comp.Corder==best_comp.Corder){ //else check for bundle size
//    //  if (cv_comp.area > best_comp.area){
//    //    best_cvlet = cvlet;
//    //    best_comp = cv_comp;
//    //  }
//    //}
//
//    if (cv_comp.cost<best_comp.cost){
//      best_cvlet = cvlet;
//      best_comp = cv_comp;
//    }
//  }
//
//  //if we found a legal curvelet to add, add it to the contour
//  if (best_cvlet){
//    add_cvlet(best_cvlet, best_comp);
//    return true;
//  }
//
//  ////OPTION C) try back tracking to the middle of the last cvlet
//  //dbdet_edgel* m_e = cur_CF.back()->edgel_chain[(cur_CF.back()->edgel_chain.size()-1)/2];
//
//  //if (e != m_e){//avoid infinite loop
//  //  vcl_cout << "Backtracking..." << vcl_endl;
//  //  return add_next_curvelet(m_e);
//  //}
//
//  ////OPTION D) try back tracking one edgel at a time on the last cvlet
//  //for (int ii=0; ii<(int)cur_CF.back()->edgel_chain.size()-3; ii++)
//  //{
//  //  if (add_next_curvelet(cur_CF.back()->edgel_chain[cur_CF.back()->edgel_chain.size()-2-ii]))
//  //    return true;
//  //}
//
//  //NO GOOD: no legal curvelets
//  return false;
//}
//
//void dbdet_sel_curvelet_linking_tool::add_cvlet(dbdet_curvelet* cvlet, const dbdet_curvelet_compatibility comp)
//{
//  //clone the curvelet and add it to the list
//  cur_CF.push_back(new dbdet_curvelet(*cvlet));
//
//  //add the curvelet to the edgel chain
//  cur_chain.append(cur_CF.back()->edgel_chain);
//
//  if (cur_CF.size()==1){//just add the cvlet, this is the first curvelet in the set
//    transition_data.push_back(vcl_pair<int,int>(-1,-1));
//    vcl_cout << "First Contourlet: No continuity constraint. " << vcl_endl;
//    return;
//  }
//  
//  //debug
//  switch(comp.Corder)
//  {
//    case 2: vcl_cout << "C2  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
//    case 1: vcl_cout << "C1  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
//    case 0: vcl_cout << "C0  Continuity. (k= " << dbdet_k_classes[comp.trans_k] << " )" << vcl_endl; break;
//  }
//
//  if (!enforce_consistency){ //don't bother updating the bundles
//    //record simple transition
//    transition_data.push_back(vcl_pair<int,int>(-1,-1));
//  }
//  else {
//    //record transition
//    transition_data.push_back(vcl_pair<int,int>(comp.Corder, comp.trans_k));
//
//    //now we need to modify the local curvelet's curve bundle to reflect global continuity constraint
//    //  a) update CB2 with the intersection
//    //  b) transport the intersected cb1 back to the CB1 ref (not necessary for now)
//
//    delete cur_CF.back()->curve_model;
//    cur_CF.back()->curve_model = comp.model;
//
//    // c) get a single curve solution by backtracking through the solution set
//    //use the centroid of the last curvelet as the best curve
//    vgl_point_2d<double> sol = cur_CF.back()->curve_model->compute_best_fit();
//
//    back_track_through_bundles(sol);
//  }
//}

////compute the compatibility information betwee a pair of curvelets
//dbdet_curvelet_compatibility dbdet_sel_curvelet_linking_tool::
//compute_compatibility(dbdet_curvelet* cur_cvlet, dbdet_curvelet* next_cvlet)
//{
//  //intersect the curve bundles at the common edgel
//  //  a) transport the cb from CB1 to e
//  //  b) intersect CB1 and CB2 (C^2, C^1 and C^0)
//
//  dbdet_edgel* e = next_cvlet->ref_edgel;
//
//  //transport CB1 to current edgel
//  dbdet_CC_curve_model_new* trans_cb = (dbdet_CC_curve_model_new*) cur_cvlet->curve_model->transport(e->pt, e->tangent);
//
//  ////Two tests : A) Model continuity
//  ////            B) Model Transition
//
//  //dbdet_curve_model* new_cb = trans_cb->intersect(next_cvlet->curve_model);
//  //if (new_cb && new_cb->bundle_is_valid()) //model continuity is possible
//  //{
//  //  return dbdet_curvelet_compatibility(2, 0.0, 0.0, -1, new_cb, 1000.0);
//  //}
//  //if (new_cb) delete new_cb; //else delete this CB
//
//  ////B) try model transition test
//  //int trans_k, trans_type;
//  //double bundle_area;
//  //new_cb = trans_cb->transition(next_cvlet->curve_model, trans_k, trans_type, bundle_area);
//  //if (new_cb && new_cb->bundle_is_valid())
//  //{ 
//  //  double dk = 0.0;
//  //  return dbdet_curvelet_compatibility(trans_type, bundle_area, dk, trans_k, new_cb, 1000.0);
//  //}
//  //if (new_cb)  delete new_cb; //delete this cb
//
//  ////C) curvelets not compatible
//  //return dbdet_curvelet_compatibility();
//
//  //find the best transition
//  int trans_k, trans_type;
//  double gc_cost;
//  dbdet_curve_model* new_cb = trans_cb->consistent_transition(next_cvlet->curve_model, trans_k, trans_type, gc_cost);
//  
//  double cc_cost = gc_cost + 1/next_cvlet->quality;
//
//  if (new_cb && new_cb->bundle_is_valid()){ //if legal 
//    return dbdet_curvelet_compatibility(trans_type, 0.0, 0.0, trans_k, new_cb, cc_cost);
//  }
//  else {
//    if (new_cb)  delete new_cb; //delete this cb
//    return dbdet_curvelet_compatibility();
//  }
//}

////: backtrack from the final curvelet all the way to define a single parametric curve
//void dbdet_sel_curvelet_linking_tool::back_track_through_bundles(vgl_point_2d<double> sol)
//{
//  if (cur_CF.size()<=1)
//    return;
//
//  // get a single curve solution by backtracking a singel curve through the curvelet sequence
//  for (int i=cur_CF.size()-2; i>=0; i--)
//  {
//    //transport the solution backward to update the curve models
//    dbdet_CC_curve_model_new* cur_cm = (dbdet_CC_curve_model_new*)cur_CF[i+1]->curve_model;
//    dbdet_CC_curve_model_new* prev_cm = (dbdet_CC_curve_model_new*)cur_CF[i]->curve_model;
//
//    //if the model transitioned
//    double k;
//    if (transition_data[i+1].first<=1)//for C0 and C1 transitions, the curvature needs to be specified
//      k = dbdet_k_classes[transition_data[i+1].second];
//    else 
//      k = cur_cm->k;
//
//    if (transition_data[i+1].first==0){ //for C0 transitions, the tangent needs to be specified too
//      //pick the center of the transported cb1 bundle at the specified dx value
//      
//      //transport CB1 to current transition point (this is redundant, we already did it once before during transitioning)
//      dbdet_CC_curve_model_new* trans_cb1 = (dbdet_CC_curve_model_new*) prev_cm->transport(cur_cm->ref_pt, cur_cm->ref_theta);
//
//      //intersect a line with the bundle polygon : simulate this by intersecting a very thin box with the polygon
//      vgl_polygon<double> line_b(1);
//      line_b.push_back(sol.x()-1e-5,-0.5);line_b.push_back(sol.x()+1e-5,-0.5);line_b.push_back(sol.x()+1e-5,0.5);line_b.push_back(sol.x()-1e-5,0.5);
//      vgl_polygon<double> int_p = vgl_clip(line_b, trans_cb1->cv_bundles[transition_data[i+1].second], vgl_clip_type_intersect);
//
//      if (int_p.num_sheets()!=1)//backtracking failed
//      {
//        vcl_cout << "!!!!!!!!!! C0 Issue !!!!!!!" << vcl_endl;
//        ////pick the nearest point on the bundle to the current solution
//        //double min_d=1000.0; unsigned min_p=0;
//        //for (unsigned p=0; p<trans_cb1->cv_bundles[transition_data[i+1].second][0].size(); p++){
//        //  double d = vgl_distance(sol, trans_cb1->cv_bundles[transition_data[i+1].second][0][p]);
//        //  if (d<min_d){
//        //    min_d=d;
//        //    min_p = p;
//        //  }
//        //}
//        //sol = trans_cb1->cv_bundles[transition_data[i+1].second][0][min_p];
//        return;
//      }
//      else {
//        //find dt range and pick the center as the solution
//        double dt_min = 100, dt_max = -100;
//        for (unsigned p=0; p<int_p[0].size(); p++){
//          if (int_p[0][p].y()<dt_min) dt_min = int_p[0][p].y();
//          if (int_p[0][p].y()>dt_max) dt_max = int_p[0][p].y();
//        }
//        sol.y() = (dt_min+dt_max)/2;
//      }
//    }
//
//    //transport the solution backward
//    bool valid; 
//    sol = prev_cm->transport_CC(sol, k, cur_cm->ref_pt, cur_cm->ref_theta, 
//                                        prev_cm->ref_pt, prev_cm->ref_theta, 
//                                        valid);
//
//    //if transportation went wrong,pick the centroid arbitrarily
//    if (!valid)
//    {
//      vcl_cout << "!!!!!!!!!! Transport failed !!!!!!!" << vcl_endl;
//      return;
//    }
//
//    //set this constrained solution as the best fit
//    prev_cm->set_best_fit(sol, k);
//
//  }
//}


void dbdet_sel_curvelet_linking_tool::construct_hyp_tree(dbdet_curvelet* cvlet)
{
  ////construct a new tree
  //dbdet_hyp_tree* HT = new dbdet_hyp_tree(cvlet, HTG.nodes.size());

  ////save it as a node in the HTG
  //HTG.insert_node(HT);
  //HTG.resize_links(); //temp hack

  ////claim ownership of this cvlet by adding labels to its edgels
  //edge_linker->label_edgels(cvlet, HT->root);

  ////trace as far as possible
  ////breadth-first search through the tree
  //vcl_queue<dbdet_hyp_tree_node*> BFS_queue;

  ////BFS_queue.push(HT->root);
  ////while (!BFS_queue.empty())
  ////  propagate_HT_from_the_next_leaf_node(BFS_queue);

  ////then display tree info
  //HT->print_all_paths();
}


//-------------------------------------------------------------------------------------------
//  drawing routines
//-------------------------------------------------------------------------------------------

void dbdet_sel_curvelet_linking_tool::draw_CC_segment(vgl_point_2d<double> pt, double theta, double k, double L, bool col_flag, bool forward,
                                                      double R, double G, double B)
{  
  double sx = pt.x();
  double sy = pt.y();

  if (col_flag)
    glColor3f(1.0, 0.0, 0.0);
  else
    glColor3f(0.0, 1.0, 0.0);

  glLineWidth (3.0);
  //gl2psLineWidth(3.0);
  
  if (forward){
    if (vcl_fabs(k)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx, sy);
      glVertex2f(sx + L*vcl_cos(theta), sy+L*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=0; s<L; s+=0.1){
      double th = theta + s*k;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/k + vcl_cos(th-vnl_math::pi_over_2)/k, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/k + vcl_sin(th-vnl_math::pi_over_2)/k );  
    }
    glEnd();
  }
  else {
    if (vcl_fabs(k)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx, sy);
      glVertex2f(sx - L*vcl_cos(theta), sy-L*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=0; s>-L; s-=0.1){
      double th = theta + s*k;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/k + vcl_cos(th-vnl_math::pi_over_2)/k, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/k + vcl_sin(th-vnl_math::pi_over_2)/k );  
    }
    glEnd();
  }

}
void dbdet_sel_curvelet_linking_tool::draw_CC_bundle(dbdet_CC_curve_model_new* cm, double length, bool col_flag, bool forward)
{
  //// sample a number of points in the CB on a uniform grid
  //dxs = linspace(min(CB(:,1)), max(CB(:,1)), 10);
  //dts = linspace(min(CB(:,2)), max(CB(:,2)), 10);

  //hold on;
  //for i=1:length(dxs),
  //    for j=1:length(dts),
  //        % if this point is inside the polygon, draw it
  //        if (inpolygon(dxs(i), dts(j), CB(:,1), CB(:, 2)))
  //            curves = [curves; dxs(i), dts(j)];
  //            
  //            pt1 = pt + dxs(i)*[-sin(theta) cos(theta)];
  //            CCa = CC_curve(pt1, theta+dts(j), k, l);
  //            CCb = CC_curve(pt1, theta+dts(j), k, -l);
  //            plot(CCa(:,1), CCa(:,2), col_cv);
  //            plot(CCb(:,1), CCb(:,2), col_cv);

  //            draw_CC_segment(cm->pt, cm->theta, cm->k, length, col_flag);
  //        end
  //    end
  //end
}

void dbdet_sel_curvelet_linking_tool::draw_edgel_chain(dbdet_edgel_chain* chain)
{
  glColor3f(1.0, 0.0, 1.0);
  glLineWidth (1.0);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<chain->edgels.size(); j++)
    glVertex2f(chain->edgels[j]->pt.x(), chain->edgels[j]->pt.y());
  glEnd();
  
}

void dbdet_sel_curvelet_linking_tool::draw_arc_spline()
{
  //if (cur_CF.size()==0)
  //  return;

  //if (draw_bundle && enforce_consistency)
  //{
  //  // sample a number of points in the final bundle on a uniform grid
  //  // and back track the solution to form the bundle
  //  dbdet_CC_curve_model_new* fin_cm = (dbdet_CC_curve_model_new*) cur_CF.back()->curve_model;

  //  for (int i=0; i<NkClasses; i++){
  //    if (fin_cm->cv_bundles[i].num_sheets()==1){ //legal sub-bundle

  //      //sample it from a grid
  //      double dx_min=100, dx_max=-100, dt_min=100, dt_max=-100;
  //      for (unsigned p=0; p<fin_cm->cv_bundles[i][0].size(); p++){
  //        if (fin_cm->cv_bundles[i][0][p].x()<dx_min) dx_min = fin_cm->cv_bundles[i][0][p].x();
  //        if (fin_cm->cv_bundles[i][0][p].x()>dx_max) dx_max = fin_cm->cv_bundles[i][0][p].x();
  //        if (fin_cm->cv_bundles[i][0][p].y()<dt_min) dt_min = fin_cm->cv_bundles[i][0][p].y();
  //        if (fin_cm->cv_bundles[i][0][p].y()>dt_max) dt_max = fin_cm->cv_bundles[i][0][p].y();
  //      }

  //      for (double dxs=dx_min; dxs<=dx_max; dxs+=(dx_max-dx_min)/5.0){
  //        for (double dts=dt_min; dts<=dt_max; dts+=(dt_max-dt_min)/5.0){

  //          // if this point is inside the CB polygon, draw it
  //          if (fin_cm->cv_bundles[i].contains(vgl_point_2d<double>(dxs, dts)))
  //          {
  //            //set the final curvelet to this point
  //            fin_cm->set_best_fit(vgl_point_2d<double>(dxs, dts), dbdet_k_classes[i]);

  //            //update curvelets with the propagated solution
  //            back_track_through_bundles(vgl_point_2d<double>(dxs, dts));

  //            //finally draw it
  //            for (unsigned i=0; i<cur_CF.size(); i++){
  //              dbdet_CC_curve_model_new* cm = (dbdet_CC_curve_model_new*) cur_CF[i]->curve_model;
  //              draw_CC_segment(cm->pt, cm->theta, cm->k, cur_CF[i]->length, i%2==0);
  //            }
  //          }
  //        }
  //      }
  //    }
  //  }
  //}
  //else {
  //  //single arc spline solution
  //  for (unsigned i=0; i<cur_CF.size(); i++){
  //    dbdet_CC_curve_model_new* cm = (dbdet_CC_curve_model_new*) cur_CF[i]->curve_model;
  //    draw_CC_segment(cm->pt, cm->theta, cm->k, cur_CF[i]->length, i%2==0);
  //  }
  //}
}

void dbdet_sel_curvelet_linking_tool::draw_HT()
{ 
  if (draw_bundle)
  {
    if (!HT.root)
      return;

    //find the curvelet list corresponding to the best path
    vcl_vector<dbdet_curvelet*> best_path;
    if (HT.best_path){
      dbdet_hyp_tree::iterator pit = HT.begin();
      for ( ; pit != HT.end(); pit++){
        if ((*pit)->is_leaf())
          best_path = pit.get_cur_path();
      }
    }
    else {
      best_path.push_back(HT.root->cvlet);
    }

    // sample a number of points in the final CB on a uniform grid
    // and back track the solution to form the bundle
    dbdet_CC_curve_model_new* fin_cm = (dbdet_CC_curve_model_new*) best_path.back()->curve_model;

    for (int i=0; i<NkClasses; i++){
      if (fin_cm->cv_bundles[i].num_sheets()==1){ //legal sub-bundle

        //sample it from a grid
        double dx_min=100, dx_max=-100, dt_min=100, dt_max=-100;
        for (unsigned p=0; p<fin_cm->cv_bundles[i][0].size(); p++){
          if (fin_cm->cv_bundles[i][0][p].x()<dx_min) dx_min = fin_cm->cv_bundles[i][0][p].x();
          if (fin_cm->cv_bundles[i][0][p].x()>dx_max) dx_max = fin_cm->cv_bundles[i][0][p].x();
          if (fin_cm->cv_bundles[i][0][p].y()<dt_min) dt_min = fin_cm->cv_bundles[i][0][p].y();
          if (fin_cm->cv_bundles[i][0][p].y()>dt_max) dt_max = fin_cm->cv_bundles[i][0][p].y();
        }

        for (double dxs=dx_min; dxs<=dx_max; dxs+=(dx_max-dx_min)/5.0){
          for (double dts=dt_min; dts<=dt_max; dts+=(dt_max-dt_min)/5.0){

            // if this point is inside the CB polygon, draw it
            if (fin_cm->cv_bundles[i].contains(vgl_point_2d<double>(dxs, dts)))
            {
              //set the final curvelet to this point
              vgl_point_2d<double> cur_sol(dxs, dts); 
              fin_cm->set_best_fit(cur_sol, dbdet_k_classes[i]);

              edge_linker->back_propagate_solution(best_path, cur_sol); //force it to recompute the path

              draw_local_hyp_tree(&HT);
            }
          }
        }
      }
    }
  }
  else
    draw_local_hyp_tree(&HT); //simply draw the current solution
}

void dbdet_sel_curvelet_linking_tool::draw_local_hyp_tree(dbdet_hyp_tree* HT)
{
  //traverse all the paths and draw them
  if (HT){
    //iterate through the nodes and draw each node
    //the final version will include different alternate paths so a path iterator ought to be used instead
    //traverse the hyp tree to find the leaf nodes
    bool col_flag = true;
    dbdet_hyp_tree::iterator pit = HT->begin();
    for ( ; pit != HT->end(); pit++)
    {
      dbdet_curvelet* cvlet = (*pit)->cvlet;
      dbdet_CC_curve_model_new* cm = (dbdet_CC_curve_model_new*) cvlet->curve_model;
      draw_CC_segment(cm->pt, cm->theta, cm->k, cvlet->length, col_flag, cvlet->forward);
      col_flag = !col_flag;
    }
  }

}

void dbdet_sel_curvelet_linking_tool::print_cvlet_info(dbdet_curvelet* cvlet, double cost)
{
  vcl_cout << "Chain: ";
  if (cvlet->forward) vcl_cout << "F : ";
  else                vcl_cout << "B : ";

  for (unsigned i=0; i < cvlet->edgel_chain.size(); ++i)
    vcl_cout << "\t" << cvlet->edgel_chain[i]->id;

  //print curve params
  cvlet->curve_model->print_info();

  //print curvelet quality info
  vcl_cout << ", L= " << cvlet->length << ", Q= " << cvlet->quality << ", cost = " << cost ;
  vcl_cout << vcl_endl;
}


void dbdet_sel_curvelet_linking_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add(((enforce_global_consistency)?on:off)+"Enforce Global Consistency", 
            bvis1_tool_toggle, (void*)(&enforce_global_consistency) );

  //menu.add(((allow_C0)?on:off)+"Allow C0 Transitions", 
  //          bvis1_tool_toggle, (void*)(&allow_C0) );

  menu.separator();

  menu.add( "Set Quality threshold for linking", 
            new dbdet_sel_tableau_set_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Max Value", &quality_threshold));
  menu.add( "Set SEL parameters", 
            new dbdet_sel_curvelet_linking_tool_set_params_command((dbdet_sel_tableau*)sel_tab_.ptr(), this));

  menu.separator();

  menu.add(((draw_bundle)?on:off)+"Draw Curve Bundles", 
            bvis1_tool_toggle, (void*)(&draw_bundle) );

  menu.add(((draw_chain)?on:off)+"Draw Edgel Chain", 
            bvis1_tool_toggle, (void*)(&draw_chain) );

}

