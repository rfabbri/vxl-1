#include "aerial_vehicle_observation.h"
#include <vil/vil_image_view.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vcl_vector.h>
#include <dbinfo/dbinfo_observation.h>

#include <vil/vil_new.h>
#include <dbru/dbru_label.h>
#include <dbru/dbru_label_sptr.h>

unsigned int getobservation_i(unsigned char * input_img, int ni, int nj,int i,unsigned int polyhandle,int frame)
{

    if(vcl_vector<vcl_vector<vsol_point_2d_sptr > > * polys=
       reinterpret_cast<vcl_vector<vcl_vector<vsol_point_2d_sptr > > *> (polyhandle))
        {
        if(i<static_cast<int>(polys->size()))
         {
     vsol_polygon_2d_sptr p= new vsol_polygon_2d((*polys)[i]);
     vil_image_view<unsigned char> imgd(input_img,ni,nj,1,1,ni,ni*nj);
     
     vil_image_resource_sptr input_img_sptr=vil_new_image_resource_of_view(imgd);
     
     
     dbinfo_observation * obs= new dbinfo_observation(frame,input_img_sptr,p, true, true, false);
     
     unsigned int dbinfo_observation_handle=reinterpret_cast<unsigned int>(obs);
     
     return dbinfo_observation_handle;
   }
  return 0;
        }

    return 0;

    }


int getlabel_i(char * name, 
         int motion_orientation_bin,
         int view_angle_bin,
         int shadow_angle_bin,
         int shadow_length)
{

  vcl_string namestr(name);
  dbru_label_sptr label= new dbru_label(namestr,motion_orientation_bin,view_angle_bin,shadow_angle_bin,shadow_length);
  
  return reinterpret_cast<int>(label.ptr());
}
