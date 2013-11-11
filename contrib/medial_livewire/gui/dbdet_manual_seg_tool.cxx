// This is brcv/seg/dbdet/vis/dbdet_manual_seg_tool.cxx
//:
// \file

#include "dbdet_manual_seg_tool.h"
#include "dbdet_image_seg_tool.h"



#include <vidpro1/vidpro1_repository.h>
#include <bgui/bgui_image_tableau.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <bvis1/bvis1_manager.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_region_2d_sptr.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_region_scan_iterator.h>

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>
#include <vil/algo/vil_region_finder.h>
#include <vil/algo/vil_threshold.h>
#include <vgui/vgui_vil_image_renderer.h>
#include <vgui/vgui_projection_inspector.h>

#include <vil/vil_convert.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
// ------------------------------------------------------------------
//: Constructor - protected
dbdet_manual_seg_tool::
dbdet_manual_seg_tool() 
{
  gesture_paint = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_erase = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  vil_renderer_ = new vgui_vil_image_renderer;
  brush_size_=1;
  active_=0;
}

// ------------------------------------------------------------------
//: This is called when the tool is activated
void dbdet_manual_seg_tool::
activate() 
{
  this->init_tool();
  this->set_brush_mask();
  this->select_io_storage();

  original_view_ = vil_convert_cast(vxl_byte(), this->image_->get_view());
  input_view_ = vil_convert_cast(vxl_byte(), this->image_->get_view());


  this->merge_io_storage();

  seg_view_ = vil_copy_deep(original_view_);  
  image_sptr_ = vil_new_image_resource(seg_view_.ni(),seg_view_.nj(),seg_view_.nplanes(),VIL_PIXEL_FORMAT_BYTE);
  image_sptr_->put_view(seg_view_);  

  vil_renderer_->set_image_resource(image_sptr_ );
  vil_renderer_->reread_image();
  bvis1_manager::instance()->post_redraw();

  previous_sptr = vil_new_image_resource_of_view(seg_view_);
  image_list.push_back(previous_sptr);

  active_=1;

}

void dbdet_manual_seg_tool::
init_tool()
{
  i_list.clear();
  j_list.clear();
  polygon_list_.clear();
  polyline_list_.clear();
  ri.clear(); 
  rj.clear();
}


void dbdet_manual_seg_tool::
merge_io_storage()
{
  //get the repository
  vidpro1_repository_sptr repository_sptr = bvis1_manager::instance()->repository();

  // get input storage class
  input_vsol.vertical_cast(repository_sptr->get_data_by_name(vsol2d_name_));

  // parse through all the vsol classes and obtain curve objects only
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();


  //search for regions (polygons or polylines)
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    vsol_spatial_object_2d_sptr p = vsol_list[b];
    //for polyline we don't want to close the region
    if( p->cast_to_curve())
    {
      vsol_curve_2d_sptr curve = p->cast_to_curve();
      if(curve->cast_to_polyline() )
      {
        vsol_polyline_2d_sptr polyline = curve->cast_to_polyline();
        for (unsigned int i=0; i<polyline->size();i++)
        {
          vsol_point_2d_sptr pt = polyline->vertex(i);
          polyline_list_.push_back(pt);
        }
      }
      for (unsigned int i=1; i<polyline_list_.size(); i++)
      {  
        vsol_point_2d p = polyline_list_[i]->get_p(); 
        original_view_(p.x(),p.y())=255;
      }
      //scan polyline
      for(unsigned int i=1; i<image_->ni(); i++){
        for(unsigned int j=1; j<image_->nj(); j++){
          if (original_view_(i,j)==255)
            original_view_(i,j)=255;
          else
            original_view_(i,j)=input_view_(i,j);
        }
      }
    }
    //for polygons, we want to fill the polygon
    if( p->cast_to_region())
    {
      vsol_region_2d_sptr region = p->cast_to_region();
      if( region->cast_to_polygon() )
      {

        vsol_polygon_2d_sptr poly = region->cast_to_polygon();

        // turn into a vgl_polygon
        vcl_vector<vgl_point_2d<double > > pts;
        for (unsigned m=0; m<poly->size(); ++m)
        {
          pts.push_back(poly->vertex(m)->get_p());
        }
        // scan polygon
        vgl_polygon<double > polygon(pts);

        vgl_polygon_scan_iterator<double> psi(polygon);
        //psi.set_include_boundary(false); // optional flag, default is true
        for (psi.reset(); psi.next(); ) {
          int y = psi.scany();
          for (int x = psi.startx(); x <= psi.endx(); ++x){
            //check to get a region with a hole
            if (original_view_(x,y)==255)
              original_view_(x,y)=0;
            else
              original_view_(x,y)=255;
          }
        }
      }
    }
  }
}

