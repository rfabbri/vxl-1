#include <dbvis/tool/dbvis_classify_curve_tool.h>
#include <bvis/bvis_manager.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
dbvis_classify_curve_tool::dbvis_classify_curve_tool()
{
    gesture_prune_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
    gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
    gesture1 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
    current_id_=-1;
    curvetype="";

}

dbvis_classify_curve_tool::~dbvis_classify_curve_tool()
{
}

bool
dbvis_classify_curve_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbvis_classify_curve_tool::set_storage ( const bpro_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "vsol2D"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbvis_classify_curve_tool::tableau()
{
  return tableau_;
}

vidpro_vsol2D_storage_sptr
dbvis_classify_curve_tool::storage()
{
  vidpro_vsol2D_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(storage_);
  return vsol_storage;
}

bool
dbvis_classify_curve_tool::handle( const vgui_event & e, const bvis_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if( e.type == vgui_MOTION ) {
    last_x = ix;
    last_y = iy;

    return false;
  }
  // get the object id and type of the curve
  if(e.type == vgui_KEY_PRESS && e.key == 'b' && vgui_SHIFT)
  {
    vgui_dialog dlg("Object properties");
    dlg.field("Object number",current_id_);
    static bool structure=false;
    dlg.checkbox("STRUCTURE",structure);

    if(!dlg.ask())
        return false;

    if(structure)
        curvetype="STRUCTURE";
    else
        curvetype="NON-STRUCTURE";

    return false;
    
  }
  //: to record the seclected curves
  if (e.type == vgui_KEY_PRESS && e.key == 's' && vgui_SHIFT ) 
    {    
      vcl_vector<vgui_soview*> selected_objects;
      vcl_vector<vgui_soview*>::iterator k;
      selected_objects = tableau_->get_selected_soviews();    
      if(current_id_>0)    
  {     
    for (unsigned int i = 0; i<selected_objects.size(); i++) 
      {         
        if (((bgui_vsol_soview2D_digital_curve*)selected_objects[i])->type_name_() == "bgui_vsol_soview2D_digital_curve") 
    {
      
      selected_ids.push_back(selected_objects[i]->get_id());
      
      curves_selected_[current_id_][curvetype].push_back(((bgui_vsol_soview2D_digital_curve*)selected_objects[i])->sptr()->cast_to_spatial_object());         
      
    }
      }  
  }
      tableau_->deselect_all();
      tableau_->post_redraw();
      return false;
    }
  
  //: to split a  curve
  if (e.type == vgui_KEY_PRESS && e.key == 'p' && vgui_SHIFT ) {
    vcl_vector<vgui_soview*> selected_objects;
    vcl_vector<vgui_soview*>::iterator k;
    selected_objects = tableau_->get_selected_soviews();
    if(selected_objects.size()==1)
      {
  /*
      if (((bgui_vsol_soview2D_digital_curve*)selected_objects[0])->type_name_() == "bgui_vsol_soview2D_digital_curve") 
      {          
      vsol_digital_curve_2d_sptr c=((bgui_vsol_soview2D_digital_curve*)selected_objects[0])->sptr();
      vsol_digital_curve_2d_sptr c1,c2;
      vgl_point_2d<double> point((double)last_x,(double)last_y);
      //split(point,c,c1,c2);
      vcl_cout<<"\n"<<c1->size()<<" "<<c2->size();
      tableau_->remove(selected_objects[0]);
      
      
      storage_->add_object(c1->cast_to_spatial_object() , "splitted" );
      storage_->add_object(c2->cast_to_spatial_object() , "splitted" );
      
      
      } */
      }
    return false;
  }   
  
  //: to finish the recording process
  if (e.type == vgui_KEY_PRESS && e.key == 'f' && vgui_SHIFT )
    {
      storage_->clear_all();
      background_curves_=tableau_->get_all();
      
      vcl_vector<vsol_spatial_object_2d_sptr> bg;
      vcl_string backs="BACKGROUND";
      
      vcl_map<int, vcl_map<vcl_string, vcl_vector<vsol_spatial_object_2d_sptr> > >::iterator iter;
      vcl_map<vcl_string, vcl_vector<vsol_spatial_object_2d_sptr> >::iterator iter1;
      for(iter=curves_selected_.begin();iter!=curves_selected_.end();iter++)
  {
    vcl_ostringstream s;
    s<<"OBJECT"<<(*iter).first;
    for(iter1=(*iter).second.begin();iter1!=(*iter).second.end();iter1++)
      {
        s<<(*iter1).first;
        storage_->add_objects((*iter1).second, s.str());
        
      }     
  }
      
      for(unsigned int k=0;k<background_curves_.size();k++)
  {
    int flag=1;
    if (((bgui_vsol_soview2D_digital_curve*)background_curves_[k])->type_name_() == "bgui_vsol_soview2D_digital_curve") 
     {
       for(unsigned int l=0;l<selected_ids.size();l++)
         {
     if(background_curves_[k]->get_id()==(unsigned int)selected_ids[l])
       {
         flag=0;
       }
         }
     }
    if(flag>0)
     bg.push_back(((bgui_vsol_soview2D_digital_curve*)background_curves_[k])->sptr()->cast_to_spatial_object());
  }
      storage_->add_objects(bg, backs);
      return false;     
    }
  tableau_->post_redraw();
  return false;
}

//: Return the name of this tool
vcl_string dbvis_classify_curve_tool::name() const
{
  return "classify curve";
}
