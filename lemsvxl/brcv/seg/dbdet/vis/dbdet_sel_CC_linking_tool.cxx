#include "dbdet_sel_CC_linking_tool.h"

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
#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbgl/algo/dbgl_fit_circ_arc_spline.h>

#include <dbdet/algo/dbdet_sel.h>

class dbdet_sel_tableau_set_display_params_double_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_display_params_double_command(dbdet_sel_tableau* tab, 
    const vcl_string& name, const void* dref) : sel_tableau(tab),  dref_((double*)dref), name_(name) {}

  void execute() 
  { 
    double param_val = *dref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *dref_ = param_val;
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau* sel_tableau;
  double* dref_;
  vcl_string name_;
};

class dbdet_sel_CC_linking_tool_misc_commands : public vgui_command
{
 public:
  dbdet_sel_CC_linking_tool_misc_commands(dbdet_sel_CC_linking_tool* cur_tool, int command) : 
    tool(cur_tool), command_id(command) {}

  void execute() 
  { 
    switch(command_id)
    {
      case 0: //print all HTs
        //tool->edge_linker->print_all_trees();
        break;
    }
  }

  int command_id; 
  dbdet_sel_CC_linking_tool* tool;

};

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------

dbdet_sel_CC_linking_tool::dbdet_sel_CC_linking_tool():  
  sel_tab_(0), sel_storage_(0), EM(0), CM(0), ELG(0), CFG(0), edge_linker(0),
  rad(7.0), dt(15.0), dx(0.5), maxN(7), token_len(0.7),
  cur_edgel(0), 
  draw_chain(true), draw_fits(false), 
  tolerance(2.0), smooth_thresh(0.9), depth(0)
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  con_start = vgui_event_condition(vgui_key('s'), vgui_CTRL, false);
  con_next = vgui_event_condition(vgui_key('n'), vgui_CTRL, false);
  con_clear = vgui_event_condition(vgui_key('k'), vgui_CTRL, false);
  con_resolve = vgui_event_condition(vgui_key('r'), vgui_CTRL, false);
}


vcl_string dbdet_sel_CC_linking_tool::name() const
{
  return "SEL Explore CC Linking";
}

bool dbdet_sel_CC_linking_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
      edge_linker = new dbdet_sel<dbdet_CC_curve_model_new>(sel_storage->EM(), sel_storage->CM(), 
                                                            sel_storage->ELG(), sel_storage->CFG(),
                                                            dbdet_curvelet_params(dbdet_curve_model::CC2, rad, dt, dx, false, 
                                                                                  token_len, 0.3, 0.01, false, false));
      //also clear existing contours
      clear_contour();
      cur_edgel = 0;
    }
    return true;
  }
  return false;
}

bool dbdet_sel_CC_linking_tool::handle( const vgui_event & e, 
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

    if (cur_edgel)
      sel_tab_->print_edgel_stats(cur_edgel);

    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (gesture1_(e)) //SHIFT-left click
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    cur_edgel = sel_tab_->find_closest_edgel(ix, iy);

    if (cur_edgel){
      clear_contour();
      construct_hyp_tree(cur_edgel);
    }

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
  else if (con_resolve(e)) //resolve all ambiguities
  {  
    resolve_contour();

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
  }

  if ( e.type == vgui_DRAW ) 
  {
    draw_hyp_tree();
  }

  if ( e.type == vgui_OVERLAY_DRAW ) 
  { 
    if (cur_edgel){ //if edgel selected, draw the curvelets anchored on it
      
      ////display all the groupings of the current edgel
      //vcl_list<dbdet_curvelet* >::iterator cv_it = CM->curvelets(cur_edgel->id).begin();
      //for ( ; cv_it!=CM->curvelets(cur_edgel->id).end(); cv_it++)
      //  sel_tab_->draw_selected_cvlet((*cv_it));

      //cv_it = CM->Rcurvelets(cur_edgel->id).begin();
      //for ( ; cv_it!=CM->Rcurvelets(cur_edgel->id).end(); cv_it++)
      //  sel_tab_->draw_selected_cvlet((*cv_it));
      
      //mark the selected edgel
      glColor3f( 1.0 , 0.0 , 1.0 );
      glPointSize(8.0);
      //gl2psPointSize(8.0);
      glBegin( GL_POINTS );
      glVertex2f(cur_edgel->pt.x(), cur_edgel->pt.y());
      glEnd();

      ////also draw a circle representing the size of the local neighborhood searched
      //glColor3f( 1.0 , 0.0 , 1.0 );
      //glLineWidth (1.0);
      //glBegin( GL_LINE_STRIP );
      //for (int th=0; th<=20; th++){
      //  double theta = cur_edgel->tangent - vnl_math::pi/2 + th*2*vnl_math::pi/20.0;
      //  glVertex2f(cur_edgel->pt.x() + rad*vcl_cos(theta), 
      //             cur_edgel->pt.y() + rad*vcl_sin(theta));
      //}
      ////glVertex2f(cur_edgel->pt.x() + rad*vcl_cos(cur_edgel->tangent - vnl_math::pi/2), 
      ////           cur_edgel->pt.y() + rad*vcl_sin(cur_edgel->tangent - vnl_math::pi/2));
      //glEnd();

    }

    draw_hyp_tree();
  }

  return false;
}


