// This is brcv/seg/dbbgm/vis/dbbgm_distribution_image_tableau.cxx
#include "dbbgm_distribution_image_tableau.h"
//:
// \file
// \brief  See dbbgm_distribution_image_tableau.h for a description of this file.
// \author Matt Leotta


#include <dbsta/dbsta_distribution.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_apply.h>
#include <dbsta/dbsta_basic_functors.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_colour_space.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vcl_sstream.h>

#include <bil/algo/bil_harr_wavelet_transform.h>

//-----------------------------------------------------------------------------

dbbgm_distribution_image_tableau::
dbbgm_distribution_image_tableau(const dbbgm_distribution_image_sptr& dimg)
  : vgui_image_tableau(),
    dimg_(dimg),
    active_idx_(0),
    active_functor_f_(0),
    active_functor_d_(0),
    color_space_YUV_(false)
{
  // set default fail color to green
  fail_color_[0] = 0.0f; fail_color_[1] = 1.0f; fail_color_[2] = 0.0f;
  
  if( const dbbgm_distribution_image<double>* d = 
        dynamic_cast<const dbbgm_distribution_image<double>*>(dimg_.ptr()) ){
    if(dynamic_cast<const dbsta_gaussian<double>*>(&(*d)(0,0)))
      this->set_active_functor(new dbsta_mean_functor<double>());
      
    else if (dynamic_cast<const dbsta_mixture<double>*>(&(*d)(0,0)))
      this->set_active_functor(
        new dbsta_mixture_functor<double>(new dbsta_mean_functor<double>(), 0) );
  }
  
  else if( const dbbgm_distribution_image<float>* d = 
            dynamic_cast<const dbbgm_distribution_image<float>*>(dimg_.ptr()) ){
    if(dynamic_cast<const dbsta_gaussian<float>*>(&(*d)(0,0)))
      this->set_active_functor(new dbsta_mean_functor<float>());
      
    else if (dynamic_cast<const dbsta_mixture<float>*>(&(*d)(0,0)))
      this->set_active_functor(
        new dbsta_mixture_functor<float>(new dbsta_mean_functor<float>(), 0) );  
  }
  
}

//-----------------------------------------------------------------------------

dbbgm_distribution_image_tableau::
~dbbgm_distribution_image_tableau()
{
}

//-----------------------------------------------------------------------------

vcl_string
dbbgm_distribution_image_tableau::
type_name() const
{
  return "dbbgm_distribution_image_tableau";
}


//-----------------------------------------------------------------------------


//: Apply the functor to the distribution image and display the result
bool 
dbbgm_distribution_image_tableau::
functor_image(const dbsta_functor<double>& functor)
{

  dbbgm_distribution_image<double>* dist_img = dynamic_cast<dbbgm_distribution_image<double>*>(dimg_.ptr());
  if(!dist_img)
    return false;
     
  vnl_vector<double> fail_color(3,0.0);
  fail_color[0] = static_cast<double>(fail_color_[0]);
  fail_color[1] = static_cast<double>(fail_color_[1]);
  fail_color[2] = static_cast<double>(fail_color_[2]);

  vil_image_view<double> double_image;
  apply(*dist_img,functor,double_image,fail_color);
  
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
  
  vil_convert_stretch_range_limited(double_image, double_image,
                                    0.0, 1.0, 0.0, 255.999); 
  vil_image_view<vxl_byte> byte_image;
  vil_convert_cast(double_image, byte_image);
  this->set_image_view(byte_image);
  
  return true;
}


//-----------------------------------------------------------------------------


//: Apply the functor to the distribution image and display the result
bool 
dbbgm_distribution_image_tableau::
functor_image(const dbsta_functor<float>& functor)
{
  
  dbbgm_distribution_image<float>* dist_img = dynamic_cast<dbbgm_distribution_image<float>*>(dimg_.ptr());
  if(!dist_img)
    return false;
     
  vnl_vector<float> fail_color(3,3,fail_color_);

  vil_image_view<float> float_image;
  apply(*dist_img,functor,float_image,fail_color);
  
  //convert YUV to RGB
  if(color_space_YUV_ && float_image.nplanes() == 3){
    for(unsigned int i=0; i<float_image.ni(); ++i){
      for(unsigned int j=0; j<float_image.nj(); ++j){
        float pixel[3] = {float_image(i,j,0), float_image(i,j,1), float_image(i,j,2)};
        if(pixel[0] == fail_color[0] && 
          pixel[1] == fail_color[1] && 
          pixel[2] == fail_color[2])
          continue;
        float yuv[3];
        vil_colour_space_YUV_to_RGB(pixel, yuv);
        float_image(i,j,0) = yuv[0];
        float_image(i,j,1) = yuv[1];
        float_image(i,j,2) = yuv[2];
      }
    }
  }
  
  //bil_harr_wavelet_inverse(float_image,float_image);
  
  vil_image_view<double> double_image;
  //vil_convert_stretch_range_limited(float_image, double_image,
  //                                  0.0f, 1.0f, 0.0, 255.999); 
  vil_convert_stretch_range(float_image, double_image,
      0.0, 255.999);
  vil_image_view<vxl_byte> byte_image;
  vil_convert_cast(double_image, byte_image);
  this->set_image_view(byte_image);
  
  return true;
}


//-----------------------------------------------------------------------------

