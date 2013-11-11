// This is brcv/seg/dbbgm/vis/dbbgm_inspector_tool.cxx
//:
// \file

#include "dbbgm_inspector_tool.h"
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_gauss_if3.h>


dbbgm_inspector_tool::dbbgm_inspector_tool()
{
}


dbbgm_inspector_tool::~dbbgm_inspector_tool()
{
}


bool dbbgm_inspector_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbbgm_image_tableau_sptr temp_tab;
  temp_tab = dynamic_cast<dbbgm_image_tableau*>(tableau.ptr());
  if (!temp_tab)
    return false;
  tableau_ = temp_tab;
  return true;
}



dbbgm_image_tableau_sptr
dbbgm_inspector_tool::tableau()
{
  return tableau_;
}


bool dbbgm_inspector_tool::handle(const vgui_event & e)
{
  typedef bsta_num_obs<bsta_gauss_if3> _component;
  typedef bsta_num_obs<bsta_mixture<_component > > _dist;

  if(!tableau_)
    return false;
    if(e.type==vgui_MOTION)
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    last_x_ = static_cast<int>(vcl_floor(ix+0.5));
    last_y_ = static_cast<int>(vcl_floor(iy+0.5));
  }
  if(e.type==vgui_MOTION || e.type==vgui_DRAW)
  {
    vgui::out<<"("<<last_x_ <<","<<last_y_ <<")";

    bbgm_image_sptr model_sptr = tableau_->dist_image();
    bbgm_image_of<_dist> *model = dynamic_cast<bbgm_image_of<_dist>*>(model_sptr.ptr());
    if(!model)
      return false;

    if(last_x_ <0 || last_x_ >= (int)model->ni() || last_y_ <0 || last_y_ >= (int)model->nj()){
      vgui::out<<'\n';
      return (e.type!=vgui_DRAW);
    }

    const _dist& d = (*model)(last_x_,last_y_);
    if(tableau_->active_idx() < d.num_components()){
      const _component& c = d.distribution(tableau_->active_idx());
      vgui::out<<" mean("<<c.mean()<<") covar("<<c.diag_covar()<<")\n";
    }

    return (e.type!=vgui_DRAW);
  }

  if(e.type==vgui_MOUSE_DOWN && e.button == vgui_LEFT)
  {
    bbgm_image_sptr model_sptr = tableau_->dist_image();
    bbgm_image_of<_dist> *model = dynamic_cast<bbgm_image_of<_dist>*>(model_sptr.ptr());
    if(!model)
      return false;

    if(last_x_ <0 || last_x_ >= (int)model->ni() || last_y_ <0 || last_y_ >= (int)model->nj())
      return false;

    vcl_cout << "pixel ("<<last_x_<<", "<<last_y_<<")\n";
    const _dist& d = (*model)(last_x_,last_y_);
    for(unsigned int i=0; i<d.num_components(); ++i){
      const _component& c = d.distribution(i);
      vcl_cout<< "weight= "<<d.weight(i)<<"\t mean("<<c.mean()<<")\t diag_covar("<<c.diag_covar()<<")\n";
    }
    vcl_cout << vcl_endl;
    return true;
  }
  return false;
}