void dbdet_sel_CC_linking_tool::construct_hyp_tree(dbdet_edgel* e)
{
  if (ELG->cLinks.size()==0){
    vcl_cout << "No Link Graph !" <<vcl_endl;
    return;
  }

  //construct 2 HTs: one in the forward direction and one in the reverse direction
  vcl_queue<dbdet_EHT_node*> BFS_queue;

  //forward HT
  dbdet_EHT_node* root1 = new dbdet_EHT_node(e);
  HTF.root = root1;
  BFS_queue.push(root1);

  depth = 0;

  while (!BFS_queue.empty() && vcl_log10(double(depth))<3)
  {
    dbdet_EHT_node* cur_node = BFS_queue.front();
    BFS_queue.pop();

    //are we at a CFG node? if we are we don't need to go any further
    if (cur_node!= root1 &&
        (CFG->pFrags[cur_node->e->id].size()>0 ||
         CFG->cFrags[cur_node->e->id].size()>0))
      continue;

    //propagate this node
    dbdet_link_list_iter lit = ELG->cLinks[cur_node->e->id].begin(); 
    for (; lit != ELG->cLinks[cur_node->e->id].end(); lit++)
    {
      if (ELG->linked[(*lit)->ce->id]) //don't go tracing in linked contours
        continue;

      if (cur_node->parent) {
        //make a simple consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->ce->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->ce->pt.y() - cur_node->e->pt.y();

        if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<smooth_thresh) //not consistent
          continue;
      }

      //else extend the tree to this edgel
      dbdet_EHT_node* new_node = new dbdet_EHT_node((*lit)->ce);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
      depth++;
    }
  }

  //empty the bfs queue
  while (!BFS_queue.empty())
    BFS_queue.pop();

  //reverse HT
  dbdet_EHT_node* root2 = new dbdet_EHT_node(e);
  HTR.root = root2;
  BFS_queue.push(root2);

  depth = 0; //reset depth

  while (!BFS_queue.empty()  && vcl_log10(double(depth))<3)
  {
    dbdet_EHT_node* cur_node = BFS_queue.front();
    BFS_queue.pop();

    //propagate this node
    dbdet_link_list_iter lit = ELG->cLinks2[cur_node->e->id].begin(); 
    for (; lit != ELG->cLinks2[cur_node->e->id].end(); lit++)
    {
      if (cur_node->parent) {
        //make a simple consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->ce->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->ce->pt.y() - cur_node->e->pt.y();

        if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<smooth_thresh) //not consistent
          continue;
      }

      //else extend the tree to this edgel
      dbdet_EHT_node* new_node = new dbdet_EHT_node((*lit)->ce);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
    }
  }
}

void dbdet_sel_CC_linking_tool::resolve_contour()
{
  //basically go thorough the hyp tree and evaluate all the paths
  //keep only the viable ones
  vcl_vector<dbdet_edgel*> dummy_chain;
  
  double min_cost = 1000000;
  dbdet_EHT::path_iterator best_pit(0);

  dbdet_EHT::path_iterator pit = HTF.path_begin();
  for (; pit != HTF.path_end(); pit++)
  { 
    vcl_vector<dbdet_edgel*>& edgel_chain = pit.get_cur_path();

    dbdet_edgel* le = edgel_chain.back();
    if (CFG->pFrags[le->id].size()==0 &&
        CFG->cFrags[le->id].size()==0)
    {
      //not a valid termination node
      //delete the node associated  with this path ( it will delete the entire path, by definition)
      HTF.delete_subtree(pit);
      continue; 
    }

    //test this path to see if it is valid
    if (!edge_linker->is_EHT_path_legal(edgel_chain)){
      HTF.delete_subtree(pit);
      continue;
    }

    //compute cost

    double path_cost = edge_linker->compute_path_metric2(dummy_chain, edgel_chain, dummy_chain);
    if (path_cost < min_cost){
      min_cost = path_cost;
      best_pit = pit;
    }
  }

  //remove all other paths except the best
  pit = HTF.path_begin();
  for (; pit != HTF.path_end(); pit++)
  {
    if (pit != best_pit)
      HTF.delete_subtree(pit);
  }

}

