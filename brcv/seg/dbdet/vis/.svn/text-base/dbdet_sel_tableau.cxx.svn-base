// This is brcv/seg/dbdet/vis/dbdet_sel_tableau.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_limits.h>

#include <gl2ps/gl2ps.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>

#include "dbdet_sel_tableau.h"
#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <dbdet/sel/dbdet_edgel_link_graph.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>

#include "dbdet_sel_tableau_commands.h"

#undef DRAW_GL2PS
#define DRAW_GL2PS 1

//: Constructor
dbdet_sel_tableau::dbdet_sel_tableau(dbdet_sel_storage_sptr sel): 
  EM_(sel->EM()), 
  CM_(sel->CM()),
  ELG_(sel->ELG()),
  CFG_(sel->CFG()),
  EULM_(sel->EULM()),
  color_mat_(sel->color_mat()),
  c_groups_(sel->c_groups()),
  prune_frags_(sel->prune_frags()),
  edge_linker_(0),
  display_points_(false),
  display_groupings_(false),
  display_largest_(false),
  display_curve_(false),
  display_curve_bundle_(false),
  display_groupings_special_(false),
  display_extra_special_(false),
  smallest_to_display_(3),
  largest_to_display_(30),
  min_quality_to_display_(0.0),
  only_display_used_curvelets_(false),
  app_threshold_(1.0),
  display_app_consistent_only_(false),
  display_multicolored_curvelets_(false),
  cvlet_color_(0.0f, 1.0f, 0.0f),
  cvlet_line_width_(1.0f),
  display_link_graph_(false),
  min_votes_to_display_(0),
  draw_links_in_color_(false),
  color_by_votes_(true),
  link_color_(0.0f, 0.0f, 0.0f),
  link_line_width_(1.0),
  display_contours_(true),
  smallest_curve_fragments_to_display_(0),
  display_multicolored_curves_(true),
  display_thick_curves_(false),
  display_CFG_end_points_(false),
  curve_color_(0.0f,1.0f,0.0f),
  curve_line_width_(3.0),
  gesture0_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  gesture1_(vgui_event_condition(vgui_LEFT, vgui_SHIFT, true)),
  draw_anchored_only_(false),
  cur_edgel(0),
  cur_link(0),
  local_zoom_factor(1),
  display_contour_groups_(true),
  display_prune_contours_(true)
{
  ////fill in the randomized color table
  //for (int i=0; i<10; i++){
  //  for (int j=0; j<3;j++)
  //    col_pal[i][j] = (rand() % 256)/256.0;
  //}

  //if color mat is not empty, it mean it is for contour evaluation, so change param
  if(!color_mat_.empty())
  {
	display_contours_ = false;
	curve_color_ = sel->prune_color();

  }

  //instantiate an SEL linker class (this is temporary; need to instantiate the right type of linker)
  edge_linker_ = new dbdet_sel<dbdet_CC_curve_model_new>(EM_, CM_, ELG_, CFG_, CM_.params_);
}

dbdet_sel_tableau::~dbdet_sel_tableau()
{
}

bool dbdet_sel_tableau::handle( const vgui_event & e )
{
  //apply local zoom
  glTranslatef(-0.5, -0.5, 0);
  glScalef(local_zoom_factor, local_zoom_factor, 1);
  glTranslatef(0.5, 0.5, 0);


  //handle queries
  // Query 1: select an edgel
  if (gesture0_(e)) 
  {
    draw_anchored_only_ = true;

    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // I) Find edgel closest to ix,iy
    cur_edgel = find_closest_edgel(ix, iy);
    cur_link = 0; //deselect the current link
    
    // II) Display info about the selected edgel
    if (cur_edgel){
      //vgl_point_2d<double> pt = cur_edgel->pt;
      //vcl_cout << "Closest edgel: " << pt << vcl_endl;

      print_edgel_stats(cur_edgel);
      post_overlay_redraw(); //for drawing the curvelets
    }
    //else
    //  vcl_cout << "No closest edgel\n";
  }

  //Query 2: select a link
  if (gesture1_(e)){ //shift left click
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // 1) Find the closest edgel link
    cur_link = find_closest_link(ix, iy);
    cur_edgel = 0; //deselect the current edgel

    // 2) display info about the selected link
    if (cur_link){
      print_link_info(cur_link);
      post_overlay_redraw(); //for drawing the curvelets
    }
  }

  if( e.type == vgui_OVERLAY_DRAW ) {

    //if edgel selected, draw the curvelets it forms
    if (cur_edgel && CM_.is_valid())
    {
      //display all the groupings of the current edgel
      vcl_list<dbdet_curvelet* >::iterator cv_it = CM_.curvelets(cur_edgel->id).begin();
      for ( ; cv_it!=CM_.curvelets(cur_edgel->id).end(); cv_it++)
      {
        if (draw_anchored_only_ && (*cv_it)->ref_edgel != cur_edgel) continue;
        draw_selected_cvlet((*cv_it));
      }

      //mark the selected edgel
      glColor3f( 1.0 , 0.0 , 1.0 );
      glPointSize(8.0);
      //gl2psPointSize(8.0);
      glBegin( GL_POINTS );
      glVertex2f(cur_edgel->pt.x(), cur_edgel->pt.y());
      glEnd();
    }

    if (cur_link)
    {
      //display all the groupings through the current link
      cvlet_list_iter cv_it = cur_link->curvelets.begin();
      for ( ; cv_it!=cur_link->curvelets.end(); cv_it++)
        draw_selected_cvlet((*cv_it));       

      //mark the selected link
      glColor3f( 1.0 , 0.0 , 1.0 );
      glLineWidth(5.0);
      glBegin(GL_LINES);
      glVertex2f(cur_link->pe->pt.x(), cur_link->pe->pt.y());
      glVertex2f(cur_link->ce->pt.x(), cur_link->ce->pt.y());
      glEnd();
    }
  }

  //handle standard display
  if( e.type == vgui_DRAW ){
    draw_edgel_groupings();
    
    if (display_link_graph_)
      draw_edgel_link_graph();

    if (display_contours_)
      draw_edgel_chains();

    if (display_contour_groups_)
      draw_contour_groups();

    if (display_prune_contours_)
      draw_prune_contours();

    return true;
  }

  return false;
}

