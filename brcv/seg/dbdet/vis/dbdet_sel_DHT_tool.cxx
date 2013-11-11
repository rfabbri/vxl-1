#include "dbdet_sel_DHT_tool.h"

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

#include <dbdet/algo/dbdet_sel.h>

#include "dbdet_sel_tableau_commands.h"

class dbdet_sel_DHT_tool_misc_commands : public vgui_command
{
 public:
  dbdet_sel_DHT_tool_misc_commands(dbdet_sel_DHT_tool* cur_tool, int command) : 
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
  dbdet_sel_DHT_tool* tool;

};

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------

dbdet_sel_DHT_tool::dbdet_sel_DHT_tool():  
  sel_tab_(0), sel_storage_(0), EM(0), CM(0), ELG(0), CFG(0), edge_linker(0),
  rad(7.0), dt(15.0), dx(0.5), maxN(7), token_len(0.7),
  cur_edgel(0), cur_cvlet(0),
  quality_threshold(0.5), 
  hide_HTG(false)
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  con_start = vgui_event_condition(vgui_key('s'), vgui_CTRL, false);
  con_next = vgui_event_condition(vgui_key('n'), vgui_CTRL, false);
  con_clear = vgui_event_condition(vgui_key('k'), vgui_CTRL, false);
  con_resolve = vgui_event_condition(vgui_key('r'), vgui_CTRL, false);
}


vcl_string dbdet_sel_DHT_tool::name() const
{
  return "SEL DHT Linking Tool";
}

bool dbdet_sel_DHT_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
      cur_edgel = 0;
      cur_cvlet = 0;

      //reset the HTG
      edge_linker->clear_HTG();
    }
    return true;
  }
  return false;
}

bool dbdet_sel_DHT_tool::handle( const vgui_event & e, 
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
    cur_edgel = 0; //reset this so that only the cur_cvlet is drawn
    //cur_cvlet = random_curvelet(sel_tab_->find_closest_edgel(ix, iy));

    if (cur_cvlet){
      //start a hyp. tree from this curvelet and go as far as possible
      //this also inserts it into the HTG
      construct_hyp_tree(cur_cvlet);

      //print_cvlet_info(cur_cvlet, cost);
    }

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_start(e))
  {
    //initialize the HTG by creating HT nodes
    edge_linker->initialize_HTG();

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_next(e))
  {
    //propagate all HTs to a depth of four curvelets from the current leafs
    edge_linker->propagate_HTs_N_steps(edge_linker->HTG.nodes.size());

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
    return true;
  }
  else if (con_clear(e))
  {
    edge_linker->clear_HTG();  //reset the HTG
    
    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
  }
  else if (con_resolve(e)) //resolve all ambiguities
  {   
    edge_linker->disambiguate_the_HTG(); //resolve the HTG

    sel_tab_->post_redraw();
    sel_tab_->post_overlay_redraw();
  }

  if ( e.type == vgui_DRAW ) 
  {
    draw_all_hyp_trees();
    if (!hide_HTG)
      draw_HTG();
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

    draw_all_hyp_trees();
    if (!hide_HTG)
      draw_HTG();
    
  }

  return false;
}

//-------------------------------------------------------------------------------------------
//  DHT routines (local)
//-------------------------------------------------------------------------------------------

void dbdet_sel_DHT_tool::construct_hyp_tree(dbdet_curvelet* cvlet)
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

