#include <dbctrk/vis/dbctrk_get_info_tool.h>
#include <bvis1/bvis1_manager.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui.h> 
dbctrk_get_info_tool::dbctrk_get_info_tool()
{

    gesture_info_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
}

dbctrk_get_info_tool::~dbctrk_get_info_tool()
{
}

bool
dbctrk_get_info_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbctrk_get_info_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "dbctrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbctrk_get_info_tool::tableau()
{
  return tableau_;
}

dbctrk_storage_sptr
dbctrk_get_info_tool::storage()
{
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage_);
  return dbctrk_storage;
}

bool
dbctrk_get_info_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  if( e.type == vgui_MOTION){
  tableau_->motion(e.wx, e.wy);
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
 
      object_ = (dbctrk_soview2D*)curr_obj;
      curr_curve_ = NULL;
      if( curr_obj && curr_obj->type_name() == "dbctrk_soview2D"){   
        curr_curve_ = object_->dbctrk_sptr();
      }
      if(curr_curve_)
  vgui::out<<curr_curve_->get_id()<<"\n";
      bvis1_manager::instance()->post_overlay_redraw();
  }
  if ( gesture_info_(e) ) {

    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();

    for (unsigned int i = 0; i<all_objects.size(); i++) {
    if(((dbctrk_soview2D*)all_objects[i])->type_name()=="dbctrk_soview2D")
    {
      if(tableau_->is_selected(all_objects[i]->get_id()))
            {
            dbctrk_tracker_curve_sptr testcurve=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr();
      vcl_cout<<"\n the id is "<<testcurve->get_id()
          <<"\n the match id is "<<testcurve->match_id_;
      if(testcurve->get_best_match_next().ptr())
      {  
        vcl_cout<<"\n id of the best match is"<<testcurve->get_best_match_next()->match_curve_set[0]->get_id();
      }
      for(unsigned int i=0;i<testcurve->next_.size();i++)
        vcl_cout<<"\n"<<testcurve->next_[i]->match_curve_set[0]->frame_number;
      vcl_cout<<"\n the size of the prev vector is"<<testcurve->next_.size();
      vcl_cout<<"\n the number of neighbors in Dt graoh is "<<testcurve->neighbors_.size();
      }
    }  
   }
    tableau_->post_redraw();
    tableau_->deselect_all();
  }

  /*  if(e.type == vgui_KEY_PRESS && e.key == 'j' && vgui_SHIFT)
    {
      static int id=0;


      vgui_dialog inputfile("Input Id");
      inputfile.field("Id",id);
      if(!inputfile.ask())
  return true;
      
  }*/
  return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_get_info_tool::name() const

{

  return "Info Curve";

}