dbdet_edgel* dbdet_sel_tableau::find_closest_edgel(float ix, float iy)
{
  // I) Find edgel closest to ix,iy
  unsigned row_cell = (unsigned)iy;
  unsigned col_cell = (unsigned)ix;

  double dmin = vcl_numeric_limits<double>::infinity();
  unsigned jcell_min = 0, icell_min = 0;
  unsigned imin = 0;

  for (unsigned icell = row_cell-2; icell <= row_cell+2; ++icell)
  for (unsigned jcell = col_cell-2; jcell <= col_cell+2; ++jcell) 
  {
    if (icell > EM_->nrows()-1 || jcell > EM_->ncols()-1)
        continue;

    for (unsigned i=0; i < EM_->edge_cells[icell][jcell].size(); ++i) {
      dbdet_edgel *edgel = EM_->edge_cells[icell][jcell][i];
      double dx =edgel->pt.x() - ix; 
      double dy =edgel->pt.y() - iy; 
      double d = dx*dx+dy*dy;
      if (d < dmin) {
        imin = i;
        dmin = d;
        jcell_min = jcell;
        icell_min = icell;
      }
    }
  }

  if (dmin == vcl_numeric_limits<double>::infinity())
    return 0;
  else
    return EM_->edge_cells[icell_min][jcell_min][imin];
}

double distPL(double px, double py, vgl_point_2d<double> p1, vgl_point_2d<double> p2)
{
  double l = vcl_sqrt((p2.y()-p1.y())*(p2.y()-p1.y()) + 
                      (p2.x()-p1.x())*(p2.x()-p1.x()));

  double t = ((py-p1.y())*(p2.y()-p1.y()) + 
              (px-p1.x())*(p2.x()-p1.x()));

  if (t<0 || t>l)
    return vcl_numeric_limits<double>::infinity();
  else
    return vcl_fabs((py-p1.y())*(p2.x()-p1.x()) - 
                    (px-p1.x())*(p2.y()-p1.y()))/l;
}

dbdet_link* dbdet_sel_tableau::find_closest_link(float ix, float iy)
{
  // 1) First find edgel closest to ix,iy
  dbdet_edgel* edge = find_closest_edgel(ix,iy);

  if (!edge)
    return 0;

  // 2) Now go over its links to find the closest one
  double dmin = vcl_numeric_limits<double>::infinity();
  dbdet_link* closest_link = 0;

  if (ELG_.cLinks.size()==0)
    return 0;

  dbdet_link_list_iter l_it = ELG_.cLinks[edge->id].begin();
  for (; l_it != ELG_.cLinks[edge->id].end(); l_it++){
    double d = distPL(ix, iy, (*l_it)->pe->pt, (*l_it)->ce->pt);

    if (d<dmin){
      dmin = d;
      closest_link = (*l_it);
    }
  }
  l_it = ELG_.pLinks[edge->id].begin();
  for (; l_it != ELG_.pLinks[edge->id].end(); l_it++){
    double d = distPL(ix, iy, (*l_it)->pe->pt, (*l_it)->ce->pt);

    if (d<dmin){
      dmin = d;
      closest_link = (*l_it);
    }
  }

  if (dmin == vcl_numeric_limits<double>::infinity()) {
    return 0;
  } 
  else {
    return closest_link;
  }
}

//: draw selected curvelet
void dbdet_sel_tableau::draw_selected_cvlet(dbdet_curvelet* cvlet)
{
  if (only_display_used_curvelets_ && !cvlet->used)
    return;

  unsigned j = cvlet->edgel_chain.size();

  //don't draw the ones that are not going to be used for anything
  if (j<smallest_to_display_ || j>largest_to_display_ || cvlet->quality < min_quality_to_display_)
     return;

  //draw ths curvelet
  //draw_curvelet(cvlet, 1.0f, 0.0f, 0.0f);     
  
  //location of the edgel
  vgl_point_2d<double> edgel_pt;

  //draw the curve fit to this curvelet
  //need to draw the curve fit according to the curve model
  switch (cvlet->curve_model->type){
    case dbdet_curve_model::LINEAR: 
      draw_uncertainty_zone(cvlet, 0.0f , 0.6f , 0.0f );
      draw_line_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0);
      edgel_pt = ((dbdet_linear_curve_model*)cvlet->curve_model)->pt;
      break;
    case dbdet_curve_model::CC:  
      draw_uncertainty_zone(cvlet, 0.0f , 0.6f , 0.0f );
      //draw_CC_fit(cvlet, 0.0f, 1.0f, 0.0f);
      draw_CC_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0);

      edgel_pt = ((dbdet_CC_curve_model*)cvlet->curve_model)->pt;
      break;
    case dbdet_curve_model::CC2:  
      draw_uncertainty_zone(cvlet, 0.0f , 0.6f , 0.0f );
      //draw_CC2_fit(cvlet, 0.0f, 1.0f, 0.0f);
      draw_CC2_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0, cvlet->forward);

      edgel_pt = ((dbdet_CC_curve_model_new*)cvlet->curve_model)->pt;
      break;
    case dbdet_curve_model::CC3d:  
      draw_uncertainty_zone(cvlet, 0.0f , 0.6f , 0.0f );
      //draw_CC3d_fit(cvlet, 0.0f, 1.0f, 0.0f);
      draw_CC3d_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0, cvlet->forward);

      edgel_pt = ((dbdet_CC_curve_model_3d*)cvlet->curve_model)->pt;
      break;
    case dbdet_curve_model::ES: 
      draw_uncertainty_zone(cvlet, 0.0f , 0.6f , 0.0f );
      
      dbdet_ES_curve_model* cm = (dbdet_ES_curve_model*)cvlet->curve_model;
      edgel_pt = cm->pt;

      ////draw a circle of the same curvature
      //double kk = cm->k;

      //double cx = cm->pt.x() + vcl_cos(cm->theta+vnl_math::pi_over_2)/(kk+1e-9);
      //double cy = cm->pt.y() + vcl_sin(cm->theta+vnl_math::pi_over_2)/(kk+1e-9);

      //glColor3f( 0.0, 0.0, 0.0 );
      //glLineWidth (1.0);
      //glBegin( GL_LINE_STRIP );
      //for (unsigned j=0; j<=100; j++){
      //  double th = j*2*vnl_math::pi/100;
      //  glVertex2f(cx+vcl_cos(th)/(kk+1e-9), cy+vcl_sin(th)/(kk+1e-9));
      //}
      //glEnd();
      
      //draw_ES_fit(cvlet, 0.0f, 1.0f, 0.0f);
      draw_ES_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0);

      break;
  }

  //mark the anchor point of this curvelet
  glColor3f( 0.0 , 1.0 , 1.0 );
  glPointSize(1.0);
  //gl2psPointSize(8.0);
  glBegin( GL_POINTS );
  glVertex2f(edgel_pt.x(), edgel_pt.y());
  glEnd();
  
}

