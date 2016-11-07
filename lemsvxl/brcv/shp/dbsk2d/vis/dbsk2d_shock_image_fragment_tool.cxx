// This is brcv/dbsk2d/vis/dbsk2d_shock_image_fragment_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_bilin_interp.h>
#include <dbsk2d/dbsk2d_shock_fragment.h>
#include <vgl/vgl_point_2d.h>

#include "dbsk2d_shock_image_fragment_tool.h"

dbsk2d_shock_image_fragment_tool::dbsk2d_shock_image_fragment_tool()

{
  left_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  middle_click = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
}

dbsk2d_shock_image_fragment_tool::~dbsk2d_shock_image_fragment_tool()
{
}

//: compile an image fragment from the visual fragment
//temporary function: should go to visual fragment?
void 
dbsk2d_shock_image_fragment_tool::form_image_fragment(vil_image_resource_sptr imgr,
                                                      dbsk2d_shock_fragment* fragment, 
                                                      vil_image_view< unsigned char >& frag_img)
{
  vil_image_view< unsigned char > img = imgr->get_view(0, imgr->ni(), 0, imgr->nj());
  //if there's no image associated with the geometry this method won't work
  if (!img)
    return;

  //Note: currently this method only works for xshock edge fragments
  
  int width = (int)fragment->shock_edge()->psi_max() + 1;
  int max_r = (int)vcl_floor(fragment->shock_edge()->r(width-1));
  int height = 2*max_r;

  //resize the image to the dimension of its intrinsic domain
  frag_img.set_size(width, height, img.nplanes());
  //fill image with 0
  frag_img.fill(0);

  //traverse through the intrinsic coordinates to get the grey values
  //from the image

  for (unsigned p=0;p<img.nplanes();++p){
    for (int x=0; x<width ; x++){ //psi
      int t_max = (int)vcl_floor(fragment->shock_edge()->r(x));
      for (int t=1; t<=t_max; t++){ //t
        vgl_point_2d<double> ex_pt1 = fragment->shock_edge()->get_ex_coords(x, t-0.5); //+ side
        vgl_point_2d<double> ex_pt2 = fragment->shock_edge()->get_ex_coords(x, -t+0.5);//- side

        frag_img(x,max_r+t-1,p) = (unsigned char) vil_bilin_interp_safe(img, ex_pt1.x(), ex_pt1.y(),p);
        frag_img(x,max_r-t,p)   = (unsigned char) vil_bilin_interp_safe(img, ex_pt2.x(), ex_pt2.y(),p);

        //frag_img(x,max_r+t-1,p) = (unsigned char) img(ex_pt1.x(), ex_pt1.y(),p);
        //frag_img(x,max_r-t,p)   = (unsigned char) img(ex_pt2.x(), ex_pt2.y(),p);

      }
    }
  }
}

//: compile an image fragment from the visual fragment
//temporary function: should go to visual fragment?
void 
dbsk2d_shock_image_fragment_tool::form_image_fragment2(vil_image_resource_sptr imgr,
                                                       dbsk2d_shock_fragment* fragment, 
                                                       vil_image_view< unsigned char >& frag_img)
{
  vil_image_view< unsigned char > img = imgr->get_view(0, imgr->ni(), 0, imgr->nj());

  //if there's no image associated with the geometry this method won't work
  if (!img)
    return;

  //Note: currently this method only works for xshock edge fragments

  int width = (int)fragment->shock_edge()->psi_max() + 1;
  int max_r = (int)vcl_floor(fragment->shock_edge()->r(width-1));
  int height = 2*max_r;

  //resize the image to the dimension of its intrinsic domain
  frag_img.set_size(width, height, img.nplanes());
  //fill image with 0
  frag_img.fill(0);

  //traverse through the intrinsic coordinates to get the grey values
  //from the image

  for (unsigned p=0;p<img.nplanes();++p){
    for (int x=0; x<width ; x++){ //psi
      int t_max = (int)vcl_floor(fragment->shock_edge()->r(x));
      for (int t=1; t<=t_max; t++){ //t
        vgl_point_2d<double> ex_pt1 = fragment->shock_edge()->get_ex_coords(x, t-0.5); //+ side
        vgl_point_2d<double> ex_pt2 = fragment->shock_edge()->get_ex_coords(x, -t+0.5);//- side

        frag_img(x,max_r+t-1+(max_r-t_max),p) = (unsigned char) vil_bilin_interp_safe(img, ex_pt1.x(), ex_pt1.y(),p);
        frag_img(x,max_r-t-(max_r-t_max),p)   = (unsigned char) vil_bilin_interp_safe(img, ex_pt2.x(), ex_pt2.y(),p);

        //frag_img(x,max_r+t-1,p) = (unsigned char) img(ex_pt1.x(), ex_pt1.y(),p);
        //frag_img(x,max_r-t,p)   = (unsigned char) img(ex_pt2.x(), ex_pt2.y(),p);

      }
    }
  }
}

bool
dbsk2d_shock_image_fragment_tool::handle( const vgui_event & e, 
                              const bvis1_view_tableau_sptr& view )
{
  if(left_click(e) || middle_click(e)) 
  {
    if (cur_geom_)
    {
      //popup a dialog box with the image of this fragment in intrinsic coordinates
      vgui_dialog mydialog("Visual Image Fragment in intrinsic coordinates");

      if (storage()->get_image()) {
        //create an image view for the fragment image
        vil_image_view< unsigned char > frag_image;

        //fill in the greyvalues of this image fragment from the original image
        if (left_click(e))
          this->form_image_fragment(storage()->get_image(), 
            (dbsk2d_shock_fragment*)cur_geom_, 
            frag_image);

        if (middle_click(e))
          this->form_image_fragment2(storage()->get_image(), 
            (dbsk2d_shock_fragment*)cur_geom_, 
            frag_image);

        vgui_image_tableau_new image(frag_image);
        vgui_viewer2D_tableau_new viewer(image);
        mydialog.inline_tableau(viewer, 3*image->width()+40, 3*image->height()+10);
      }
      else {
        //display an error message only 
        mydialog.message("No image associated with this geometry!");
      }
      vcl_string button_txt("Close Dialog");
      mydialog.set_ok_button(button_txt.c_str());
      mydialog.set_cancel_button(0);
      mydialog.ask();
      
      return true;
    }
  }
  
  return dbsk2d_rich_map_query_tool::handle(e, view);
}

vcl_string
dbsk2d_shock_image_fragment_tool::name() const
{
  return "Inspect Visual Image Fragment";
}