bool dbdet_manual_seg_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
  if(!active_)
    return false;

  float ix, iy;

  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);


  //get binary
  if( e.type == vgui_KEY_PRESS && e.key == 'b' )
  {
    this->get_binary();
    this->refresh();
    bvis1_manager::instance()->post_redraw();
    return true;
  }

  //get contour
  if( e.type == vgui_KEY_PRESS && e.key == 't' )
  {
    this->get_binary();

    //invoke the tracer
    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(1);
    ctracer.set_curvature_smooth_nsteps(1);
    ctracer.set_curvature_smooth_beta(0.3f);
    ctracer.trace(binary_img_);

    //get the interesting contours
    vcl_vector< vsol_spatial_object_2d_sptr > contours;
    for (unsigned i=0; i<ctracer.contours().size(); i++)
    {
      if ((int)(ctracer.contours()[i].size()) >= 30)
      {
        vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (ctracer.contours()[i]);//ctracer.largest_contour()
        contours.push_back(newContour->cast_to_spatial_object());
      }
    }

    input_vsol->clear_all();
    input_vsol->add_objects(contours, "trace");
    bvis1_manager::instance()->display_current_frame();
    bvis1_manager::instance()->active_selector()->set_active(this->vsol2d_name());
    bvis1_manager::instance()->active_selector()->active_to_top();
    return true;
  }

  //undo  
  if( e.type == vgui_KEY_PRESS && e.key == 'u' )
  {
    if(image_list.size()>2)
    {
      image_list.pop_back();
      image_sptr_ = image_list.back();
      seg_view_ = image_sptr_->get_view();
    }
    else
      vcl_cerr<<"Cannot Undo"<<vcl_endl;

    vil_renderer_->set_image_resource(image_sptr_ );
    vil_renderer_->reread_image();
    bvis1_manager::instance()->post_redraw();

    return true;

  }

  if ( e.button==vgui_LEFT )
  {
    last_x=ix;
    last_y=iy;


    if (( e.type==vgui_BUTTON_DOWN)||( e.type == vgui_MOTION))
    {
      for (unsigned int i=0; i<i_list.size(); i++)
      {
        if (((last_x+i_list[i])>0&&((last_x+i_list[i])<original_view_.ni()))&&
          ((last_y+j_list[i])>0&&((last_y+j_list[i])<original_view_.nj())))
        {
          if (e.modifier == vgui_SHIFT)           
            seg_view_(last_x+i_list[i], last_y+j_list[i])=original_view_(last_x+i_list[i], last_y+j_list[i]);
          else
            seg_view_(last_x+i_list[i], last_y+j_list[i])=255;
        }
      }
      this->refresh();
      bvis1_manager::instance()->post_redraw();
      return true;
    }

    if ( e.type==vgui_BUTTON_UP){ 

      //save image for undo funct.
      //first check size of list, only want to keep record of 5 undoes

      previous_sptr = vil_new_image_resource_of_view(vil_copy_deep(seg_view_));
      if (image_list.size()<6)
      {
        image_list.push_back(previous_sptr);
      }else
      {
        image_list.pop_front();
        image_list.push_back(previous_sptr);
      }
      return true;
    }
  }
  else  if ( e.type == vgui_MOTION)
  {
    last_x=ix;
    last_y=iy;
    bvis1_manager::instance()->post_overlay_redraw();

  }else if (( e.button==vgui_MIDDLE )&& (e.modifier == vgui_SHIFT))
  {
    int rx=ix;
    int ry=iy;

    this->get_binary();


    //create an instance of region finder and get same region as rx, ry
    vil_region_finder<vxl_byte> finder(seg_view_, vil_region_finder_4_conn);
    finder.same_int_region(rx, ry, ri, rj);

    for (int i=1; i<ri.size(); i++)
    {   
      seg_view_(ri[i],rj[i])=255;
    }
    //fill region
    for(unsigned int i=1; i<seg_view_.ni(); i++){
      for(unsigned int j=1; j<seg_view_.nj(); j++){
        if (seg_view_(i,j)==255)
          seg_view_(i,j)=255;
        else
          seg_view_(i,j)=original_view_(i,j);
      }
    }
    //fill current mouse location.
    seg_view_(rx,ry)=255;
    this->refresh();
    bvis1_manager::instance()->post_redraw();

  }


  if( e.type == vgui_OVERLAY_DRAW ) 
  {
    glColor3f(0,0,0);
    glBegin(GL_POINTS);
    for (unsigned int i=0; i<i_list.size(); i++)
    {
      glVertex2f(last_x+i_list[i], last_y+j_list[i]);
    }
    glEnd();
  }

  if( e.type == vgui_DRAW ) 
  {
    //Turn off blending before calling the renderer for better performanece.
    GLboolean blend_on;
    glGetBooleanv(GL_BLEND, &blend_on);
    if (blend_on)
      glDisable(GL_BLEND);

    if ( vil_renderer_ ) vil_renderer_->render(0); //rmp_tableau_->map_params()

    if (blend_on)
      glEnable(GL_BLEND);
    return true;
  }
  return false;
}