void dbdet_sel_tableau::draw_edgel_groupings()
{
  if( EM_.ptr() == 0 ) 
    return;

  if (!display_groupings_ && !display_curve_)
    return;

  //for all the edgels
  for (unsigned i=0; i<EM_->edgels.size(); i++)
  {
    dbdet_edgel* eA = EM_->edgels[i];

    //find the largest order grouping for this edgel
    unsigned largest_order = 0;
    dbdet_curvelet* largest_cvlet = CM_.largest_curvelet(eA);
    if (largest_cvlet)
      largest_order = largest_cvlet->order();

    curvelet_list_iter cv_it = CM_.curvelets(eA->id).begin();
    for ( ; cv_it!=CM_.curvelets(eA->id).end(); cv_it++)
    {
      dbdet_curvelet* cvlet = (*cv_it);
      unsigned j = cvlet->edgel_chain.size();
      
      //display all the edgels that are larger than the smallest to display
      if (j<smallest_to_display_ || j>largest_to_display_ || cvlet->quality < min_quality_to_display_)
        continue;

      if (only_display_used_curvelets_ && !cvlet->used)
        return;

      if (display_largest_ && j!=largest_order)
        continue;

      if (display_app_consistent_only_)//check for appearance consistency
      {
        //compute saliency of the curvelet
        double Lmean=0.0, Rmean=0.0, Lstd=0.0, Rstd=0.0;

        for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
          Lmean += cvlet->edgel_chain[k]->left_app->value();
          Rmean += cvlet->edgel_chain[k]->right_app->value();
        }
        Lmean /= cvlet->edgel_chain.size();
        Rmean /= cvlet->edgel_chain.size();

        for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
          Lstd += (cvlet->edgel_chain[k]->left_app->value()-Lmean)*(cvlet->edgel_chain[k]->left_app->value()-Lmean);
          Rstd += (cvlet->edgel_chain[k]->right_app->value()-Rmean)*(cvlet->edgel_chain[k]->right_app->value()-Rmean);
        }
        Lstd = vcl_sqrt(Lstd/cvlet->edgel_chain.size());
        Rstd = vcl_sqrt(Rstd/cvlet->edgel_chain.size());

        if (vcl_fabs(Lmean-Rmean)<app_threshold_*(Lstd+Rstd)) //saliency test
          continue;
      }

      if (display_groupings_){
        if (display_extra_special_)
          draw_curvelet_extra_special(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
        else if (display_groupings_special_)
          draw_curvelet_special(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
        else
          draw_curvelet(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
      }

      //draw the curve fits
      if (display_curve_){
        //need to draw the curve fit according to the curve model
        switch (cvlet->curve_model->type){
          case dbdet_curve_model::LINEAR: 
            draw_line_fit(cvlet,cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
            break;
          case dbdet_curve_model::CC:  
            draw_CC_fit(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
            break;
          case dbdet_curve_model::CC2:  
            draw_CC2_fit(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2], 2.0, cvlet->forward);
            break;
          case dbdet_curve_model::CC3d: 
            if (display_multicolored_curvelets_)//randomized color
              draw_CC3d_fit(cvlet, (rand()%256)/255.0, (rand()%256)/255.0, (rand()%256)/255.0, 2.0, cvlet->forward);
            else
              draw_CC3d_fit(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2], 2.0, cvlet->forward);
            break;
          case dbdet_curve_model::ES: 
            draw_ES_fit(cvlet, cvlet_color_[0], cvlet_color_[1], cvlet_color_[2]);
            break;
        }
      }
    }
  }

  if (display_points_) {
    glColor3f( 1.0 , 0.0 , 1.0 );
    glPointSize(8.0);
    //gl2psPointSize(8.0);
    glBegin( GL_POINTS );
    for (unsigned i = 0; i < EM_->nrows(); ++i)
      for (unsigned j = 0; j < EM_->ncols(); ++j)
        for (unsigned k = 0; k < EM_->edge_cells[i][j].size() ; ++k) {
          dbdet_edgel *edgel = EM_->edge_cells[i][j][k];
          glVertex2f(edgel->pt.x(),edgel->pt.y());
    }
    glEnd();
  }
}

void dbdet_sel_tableau::draw_uncertainty_zone(dbdet_curvelet* cvlet, float r, float g, float b)
{
  //draw the bounding circles around each edgel
  glColor3f(r, g, b );
  glLineWidth (1.0);

  double dpos;
  for (unsigned j=0; j<cvlet->edgel_chain.size(); j++)
  {
    //size of the uncertainty
    if (CM_.params_.badap_uncer_)
      dpos = CM_.params_.dpos_ + cvlet->edgel_chain[j]->uncertainty; //if adaptive use the edgel's uncertainty
    else
      dpos = CM_.params_.dpos_;

    glBegin( GL_LINE_STRIP );
    for (int th=0; th<=20; th++){
      double theta = th*2*vnl_math::pi/20.0;

      glVertex2f(cvlet->edgel_chain[j]->pt.x() + dpos*vcl_cos(theta), 
                 cvlet->edgel_chain[j]->pt.y() + dpos*vcl_sin(theta));
    }
    glEnd();
  }

  ////draw bounding rectangles around each edgel
  //glColor3f(r, g, b );
  //glLineWidth (1.0);
  //for (unsigned j=0; j<cvlet->edgel_chain.size(); j++)
  //{
  //  dbdet_edgel* e = cvlet->edgel_chain[j];

  //  glBegin( GL_LINE_STRIP );
  //  glVertex2f(e->pt.x() + CM_.dpos_*-vcl_sin(e->tangent) + CM_.token_len_/2*vcl_cos(e->tangent), 
  //             e->pt.y() + CM_.dpos_* vcl_cos(e->tangent) + CM_.token_len_/2*vcl_sin(e->tangent));
  //  glVertex2f(e->pt.x() - CM_.dpos_*-vcl_sin(e->tangent) + CM_.token_len_/2*vcl_cos(e->tangent), 
  //             e->pt.y() - CM_.dpos_* vcl_cos(e->tangent) + CM_.token_len_/2*vcl_sin(e->tangent));
  //  glVertex2f(e->pt.x() - CM_.dpos_*-vcl_sin(e->tangent) - CM_.token_len_/2*vcl_cos(e->tangent), 
  //             e->pt.y() - CM_.dpos_* vcl_cos(e->tangent) - CM_.token_len_/2*vcl_sin(e->tangent));
  //  glVertex2f(e->pt.x() + CM_.dpos_*-vcl_sin(e->tangent) - CM_.token_len_/2*vcl_cos(e->tangent), 
  //             e->pt.y() + CM_.dpos_* vcl_cos(e->tangent) - CM_.token_len_/2*vcl_sin(e->tangent));
  //  glVertex2f(e->pt.x() + CM_.dpos_*-vcl_sin(e->tangent) + CM_.token_len_/2*vcl_cos(e->tangent), 
  //             e->pt.y() + CM_.dpos_* vcl_cos(e->tangent) + CM_.token_len_/2*vcl_sin(e->tangent));
  //  glEnd();
  //}

}

//: draw a curvelet as a polyline
void dbdet_sel_tableau::draw_curvelet(dbdet_curvelet* cvlet, float r, float g, float b)
{
  glColor3f( r, g, b );
  glLineWidth (2.0);
  //gl2psLineWidth(2.0);
  glBegin( GL_LINE_STRIP );
  for (unsigned j=0; j<cvlet->edgel_chain.size(); j++){
    glVertex2f(cvlet->edgel_chain[j]->pt.x(), cvlet->edgel_chain[j]->pt.y());
  }
  glEnd();

}

//: draw only the special links of the curvelet (i.e., the ones it is immediately connected to)
void dbdet_sel_tableau::draw_curvelet_special(dbdet_curvelet* cvlet, float r, float g, float b)
{
  dbdet_edgel* eA = cvlet->ref_edgel;

  glColor3f( r, g, b );
  glLineWidth (2.0);
  //gl2psLineWidth(2.0);
  glBegin( GL_LINE_STRIP );
  for (unsigned j=0; j<cvlet->edgel_chain.size(); j++)
  {
    if ( cvlet->edgel_chain[j]==eA || //current edgel
        (j>0 && cvlet->edgel_chain[j-1]==eA) || //edgel after it
        (j<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[j+1]==eA) //edgel before it
        )
      glVertex2f(cvlet->edgel_chain[j]->pt.x(), cvlet->edgel_chain[j]->pt.y());
  }
  glEnd();

} 

//: draw only the extra special links of the curvelet (i.e., if the edgel it is connected to is also connected to it)
void dbdet_sel_tableau::draw_curvelet_extra_special(dbdet_curvelet* cvlet, float r, float g, float b)
{
  glColor3f( r, g, b );
  glLineWidth (2.0);
  //gl2psLineWidth(2.0);
  
  dbdet_edgel* eA = cvlet->ref_edgel;

  for (unsigned i=0; i<cvlet->edgel_chain.size(); i++)
  {
    if ((i<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[i+1]==eA) || // the link right before it
        (i>0 && cvlet->edgel_chain[i-1]==eA)) //link right after it
    {
      dbdet_edgel* eB = cvlet->edgel_chain[i];

      //go over all the curvelets of minimum size formed by the neighboring edgel and 
      //see if it is connected to this edgel
      //bool extra_special = sel_->link_is_reciprocal(eA, eB, cvlet->ref_edgel, smallest_to_display_);
      bool extra_special = true; //(TODO: FIX ME)

      if (extra_special){
        glBegin( GL_LINE_STRIP );
        glVertex2f(eA->pt.x(), eA->pt.y());
        glVertex2f(eB->pt.x(), eB->pt.y());
        glEnd();
      }
    }
  }
}

//: draw the best fit line of a curvelet
void dbdet_sel_tableau::draw_line_fit(dbdet_curvelet* cvlet, float r, float g, float b, float /*line_width*/)
{
  //draw the centroid Line of the bundle
  dbdet_linear_curve_model_base* cm = (dbdet_linear_curve_model_base*)cvlet->curve_model;

  //length on the minus side
  double Lm = vgl_distance(cvlet->edgel_chain[0]->pt, cvlet->ref_edgel->pt);
  //length on the plus side
  double Lp = vgl_distance(cvlet->ref_edgel->pt, cvlet->edgel_chain.back()->pt);

  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(line_width);
  glBegin(GL_LINES);
    glVertex2f(cm->pt.x()-Lm*vcl_cos(cm->theta), cm->pt.y()-Lm*vcl_sin(cm->theta));
    glVertex2f(cm->pt.x()+Lp*vcl_cos(cm->theta), cm->pt.y()+Lp*vcl_sin(cm->theta));
  glEnd();

}

//: draw the best fit CC of a curvelet
void dbdet_sel_tableau::draw_CC_fit(dbdet_curvelet* cvlet, float r, float g, float b, float /*line_width*/)
{
  //draw the centroid curveof the bundle
  dbdet_CC_curve_model* cm = (dbdet_CC_curve_model*)cvlet->curve_model;

  //draw a circular arc
  double kk = cm->k;
  double cx = cm->pt.x() + vcl_cos(cm->theta+vnl_math::pi_over_2)/(kk+1e-9);
  double cy = cm->pt.y() + vcl_sin(cm->theta+vnl_math::pi_over_2)/(kk+1e-9);

  //length on the minus side
  double Lm = vgl_distance(cvlet->edgel_chain[0]->pt, cvlet->ref_edgel->pt);
  //Lm = 10;//for debug

  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(3.0);
  glBegin(GL_LINE_STRIP);
  for (double s=-Lm; s<0; s+=0.1){
    double th = cm->theta + s*kk;
    glVertex2f(cx+vcl_cos(th-vnl_math::pi_over_2)/(kk+1e-9), cy+vcl_sin(th-vnl_math::pi_over_2)/(kk+1e-9));
  }
  glEnd();

  double Lp = vgl_distance(cvlet->ref_edgel->pt, cvlet->edgel_chain.back()->pt);
  //Lp = 10;//for debug

  glColor3f(r, g, b);  
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(3.0);
  glBegin(GL_LINE_STRIP);
  for (double s=0; s<Lp; s+=0.1){
    double th = cm->theta + s*kk;
    glVertex2f(cx+vcl_cos(th-vnl_math::pi_over_2)/(kk+1e-9), cy+vcl_sin(th-vnl_math::pi_over_2)/(kk+1e-9));
  }
  glEnd();

}

//: draw the best fit CC2 of a curvelet
void dbdet_sel_tableau::draw_CC2_fit(dbdet_curvelet* cvlet, float r, float g, float b, float /*line_width*/, bool forward)
{
  //draw the centroid curveof the bundle
  dbdet_CC_curve_model_new* cm = (dbdet_CC_curve_model_new*)cvlet->curve_model;

  //draw a circular arc
  double sx = cm->pt.x();
  double sy = cm->pt.y();
  double theta = cm->theta;
  double kk = cm->k;

  //length on the minus side
  double Lm = vgl_distance(cvlet->edgel_chain[0]->pt, cvlet->ref_edgel->pt);
  double Lp = vgl_distance(cvlet->ref_edgel->pt, cvlet->edgel_chain.back()->pt);
  //Lm = 10; Lp = 10; //for debug

  ////for new type of cvlets
  //if (cvlet->ref_edgel == cvlet->edgel_chain[0])
  //  Lp = cvlet->length;

  //CM_.params_.centered_

  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(3.0);

  if (forward){
    if (vcl_fabs(kk)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx - Lm*vcl_cos(theta), sy - Lm*vcl_sin(theta));
      glVertex2f(sx + Lp*vcl_cos(theta), sy + Lp*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=-Lm; s<Lp; s+=0.1){
      double th = theta + s*kk;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/kk + vcl_cos(th-vnl_math::pi_over_2)/kk, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/kk + vcl_sin(th-vnl_math::pi_over_2)/kk );  
    }
    glEnd();
  }
  else {
    if (vcl_fabs(kk)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx + Lm*vcl_cos(theta), sy + Lm*vcl_sin(theta));
      glVertex2f(sx - Lp*vcl_cos(theta), sy - Lp*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=Lm; s>-Lp; s-=0.1){
      double th = theta + s*kk;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/kk + vcl_cos(th-vnl_math::pi_over_2)/kk, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/kk + vcl_sin(th-vnl_math::pi_over_2)/kk );  
    }
    glEnd();
  }
}

