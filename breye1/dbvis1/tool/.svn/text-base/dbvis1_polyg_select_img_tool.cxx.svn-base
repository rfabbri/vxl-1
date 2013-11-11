#include <dbvis1/tool/dbvis1_polyg_select_img_tool.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

dbvis1_polyg_select_img_tool::dbvis1_polyg_select_img_tool()
: active_(true),last_x(0.0f), last_y(0.0f), sel_num_(0)
{
    gesture_add_vert_= vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
   gesture_close_polyg_ = vgui_event_condition (vgui_MIDDLE, vgui_MODIFIER_NULL, true);
   form_mask = true;
   cleaning_curves = true;
   
}

dbvis1_polyg_select_img_tool::~dbvis1_polyg_select_img_tool()
{
}

bool
dbvis1_polyg_select_img_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
 
  vgui_image_tableau_sptr temp_img_tab;
 
  temp_img_tab.vertical_cast(tableau);
  if (!temp_img_tab.ptr())
    
      return false;
  
 
      if (temp_img_tab.ptr())  
      {
          if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
              tableau_ = temp_img_tab;
              return true;
              }
      }
  return false;
}

bool
dbvis1_polyg_select_img_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class

      if (storage->type() == "image"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

vgui_image_tableau_sptr
dbvis1_polyg_select_img_tool::tableau()
{
  return tableau_;
}

vidpro1_image_storage_sptr
dbvis1_polyg_select_img_tool::storage()
{
  vidpro1_image_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(storage_);
  return vsol_storage;
}

bool
dbvis1_polyg_select_img_tool::handle( const vgui_event & e, 
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

      vidpro1_image_storage_sptr stor;
                        
      vcl_vector <vcl_string > namesake;
      vsol_polygon_2d_sptr drawn(new vsol_polygon_2d( drawnvert ) );
      vcl_vector<vsol_spatial_object_2d_sptr> select_img_pts;
      int count=0;
          
      stor.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableau_) );
      vil_image_resource_sptr image_rsrc = stor->get_image();
      
      vil_image_view<float> origedgemap = image_rsrc->get_view();
     // assert(stor->get_image_view()->pixel_format()==float)
      vcl_cout<<origedgemap.ni()<<"   "<<origedgemap.nj()<<"\n";
     


     //Creating a mask for background and setting the edgemap pixels in bg to 0
      
      
  
      vil_image_view<vxl_byte> boundary_mask(origedgemap.ni(),origedgemap.nj() );
      boundary_mask.fill(vxl_byte(255));


      for (int j=0; j<origedgemap.ni();j++)
          for (int k=0; k<origedgemap.nj();k++)
              {
                  vsol_point_2d_sptr p= new vsol_point_2d(j,k) ;
                  if (! drawn->is_inside(p) )
                  {
                      if (cleaning_curves)
                      origedgemap(j,k) = 0.0f;  
                      if (form_mask)
                      boundary_mask(j,k) = vxl_byte(0);
                  }
              }
#if 0
              for (int i = 0;i<origedgemap.ni(); i++)
                  for (int j = 0;j<origedgemap.nj(); j++)
                  {
                   //   if (origedgemap(i,j))
                  //    vcl_cout<<origedgemap(i,j);
                  }
#endif
      // image_tab = vgui_image_tableau_new(origedgemap);   
            
      
          vidpro1_image_storage_sptr new_img_stor = vidpro1_image_storage_new();
          vidpro1_image_storage_sptr new_mask_stor = vidpro1_image_storage_new();
          vil_image_resource_sptr newimg = vil_new_image_resource_of_view(origedgemap);
          
          vil_image_resource_sptr boundary_mask_img_rsrc =vil_new_image_resource_of_view(boundary_mask);
          
          char index[4];
          vcl_string string_index = (vcl_string)itoa(sel_num_,index,10);

          vcl_string point_group_name(((vcl_string)"ROI_image")+string_index);
          vcl_string mask_name(((vcl_string)"ROI_mask")+string_index);
         
          new_img_stor->set_image(newimg);
          new_img_stor->set_name(point_group_name);

          new_mask_stor->set_image(boundary_mask_img_rsrc);
          new_mask_stor->set_name(mask_name); 
          
         
          if (cleaning_curves)
          {

          bvis1_manager::instance()->repository()->store_data(new_img_stor);
          bvis1_manager::instance()->add_to_display(new_img_stor);
          bvis1_manager::instance()->make_tableau(new_img_stor);
          }


          if (form_mask)
          {
          bvis1_manager::instance()->repository()->store_data(new_mask_stor);
             bvis1_manager::instance()->add_to_display(new_mask_stor);
          bvis1_manager::instance()->make_tableau(new_mask_stor);
          }
       

          bvis1_manager::instance()->display_current_frame();
       
          


      
       //   vgui_style_sptr polyg_style = vgui_style::new_style(1.0f, 0.5f, 0.0f, 1.0f, 1.0f);
      //    tableau_->add_vsol_polygon_2d( drawn, polyg_style);
          drawnvert.clear();
       //   tableau_->post_redraw();
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
dbvis1_polyg_select_img_tool::name() const
{
  return "Image ROI Selection";
}

void dbvis1_polyg_select_img_tool::activate()
{
    active_ = true;
}
void dbvis1_polyg_select_img_tool::deactivate()
{
    drawnvert.clear();
    active_ = false;

}
void 
dbvis1_polyg_select_img_tool::get_popup( const vgui_popup_params& params, 
                                                                         vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((form_mask)?on:off)+"Form Mask", 
                        bvis1_tool_toggle, (void*)(&form_mask) );
   menu.add( ((cleaning_curves)?on:off)+"Prune Curves", 
                        bvis1_tool_toggle, (void*)(&cleaning_curves) );
 // menu.add( ((end_sel_flag)?on:off)+"Selection of Pts have ended", 
 //               bvis1_tool_toggle, (void*)(&end_sel_flag) );



   
//      menu.add( "Prune by Gamma", new bvis1_prune_command(this, bvis1_prune_command::GAMMA));
//      menu.add( "Prune by Directed", new bvis1_prune_command(this, bvis1_prune_command::DIRECT));
}
