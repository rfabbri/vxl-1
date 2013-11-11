// This is breye1/dbvis1/tool/dbmrf_3d_picker_tool.cxx
//:
// \file

#include "dbmrf_3d_picker_tool.h"
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_svd.h>

#include <vpgl/vpgl_proj_camera.h>

#include <bgui3d/bgui3d_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoRayPickAction.h>


//: Constructor
dbmrf_3d_picker_tool::dbmrf_3d_picker_tool()
 : tableau_(NULL)
{
}
  

//: Destructor
dbmrf_3d_picker_tool::~dbmrf_3d_picker_tool()
{
}


//: Set the tableau to work with
bool 
dbmrf_3d_picker_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  tableau_ = dynamic_cast<bgui3d_tableau*>(tableau.ptr());
  if( tableau_ != NULL )
    return true;
  
  vgui_deck_tableau_sptr deck = dynamic_cast<vgui_deck_tableau*>(tableau.ptr());
  if( deck != NULL ){
    tableau_ = dynamic_cast<bgui3d_tableau*>(deck->current().ptr());
    if( tableau_ != NULL )
      return true;
  }

  return false;
}


//: Find the picked point in the world
SoPickedPoint* 
dbmrf_3d_picker_tool::pick(int x, int y) const
{
  if(!tableau_)
    return NULL;

  SoRayPickAction rp(tableau_->get_viewport_region());

  // Set up the ray in space
  if(bgui3d_project2d_tableau_sptr proj =
      dynamic_cast<bgui3d_project2d_tableau*>(tableau_.ptr())){

    vcl_auto_ptr<vpgl_proj_camera<double> > cam(proj->camera());
    if(!cam.get())
      return NULL;
    vnl_double_3x4 C = cam->get_matrix();

    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(x, y, ix, iy);
    vnl_svd<double> svd_M(C.extract(3,3));
    vnl_double_3 dir = svd_M.solve(vnl_double_3(ix,iy,1)).normalize();
    vnl_double_3 center = svd_M.solve(-C.get_column(3));
    
    rp.setRay( SbVec3f(center[0], center[1], center[2]), 
               SbVec3f(dir[0], dir[1], dir[2]) );
  }
  else
    rp.setPoint(SbVec2s(x,y));

  rp.apply(tableau_->scene_root());
  SoPickedPoint* pp = rp.getPickedPoint();
  if(pp)
    return new SoPickedPoint(*pp);

  return NULL;
}


