#include "dbdet_crop_curvelet_map_tool.h"

#include <vcl_limits.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>

#include <vgl/vgl_polygon.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <dbdet/vis/dbdet_sel_tableau.h>

dbdet_crop_curvelet_map_tool::dbdet_crop_curvelet_map_tool() :  
  sel_tab_(0), sel_storage_(0),
  left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  end_button(vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true)),
  active(false),
  point_list(0),
  last_x(0.0), last_y(0.0)
{
}

vcl_string dbdet_crop_curvelet_map_tool::name() const
{
  return "Crop Curvelet Map";
}

//: Set the tableau associated with the current view
bool dbdet_crop_curvelet_map_tool::set_tableau( const vgui_tableau_sptr& tableau )
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

    sel_storage_ = sel_storage;
    sel_tab_ = temp_tab;

    return true;
  }
  return false;
}

bool dbdet_crop_curvelet_map_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if( e.type == vgui_MOTION ) {
    last_x = ix;
    last_y = iy;
    if( active ) {
        sel_tab_->post_overlay_redraw();
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
      crop_curvelet_map();

      sel_tab_->post_redraw();
      active = false;
      point_list.clear();

      return true;
    }
  }
    
  return false;
}

void dbdet_crop_curvelet_map_tool::crop_curvelet_map()
{
  // 0) Delete all structures in the sel storage class that is dependent on the edgemap
  sel_storage_->CFG().clear();
  sel_storage_->ELG().clear();
  sel_storage_->EULM().clear();

  // 1) first mark the edgels for deletion
  vcl_vector<bool> del_flag(sel_storage_->EM()->edgels.size(), false);

  //go over all the edgels in the edgemap and test if they are inside/outside the polygon
  vgl_polygon<float> cur_poly(point_list);
  for (unsigned i=0; i<sel_storage_->EM()->edgels.size(); i++){
    dbdet_edgel* edgel = sel_storage_->EM()->edgels[i];
    if (!cur_poly.contains(edgel->pt.x(), edgel->pt.y()))
      del_flag[i] = true;
  }

  // 2) delete all the curvelets containing edgels that are marked to be deleted
  vcl_vector<dbdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<sel_storage_->EM()->edgels.size(); i++){
    if (del_flag[i]) //this edgel is marked for deletion so delete all the curvelets anchored on it
      sel_storage_->CM().delete_all_curvelets(sel_storage_->EM()->edgels[i]);
    else {
      //go over all the curvelets anchored at this edgel and 
      //determine if it contains any of the edgels marked for deletion
      cvlet_list& cvlist = sel_storage_->CM().curvelets(i);
      
      cvlet_list_iter cvit = cvlist.begin();
      for (; cvit != cvlist.end(); cvit++){
        for (unsigned j=0; j<(*cvit)->edgel_chain.size(); j++)
          if (del_flag[(*cvit)->edgel_chain[j]->id]){
            cvlets_to_del.push_back(*cvit);
            break;
          }
      }
    }
  }
  //now delete the ones that were marked
  for (unsigned i=0; i<cvlets_to_del.size(); i++)
    sel_storage_->CM().remove_curvelet(cvlets_to_del[i]);

  // 3) finally delete the edgels :: go over each cell and delete the marked edgels
  vcl_vector<dbdet_edgel*> edgels_to_keep;
  for (unsigned i=0; i<del_flag.size(); i++){
    if (!del_flag[i])
      edgels_to_keep.push_back(sel_storage_->EM()->edgels[i]);
  }

  sel_storage_->EM()->edgels.clear();
  dbdet_edgemap_iter it = sel_storage_->EM()->edge_cells.begin();
  for (; it!=sel_storage_->EM()->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++)
      (*it).clear(); //clear the edgel list
  }

  // 4) relabel the remaining edgels as well as the remaining curvelets 

  //make a copy of the cvlet map
  vcl_vector<cvlet_list> CM_map = sel_storage_->CM().map_;

  //clear the CM map
  sel_storage_->CM().map_.clear();
  sel_storage_->CM().map_.resize(edgels_to_keep.size());

  for (unsigned i=0; i<edgels_to_keep.size(); i++){
    //first copy the curvelets
    sel_storage_->CM().map_[i] = CM_map[edgels_to_keep[i]->id];

    //then relabel and reinsert the edgel
    edgels_to_keep[i]->id = i;
    sel_storage_->EM()->insert(edgels_to_keep[i]);
  }

}

