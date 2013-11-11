// This is brcv/seg/dbbgm/vis/dbbgm_image_tableau.cxx
#include "dbbgm_image_tableau.h"
//:
// \file
// \brief  See dbbgm_image_tableau.h for a description of this file.
// \author Matt Leotta


#include <bsta/bsta_distribution.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_apply.h>
#include <bsta/bsta_basic_functors.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_colour_space.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_range_map_params.h>
#include <vcl_sstream.h>


#include <bbgm/bbgm_viewer.h>

//-----------------------------------------------------------------------------

dbbgm_image_tableau::
dbbgm_image_tableau(const bbgm_image_sptr& dimg)
  : bgui_image_tableau(),
    dimg_(dimg),
    active_idx_(0),
    active_viewer_(0),
    color_space_YUV_(false)
{
  // set default fail color to green
  fail_color_[0] = 0.0f; fail_color_[1] = 1.0f; fail_color_[2] = 0.0f;

  all_viewers_.push_back(new bbgm_mean_viewer);
  all_viewers_.push_back(new bbgm_variance_viewer);
  all_viewers_.push_back(new bbgm_weight_viewer);

  vcl_vector<bbgm_viewer_sptr>::const_iterator itr = all_viewers_.begin();
  for(; itr != all_viewers_.end(); ++itr){
    if((*itr)->probe(dimg_)){
      this->set_active_viewer(*itr);
      break;
    }
  }
}

//-----------------------------------------------------------------------------

dbbgm_image_tableau::
~dbbgm_image_tableau()
{
}

//-----------------------------------------------------------------------------

vcl_string
dbbgm_image_tableau::
type_name() const
{
  return "dbbgm_image_tableau";
}


//-----------------------------------------------------------------------------

//: Set the active mixture index
void
dbbgm_image_tableau::
set_active_idx(unsigned int idx)
{
  active_idx_ = idx;
  if(active_viewer_)
    active_viewer_->set_active_component(idx);
}


//-----------------------------------------------------------------------------

//: Set the active mixture functor
void
dbbgm_image_tableau::
set_active_viewer(const bbgm_viewer_sptr& viewer)
{
  active_viewer_ = viewer;
  if(active_viewer_)
    active_viewer_->set_active_component(active_idx_);
}


//-----------------------------------------------------------------------------


//: Apply the functor to the distribution image and display the result
bool 
dbbgm_image_tableau::
functor_image(const bbgm_viewer_sptr& viewer)
{
  if(!viewer)
    return false;

  vil_image_view<double> double_image;

  vnl_vector<double> fail_color(3,0.0);
  fail_color[0] = static_cast<double>(fail_color_[0]);
  fail_color[1] = static_cast<double>(fail_color_[1]);
  fail_color[2] = static_cast<double>(fail_color_[2]);

  viewer->set_failure_value(fail_color);
  if(!viewer->apply(dimg_, double_image))
    return false;

  if(double_image.ni()==0 && double_image.nj()==0){
    this->set_image_resource(NULL);
    return false;
  }

  //convert YUV to RGB
  if(color_space_YUV_ && double_image.nplanes() == 3){
    for(unsigned int i=0; i<double_image.ni(); ++i){
      for(unsigned int j=0; j<double_image.nj(); ++j){
        double pixel[3] = {double_image(i,j,0), double_image(i,j,1), double_image(i,j,2)};
        if(pixel[0] == fail_color[0] && 
          pixel[1] == fail_color[1] && 
          pixel[2] == fail_color[2])
          continue;
        double yuv[3];
        vil_colour_space_YUV_to_RGB(pixel, yuv);
        double_image(i,j,0) = yuv[0];
        double_image(i,j,1) = yuv[1];
        double_image(i,j,2) = yuv[2];
      }
    }
  }

  //vil_convert_stretch_range_limited(double_image, double_image,
  //                                  0.0, 1.0, 0.0, 255.999); 
  //vil_image_view<vxl_byte> byte_image;
  //vil_convert_cast(double_image, byte_image);
  this->set_image_view(double_image);
  vgui_range_map_params_sptr rmp;
  long double one = 1.0;
  long double zero = 0.0;
  if(double_image.nplanes() == 3)
    rmp = new vgui_range_map_params(zero,one, zero,one, zero,one);
  else
    rmp = new vgui_range_map_params(zero,one);
  this->set_mapping(rmp);

  return true;
}

//-----------------------------------------------------------------------------