void 
dbdet_manual_seg_tool::set_brush_mask( )
{

  i_list.clear();
  j_list.clear();
  for( signed int i=-(brush_size_); i<=brush_size_; i++){
    for( signed int j=-(brush_size_); j<=brush_size_; j++){
      if (sqrt (double(i*i + j*j)) <(brush_size_+ 0.5)){
        i_list.push_back( i ); 
        j_list.push_back( j ) ;
      }
    }
  }
}

void 
dbdet_manual_seg_tool::get_binary( )
{
  for(unsigned int i=1; i<seg_view_.ni(); i++){
    for(unsigned int j=1; j<seg_view_.nj(); j++){
      if (seg_view_(i,j)==255)
        seg_view_(i,j)=255;
      else
        seg_view_(i,j)=0;
    }
  }

  image_sptr_->put_view(seg_view_);

  // no conversion necessary when input is binary image
  if (image_sptr_->pixel_format()==binary_img_.pixel_format() && 
    image_sptr_->nplanes() == 1)
  {
    binary_img_ = image_sptr_->get_view();
  }
  else
  {
    // threshold the image to make sure that it is binary
    vxl_byte min_value = 0, max_value = 0;
    vil_math_value_range(seg_view_, min_value, max_value);
    vil_threshold_above<vxl_byte >(seg_view_, binary_img_, (min_value+max_value+1)/2);
  }

}

void 
dbdet_manual_seg_tool::refresh( )
{
  image_sptr_->put_view(seg_view_);
  vil_renderer_-> reread_image();
}

class seg_tool_set_brush_size__command : public vgui_command
{
public:
  seg_tool_set_brush_size__command(dbdet_manual_seg_tool* tool) : 
      tool_(tool){}

      void execute() 
      { 
        int param_val = tool_->get_brush_size();
        vgui_dialog param_dlg("Set brush size");
        param_dlg.field("brush_size_", param_val);
        if(!param_dlg.ask())
          return;

        tool_->set_brush_size(param_val);
        tool_->set_brush_mask();

      }

      dbdet_manual_seg_tool *tool_;
};


void 
dbdet_manual_seg_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( "Set brush size", new seg_tool_set_brush_size__command(this) );

}
