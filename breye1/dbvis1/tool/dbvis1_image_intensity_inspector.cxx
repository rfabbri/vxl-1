#include <dbvis1/tool/dbvis1_image_intensity_inspector.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <bvis1/bvis1_manager.h>
#include <vil/vil_rgb.h>
#include <bseg/brip/brip_vil_float_ops.h>
dbvis1_image_intensity_inspector::dbvis1_image_intensity_inspector()
{
  rgb_=true;
  ihs_=false;
}

bool dbvis1_image_intensity_inspector::handle(const vgui_event & e,const bvis1_view_tableau_sptr& selector )
{
  if(e.type==vgui_MOTION)
    {
      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
      vil_image_view<vxl_byte> image=tableau_->get_image_resource()->get_view();
      ix=vcl_floor(ix+0.5);
      iy=vcl_floor(iy+0.5);
      vgui::out<<"("<<(int)ix<<","<<(int)iy<<")";
      
      if((int)ix>=0 && (int)ix<image.ni() && (int)iy>=0 && (int)iy<image.nj())
  {
    if(rgb_)
      {
        if( image.nplanes() == 3 ) {
    vgui::out<<"("<<(int)image((int)ix,(int)iy,0)<<","
       <<(int)image((int)ix,(int)iy,1)<<","
         <<(int)image((int)ix,(int)iy,2)<<")";
        }
        else if ( image.nplanes() == 1 ) {
    vgui::out<<"("<<(int)image((int)ix,(int)iy)<<","
       <<(int)image((int)ix,(int)iy)<<","
       <<(int)image((int)ix,(int)iy)<<")";
        }
      }
    if(ihs_)
      {
        if( image.nplanes() == 3 ) {
    vil_rgb<vxl_byte> rgbpixel(image((int)ix,(int)iy,0),image((int)ix,(int)iy,1),image((int)ix,(int)iy,2));
    float i,h,s;
    brip_vil_float_ops::rgb_to_ihs(rgbpixel,i,h,s);
    vgui::out<<"("<<i<<","
       <<h*360/255<<","
       <<s/255<<")";
        }
        else if ( image.nplanes() == 1 ) {
    vgui::out<<"("<<(int)image((int)ix,(int)iy)<<","
       <<(int)image((int)ix,(int)iy)<<","
       <<(int)image((int)ix,(int)iy)<<")";
        }
      }
    
  }
      vgui::out<<"\n";
      return true;
    }
  return false;
}  
//: Set the tableau to work with
bool
dbvis1_image_intensity_inspector::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
    return false;

  if( tableau.ptr() != NULL && tableau->type_name() == "vgui_image_tableau" ){

    tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = NULL;
  return false;
}
bool
dbvis1_image_intensity_inspector::set_storage ( const bpro1_storage_sptr& storage )
{
  if (!storage.ptr())
    return false;
  //make sure its a image storage class
  if (storage->type() == "image"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}
void
dbvis1_image_intensity_inspector::activate()
{

}

void
dbvis1_image_intensity_inspector::deactivate()
{

}
void 
dbvis1_image_intensity_inspector::get_popup( const vgui_popup_params& params, 
                                            vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((rgb_)?on:off)+"RGB ", 
            bvis1_tool_toggle, (void*)(&rgb_) );
  menu.add( ((ihs_)?on:off)+"IHS", 
            bvis1_tool_toggle, (void*)(&ihs_) );

}