//: draw a CC curve
void dbdet_sel_tableau::draw_CC(double sx, double sy, double theta, double k, 
                                double Lm, double Lp,
                                float r, float g, float b, float /*line_width*/, bool forward)
{
  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(3.0);

  if (forward){
    if (vcl_fabs(k)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx - Lm*vcl_cos(theta), sy - Lm*vcl_sin(theta));
      glVertex2f(sx + Lp*vcl_cos(theta), sy + Lp*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=-Lm; s<Lp; s+=0.1){
      double th = theta + s*k;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/k + vcl_cos(th-vnl_math::pi_over_2)/k, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/k + vcl_sin(th-vnl_math::pi_over_2)/k );  
    }
    glEnd();
  }
  else {
    if (vcl_fabs(k)<1e-7){ //arc degenerate draw a line
      glBegin(GL_LINE_STRIP);
      glVertex2f(sx + Lm*vcl_cos(theta), sy + Lm*vcl_sin(theta));
      glVertex2f(sx - Lp*vcl_cos(theta), sy - Lp*vcl_sin(theta));
      glEnd();
      return;
    }
    //draw a circular arc
    glBegin(GL_LINE_STRIP);
    for (double s=Lm; s>-Lp; s-=0.1){
      double th = theta + s*k;  
      glVertex2f(sx + vcl_cos(theta+vnl_math::pi_over_2)/k + vcl_cos(th-vnl_math::pi_over_2)/k, 
                 sy + vcl_sin(theta+vnl_math::pi_over_2)/k + vcl_sin(th-vnl_math::pi_over_2)/k );  
    }
    glEnd();
  }
}

