#include "dbdet_sel_explorer_tool.h"

#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#define MANAGER bvis1_manager::instance()

#include <dbdet/algo/dbdet_sel.h>


dbdet_sel_explorer_tool::dbdet_sel_explorer_tool() : 
  nviews_(1), 
  sel_storage_(0),  
  p0_(0),
  p0_style_(vgui_style::new_style(1, 1, 0, 
                                    8.0f/*ptsize*/, 3.0f/*linesize*/)),  // Yellow
  cur_edgel(0),
  display_ES_(false),
  min_order_to_display_(4),
  display_immediate_links_only_(false)
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  frame_v_.resize(nviews_);
  tab_.resize(nviews_);
}


vcl_string dbdet_sel_explorer_tool::
name() const
{
  return "SEL explorer";
}

bool dbdet_sel_explorer_tool::
set_tableau( const vgui_tableau_sptr& /*tableau*/ )
{
  return true;
}


void   
dbdet_sel_explorer_tool::
activate ()
{
  vcl_cout << "dbdet_sel_explorer_tool ON\n";

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if (views.size() < nviews_) {
    vgui::out << "Error: need at least " << nviews_ << " views for this tool" << vcl_endl;
    vcl_cerr << "Error: need at least " << nviews_ << " views for this tool" << vcl_endl;
    return;
  }

  for (unsigned i=0; i<nviews_; ++i) {
    frame_v_[i] = views[i]->frame();
  }
  
  vcl_cout << "Working in frames ";
  for (unsigned i=0; i<nviews_; ++i) {
    vcl_cout << frame_v_[i] << "  " ;
  }
  vcl_cout << vcl_endl;

  bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("sel",frame_v_[0]);

  sel_storage_.vertical_cast(p);
  if(!sel_storage_) {
    vcl_cerr << "Error: tool requires a dbdet sel storage" << vcl_endl;
    return;
  }
  vcl_cout << "success" << vcl_endl;

  // -------- Add tableaus to draw on

  vcl_string type("vsol2D");
  vcl_string name("dbdet_sel_explorer_aux");

  for (unsigned i=0 ; i < nviews_; ++i) {

    bpro1_storage_sptr 
      n_data  = MANAGER->repository()->new_data_at(type,name,frame_v_[i]);

    if (n_data) {
       MANAGER->add_to_display(n_data);
    } else {
       vcl_cerr << "error: unable to register new data\n";
       return ;
    }

  }

  MANAGER->display_current_frame();

  for (unsigned i=0 ; i < nviews_; ++i) {
    vgui_tableau_sptr tab_ptr1 = views[i]->selector()->get_tableau(name);
    if (tab_ptr1) {
      tab_[i].vertical_cast(tab_ptr1);
    } else {
      vcl_cerr << "error: Could not find child tableaus in selector\n";
      return ;
    }

    vcl_string active;
    active = views[i]->selector()->active_name();

    views[i]->selector()->set_active(name);
    views[i]->selector()->active_to_top();
    views[i]->selector()->set_active(active);
  }

}

void dbdet_sel_explorer_tool::
deactivate ()
{
  vcl_cout << "dbdet_sel_explorer_tool OFF\n";
}