void dbdet_sel_CC_linking_tool::draw_edgel_chain(vcl_vector<dbdet_edgel*>& edgel_chain, float R, float G, float B)
{
  glColor3f(R, G, B);
  glLineWidth (1.0);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<edgel_chain.size(); j++)
    glVertex2f(edgel_chain[j]->pt.x(), edgel_chain[j]->pt.y());
  glEnd();
  
}

void dbdet_sel_CC_linking_tool::draw_arc_spline(vcl_vector<dbdet_edgel*>& edgel_chain, float R, float G, float B)
{
  //construct an arc spline from a vector of points
  vcl_vector<vgl_point_2d<double> > pts;
  vcl_vector<vgl_vector_2d<double> >tans;
  for (unsigned i=0; i<edgel_chain.size(); i++)
    pts.push_back(edgel_chain[i]->pt);

  vcl_vector<dbgl_circ_arc > arc_list;
  dbgl_fit_circ_arc_spline_to_polyline(arc_list, pts, tolerance);

  pts.clear(); tans.clear();
  for (unsigned i=0; i<arc_list.size(); i++)
    arc_list[i].compute_samples(0.5, pts, tans);

  //now draw it
  glColor3f(R, G, B);
  glLineWidth (3.0);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<pts.size(); j++)
    glVertex2f(pts[j].x(), pts[j].y());
  glEnd();
}

void dbdet_sel_CC_linking_tool::draw_hyp_tree()
{
  //traverse all the paths and draw them
  //glColor3f((rand() % 256)/256.0, (rand() % 256)/256.0, (rand() % 256)/256.0);
  
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth (2.0);

  //traverse the HT and draw all the paths
  dbdet_EHT::path_iterator pit = HTF.path_begin();
  for (; pit != HTF.path_end(); pit++){ 
    if (draw_chain)
      draw_edgel_chain(pit.get_cur_path(), 0.0f, 0.0f, 1.0f);
    if (draw_fits)
      draw_arc_spline(pit.get_cur_path(), 0.0f, 0.0f, 1.0f);
  }

  //draw_link(HTF.root);

  glColor3f(0.0f, 1.0f, 0.0f);
  glLineWidth (2.0);
  
  pit = HTR.path_begin();
  for (; pit != HTR.path_end(); pit++) {
    if (draw_chain)
      draw_edgel_chain(pit.get_cur_path(), 0.0f, 1.0f, 0.0f);
    if (draw_fits)
      draw_arc_spline(pit.get_cur_path(), 0.0f, 1.0f, 0.0f);
  }
  //draw_link(HTR.root);
}

void dbdet_sel_CC_linking_tool::draw_link(dbdet_EHT_node* cur_node)
{
  if (!cur_node)
    return;

  vcl_list<dbdet_EHT_node*>::iterator cit = cur_node->children.begin();
  for (; cit != cur_node->children.end(); cit++){
    glBegin(GL_LINE_STRIP);
    glVertex2f(cur_node->e->pt.x(), cur_node->e->pt.y());
    glVertex2f((*cit)->e->pt.x(), (*cit)->e->pt.y());
    glEnd();

    draw_link(*cit);
  }
}

void dbdet_sel_CC_linking_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add(((draw_chain)?on:off)+"Draw Edgel Chain", 
            bvis1_tool_toggle, (void*)(&draw_chain) );

  menu.add(((draw_fits)?on:off)+"Draw CC Chain", 
            bvis1_tool_toggle, (void*)(&draw_fits) );

  menu.add( "Set tolerance", 
            new dbdet_sel_tableau_set_display_params_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Tolerance", &tolerance) );

  menu.add( "Set smoothness thresh (0-1)", 
            new dbdet_sel_tableau_set_display_params_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Smooth Thresh", &smooth_thresh) );
  
  //menu.add( "Print all HTs", 
  //          new dbdet_sel_CC_linking_tool_misc_commands(this, 0));
  
}