//: draw the best fit CC3d of a curvelet
void dbdet_sel_tableau::draw_CC3d_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width, bool forward)
{
  //draw the centroid curveof the bundle
  dbdet_CC_curve_model_3d* cm = (dbdet_CC_curve_model_3d*)cvlet->curve_model;

  //draw the best fit circular arc
  double sx = cm->pt.x();
  double sy = cm->pt.y();
  double theta = cm->theta;
  double kk = cm->k;

  //length on the minus side
  double Lm = vgl_distance(cvlet->edgel_chain[0]->pt, cvlet->ref_edgel->pt);
  double Lp = vgl_distance(cvlet->ref_edgel->pt, cvlet->edgel_chain.back()->pt);
  
  if (!display_curve_bundle_)
    draw_CC(sx, sy, theta, kk, Lm, Lp, r,g,b, line_width, forward);
  else {
    //draw samples from the bundle
    for (int i=0; i<cm->Kmax.rows(); i++){
      for (int j=0; j<cm->Kmax.cols(); j++){

        if (cm->Kmax(i,j)>cm->Kmin(i,j)){
          theta = dbdet_angle0To2Pi(cm->ref_theta + cm->Dt(j));
          sx = cm->ref_pt.x() + cm->Dx(i)*-vcl_sin(theta);
          sy = cm->ref_pt.y() + cm->Dx(i)*vcl_cos(theta);
          
          //draw five samples from each k range
          for (int m=0; m<6; m++){
            kk = cm->Kmin(i,j)+ m*(cm->Kmax(i,j)-cm->Kmin(i,j))/5.0;
            draw_CC(sx, sy, theta, kk, Lm, Lp, r,g,b, line_width, forward);
          }
        }
      }
    }
  }
}

//: draw the best fit ES of a curvelet grounded at the given edgel
void dbdet_sel_tableau::draw_ES_fit(dbdet_curvelet* cvlet, float r, float g, float b, float /*line_width*/)
{
  //draw the centroid ES of the bundle
  dbdet_ES_curve_model* cm = (dbdet_ES_curve_model*)cvlet->curve_model;

  //compute the ES
  dbgl_eulerspiral es_fit(cm->pt, cm->theta, cm->k, cm->gamma, 1);

  //length on the minus side
  double Lm = vgl_distance(cvlet->edgel_chain[0]->pt, cvlet->ref_edgel->pt);
  //Lm = 10;//for debug

  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(line_width);
  glBegin(GL_LINE_STRIP);
  for (double s=-Lm; s<0; s+=0.05)
    glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
  glEnd();

  double Lp = vgl_distance(cvlet->ref_edgel->pt, cvlet->edgel_chain.back()->pt);
  //Lp = 10;//for debug

  glColor3f(r, g, b);
  glLineWidth (cvlet_line_width_);
  //gl2psLineWidth(line_width);
  glBegin(GL_LINE_STRIP);
  for (double s=0; s<Lp; s+=0.05)
    glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
  glEnd();

}