bool dbdet_sel_explorer_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{

  if (e.type == vgui_KEY_PRESS) {
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;

    switch (e.key) {
      case 'p': { // print info on the storage
        vcl_cout << "Name: " << sel_storage_->name() << vcl_endl;
        print_sel_stats();

        return true;
        break;
      }

      case 'a': { // print info on the currently selected edgel
        if (cur_edgel)
          print_complete_edgel_stats(cur_edgel);

        return true;
        break;
      }

      default:
        vcl_cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  if (gesture0_(e)) {
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;

    if (p0_) {
      tab_[0]->remove(p0_);
      tab_[0]->post_overlay_redraw();
    }

    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // I) Find edgel closest to ix,iy
    cur_edgel = get_nearest_edgel(vgl_point_2d<double>(ix, iy));

    if (cur_edgel)
    {
      vgl_point_2d<double> pt = cur_edgel->pt;
      vcl_cout << "Closest edgel: " << pt << vcl_endl;

      // II) Add a point to the vsol tableau
      for (unsigned i=0 ; i < nviews_; ++i) 
        tab_[i]->set_current_grouping( "Drawing" );


      p0_ = tab_[0]->add_point(pt.x(),pt.y());
      p0_->set_style(p0_style_);

      tab_[0]->post_overlay_redraw();
      
      // III) Display info about the selected edgel
      vcl_cout << vcl_endl;
      print_edgel_stats(cur_edgel);
    }

  }

  if( e.type == vgui_OVERLAY_DRAW ) {
    //if edgel selected, draw the groups it forms
    if (cur_edgel){
      dbdet_curvelet_map& CM = sel_storage_->CM();

      curvelet_list_iter cv_it = CM.curvelets(cur_edgel->id).begin();
      for ( ; cv_it!=CM.curvelets(cur_edgel->id).end(); cv_it++)
      {
        dbdet_curvelet* cvlet = (*cv_it);

        if (cvlet->order()>min_order_to_display_){
          draw_grouping(cvlet);
          
          if (display_ES_)
            draw_ES_ft( cvlet, cur_edgel);
        }
      }
    }
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

dbdet_edgel* dbdet_sel_explorer_tool::get_nearest_edgel(vgl_point_2d<double> pt)
{
  dbdet_edgel* ne=0;

  // Find edgel closest to pt

  // a) find the cell that this point belongs to
  int xx = dbdet_round(pt.x());
  int yy = dbdet_round(pt.y());

  dbdet_edgemap_sptr edgemap = sel_storage_->EM();

  // b) find the closest edgel in the neighboring cells
  double dmin = vcl_numeric_limits<double>::infinity();
  for (int xcell = xx-2; xcell <= xx+2; xcell++){
    for (int ycell = yy-2; ycell <= yy+2; ycell++){
      if (xcell < 0 || ycell < 0 || xcell >= (int)edgemap->width() || ycell >= (int)edgemap->height()) 
        continue;

      for (unsigned i=0; i<edgemap->cell(xcell, ycell).size(); i++){
        dbdet_edgel* edgel = edgemap->cell(xcell, ycell)[i];

        double dx = edgel->pt.x() - pt.x(); 
        double dy = edgel->pt.y() - pt.y(); 
        double d = dx*dx+dy*dy;
        if (d < dmin){
          dmin = d;
          ne = edgel;
        }
      }
    }
  }

  return ne;
}

void dbdet_sel_explorer_tool::draw_grouping(const dbdet_curvelet* cvlet)
{
  dbdet_edgel* eA = cvlet->ref_edgel;

  double dpos = sel_storage_->CM().params_.dpos_;

  if (display_immediate_links_only_)
  {
    //draw the bounding circles around each edgel
    glColor3f( 0.0f , 0.6f , 0.0f );
    glLineWidth (1.0);
    for (unsigned j=0; j<cvlet->edgel_chain.size(); j++)
    {
      if ( cvlet->edgel_chain[j]==eA || //current edgel
          (j>0 && cvlet->edgel_chain[j-1]==eA) || //edgel after it
          (j<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[j+1]==eA) //edgel before it
          )
      {
        glBegin( GL_LINE_STRIP );
        for (int th=0; th<=20; th++){
          double theta = th*2*vnl_math::pi/20.0;
          glVertex2f(cvlet->edgel_chain[j]->pt.x() + dpos*vcl_cos(theta), 
                     cvlet->edgel_chain[j]->pt.y() + dpos*vcl_sin(theta));
        }
        glEnd();
      }
    }

    glColor3f( 1.0f, 0.0f, 0.0f );
    glLineWidth (2.0);
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
  else {
    //draw the bounding circles around each edgel
    glColor3f( 0.0f , 0.6f , 0.0f );
    glLineWidth (1.0);
    for (unsigned j=0; j<cvlet->edgel_chain.size(); j++){
      glBegin( GL_LINE_STRIP );
      for (int th=0; th<=20; th++){
        double theta = th*2*vnl_math::pi/20.0;
        glVertex2f(cvlet->edgel_chain[j]->pt.x() + dpos*vcl_cos(theta), 
                   cvlet->edgel_chain[j]->pt.y() + dpos*vcl_sin(theta));
      }
      glEnd();
    }
    
    //draw the links between edgels of the group
    glColor3f( 1.0f, 0.0f, 0.0f );
    glLineWidth (2.0);
    glBegin( GL_LINE_STRIP );
    for (unsigned j=0; j<cvlet->edgel_chain.size(); j++){
      glVertex2f(cvlet->edgel_chain[j]->pt.x(), cvlet->edgel_chain[j]->pt.y());
    }
    glEnd();
  }

}

//: draw the best fit ES of a curvelet grounded at the given edgel
void dbdet_sel_explorer_tool::draw_ES_ft(dbdet_curvelet* /*cvlet*/, dbdet_edgel* /*eA*/)
{
  //for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
  //  if (cvlet->edgel_chain[k]!=eA)
  //    continue;
  //
  //  for (int pos=0; pos<NPerturb*NPerturb; pos++){
  //    if (cvlet->curve_bundles[k] &&
  //        cvlet->curve_bundles[k]->bundle_valid[pos])
  //    {
  //      //draw the centroid ES of each bundle

  //      //assign the rough centroid of this bundle as the best estimate
  //      double k_est = 0;
  //      double gamma_est = 0;
  //      for (unsigned ii=0; ii<cvlet->curve_bundles[k]->cv_bundle[pos][0].size(); ii++){
  //        k_est += cvlet->curve_bundles[k]->cv_bundle[pos][0][ii].x();
  //        gamma_est += cvlet->curve_bundles[k]->cv_bundle[pos][0][ii].y();
  //      }
  //      k_est /= cvlet->curve_bundles[k]->cv_bundle[pos].num_vertices();
  //      gamma_est /= cvlet->curve_bundles[k]->cv_bundle[pos].num_vertices();

  //      cvlet->curve_bundles[k]->k = k_est;
  //      cvlet->curve_bundles[k]->gamma = gamma_est;

  //      //compute the ES
  //      vgl_point_2d<double> edgel_pt;
  //      double edgel_tangent;
  //      sel_->compute_perturbed_position_of_an_edgel(eA, pos, edgel_pt, edgel_tangent);

  //      dbgl_eulerspiral es_fit(edgel_pt, edgel_tangent, k_est, gamma_est, 3);

  //      // compute length on the minus and plus sides
  //      bool before_eA = true;
  //      double Lm=0, Lp=0;
  //      for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++)
  //      {
  //        if (before_eA)
  //          Lm += vgl_distance(cvlet->edgel_chain[i]->pt, cvlet->edgel_chain[i+1]->pt);
  //        else
  //          Lp += vgl_distance(cvlet->edgel_chain[i]->pt, cvlet->edgel_chain[i+1]->pt);

  //        if (cvlet->edgel_chain[i+1]==eA)
  //          before_eA = false;
  //      }

  //      //draw the ES
  //      glColor3f(0.0f,1.0f,1.0f);
  //      glLineWidth (3.0);
  //      glBegin(GL_LINE_STRIP);
  //      for (double s=-Lm; s<Lp; s+=0.1)
  //        glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
  //      glEnd();

  //      ////draw each of the ES in the valid bundles
  //      //for (unsigned p=0; p<cvlet->curve_bundles[k]->cv_bundle[pos][0].size(); p++)
  //      //{
  //      //  vgl_point_2d<double> edgel_pt;
  //      //  double edgel_tangent;

  //      //  sel_->compute_perturbed_position_of_an_edgel(eA, pos, edgel_pt, edgel_tangent);

  //      //  dbgl_eulerspiral es_fit(edgel_pt, edgel_tangent, 
  //      //    cvlet->curve_bundles[k]->cv_bundle[pos][0][p].x(), 
  //      //    cvlet->curve_bundles[k]->cv_bundle[pos][0][p].y(), 3);

  //      //  glColor3f(1.0f,0.0f,0.0f);
  //      //  glBegin(GL_LINE_STRIP);
  //      //  for (double s=-4.0; s<0; s+=0.1)
  //      //    glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
  //      //  glEnd();

  //      //  glColor3f(1.0f,0.0f,0.0f);
  //      //  glBegin(GL_LINE_STRIP);
  //      //  for (double s=0; s<4.0; s+=0.1)
  //      //    glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
  //      //  glEnd();
  //      //}
  //    }
  //  }
  //}
}

void dbdet_sel_explorer_tool::
print_sel_stats() const
{
  /*
  unsigned nquads = 0, nedgels = 0, max_nquads=0, min_nquads=(unsigned)-1;

  // print quads per edgel, max quads per edgel
  for (unsigned i = 0; i < sel_->nrows(); ++i)
    for (unsigned j = 0; j < sel_->ncols(); ++j)
      for (unsigned k = 0; k < sel_->cells()[i][j].size() ; ++k) {
        dbdet_edgel *edgel = sel_->cells()[i][j][k];
        unsigned current_nquads
            = edgel->local_curvelets[2].size();
        nquads += current_nquads;
        if (current_nquads > max_nquads)
          max_nquads = current_nquads;
        if (current_nquads < min_nquads)
          min_nquads = current_nquads;
        nedgels++;
      }
  
  vcl_cout << "#edgels: " << nedgels << vcl_endl;
  vcl_cout << "#quads: " << nquads << vcl_endl;
  vcl_cout << "max  #quads per edgel: " << max_nquads << vcl_endl;
  vcl_cout << "min  #quads per edgel: " << min_nquads << vcl_endl;
  vcl_cout << "mean #quads per edgel: " << (double)nquads/(double)nedgels << vcl_endl;
  */
}

void dbdet_sel_explorer_tool::
print_complete_edgel_stats(dbdet_edgel * /*e*/) const
{
/*  vcl_cout << "========= EDGEL =========\n";
  vcl_cout << "id: " << e->id << vcl_endl;  
  for (unsigned j=0; j<e->local_curvelets.size(); j++)
    vcl_cout << j+2 << ":" << e->local_curvelets[j].size() << ", ";
  vcl_cout << vcl_endl;  

  //write out all the groupings around this edgel
  for (unsigned j=1; j<e->local_curvelets.size(); j++)
  {
    curvelet_list_iter it = e->local_curvelets[j].begin();
    for (; it != e->local_curvelets[j].end(); ++it) 
    {
      vcl_cout << "Chain:"; 
      for (unsigned i=0; i < (*it)->edgel_chain.size(); ++i) {
        vcl_cout << " " << (*it)->edgel_chain[i]->id;
      }

      vcl_cout << " : ";
      //for (unsigned i=0; i < (*it)->curve_bundles.size(); ++i) {
      //  if ( (*it)->edgel_chain[i] == e){
      //    if ( (*it)->curve_bundles[i]) {
      //      vcl_cout << "Bundle: (k,kdot): " << (*it)->curve_bundles[i]->k <<
      //        ", " <<  (*it)->curve_bundles[i]->gamma << vcl_endl;
      //    } else {
      //      vcl_cout << "Bundle: NULL" << vcl_endl;
      //    }
      //  }
      //}
      vcl_cout << vcl_endl;
    }
  }


  // - number of curve bundles per curvelet
  // - max curvature, min curvature

  curvelet_list_iter it = e->local_curvelets[2].begin();
  unsigned n=1;
  for (; it != e->local_curvelets[2].end(); ++it, ++n) {
    vcl_cout << "   ==== Curvelet #" << n << " ====" << vcl_endl;
    vcl_cout << "Chain:"; 
    for (unsigned i=0; i < (*it)->edgel_chain.size(); ++i) {
      vcl_cout << " " << (*it)->edgel_chain[i]->id;
    }
    vcl_cout << vcl_endl;
    vcl_cout << "#bundles: " << (*it)->curve_bundles.size() << vcl_endl;
    for (unsigned i=0; i < (*it)->curve_bundles.size(); ++i) {
      vcl_cout << " --- Bundle #" << i << ":  ";
      if ( (*it)->curve_bundles[i] ) {
        vcl_cout << "k,kdot: " << (*it)->curve_bundles[i]->k <<
          ", " <<  (*it)->curve_bundles[i]->gamma << "    ";
        vcl_cout << "ref_edgel: " << (*it)->curve_bundles[i]->edgel->id << "    ";
      } else {
        vcl_cout << "NULL    ";
      }
      vcl_cout << vcl_endl;
    }
  }
*/
}

void dbdet_sel_explorer_tool::
print_edgel_stats(dbdet_edgel * e) const
{
  vcl_cout << "========= EDGEL =========\n";
  vcl_cout << "id: " << e->id << vcl_endl;  
  
  //write out the largest order groupings
  dbdet_curvelet_map& CM = sel_storage_->CM();

  curvelet_list_iter it = CM.curvelets(e->id).begin();
  for (; it != CM.curvelets(e->id).end(); ++it) 
  {
    dbdet_curvelet* cvlet = (*it);

    if (display_immediate_links_only_)
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
      vcl_cout << "Chain:"; 
      for (unsigned i=0; i < cvlet->edgel_chain.size(); ++i) {
        vcl_cout << " " << cvlet->edgel_chain[i]->id;
      }
    }
    vcl_cout << vcl_endl;
  }

}

void bvis1_tool_set_int_param(const void* intref)
{
  int* iref = (int*)intref;
  int param_val = *iref;
  vgui_dialog param_dlg("Set Tool Param");
  param_dlg.field("Parameter Value", param_val);
  if(!param_dlg.ask())
    return;

  *iref = param_val;
}

void 
dbdet_sel_explorer_tool::get_popup( const vgui_popup_params& /*params*/, vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( ((display_ES_)?on:off)+"Display ES fit", 
            bvis1_tool_toggle, (void*)(&display_ES_) );

  menu.add( "Min order to display", 
            bvis1_tool_set_int_param, (void*)(&min_order_to_display_) );

  menu.add( ((display_immediate_links_only_)?on:off)+"Display immeidate links only", 
            bvis1_tool_toggle, (void*)(&display_immediate_links_only_) );

}

