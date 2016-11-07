#include <vis/dbrl_select_id_point_process_tool.h>
#include <vis/dbrl_match_set_tableau.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <vcl_algorithm.h>
#include <vidpro1/vidpro1_repository.h>

#include <georegister/dbrl_rpm_tps.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_translation.h>

#include <georegister/dbrl_feature_point.h>

#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vgl/vgl_polygon_test.h>
dbrl_select_id_point_process_tool::dbrl_select_id_point_process_tool()
    {
    }

dbrl_select_id_point_process_tool::~dbrl_select_id_point_process_tool()
    {
    }

bool
dbrl_select_id_point_process_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbrl_select_id_point_process_tool::set_storage ( const bpro1_storage_sptr& storage)
    {
    if (!storage.ptr())
        return false;
    //make sure its a vsol storage class
    if (storage->type() == "dbrl_id_point_2d"){
        storage_.vertical_cast(storage);
        return true;
        }
return false;
    }

bgui_vsol2D_tableau_sptr
dbrl_select_id_point_process_tool::tableau()
    {
    return tableau_;
    }

dbrl_id_point_2d_storage_sptr
dbrl_select_id_point_process_tool::storage()
    {
    dbrl_id_point_2d_storage_sptr match_storage;
    match_storage.vertical_cast(storage_);
    return match_storage;
    }

bool
dbrl_select_id_point_process_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
    {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    int frame = view->frame();

    if ( e.type == vgui_KEY_PRESS && e.key == 'i' && vgui_SHIFT)
        {
          dbrl_id_point_2d_storage_sptr pcurr;
          pcurr.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbrl_id_point_2d",frame));   

          vidpro1_vsol2D_storage_sptr vsolpcurr;
          vsolpcurr.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("vsol2D",frame));    

          vcl_vector<vsol_spatial_object_2d_sptr> p=vsolpcurr->all_data();
          vcl_vector<dbrl_id_point_2d_sptr> pt=pcurr->points();

          vcl_vector<dbrl_id_point_2d_sptr> sel_pts;

          if(p.size()==1)
              if(vsol_polygon_2d_sptr poly=p[0]->cast_to_region()->cast_to_polygon())
                  { 
                  double * xs=new double[poly->size()];
                  double * ys=new double[poly->size()];
                  for(unsigned i=0;i<poly->size();i++)
                      {
                         xs[i]=poly->vertex(i)->x();
                         ys[i]=poly->vertex(i)->y();
                      }
                  for(unsigned i=0;i<pt.size();i++)
                      {
                        double x=pt[i]->x();
                        double y=pt[i]->y();
                        if(vgl_polygon_test_inside<double>(xs,ys,poly->size(),x,y))
                            {
                                sel_pts.push_back(pt[i]);
                            }
                      }
                  dbrl_id_point_2d_storage_sptr pcurrnew=dbrl_id_point_2d_storage_new();
                  pcurrnew->set_id_points(sel_pts);
                  bvis1_manager::instance()->repository()->new_data_at("dbrl_id_point_2d","selected points",frame);
                  bvis1_manager::instance()->repository()->store_data_at(pcurrnew,frame);
                  tableau()->post_redraw();

          }
        
        }
   
 

return false;
    }



//: Return the name of this tool
vcl_string 
dbrl_select_id_point_process_tool::name() const
    {

    return "Select Id points";

    }