//: draw the edgel link graph
void dbdet_sel_tableau::draw_edgel_link_graph()
{
  //hsv pallete from matlab
  const double col_pal[8][3] = {   {     0,      0,      0},
                                  {     0,      0, 0.4000},
                                  {     0, 0.4000, 0.4000},
                                  {     0, 0.8000, 1.0000},
                                  //{     0, 1.0000,      0},
                                  {0.4000, 1.0000,      0},
                                  {0.8000, 1.0000,      0},
                                  {1.0000, 0.8000,      0},
                                  {1.0000, 0.4000,      0}};

  vcl_vector<dbdet_edgel*>& edgels = EM_->edgels;
  dbdet_edgel_link_graph& link_graph = ELG_;

  if (link_graph.cLinks.size()==0)
    return;

  //for all the edgels
  for (unsigned i=0; i<edgels.size(); i++)
  {
    dbdet_edgel* eA = edgels[i];

    //draw all the links
    dbdet_link_list_iter l_it = link_graph.cLinks[i].begin();
    for (; l_it != link_graph.cLinks[i].end(); l_it++)
    {
      dbdet_edgel* eB =(*l_it)->ce;

      int deg=0;
      if (color_by_votes_)
        deg = (*l_it)->vote; //display the number of votes
      else
        deg = (*l_it)->deg_overlap; //display min deg of overlap

      //only draw this link if it has a larger number of votes than the min votes to display parameter
      if ( deg >= min_votes_to_display_)
      {
        if (draw_links_in_color_){
          //display the different degrees of overlap in different colors and line widths
          glColor3f(col_pal[deg%8][0], col_pal[deg%8][1], col_pal[deg%8][2]);

          glLineWidth (float(deg) + 0.5);
        }
        else {
          glColor3f(link_color_[0], link_color_[1], link_color_[2]);
          glLineWidth (link_line_width_);
          //gl2psLineWidth(2.0);
        }

        glBegin(GL_LINES);
        glVertex2f(eA->pt.x(), eA->pt.y());
        glVertex2f(eB->pt.x(), eB->pt.y());
        glEnd();
      }
    }
  }

  ////for all the edgels
  //glColor3f( 0.0 , 0.0 , 1.0 );
  //glPointSize(5.0);
  //gl2psPointSize(5.0);
  //glBegin( GL_POINTS );
  //for (unsigned i=0; i<edgels.size(); i++)
  //{
  //  dbdet_edgel* eA = edgels[i];

  //  if (link_graph.cEdgels[i].size()>0 || link_graph.pEdgels[i].size()>0)
  //    glVertex2f(eA->pt.x(), eA->pt.y());
  //}
  //glEnd();

}

//: draw the edgel chains
void dbdet_sel_tableau::draw_edgel_chains()
{
  if (display_thick_curves_){
    float x1, y1, x2, y2;
    vgui_projection_inspector().image_to_window_coordinates(0, 0, x1, y1);
    vgui_projection_inspector().image_to_window_coordinates(CM_.params_.dpos_, 0, x2, y2);
    glLineWidth (vcl_fabs(x2-x1));
    //gl2psLineWidth(vcl_fabs(x2-x1));
  }
  else {
    glLineWidth (curve_line_width_);
    //gl2psLineWidth(curve_line_width_);
  }

  dbdet_edgel_chain_list_iter f_it = CFG_.frags.begin();
  for (; f_it != CFG_.frags.end(); f_it++)
  {
    dbdet_edgel_chain* chain = *f_it;

    //set contour color
    if (chain->temp)
      glColor3f(1.0f, 0.0f, 0.0f);
    else
      glColor3f(curve_color_[0], curve_color_[1], curve_color_[2]);
    
    if (chain->edgels.size()>=smallest_curve_fragments_to_display_){
      //By Naman Kumar
      if (display_multicolored_curves_)//randomized color for each chain
	{
		int i=0;
		while(i==0)
		{
			double a=(rand()%256); double b=(rand()%256); double c=(rand()%256);
        		if(a>10 && b>10 && c<50) {i=1;glColor3f(a/255.0,b/255.0,c/255.0);} //Not using Blue color as this color is used by edges.
			else continue;	
		}
	}
      glBegin(GL_LINE_STRIP);
      for (unsigned j=0; j<chain->edgels.size(); j++)
        glVertex2f(chain->edgels[j]->pt.x(), chain->edgels[j]->pt.y());
      glEnd();
    }
  }

  //draw the links in the CFTG as lines
  dbdet_CFTG_link_list_iter l_it = CFG_.CFTG.Links.begin();
  for (; l_it != CFG_.CFTG.Links.end(); l_it++)
  {
    dbdet_CFTG_link* link = (*l_it);

    glLineWidth (1.0f);//curve_line_width_
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);
    glVertex2f(link->eS->pt.x(), link->eS->pt.y());
    glVertex2f(link->eE->pt.x(), link->eE->pt.y());
    glEnd();
  }

  //next draw all the segments in the CFTG

  //display the node of the CFG in a different color
  if (display_CFG_end_points_){
    dbdet_edgel_chain_list_iter f_it = CFG_.frags.begin();
    for (; f_it != CFG_.frags.end(); f_it++)
    {
      dbdet_edgel_chain* chain = (*f_it);

      glColor3f( 0.0f , 0.0f , 1.0f );
      glPointSize(5.0);
      //gl2psPointSize(5.0);
      glBegin( GL_POINTS );
      glVertex2f(chain->edgels.front()->pt.x(),chain->edgels.front()->pt.y());
      glVertex2f(chain->edgels.back()->pt.x(),chain->edgels.back()->pt.y());
      glEnd();
    }
  }

}

//draw the contour groups
void
dbdet_sel_tableau::draw_contour_groups()
{
  if (display_thick_curves_){
    float x1, y1, x2, y2;
    vgui_projection_inspector().image_to_window_coordinates(0, 0, x1, y1);
    vgui_projection_inspector().image_to_window_coordinates(CM_.params_.dpos_, 0, x2, y2);
    glLineWidth (vcl_fabs(x2-x1));
    //gl2psLineWidth(vcl_fabs(x2-x1));
  }
  else {
    glLineWidth (curve_line_width_);
    //gl2psLineWidth(curve_line_width_);
  }

  vcl_list<dbdet_edgel_chain_list>::iterator l_it=c_groups_.begin();
  unsigned i=0;
  for(; l_it != c_groups_.end(); l_it++,i++)
  {
    dbdet_edgel_chain_list_iter f_it = (*l_it).begin();
    //set contours in the same group identical color
    glColor3f(color_mat_[i][0], color_mat_[i][1], color_mat_[i][2]);
    for (; f_it != (*l_it).end(); f_it++)
    {
      dbdet_edgel_chain* chain = *f_it;

    //set contour color
    /*if (chain->temp)
      glColor3f(1.0f, 0.0f, 0.0f);
    else
      glColor3f(curve_color_[0], curve_color_[1], curve_color_[2]);*/
    
      if (chain->edgels.size()>=smallest_curve_fragments_to_display_){		
        glBegin(GL_LINE_STRIP);
        for (unsigned j=0; j<chain->edgels.size(); j++)
	  glVertex2f(chain->edgels[j]->pt.x(), chain->edgels[j]->pt.y());
        glEnd();
      }
    }
  }

}