bool 
dbbgm_distribution_image_tableau::update()
{
  if(active_functor_f_){
    functor_image(*active_functor_f_);
    return true;
  }
  else if(active_functor_d_){
    functor_image(*active_functor_d_);
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------


bool
dbbgm_distribution_image_tableau::
handle(vgui_event const &e)
{
  if(e.type == vgui_DRAW ){//&& !this->get_image_resource()){
    update();
  }  
  return vgui_image_tableau::handle(e);
}


//----------------------------------------------------------------------------

namespace {

//: A vgui command to set the active functor
template <class T>
class dbsta_functor_command : public vgui_command
{
  public:
  dbsta_functor_command(const dbbgm_distribution_image_tableau_sptr& tab, 
                        const vbl_smart_ptr<dbsta_functor<T> >& functor) 
   : tab_(tab), functor_(functor) {}
   
  void execute()
  {
    tab_->set_active_functor(functor_);
    tab_->set_image_resource(NULL);
    //tab_->functor_image(*functor_);
  }

  dbbgm_distribution_image_tableau_sptr tab_;
  vbl_smart_ptr<dbsta_functor<T> > functor_;
};


//----------------------------------------------------------------------------

//: A vgui command to set the active mixture index
class dbsta_mix_idx_command : public vgui_command
{
  public:
  dbsta_mix_idx_command(const dbbgm_distribution_image_tableau_sptr& tab, 
                        unsigned int idx) 
   : tab_(tab), idx_(idx) {}
   
  void execute()
  {
    tab_->set_active_idx(idx_);
  }

  dbbgm_distribution_image_tableau_sptr tab_;
  unsigned int idx_;
};

//----------------------------------------------------------------------------

//: A vgui command to toggle YUV conversion
class dbbgm_yuv_command : public vgui_command
{
  public:
  dbbgm_yuv_command(const dbbgm_distribution_image_tableau_sptr& tab) 
   : tab_(tab) {}
   
  void execute()
  {
    tab_->color_space_YUV_ = !tab_->color_space_YUV_;
  }

  dbbgm_distribution_image_tableau_sptr tab_;
};

//----------------------------------------------------------------------------

//: A vgui command to set the active mixture index
class dbbgm_fail_color_command : public vgui_command
{
  public:
  dbbgm_fail_color_command(const dbbgm_distribution_image_tableau_sptr& tab) 
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
  }

  dbbgm_distribution_image_tableau_sptr tab_;
};


//----------------------------------------------------------------------------


template <class T>
static inline vgui_menu 
make_functor_menu(const dbbgm_distribution_image_tableau_sptr& tab, 
                  const vgui_popup_params& params)
{
  vgui_menu f_menu;
  const dbbgm_distribution_image<T>& dist_img = 
    static_cast<const dbbgm_distribution_image<T>&>(*tab->dist_image());
  // get the upper left distribution
  const dbsta_distribution<T>& dist = dist_img(0,0);
  // Gaussian Options
  if(dynamic_cast<const dbsta_gaussian<T>*>(&dist)){
    f_menu.add("mean", new dbsta_functor_command<T>(tab,
                             new dbsta_mean_functor<T>));
    f_menu.add("variance", new dbsta_functor_command<T>(tab,
                                 new dbsta_var_functor<T>));
  }
  if(dynamic_cast<const dbsta_mixture<T>*>(&dist)){
    unsigned int idx = tab->active_idx();
    f_menu.add("weighted mean", new dbsta_functor_command<T>(tab,
                                      new dbsta_weighted_sum_functor<T>(
                                            new dbsta_mean_functor<T>())));
    f_menu.add("weight", new dbsta_functor_command<T>(tab,
                               new dbsta_weight_functor<T>(idx)));
    f_menu.add("mean", new dbsta_functor_command<T>(tab,
                             new dbsta_mixture_functor<T>(new dbsta_mean_functor<T>(), idx)));
    f_menu.add("variance", new dbsta_functor_command<T>(tab,
                                 new dbsta_mixture_functor<T>(new dbsta_var_functor<T>(), idx)));
    f_menu.separator();
    vgui_menu ind_menu;
    for(unsigned int i=0; i<10; ++i){
      vcl_stringstream num;
      if(i==tab->active_idx())
        num << "("<<i<<")";
      else
        num << " "<<i<<" ";
      ind_menu.add(num.str(), new dbsta_mix_idx_command(tab,i));
    }
    f_menu.add("active index",ind_menu);
    
    f_menu.add("num components", new dbsta_functor_command<T>(tab,
                                       new dbsta_mixture_size_color_functor<T>()));
  }
  //
  return f_menu;
}

}; // end anonymous namespace

//----------------------------------------------------------------------------

//: Generate the popup menu
void 
dbbgm_distribution_image_tableau::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  if(dimg_){
    vgui_menu functor_menu;
    if(dynamic_cast<dbbgm_distribution_image<double>*>(dimg_.ptr())){
      functor_menu = make_functor_menu<double>(this,params);
    }
    else if(dynamic_cast<dbbgm_distribution_image<float>*>(dimg_.ptr())){
      functor_menu = make_functor_menu<float>(this,params);
    }
    functor_menu.separator();
    functor_menu.add("failure color", new dbbgm_fail_color_command(this));
    
    vcl_string check = (color_space_YUV_)?"[x]":"[ ]";
    functor_menu.add(check+" display YUV", new dbbgm_yuv_command(this));
    menu.add("Distribution Image", functor_menu); 
  }
}

