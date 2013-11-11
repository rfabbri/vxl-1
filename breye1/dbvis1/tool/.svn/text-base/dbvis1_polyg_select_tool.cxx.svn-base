#include <dbvis1/tool/dbvis1_polyg_select_tool.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_sptr.h>
#include <vidpro1/vidpro1_repository.h>

dbvis1_polyg_select_tool::dbvis1_polyg_select_tool()
: active_(true),last_x(0.0f), last_y(0.0f), sel_num_(0)
{
    gesture_add_vert_= vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
   gesture_close_polyg_ = vgui_event_condition (vgui_MIDDLE, vgui_MODIFIER_NULL, true);
   
}

dbvis1_polyg_select_tool::~dbvis1_polyg_select_tool()
{
}

bool
dbvis1_polyg_select_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbvis1_polyg_select_tool::set_storage ( const bpro1_storage_sptr& storage)
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
dbvis1_polyg_select_tool::tableau()
{
  return tableau_;
}

vidpro1_vsol2D_storage_sptr
dbvis1_polyg_select_tool::storage()
{
  vidpro1_vsol2D_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(storage_);
  return vsol_storage;
}

bool
dbvis1_polyg_select_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& selector
                                   )
{

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  if( e.type == vgui_MOTION ) 
    {
        last_x = ix;
        last_y = iy;
        if( active_ ) 
        {
            tableau_->post_overlay_redraw();
        }
        return true;
  }
  if (active_&gesture_close_polyg_(e))
  {
          active_=false;
          //adding last point
      drawnvert.push_back( new vsol_point_2d (last_x , last_y) );

      vidpro1_vsol2D_storage_sptr stor;
                        
      vcl_vector <vcl_string > namesake;
      vsol_polygon_2d_sptr drawn(new vsol_polygon_2d( drawnvert ) );
      vcl_vector<vsol_spatial_object_2d_sptr> select_pts;
      int count=0;
          
      stor.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableau_) );
      
      

     //Creating a tableeu for the drawn polygon
      
      bgui_vsol2D_tableau_sptr  vsol_tableau_ = (bgui_vsol2D_tableau*)(tableau_.ptr());
      vsol_tableau_->add_vsol_polygon_2d( drawn );


      vcl_vector< vsol_spatial_object_2d_sptr > pts_in_stor = stor->all_data();


      for (int k=0;k<pts_in_stor.size();k++)
      {
                  vsol_point_2d_sptr p= pts_in_stor[k]->cast_to_point() ;
          if ( drawn->is_inside(p) )
          {
                    select_pts.push_back(p->cast_to_spatial_object());
                    count++;
          }
      }

          
            
          vcl_cout<<"Total no of pts in polygon:"<<count<<"\n";
          vidpro1_vsol2D_storage_sptr selected_pts_stor = vidpro1_vsol2D_storage_new();
          selected_pts_stor->add_objects(select_pts,"Points inside region");
          char index[4];
          vcl_string string_index = (vcl_string)itoa(sel_num_,index,10);
          
          vcl_string point_group_name(((vcl_string)"Inside_Polyg")+string_index);
          selected_pts_stor->set_name(point_group_name);
          
          bvis1_manager::instance()->repository()->store_data(selected_pts_stor);
          bvis1_manager::instance()->add_to_display(selected_pts_stor);
          bvis1_manager::instance()->make_tableau(selected_pts_stor);
          bvis1_manager::instance()->display_current_frame();
          

      
          vgui_style_sptr polyg_style = vgui_style::new_style(1.0f, 0.5f, 0.0f, 1.0f, 1.0f);
          tableau_->add_vsol_polygon_2d( drawn, polyg_style);
          drawnvert.clear();
          tableau_->post_redraw();
          sel_num_++;
          return true;
  }


      if ( active_&&gesture_add_vert_(e) )
        { drawnvert.push_back( new vsol_point_2d (last_x , last_y) );
              active_ = true;
              return true;
           
        }
          if( e.type == vgui_OVERLAY_DRAW ) 
        {
            glLineWidth(1);
            glColor3f(1,1,1);
            glBegin(GL_LINE_STRIP);
            for (unsigned i=0; i<drawnvert.size(); ++i)
                glVertex2f(drawnvert[i]->x(), drawnvert[i]->y() );
            glVertex2f(last_x,last_y);



            glEnd();
            return true;


        }

  return false;

}

//: Return the name of this tool
vcl_string 
dbvis1_polyg_select_tool::name() const
{
  return "Polygonal Selection";
}

void dbvis1_polyg_select_tool::activate()
{
    active_ = true;
}
void dbvis1_polyg_select_tool::deactivate()
{
    drawnvert.clear();
    active_ = false;

}