//draw the contour groups
void
dbdet_sel_tableau::draw_prune_contours()
{
  if (display_thick_curves_){
    float x1, y1, x2, y2;
    vgui_projection_inspector().image_to_window_coordinates(0, 0, x1, y1);
    vgui_projection_inspector().image_to_window_coordinates(CM_.params_.dpos_, 0, x2, y2);
    glLineWidth (vcl_fabs(x2-x1));
    //gl2psLineWidth(vcl_fabs(x2-x1));
  }
  else {
    glLineWidth (curve_line_width_);
    //gl2psLineWidth(curve_line_width_);
  }

    dbdet_edgel_chain_list_iter f_it = prune_frags_.begin();

    glColor3f(curve_color_[0], curve_color_[1], curve_color_[2]);
    for (; f_it != prune_frags_.end(); f_it++)
    {
      dbdet_edgel_chain* chain = *f_it;
    
      if (chain->edgels.size()>=smallest_curve_fragments_to_display_){		
        glBegin(GL_LINE_STRIP);
        for (unsigned j=0; j<chain->edgels.size(); j++)
	  glVertex2f(chain->edgels[j]->pt.x(), chain->edgels[j]->pt.y());
        glEnd();
      }
    }

}
void 
dbdet_sel_tableau::print_edgel_stats(dbdet_edgel * e)
{
  vcl_cout << "========= EDGEL =========\n";
  vcl_cout << "id: " << e->id;
  if (ELG_.linked.size()==0)
    vcl_cout << " : LG [NL]";
  else {
    if (ELG_.linked[e->id])
      vcl_cout << " : LG [L]";
    else
      vcl_cout << " : LG [NL]";  
  }

  //if (EULM_(e->gpt.x(), e->gpt.y()))
  //  vcl_cout << ", GRID [NL]" << vcl_endl;
  //else
  //  vcl_cout << ", GRID [L]" << vcl_endl;
  vcl_cout << vcl_endl;

  //Print the curvelet info
  if (!CM_.is_valid())
    return ;

  //first determine the max number of edgels before the ref edgel in all of the curvelets
  unsigned num = 0;
  vcl_list<dbdet_curvelet* >::iterator cv_it = CM_.curvelets(e->id).begin();
  for ( ; cv_it!=CM_.curvelets(e->id).end(); cv_it++){
    dbdet_curvelet* cvlet = (*cv_it);

    //print anchored curvelets only
    //if (draw_anchored_only_ && (*cv_it)->ref_edgel != e) continue; 

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]==e && k>num)
        num=k;
    }
  }

  //print the marker for the ref edge column
  for (unsigned i=0; i<=num; i++) vcl_cout << "\t";
  vcl_cout << "***" << vcl_endl;

  //now print the curvelet with the ref edgel at the correct position
  cv_it = CM_.curvelets(e->id).begin();
  for ( ; cv_it!=CM_.curvelets(e->id).end(); cv_it++)
    print_cvlet_info((*cv_it), e, num);
  
}

void dbdet_sel_tableau::print_link_info(dbdet_link* link)
{
  vcl_cout << "========= LINK =========\n";
  vcl_cout << "(" << link->pe->id << " --> " << link->ce->id << ") : ";
  vcl_cout << "deg. overlap = " << link->deg_overlap << vcl_endl;  

  //first determine the max number of edgels before the ref edgel in all of the curvelets
  unsigned num = 0;
  cvlet_list_iter cv_it = link->curvelets.begin();
  for ( ; cv_it != link->curvelets.end(); cv_it++){
    dbdet_curvelet* cvlet = (*cv_it);

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]== link->pe && k>num)
        num=k;
    }
  }

  //print the marker for the ref edge column
  for (unsigned i=0; i<=num; i++) vcl_cout << "\t";
  vcl_cout << "***" << vcl_endl;

  //now print the curvelet with the ref edgel at the correct position
  cv_it = link->curvelets.begin();
  for ( ; cv_it != link->curvelets.end(); cv_it++)
    print_cvlet_info((*cv_it), link->pe, num);
}

void dbdet_sel_tableau::print_cvlet_info(dbdet_curvelet* cvlet, dbdet_edgel* e, unsigned pos)
{
  //pos refers to the position of the ref edgel

  if (display_groupings_special_) //immediate links only
  {
    vcl_cout << "Local Chain:"; 
    for (unsigned i=0; i < cvlet->edgel_chain.size(); ++i) {
      if ( cvlet->edgel_chain[i]==e || //current edgel
      (i>0 && cvlet->edgel_chain[i-1]==e) || //edgel after it
      (i<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[i+1]==e) //edgel before it
      )
        vcl_cout << " " << cvlet->edgel_chain[i]->id;
    }
  }
  else {
    vcl_cout << "Chain: "; 
    if (cvlet->forward) vcl_cout << "F : ";
    else                vcl_cout << "B : ";

    
    //find the position of the ref in the current cvlet
    int num=0;
    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]==e)
          num=k;
    }

    //print the edgels in the correct positions
    for (int i=0; i<(int(pos) - num); i++) vcl_cout << "\t";

    for (unsigned i=0; i < cvlet->edgel_chain.size(); ++i)
      vcl_cout << "\t" << cvlet->edgel_chain[i]->id;

    ////put more tabs to the end of the cvlets
    //for (int i=0; i<(CM_.maxN_/CM_.token_len_ - (cvlet->edgel_chain.size()-num)); i++)  vcl_cout << "\t";
  }

  //print curve params
  cvlet->curve_model->print_info();

  //print curvelet quality info
  vcl_cout << ", dir= " << cvlet->forward << ", Q= " << cvlet->quality << ", L= " << cvlet->length ;
  vcl_cout << vcl_endl;
}

