#include "dbrl_id_point_2d_crop_tool.h"
#include <vgui/vgui_style.h>
//#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_polygon_test.h>
#include <bvis1/bvis1_manager.h>

/*************************************************************************
 * Function Name: dbrl_id_point_2d_crop_tool::dbrl_id_point_2d_crop_tool
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbrl_id_point_2d_crop_tool::dbrl_id_point_2d_crop_tool()
{
  draw_polygon_=false;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  gesture2 = vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true);
  point_list.clear();

}

/*************************************************************************
 * Function Name: dbrl_id_point_2d_crop_tool::~dbrl_id_point_2d_crop_tool
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbrl_id_point_2d_crop_tool::~dbrl_id_point_2d_crop_tool()
{
  point_list.clear();
}

/*************************************************************************
 * Function Name: dbrl_id_point_2d_crop_tool::set_tableau
 * Parameters:  vgui_tableau_sptr tableau 
 * Returns: void
 * Effects: 
 *************************************************************************/
bool
dbrl_id_point_2d_crop_tool::set_tableau( const vgui_tableau_sptr& tableau )
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

/*************************************************************************
 * Function Name: dbrl_id_point_2d_crop_tool::set_storage
 * Parameters:  bpro1_storage_sptr storage 
 * Returns: void
 * Effects: 
 *************************************************************************/
bool
dbrl_id_point_2d_crop_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "dbrl_id_point_2d"){
    storage_ = storage;
    return true;
  }
  return false;
}

/*************************************************************************
 * Function Name: dbrl_id_point_2d_crop_tool::tableau
 * Parameters:
 * Returns: bui_vsol2D_tableau_sptr
 * Effects:
 *************************************************************************/
bgui_vsol2D_tableau_sptr
dbrl_id_point_2d_crop_tool::tableau()
{
  return tableau_;
}

dbrl_id_point_2d_storage_sptr
dbrl_id_point_2d_crop_tool::storage()
{
  dbrl_id_point_2d_storage_sptr id_point_2d;
  id_point_2d.vertical_cast(storage_);
  return id_point_2d;
}




bool
dbrl_id_point_2d_crop_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);


  if( e.type == vgui_KEY_PRESS && e.key == 'p' && vgui_SHIFT  ) {
        
      if(point_list.size()>0)
          {
            double *pointx=new double[point_list.size()];
            double *pointy=new double[point_list.size()];
            for(int i=0;i<point_list.size();i++)
                {
                    pointx[i]=point_list[i]->x();
                    pointy[i]=point_list[i]->y();
                }
            dbrl_id_point_2d_storage_sptr id_point_2d;
            id_point_2d.vertical_cast(storage_);
            vcl_vector<dbrl_id_point_2d_sptr> idpoints;
            vcl_vector<dbrl_id_point_2d_sptr> idpointscropped;
            idpoints=id_point_2d->points();
            for(int i=0;i<idpoints.size();i++)
                {
                    double x=idpoints[i]->x();
                    double y=idpoints[i]->y();
                    if(vgl_polygon_test_inside<double>(pointx,pointy,point_list.size(),x,y))
                        idpointscropped.push_back(idpoints[i]);
                }
            id_point_2d->set_id_points(idpointscropped);
            point_list.clear();
            tableau()->post_redraw();
          }
      }
  if( e.type == vgui_KEY_PRESS && e.key == 'r' && vgui_SHIFT  ) {
        
      
            point_list.clear();
            tableau()->post_redraw();
      }
  if( e.type == vgui_MOTION ) {
      last_x = ix;
      last_y = iy;
      tableau()->post_overlay_redraw();
  }
  if(  gesture0(e) && !draw_polygon_ ) {
    draw_polygon_=true;
    point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ix , iy ) ) );
    return true;
  } 
  else if(gesture0(e) && draw_polygon_){
    point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ix , iy ) ) );
    return true;
  } 

    if( e.type == vgui_OVERLAY_DRAW ) {
        if(draw_polygon_)
            {
      glLineWidth(1);
      glColor3f(0,0,0);
      glBegin(GL_LINE_LOOP);
      for (unsigned i=0; i<point_list.size(); ++i)
        glVertex2f(point_list[i]->x(), point_list[i]->y() );
      glVertex2f(last_x,last_y);
      glEnd();
      }
      return true;
            
    }
    if( gesture1(e)||gesture2(e) && draw_polygon_) {
      if(gesture1(e))
         point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ix , iy ) ) );

      if (point_list.size() < 3)
         return true;

      vsol_polygon_2d_sptr newpgon =  new vsol_polygon_2d( point_list );
      tableau()->add_vsol_polygon_2d( newpgon );
      tableau()->set_current_grouping( "default" );

      tableau()->post_redraw();
      draw_polygon_=false;
      return true;
    }
  
  return false;
}


void 
dbrl_id_point_2d_crop_tool::get_popup( const vgui_popup_params& params, 
                                            vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((draw_polygon_)?on:off)+"Draw Polygon ", 
            bvis1_tool_toggle, (void*)(&draw_polygon_) );
  
}

vcl_string
dbrl_id_point_2d_crop_tool::name() const
{
  return "Crop Id Points";
}