void dbdet_sel_DHT_tool::draw_CC_segment(vgl_point_2d<double> pt, double theta, double k, double L, bool col_flag, bool forward,
                                                      double R, double G, double B)
{  
  double sx = pt.x();
  double sy = pt.y();

  glColor3f(R,G,B);
  
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
void dbdet_sel_DHT_tool::draw_CC_bundle(dbdet_CC_curve_model_new* cm, double length, bool col_flag, bool forward)
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


void dbdet_sel_DHT_tool::draw_all_hyp_trees()
{
  //draw all the hyp trees stored as HTG nodes
  for (unsigned i=0; i< edge_linker->HTG.nodes.size(); i++){
    dbdet_hyp_tree* HT1 = edge_linker->HTG.nodes[i];

    //draw each tree in a different color
    draw_hyp_tree(HT1, (rand() % 256)/256.0, (rand() % 256)/256.0, (rand() % 256)/256.0);
  }
}

void dbdet_sel_DHT_tool::draw_hyp_tree(dbdet_hyp_tree* HT, double R, double G, double B)
{
  //traverse all the paths and draw them
  if (HT){
    //iterate through the nodes and draw each node
    //the final version will include different alternate paths so a path iterator ought to be used instead
    //traverse the hyp tree to find the leaf nodes
    dbdet_hyp_tree::iterator pit = HT->begin();
    for ( ; pit != HT->end(); pit++)
    {
      dbdet_curvelet* cvlet = (*pit)->cvlet;
      dbdet_CC_curve_model_new* cm = (dbdet_CC_curve_model_new*) cvlet->curve_model;
      draw_CC_segment(cm->pt, cm->theta, cm->k, cvlet->length, true, cvlet->forward,R,G, B);
    }
  }

}

void dbdet_sel_DHT_tool::draw_HTG()
{
  //draw circles around the nodes and draw straight lines for links
  for (unsigned i=0; i< edge_linker->HTG.nodes.size(); i++)
  {
    dbdet_hyp_tree* HT1 = edge_linker->HTG.nodes[i];

    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth (2.0);
    glBegin( GL_LINE_STRIP );
    vgl_point_2d<double> pt = HT1->root->cvlet->ref_edgel->pt;
    for (int th=0; th<=20; th++){
      double theta = th*2*vnl_math::pi/20.0;
      glVertex2f(pt.x() + 1*vcl_cos(theta), pt.y() + 1*vcl_sin(theta));
    }
    glEnd();
  
    //now draw the CPL links from this node
    vcl_set<int>::iterator lit = edge_linker->HTG.CPL_links[i].begin();
    for (; lit != edge_linker->HTG.CPL_links[i].end(); lit++)
    {
      dbdet_hyp_tree* HT2 = edge_linker->HTG.nodes[*lit];
      glColor3f(1.0f, 0.0f, 0.0f);
      glLineWidth (3.0);
      glBegin( GL_LINE_STRIP );
      glVertex2f(HT1->root->cvlet->ref_edgel->pt.x(), HT1->root->cvlet->ref_edgel->pt.y()); 
      glVertex2f(HT2->root->cvlet->ref_edgel->pt.x(), HT2->root->cvlet->ref_edgel->pt.y());
      glEnd();
    }

    //now draw the CPL links from this node
    lit = edge_linker->HTG.CPT_links[i].begin();
    for (; lit != edge_linker->HTG.CPT_links[i].end(); lit++)
    {
      dbdet_hyp_tree* HT2 = edge_linker->HTG.nodes[*lit];
      glColor3f(0.0f, 1.0f, 0.0f);
      glLineWidth (3.0);
      glBegin( GL_LINE_STRIP );
      glVertex2f(HT1->root->cvlet->ref_edgel->pt.x(), HT1->root->cvlet->ref_edgel->pt.y()); 
      glVertex2f(HT2->root->cvlet->ref_edgel->pt.x(), HT2->root->cvlet->ref_edgel->pt.y());
      glEnd();
    }
  }
}

void dbdet_sel_DHT_tool::print_cvlet_info(dbdet_curvelet* cvlet, double cost)
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


void dbdet_sel_DHT_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  if (edge_linker)
    menu.add(((edge_linker->propagate_constraints)?on:off)+" Propagate C1 constraints", 
              bvis1_tool_toggle, (void*)(&edge_linker->propagate_constraints) );

  menu.add( "Set Quality threshold for linking", 
            new dbdet_sel_tableau_set_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Max Value", &quality_threshold));
  
  menu.separator();

  menu.add(((hide_HTG)?on:off)+"Hide HTG", 
            bvis1_tool_toggle, (void*)(&hide_HTG) );

  menu.add( "Print all HTs", 
            new dbdet_sel_DHT_tool_misc_commands(this, 0));
  
}

