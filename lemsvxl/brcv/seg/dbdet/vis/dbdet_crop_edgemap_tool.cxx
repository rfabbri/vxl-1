#include "dbdet_crop_edgemap_tool.h"

#include <vcl_limits.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>

#include <vgl/vgl_polygon.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/vis/dbdet_edgemap_tableau.h>

dbdet_crop_edgemap_tool::dbdet_crop_edgemap_tool() :  
  edge_tab_(0),
  edgemap_(0),
  left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  end_button(vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true)),
  active(false),
  point_list(0),
  last_x(0.0), last_y(0.0)
{
}

vcl_string dbdet_crop_edgemap_tool::name() const
{
  return "Crop Edgemap";
}

//: Set the tableau associated with the current view
bool dbdet_crop_edgemap_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbdet_edgemap_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its an edgemap storage class
  if (storage->type() == "edge_map"){
    dbdet_edgemap_storage_sptr edgemap_storage;
    edgemap_storage.vertical_cast(storage);
    edgemap_ = edgemap_storage->get_edgemap();
    edge_tab_ = temp_tab;
    return true;
  }
  return false;
}

bool dbdet_crop_edgemap_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& /*view*/ )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if( e.type == vgui_MOTION ) {
    last_x = ix;
    last_y = iy;
    if( active ) {
        edge_tab_->post_overlay_redraw();
    }
  }

  if( !active && left_click(e) ) {
    active = true;
    point_list.push_back(vgl_point_2d<float>(ix, iy));
    return true;
  } 
  else if( active ) 
  {
    if( left_click(e) ) {
      point_list.push_back(vgl_point_2d<float>(ix, iy));
      return true;
    }

    //draw the current polygon
    if( e.type == vgui_OVERLAY_DRAW ) 
    {
      glLineWidth(1);
      glColor3f(1,0,0);

      glBegin(GL_LINE_LOOP);
      for (unsigned i=0; i<point_list.size(); ++i)
        glVertex2f(point_list[i].x(), point_list[i].y() );
      glVertex2f(last_x,last_y);
      glEnd();
      return true;
    }

    if( middle_click(e) || end_button(e) ) {
      if(middle_click(e))
         point_list.push_back(vgl_point_2d<float>(ix, iy));

      if (point_list.size() < 3)
         return true;

      //crop the edgemap with the current polygon
      crop_edgemap();

      edge_tab_->post_redraw();
      active = false;
      point_list.clear();

      return true;
    }
  }
    
  return false;
}

void dbdet_crop_edgemap_tool::crop_edgemap()
{
  vgl_polygon<float> cur_poly(point_list);

  //go over each cell and delete the edgels outside the polygon
  dbdet_edgemap_iter it = edgemap_->edge_cells.begin();
  for (; it!=edgemap_->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* edgel =(*it)[j];
      if (!cur_poly.contains(edgel->pt.x(), edgel->pt.y())){
        //delete all edgels in this cell
        for (unsigned k=0; k<(*it).size(); k++)
          delete (*it)[k];

        //clear the edgel list
        (*it).clear();
        break;
      }
    }
  }

  //we need to update the ids on the remaining edgels
  //these IDs are expected to be continuous and in sync
  //with the edgels in the edgel list
  edgemap_->edgels.clear();

  it = edgemap_->edge_cells.begin();
  for (; it!=edgemap_->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* edgel =(*it)[j];

      edgel->id = edgemap_->edgels.size();
      edgemap_->edgels.push_back(edgel);
    }
  }
}

