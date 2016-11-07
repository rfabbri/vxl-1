// This is brcv/rec/dbkpr/vis/dbkpr_keypoint_tools.cxx
//:
// \file

#include "dbkpr_keypoint_tools.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <vcl_algorithm.h>



//: Constructor - protected
dbkpr_keypoint_tool::dbkpr_keypoint_tool()
 :  tableau_(NULL), storage_(NULL)
{
}


//: Set the tableau to work with
bool
dbkpr_keypoint_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
    return false;

  if( tableau.ptr() != NULL /*&& tableau->type_name() == "vgui_easy2D_tableau"*/ ){
    tableau_.vertical_cast(tableau);
    return true;
  }

  tableau_ =  vgui_easy2D_tableau_sptr(NULL);
  return false;
}


//: Set the storage class for the active tableau
bool
dbkpr_keypoint_tool::set_storage ( const bpro1_storage_sptr& storage )
{
  if (!storage.ptr())
    return false;
  //make sure its a bmrf storage class
  if (storage->type() == "keypoints"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

//----------------------dbkpr_keypoint_inspector_tool---------------------------


//: Constructor
dbkpr_keypoint_inspector_tool::dbkpr_keypoint_inspector_tool()
 : object_(NULL), curr_keypt_(NULL), match_index_(-1)
{
}


//: Destructor
dbkpr_keypoint_inspector_tool::~dbkpr_keypoint_inspector_tool()
{
}


//: Return the name of this tool
vcl_string
dbkpr_keypoint_inspector_tool::name() const
{
  return "Keypoint Inspector"; 
}



//: Handle events
bool
dbkpr_keypoint_inspector_tool::handle( const vgui_event & e, 
                                       const bvis1_view_tableau_sptr& view )
{
  if( !tableau_.ptr() )
    return false;

  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

  if ( e.type == vgui_DRAW )
    return false;

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY){
    if(is_mouse_over){
      if(curr_keypt_){
        dbdet_keypoint_soview2D(curr_keypt_,true).draw();
      }
    }
    // draw the matching keypoint (with same index)
    else if(match_index_ >= 0){
      bpro1_storage_sptr stg = bvis1_manager::instance()->
                                storage_from_tableau(view->selector()->active_tableau());
      if (stg->type() == "keypoints"){
        dbdet_keypoint_storage_sptr kstg;
        kstg.vertical_cast(stg);
        if(match_index_ < (int)kstg->keypoints().size()){
          dbdet_keypoint_sptr match = kstg->keypoints()[match_index_];
          if(match)
            dbdet_keypoint_soview2D(match,true).draw();
        }
      }
    }
    return !is_mouse_over;
  }

  
  

  if( e.type == vgui_MOUSE_DOWN && e.button == vgui_LEFT ){
    
  }

  if( e.type == vgui_MOTION){
    tableau_->motion(e.wx, e.wy);
    
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
    if( curr_obj != object_ ){
 
      object_ = (dbdet_keypoint_soview2D*)curr_obj;
      curr_keypt_ = NULL;
      if( curr_obj && curr_obj->type_name() == "dbdet_keypoint_soview2D"){   
        curr_keypt_ = object_->sptr;
        const vcl_vector< dbdet_keypoint_sptr > & all_keypts = storage_->keypoints();
        vcl_vector< dbdet_keypoint_sptr >::const_iterator itr =
          vcl_find(all_keypts.begin(),all_keypts.end(), curr_keypt_);
        if(itr != all_keypts.end())
          match_index_ = int(itr - all_keypts.begin());
        else
          match_index_ = -1;
      }
      
      bvis1_manager::instance()->post_overlay_redraw();
    }
    
    return true;
  }
  return false;
}




//: Add popup menu items
void 
dbkpr_keypoint_inspector_tool::get_popup( const vgui_popup_params& params, 
                                          vgui_menu &menu )
{
  //vcl_string on = "[x] ", off = "[ ] ";
  //menu.add( ((draw_epipolar_line_)?on:off)+"Epipolar Line", 
  //          bvis1_tool_toggle, (void*)(&draw_epipolar_line_) );
}