bool 
dbbgm_image_tableau::update()
{
  if(active_viewer_){
    functor_image(active_viewer_);
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------


bool
dbbgm_image_tableau::
handle(vgui_event const &e)
{
  if(e.type == vgui_DRAW && !this->get_image_resource()){
    update();
  }
  if(e.type == vgui_KEY_PRESS)
  {
    // switch the active index
    int num = e.key - '0';
    if(num >=0 && num < (int)active_viewer_->max_components(dimg_)){
      set_active_idx(num);
      set_image_resource(NULL);
      this->post_redraw();
      return true;
    }
  }
  return bgui_image_tableau::handle(e);
}


//----------------------------------------------------------------------------

namespace{

//: A vgui command to set the active functor
class bbgm_viewer_command : public vgui_command
{
  public:
  bbgm_viewer_command(const dbbgm_image_tableau_sptr& tab,
                       const bbgm_viewer_sptr& viewer)
    : tab_(tab), viewer_(viewer) {}

  void execute()
  {
    tab_->set_active_viewer(viewer_);
    tab_->set_image_resource(NULL);
    tab_->post_redraw();
  }

  dbbgm_image_tableau_sptr tab_;
  bbgm_viewer_sptr viewer_;
};


//----------------------------------------------------------------------------

//: A vgui command to set the active mixture index
class dbbgm_mix_idx_command : public vgui_command
{
  public:
  dbbgm_mix_idx_command(const dbbgm_image_tableau_sptr& tab,
                        unsigned int idx) 
   : tab_(tab), idx_(idx) {}

  void execute()
  {
    tab_->set_active_idx(idx_);
    tab_->set_image_resource(NULL);
    tab_->post_redraw();
  }

  dbbgm_image_tableau_sptr tab_;
  unsigned int idx_;
};

//----------------------------------------------------------------------------

//: A vgui command to toggle YUV conversion
class dbbgm_yuv_command : public vgui_command
{
  public:
  dbbgm_yuv_command(const dbbgm_image_tableau_sptr& tab)
   : tab_(tab) {}

  void execute()
  {
    tab_->color_space_YUV_ = !tab_->color_space_YUV_;
    tab_->set_image_resource(NULL);
    tab_->post_redraw();
  }

  dbbgm_image_tableau_sptr tab_;
};

//----------------------------------------------------------------------------

//: A vgui command to set the active mixture index
class dbbgm_fail_color_command : public vgui_command
{
  public:
  dbbgm_fail_color_command(const dbbgm_image_tableau_sptr& tab)
   : tab_(tab) {}

  void execute()
  {
    vcl_stringstream color_istm;
    color_istm << tab_->fail_color_[0]<<' '
               << tab_->fail_color_[1]<<' '
               << tab_->fail_color_[2];
    vcl_string color = color_istm.str();
    vgui_dialog color_dlg("Select the Failure Color");
    color_dlg.inline_color("Failure Color", color);
    if(!color_dlg.ask())
      return;
    vcl_stringstream color_ostm;
    color_ostm << color;
    color_ostm >> tab_->fail_color_[0]
               >> tab_->fail_color_[1]
               >> tab_->fail_color_[2];
    tab_->set_image_resource(NULL);
    tab_->post_redraw();
  }

  dbbgm_image_tableau_sptr tab_;
};

}; // end anonymous namespace

//----------------------------------------------------------------------------

//: Generate the popup menu
void 
dbbgm_image_tableau::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  if(dimg_){
    vgui_menu view_menu;

    vcl_vector<bbgm_viewer_sptr>::const_iterator itr = all_viewers_.begin();
    for(; itr != all_viewers_.end(); ++itr){
      if((*itr)->probe(dimg_))
        view_menu.add((*itr)->name(), new bbgm_viewer_command(this,*itr));
    }

    if(active_viewer_){
      unsigned int nc = active_viewer_->max_components(dimg_);
      view_menu.separator();
      vgui_menu ind_menu;
      for(unsigned int i=0; i<nc; ++i){
        vcl_stringstream num;
        if(i==this->active_idx())
          num << "("<<i<<")";
        else
          num << " "<<i<<" ";
        ind_menu.add(num.str(), new dbbgm_mix_idx_command(this,i));
      }
      view_menu.add("active index",ind_menu);
    }

    view_menu.separator();
    view_menu.add("failure color", new dbbgm_fail_color_command(this));

    vcl_string check = (color_space_YUV_)?"[x]":"[ ]";
    view_menu.add(check+" display YUV", new dbbgm_yuv_command(this));
    menu.add("Distribution Image", view_menu);
  }
  bgui_image_tableau::get_popup(params,menu);
}

