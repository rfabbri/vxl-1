#include "dbdet_sel_edge_linking_tool.h"

#include <vcl_limits.h>

#include <vcl_algorithm.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bpro1/bpro1_storage_sptr.h>

#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/vis/dbdet_sel_tableau.h>


dbdet_sel_edge_linking_tool::dbdet_sel_edge_linking_tool(): sel_storage_(0), active(false)
{
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  gesture2 = vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true);
  point_list.clear();
}


vcl_string dbdet_sel_edge_linking_tool::name() const
{
  return "SEL Edge Linker";
}

bool dbdet_sel_edge_linking_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
    sel_tab_ = temp_tab;
    return true;
  }
  return false;
}

bool dbdet_sel_edge_linking_tool::handle( const vgui_event & e, 
                                          const bvis1_view_tableau_sptr& /*view*/ )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if (e.type == vgui_MOTION){
    last_x = ix;
    last_y = iy;
    if (active){
        sel_tab_->post_overlay_redraw();
    }
  }

  if (!active && gesture0(e)){ //first left click
    active = true;
    point_list.push_back(vgl_point_2d<double>(ix,iy));
    return true;
  } 
  else if (active)
  {
    if( gesture0(e)){ //subsequent left clicks (add to contour)
      point_list.push_back(vgl_point_2d<double>(ix,iy));
      return true;
    }

    if (e.type == vgui_OVERLAY_DRAW) //draw current contour
    {
      glLineWidth(1.0);
      glColor3f(0.0,1.0,0.0);

      glBegin(GL_LINE_STRIP);
      for (unsigned i=0; i<point_list.size(); ++i)
        glVertex2f(point_list[i].x(), point_list[i].y());
      glVertex2f(last_x,last_y);
      glEnd();
      return true;
    }

    if (gesture1(e) || gesture2(e)) //middle button to end the contour trace
    {
      if (gesture1(e)) 
        point_list.push_back(vgl_point_2d<double>(ix,iy));

      //form the best contour fragment from this trace 
      dbdet_edgel_chain* new_chain = form_edgel_chain_from_trace(point_list);

      //and add it to the contour fragment graph
      if (new_chain->edgels.size()>0){
        sel_storage_->CFG().insert_fragment(new_chain);
      }
      else
        delete new_chain;

      sel_tab_->post_redraw();
      active = false;

      point_list.clear();
      return true;
    }
  }

  return false;
}

#define samp_int 0.5

dbdet_edgel_chain* dbdet_sel_edge_linking_tool::form_edgel_chain_from_trace(vcl_vector<vgl_point_2d<double> > &pts)
{
  vcl_vector<dbdet_edgel*> edgels;

  //sample the trace curve at regular intervals
  for (unsigned i=0; i<pts.size(); i++)
  {
    if (i<pts.size()-1){
      //form samples at constant intervals
      vgl_vector_2d<double> interval = pts[i+1]-pts[i];
      unsigned num = (int) vcl_floor(interval.length()/samp_int);

      for (unsigned j=0; j<num; j++){
        vgl_point_2d<double> spt = pts[i] + (double(j)/double(num))*interval;

        //find the closest edgel to each sample on the trace curve
        dbdet_edgel* ne = get_nearest_edgel(spt);

        if (ne)
          edgels.push_back(ne);
      }
    }
  }

  //form an edgel chain out of the list of edgels
  dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();

  //remove redundant edgels
  dbdet_edgel* last_edgel = 0;
  for (unsigned j=0; j<edgels.size(); j++){
    if (edgels[j]==last_edgel)
      continue;
    else {
      new_chain->push_back(edgels[j]);
      last_edgel = edgels[j];
    }
  }

  return new_chain;
}

dbdet_edgel* dbdet_sel_edge_linking_tool::get_nearest_edgel(vgl_point_2d<double> pt)
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


void dbdet_sel_edge_linking_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                vgui_menu &/*menu*/ )
{
  vcl_string on = "[x] ", off = "[ ] ";

  //menu.add( ((plot_intensity)?on:off)+"Plot Intensity", 
  //          bvis1_tool_toggle, (void*)(&plot_intensity) );

  //menu.add( ((dist_wrt_cur)?on:off)+"Compute Dist wrt current edgel", 
  //          bvis1_tool_toggle, (void*)(&dist_wrt_cur) );

  //menu.add( ((auto_scale_val)?on:off)+"AutoScale Value-Axis", 
  //          bvis1_tool_toggle, (void*)(&auto_scale_val) );

  //menu.add( "Set max val", 
  //          new dbdet_sel_tableau_set_display_params_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Max Value", &vmax));
  //

}

