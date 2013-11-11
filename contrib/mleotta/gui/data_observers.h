// This is contrib/mleotta/gui/data_observers.h
#ifndef data_observers_h_
#define data_observers_h_

//:
// \file
// \brief dbpro observers for vgui display of various data types
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/18/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbpro/dbpro_observer.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vidreg/vidreg_feature_group.h>
#include <vidreg/vidreg_salient_group.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_feature_point.h>

class image_observer: public dbpro_observer
{
  public:
    image_observer(const vgui_image_tableau_sptr& itab)
    : image_tab(itab) {}
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long timestamp)
    {
      assert(image_tab);
      assert(data);
      if(data->info() != DBPRO_VALID)
        image_tab->set_image_resource(NULL);
      else{
        assert(data->type_id() == typeid(vil_image_resource_sptr));
        image_tab->set_image_resource(data->data<vil_image_resource_sptr>());
      }
      vgui::out << "timestamp: "<<timestamp << '\n';
      image_tab->post_redraw();
      return true;
    }
    vgui_image_tableau_sptr image_tab;
};


class edgel_observer: public dbpro_observer
{
  public:
    edgel_observer(const vgui_easy2D_tableau_sptr& t,
                     float r=1.0, float g=1.0, float b=1.0,
                     bool clear = true)
  : tab(t), r_(r), g_(g), b_(b), clr_(clear) {}
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long)
    {
      assert(tab);
      assert(data);
      if(clr_)
        tab->clear();
      if(data->info() == DBPRO_VALID){
        assert(data->type_id() == typeid(vidreg_feature_group_sptr));
        vidreg_feature_group_sptr features = data->data<vidreg_feature_group_sptr >();
        if(!features)
          return true;

        tab->set_foreground(b_,g_,r_,1.0f);

        vcl_set<vidreg_feature_edgel*> edgels;
        for(vcl_vector<rgrl_feature_sptr>::const_iterator i = features->edgels.begin();
            i != features->edgels.end(); ++i){
          if(vidreg_feature_edgel* f = dynamic_cast<vidreg_feature_edgel*>(i->ptr())){
            edgels.insert(f);
            vnl_vector<double> pt = f->location();
            vnl_vector<double> pt2 = pt + f->normal();
            tab->add_line(pt[0],pt[1],pt2[0],pt2[1]);
          }
        }

        tab->set_foreground(r_,g_,b_,1.0f);

        float x[2000], y[2000];
        while(!edgels.empty()){
          vidreg_feature_edgel *e = *edgels.begin(), *first = e;
          while(e && e->prev() && first != e->prev()) e = e->prev();
          unsigned n=0;
          for(; e; ++n, e=e->next()){
            x[n] = e->location()[0];
            y[n] = e->location()[1];
            vcl_set<vidreg_feature_edgel*>::iterator i = edgels.find(e);
            if(i == edgels.end())
              break;
            edgels.erase(i);
          }
          if(n>10)
            vcl_cout << "adding edge of size "<<n<<vcl_endl;
          if(n>1)
            tab->add_linestrip(n,x,y);
        }
      }
      tab->post_redraw();
      return true;
    }
    vgui_easy2D_tableau_sptr tab;
    float r_,g_,b_;
    bool clr_;
};


class feature_observer: public dbpro_observer
{
  public:
    feature_observer(const vgui_easy2D_tableau_sptr& t, unsigned idx,
                     float r=1.0, float g=1.0, float b=1.0,
                     bool clear = true)
  : tab(t), idx_(idx), r_(r), g_(g), b_(b), clr_(clear) {}
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long)
    {
      assert(tab);
      assert(data);
      if(clr_)
        tab->clear();
      if(data->info() == DBPRO_VALID){
        assert(data->type_id() == typeid(vcl_vector<vidreg_salient_group_sptr>));
        vcl_vector<vidreg_salient_group_sptr> groups =
          data->data<vcl_vector<vidreg_salient_group_sptr> >();
        if(idx_ >= groups.size() || !groups[idx_])
          return true;
        vidreg_feature_group_sptr features = groups[idx_]->make_feature_group(1.0);

        tab->set_foreground(r_,g_,b_,0.5f);

        rgrl_mask_box box = groups[idx_]->view()->region();
        float x[4] = {box.x0()[0], box.x1()[0], box.x1()[0], box.x0()[0]};
        float y[4] = {box.x0()[1], box.x0()[1], box.x1()[1], box.x1()[1]};
        tab->add_polygon(4,x,y);

        for(vcl_vector<rgrl_feature_sptr>::const_iterator i = features->edgels.begin();
            i != features->edgels.end(); ++i){
          if(rgrl_feature_face_pt* f = dynamic_cast<rgrl_feature_face_pt*>(i->ptr())){
            vnl_vector<double> pt = f->location();
            vnl_vector<double> pt2 = pt + f->normal();
            tab->add_line(pt[0],pt[1],pt2[0],pt2[1]);
          }
        }
        for(vcl_vector<rgrl_feature_sptr>::const_iterator i = features->corners.begin();
            i != features->corners.end(); ++i){
          if(rgrl_feature_point* f = dynamic_cast<rgrl_feature_point*>(i->ptr()))
          {
            vnl_vector<double> pt = f->location();
            tab->add_point(pt[0],pt[1]);
#if 0
            if(f->scale() > 0.0){
              vnl_vector<double> norm(2);
              norm[0] = 10*vcl_cos(f->scale());
              norm[1] = 10*vcl_sin(f->scale());
              vnl_vector<double> pt2 = pt + norm;
              tab->add_line(pt[0],pt[1],pt2[0],pt2[1]);
            }
#endif
          }
        }
      }
      tab->post_redraw();
      return true;
    }
    vgui_easy2D_tableau_sptr tab;
    unsigned idx_;
    float r_,g_,b_;
    bool clr_;
};


class track_observer: public dbpro_observer
{
  public:
    track_observer(const vgui_easy2D_tableau_sptr& t,
                     float r=1.0, float g=1.0, float b=1.0,
                     bool clear = true)
  : tab(t), r_(r), g_(g), b_(b), clr_(clear) {}
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long )
    {
      assert(tab);
      assert(data);
      if(clr_)
        tab->clear();
      if(data->info() == DBPRO_VALID){
        typedef vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > Tvec;
        assert(data->type_id() == typeid(Tvec));
        Tvec tracks = data->data<Tvec >();

        tab->set_foreground(r_,g_,b_,1.0f);

        for(Tvec::const_iterator i = tracks.begin();
            i != tracks.end(); ++i){
          if(vidreg_feature_edgel* f = dynamic_cast<vidreg_feature_edgel*>(i->second.ptr())){
            vnl_vector<double> pt = f->location();
            vnl_vector<double> pt2 = pt + f->normal();
            if(i->first == 1){
              tab->set_foreground(b_,g_,r_,1.0f);
              tab->add_line(pt[0],pt[1],pt2[0],pt2[1]);
              tab->set_foreground(r_,g_,b_,1.0f);
            }else
              tab->add_line(pt[0],pt[1],pt2[0],pt2[1]);
          }
        }
      }
      tab->post_redraw();
      return true;
    }
    vgui_easy2D_tableau_sptr tab;
    float r_,g_,b_;
    bool clr_;
};






#endif // data_observers_h_