void 
dbdet_sel_tableau::get_popup(const vgui_popup_params& /*params*/, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_points_)?on:off)+"Show Points", 
               new dbdet_sel_tableau_toggle_command(this, &display_points_));

  submenu.separator();

  submenu.add( "Recompute Curvelet quality", new dbdet_sel_tableau_sel_command(this, 14));
  submenu.add( "Prune curvelets by quality", new dbdet_sel_tableau_sel_command(this, 13));
  //submenu.add( "Prune curvelets by gap size", new dbdet_sel_tableau_sel_command(this, 19));
  submenu.add( "Prune curvelets by length", new dbdet_sel_tableau_sel_command(this, 21));
  submenu.add( "Prune curvelets by C1 continuity", new dbdet_sel_tableau_sel_command(this, 22));

  submenu.separator();

  submenu.add( ((display_groupings_)?on:off)+"Show Groupings", 
               new dbdet_sel_tableau_toggle_command(this, &display_groupings_));

  submenu.add( ((display_curve_)?on:off)+"Show curve fits for each grouping", 
               new dbdet_sel_tableau_toggle_command(this, &display_curve_));

  submenu.add( ((display_curve_bundle_)?on:off)+"Show the whole bundle", 
               new dbdet_sel_tableau_toggle_command(this, &display_curve_bundle_));

  submenu.add( "Set MIN size of groupings to display", 
               new dbdet_sel_tableau_set_int_command(this, "smallest order", &smallest_to_display_));

  submenu.add( "Set MAX size of groupings to display", 
               new dbdet_sel_tableau_set_int_command(this, "largest order", &largest_to_display_));

  submenu.add( "Set min quality of groupings to display", 
               new dbdet_sel_tableau_set_double_command(this, "min quality", &min_quality_to_display_));

  submenu.add( "Set adaptive appearance threshold", 
               new dbdet_sel_tableau_set_double_command(this, "threshold", &app_threshold_));

  //submenu.add( ((display_largest_)?on:off)+"Show Largest Groupings Only", 
  //             new dbdet_sel_tableau_toggle_command(this, &display_largest_));

  //submenu.add( ((display_groupings_special_)?on:off)+"Show Special Links Only", 
  //             new dbdet_sel_tableau_toggle_command(this, &display_groupings_special_));

  //submenu.add( ((display_extra_special_)?on:off)+"Show Reciprocal Special Links Only", 
  //             new dbdet_sel_tableau_toggle_command(this, &display_extra_special_));

  submenu.add( ((only_display_used_curvelets_)?on:off)+"Show Used Curvelets Only", 
               new dbdet_sel_tableau_toggle_command(this, &only_display_used_curvelets_));

  submenu.add( ((display_app_consistent_only_)?on:off)+"Show Appearance consistent Only", 
               new dbdet_sel_tableau_toggle_command(this, &display_app_consistent_only_));

  submenu.add( "Set Curvelet Style",
               new dbdet_sel_tableau_set_style_command(this, &cvlet_color_, &cvlet_line_width_));

  submenu.add( ((display_multicolored_curvelets_)?on:off)+"Show Multi-colored curvelets", 
               new dbdet_sel_tableau_toggle_command(this, &display_multicolored_curvelets_));

  submenu.separator();

  // Linking commands are temporarily located here so that we can interactively look at it
  
  submenu.add( "Construct A Naive Link Graph", new dbdet_sel_tableau_sel_command(this, 18));

  submenu.add( "Construct Link Graph", new dbdet_sel_tableau_sel_command(this, 1));

  //prune the link graph
  submenu.add( "Prune the Link Graph", new dbdet_sel_tableau_sel_command(this, 12));

  // make the link graph bidirectionally consistent
  submenu.add( "Make the Link Graph Consistent", new dbdet_sel_tableau_sel_command(this, 15));

  //separate the link graphs by direction
  submenu.add( "Separate LGs and CMs", new dbdet_sel_tableau_sel_command(this, 16));

  //setup DHT mode
  submenu.add( "Setup for DHT mode", new dbdet_sel_tableau_sel_command(this, 17));

  submenu.separator();

  submenu.add( ((display_link_graph_)?on:off)+"Display link graph", 
               new dbdet_sel_tableau_toggle_command(this, &display_link_graph_));

  submenu.add( "Set MIN votes to display", 
               new dbdet_sel_tableau_set_int_command(this, "min_votes_to_dislay_", &min_votes_to_display_));

  submenu.add( ((color_by_votes_)?on:off)+"Color by votes/Color by deg overlap", 
               new dbdet_sel_tableau_toggle_command(this, &color_by_votes_));

  submenu.add( ((draw_links_in_color_)?on:off)+"Draw Links in Color", 
               new dbdet_sel_tableau_toggle_command(this, &draw_links_in_color_));

  submenu.add( "Set Link Style",
               new dbdet_sel_tableau_set_style_command(this, &link_color_, &link_line_width_));

  submenu.separator();

  submenu.add( "Clear All Contours", new dbdet_sel_tableau_sel_command(this,  2));

  submenu.add( "Set min. deg overlap to Link", new dbdet_sel_tableau_sel_command(this,  6));
  
  submenu.add( "Extract Contours From the Link Graph (iteratively)", new dbdet_sel_tableau_sel_command(this, 3));

  submenu.add( "Extract Regular Contours", new dbdet_sel_tableau_sel_command(this,  5));

  //submenu.add( "Extract Contours From the Image Grid", new dbdet_sel_tableau_sel_command(this,  4));

  submenu.add( "Construct All Hyp Trees", new dbdet_sel_tableau_sel_command(this,  23));

  submenu.add( "Disambiguate the CFTG", new dbdet_sel_tableau_sel_command(this,  24));
 
  submenu.add( "Construct Hypothesis Tree-2-", new dbdet_sel_tableau_sel_command(this,  27)); // By Naman Kumar

  submenu.add( "Disambiguation-2-", new dbdet_sel_tableau_sel_command(this,  28)); // By Naman Kumar

  submenu.add( "Correct the CFG Topology", new dbdet_sel_tableau_sel_command(this,  25)); 

  submenu.add( "Post_Process", new dbdet_sel_tableau_sel_command(this,  30)); // By Naman Kumar

  submenu.add( "Get Final Contours in One Step", new dbdet_sel_tableau_sel_command(this, 26));//  Add by Yuliang Guo 10/13/2010

  submenu.add( "Fit Poly-arc Bundles", new dbdet_sel_tableau_sel_command(this,  20));

  submenu.separator();

  submenu.add( ((display_contours_)?on:off)+"Display Regular Contours", 
               new dbdet_sel_tableau_toggle_command(this, &display_contours_));

  submenu.add( ((display_multicolored_curves_)?on:off)+"Show Multi-colored curves", 
               new dbdet_sel_tableau_toggle_command(this, &display_multicolored_curves_));

  submenu.add( ((display_thick_curves_)?on:off)+"Show Thick curves", 
               new dbdet_sel_tableau_toggle_command(this, &display_thick_curves_));

  submenu.add( ((display_CFG_end_points_)?on:off)+"Show CFG Nodes", 
               new dbdet_sel_tableau_toggle_command(this, &display_CFG_end_points_));

  submenu.add( ((display_contour_groups_)?on:off)+"show contour groups", 
               new dbdet_sel_tableau_toggle_command(this, &display_contour_groups_));

  submenu.add( ((display_prune_contours_)?on:off)+"show prune contours", 
               new dbdet_sel_tableau_toggle_command(this, &display_prune_contours_));
 
  submenu.add( "Set Curve Fragment Style",
               new dbdet_sel_tableau_set_style_command(this, &curve_color_, &curve_line_width_));

  submenu.separator();

  submenu.add( "Post Process", new dbdet_sel_tableau_sel_command(this,  7));

  submenu.add( "Prune Short Contours", new dbdet_sel_tableau_sel_command(this,  10));

  //submenu.separator();

  //submenu.add( "Report Accuracy", new dbdet_sel_tableau_sel_command(this,  8));

  //submenu.add( "Compute Curvelet Quality", new dbdet_sel_tableau_sel_command(this,  9));
  
  //submenu.separator();

  //submenu.add( "Set Local Zoom Factor", 
  //             new dbdet_sel_tableau_set_int_command(this, "Local Zoom factor", &local_zoom_factor));

  //add this submenu to the popup menu
  menu.add("Tableau Options", submenu);
}
