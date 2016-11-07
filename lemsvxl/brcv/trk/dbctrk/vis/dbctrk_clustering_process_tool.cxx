#include <dbctrk/vis/dbctrk_clustering_process_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/dbctrk_algs.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vgui/vgui.h>
#include <dbctrk/dbctrk_curve_clustering.h>
#include <vcl_algorithm.h>
#include <dbctrk/dbctrk_utils.h> 
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
struct less_size
{
  bool operator()(dbctrk_curve_cluster x, dbctrk_curve_cluster y)
  { return x.curve_cluster_.size() < y.curve_cluster_.size(); }
};

dbctrk_clustering_process_tool::dbctrk_clustering_process_tool()
{

    cp = new dbctrk_curve_clustering_params(3,6.0,6.0);
  cl = new dbctrk_curve_clustering(*cp);
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}

dbctrk_clustering_process_tool::~dbctrk_clustering_process_tool()
{
  delete(cp);
  delete(cl);

}

bool
dbctrk_clustering_process_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_clustering_process_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "dbctrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbctrk_clustering_process_tool::tableau()
{
  return tableau_;
}

dbctrk_storage_sptr
dbctrk_clustering_process_tool::storage()
{
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage_);
  return dbctrk_storage;
}

bool
dbctrk_clustering_process_tool::handle( const vgui_event & e, 
                                            const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  int frame = view->frame();
  vcl_vector<dbctrk_tracker_curve_sptr> tc;
  if ( e.type == vgui_KEY_PRESS && e.key == 'i' && vgui_SHIFT)
  {
  dbctrk_storage_sptr p;
  p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
   
  tc.clear();
  p->get_tracked_curves(tc);
  cl->init_clusters(&tc);
  cl->build_table();
  vgui_dialog clustering("Clustering params");

  clustering.field("Inter cluster distance",cp->min_cost_threshold);
  clustering.field("Number of clusters",cp->no_of_clusters);
  clustering.field("Threshold figure-ground",cp->foreg_backg_threshold);

  if(!clustering.ask())
    return false;
   vcl_vector<vgui_soview*> all_objects;
   all_objects = tableau_->get_all();
   for(unsigned int i=0;i<all_objects.size();i++)
   {
   if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
   {
    
      neighbor_style_ = vgui_style::new_style(0.92f,0.92f,0.92f,2.0f,2.0f);
  ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
    
    }
   }
   bvis1_manager::instance()->post_redraw();
  }
  if ( e.type == vgui_KEY_PRESS && e.key == 'u' && vgui_SHIFT)
  {  
  for(unsigned int i=0;i<dcs_.size();i++)
    { 
     tableau_->remove(dcs_[i]);
    }
    dcs_.clear();
    vgui::out<<"\n";
    bvis1_manager::instance()->post_redraw();

  }
  if ( e.type == vgui_KEY_PRESS && e.key == 'j' && vgui_SHIFT)
  {
    vcl_sort(cl->clusters_.begin(),cl->clusters_.end(),less_size());
    int cnt=0;
    for(unsigned int j=0;j<cl->clusters_.size();j++,cnt++)
    {
     for(unsigned int i=0;i<cl->clusters_[j].curve_cluster_.size();i++)
     {
       vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(cl->clusters_[j].curve_cluster_[i]->desc->curve_);
       float r,g,b;
       utils::set_rank_colors(cnt,r,g,b);
       vgui_style_sptr pstyle= vgui_style::new_style(r,g,b,3.0,3.0);
       dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
     }
    }

  }
  if ( e.type == vgui_KEY_PRESS && e.key == 's' && vgui_SHIFT)
  {  
   if(dcs_.size()>0)
   {
   for(unsigned int i=0;i<dcs_.size();i++)
   { 
      tableau_->remove(dcs_[i]);
   }
   dcs_.clear();
   }
   if(cl->clusters_.size()<=0)
    return true;
   if((int)cl->clusters_.size()>cp->no_of_clusters)
   {
    int mini=-1,minj=-1;
  double min_cost= 1e5;
    if(((float)min_cost<cp->min_cost_threshold) && cl->return_min(mini,minj,min_cost))
    {
    for(unsigned int i=0;i<cl->clusters_[mini].curve_cluster_.size();i++)
    {
      vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(cl->clusters_[mini].curve_cluster_[i]->desc->curve_);
      vgui_style_sptr pstyle= vgui_style::new_style(0.0,0.0,1.0,3.0,3.0);
      dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
    }
    for(unsigned int j=0;j<cl->clusters_[minj].curve_cluster_.size();j++)
    {
      vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(cl->clusters_[minj].curve_cluster_[j]->desc->curve_);
      vgui_style_sptr qstyle= vgui_style::new_style(0.0,1.0,0.0,3.0,3.0);
      dcs_.push_back(tableau_->add_edgel_curve(dc2,qstyle));    
    }

    cl->merge_clusters(mini,minj);
    vgui::out<<min_cost<<"\n";

    }
      
   }
   bvis1_manager::instance()->post_redraw();
  }  
  if ( e.type == vgui_KEY_PRESS && e.key == 'k' && vgui_SHIFT)
  {  
   if(dcs_.size()>0)
   {
   for(unsigned int i=0;i<dcs_.size();i++)
   { 
      tableau_->remove(dcs_[i]);
   }
   dcs_.clear();
   }
   static int step=0;
   vgui_dialog step_dlg("no of steps to skip");
   step_dlg.field("Steps",step);
   if(!step_dlg.ask())
    return false;
   
   
   if(cl->clusters_.size()<=0)
    return true;
   for(int l=0;l<step;l++)
   {
   if((int)cl->clusters_.size()>cp->no_of_clusters)
   {
    int mini=-1,minj=-1;
    double min_cost=1e6;
    if(((float)min_cost<cp->min_cost_threshold) && cl->return_min(mini,minj,min_cost))
    {
    for(unsigned int i=0;i<cl->clusters_[mini].curve_cluster_.size();i++)
    {
      vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(cl->clusters_[mini].curve_cluster_[i]->desc->curve_);
      vgui_style_sptr pstyle= vgui_style::new_style(0.0,0.0,1.0,3.0,3.0);
      dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
    }
    for(unsigned int j=0;j<cl->clusters_[minj].curve_cluster_.size();j++)
    {
      vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(cl->clusters_[minj].curve_cluster_[j]->desc->curve_);
      vgui_style_sptr qstyle= vgui_style::new_style(0.0,1.0,0.0,3.0,3.0);
      dcs_.push_back(tableau_->add_edgel_curve(dc2,qstyle));    
    }

    cl->merge_clusters(mini,minj);
    vgui::out<<min_cost<<"\n";

    }
   } 
   }
   bvis1_manager::instance()->post_redraw();
  }  

  if( e.type == vgui_MOTION){
  tableau_->motion(e.wx, e.wy);
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
 
      object_ = (dbctrk_soview2D*)curr_obj;
      curr_curve_ = NULL;
      if( curr_obj && curr_obj->type_name() == "dbctrk_soview2D"){   
        curr_curve_ = object_->dbctrk_sptr();
      }
      
      bvis1_manager::instance()->post_overlay_redraw();
  }

 if( e.type == vgui_DRAW_OVERLAY){
  

  if(!curr_curve_)
    return false;


  vcl_map<double,int> bestmatches;
  vcl_map<double,int>::iterator itermatches;
  int mini=-1;

  if(curr_curve_->get_best_match_prev())
    {
    for(unsigned int i=0;i<cl->clusters_.size();i++)
    {
      for(unsigned int j=0;j<cl->clusters_[i].curve_cluster_.size();j++)
     {
       if(cl->clusters_[i].curve_cluster_[j]->frame_number==curr_curve_->frame_number && cl->clusters_[i].curve_cluster_[j]->get_id()==curr_curve_->get_id())
         mini=i;
     }
    }

  if(mini>0)
  {
    for(int i=0;i<(int)cl->clusters_.size();i++)
    {
      double cost=cl->compute_cluster_dist(i,mini);
      bestmatches[cost]=i;
    }
    int cnt=0;
    vgui::out<<"{";
    for(itermatches=bestmatches.begin();itermatches!=bestmatches.end() && cnt<7;itermatches++,cnt++)
    {

        for(unsigned int i=0;i<cl->clusters_[(*itermatches).second].curve_cluster_.size();i++)
        {
            if(cl->clusters_[(*itermatches).second].curve_cluster_[i]->frame_number==frame)
            {

                dbctrk_soview2D curve(cl->clusters_[(*itermatches).second].curve_cluster_[i]);
                float r,g,b;
                utils::set_rank_colors(cnt,r,g,b);
                neighbor_style_->rgba[0] =r;
                neighbor_style_->rgba[1] =g;
                neighbor_style_->rgba[2] =b;
                neighbor_style_->apply_all();  
                curve.draw();
            }
        }
        vgui::out<<(*itermatches).first<<",";
    }
    vgui::out<<"}\n";
  }
    
  }
 }
 if ( e.type == vgui_KEY_PRESS && e.key == 'f' && vgui_SHIFT)
   {   
     if(dcs_.size()>0)
       {
          for(unsigned int i=0;i<dcs_.size();i++)
          { 
            tableau_->remove(dcs_[i]);
          }
          dcs_.clear();
       }
    return false;
   }
 if ( e.type == vgui_KEY_PRESS && e.key == 'h' && vgui_SHIFT)
  {
      dbctrk_storage_sptr p;
      p.vertical_cast(bvis1_manager::instance()->repository()->get_data("dbctrk",0));
      tc.clear();
      p->get_tracked_curves(tc);

      vcl_map<int,vcl_vector<dbctrk_tracker_curve_sptr> > groups;
      for(unsigned int i=0;i<tc.size();i++)
      {
          if(tc[i]->group_id_>= 0)
          {
              groups[tc[i]->group_id_].push_back(tc[i]);
          }
          else
          {
              vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(tc[i]->desc->curve_);
              float r=0,g=0,b=0;
              vgui_style_sptr pstyle= vgui_style::new_style(r,g,b,3.0,3.0);
              dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
          }
      }

      vcl_map<int,vcl_vector<dbctrk_tracker_curve_sptr> >::iterator iter;
      for(iter=groups.begin();iter!=groups.end();iter++)
      {
          vcl_cout<<iter->first<<" ";
                            float r,g,b;
          utils::set_rank_colors(iter->first,r,g,b);
          vcl_cout<<"r="<<r<<","<<g<<","<<b<<"\n";

          for(unsigned int i=0;i<(*iter).second.size();i++)
          {

              vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves((*iter).second[i]->desc->curve_);
              //float r,g,b;
              //utils::set_rank_colors((*iter).second[i]->group_id_,r,g,b);
              //
              vgui_style_sptr pstyle= vgui_style::new_style(r,g,b,3.0,3.0);
              dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
          }
      }


 }
if (e.type == vgui_KEY_PRESS && e.key == 'b' &&  vgui_SHIFT ) {
        vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq;//=bvis1_manager::instance()->tableau_sequence();

        vgui_dialog m_dlg("Show Group curves");
     dbctrk_storage_sptr p;
      p.vertical_cast(bvis1_manager::instance()->repository()->get_data("dbctrk",0));
      tc.clear();
      p->get_tracked_curves(tc);

        
        static int groupid=0;

        m_dlg.field("Group ID",groupid);
        if(!m_dlg.ask())
            return false;
      vcl_map<int,vcl_vector<dbctrk_tracker_curve_sptr> > groups;
      vcl_vector<vsol_spatial_object_2d_sptr> polylines;
      for(unsigned int i=0;i<tc.size();i++)
      {
          if(tc[i]->group_id_>= 0)
          {
              groups[tc[i]->group_id_].push_back(tc[i]);
              if(tc[i]->group_id_==groupid)
              {
                  polylines.push_back(tc[i]->dc_->cast_to_spatial_object());

              }
          }
          else
          {
            groups[-1].push_back(tc[i]);  
          }
      }

        vidpro1_vsol2D_storage_sptr g =vidpro1_vsol2D_storage_new();
        g->set_name("Group");
        g->add_objects(polylines);
        bvis1_manager::instance()->repository()->store_data(g);

}
if ( e.type == vgui_KEY_PRESS && e.key == 'l' && vgui_SHIFT)
{
    dbctrk_storage_sptr p;
    p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
    tc.clear();
    p->get_tracked_curves(tc);
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();

    for(unsigned int i=0;i<all_objects.size();i++)
    {
        if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
        {

            neighbor_style_ = vgui_style::new_style(0.92f,0.92f,0.92f,2.0f,2.0f);
            ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);

        }
    }
    bvis1_manager::instance()->post_redraw();
}
 return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_clustering_process_tool::name() const
{
  return "visualize clustering";
}


