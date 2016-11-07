
// This is breye1/dbctrk/vis/dbctrk_clustering_tools.cxx
//:
// \file

#include "dbctrk/vis/dbctrk_clustering_tool.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_displayer.h>
#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage.h> 
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_utils.h>



struct less_val
{
  bool operator()( vcl_pair< int, vcl_vector<dbctrk_soview2D*> > x, vcl_pair<int , vcl_vector<dbctrk_soview2D*> > y)
  { return x.second.size() > y.second.size(); }
};
//: Constructor - protected
dbctrk_clustering_tool::dbctrk_clustering_tool()
 :  tableau_(NULL), storage_(NULL)
{
//  gesture_neighbors_=vgui_event_condition(vgui_RIGHT,vgui_SHIFT,true);
//  gesture_transform_=vgui_event_condition(vgui_RIGHT,vgui_SHIFT,true);

}


//: Set the tableau to work with
bool
dbctrk_clustering_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
    return false;

  if( tableau.ptr() != NULL && tableau->type_name() == "bgui_vsol2D_tableau" ){
    tableau_.vertical_cast(tableau);
    return true;
  }

  tableau_ = NULL;
  return false;
}


//: Set the storage class for the active tableau
bool
dbctrk_clustering_tool::set_storage ( const bpro1_storage_sptr& storage )
{
  if (!storage.ptr())
    return false;
  //make sure its a bmrf storage class
  if (storage->type() == "dbctrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

//----------------------dbctrk_inspector_tool---------------------------


//: Constructor
dbctrk_inspector_tool::dbctrk_inspector_tool()
 : object_(NULL)
{
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
  numtopmatches=6;
  threshold=1.0;
  thresh=false;
  topmatches=true;
  
}


//: Destructor
dbctrk_inspector_tool::~dbctrk_inspector_tool()
{
}


//: Return the name of this tool
vcl_string
dbctrk_inspector_tool::name() const
{
  return "Transformation induced neighbors"; 
}


//: Handle events
bool
dbctrk_inspector_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
{
  
    
  if( !tableau_.ptr() )
    return false;

  if( e.type == vgui_KEY_PRESS && e.key == 's' && vgui_SHIFT ){
    vcl_vector<dbctrk_tracker_curve_sptr> tc;
    storage_->get_tracked_curves(tc);
    typedef vcl_vector<dbctrk_tracker_curve_sptr>::const_iterator curve_iterator;
    for( curve_iterator c_itr = tc.begin(); c_itr != tc.end();  ++c_itr )
    {
      if((*c_itr)->ismovingobject_)
      {
        neighbor_style_->rgba[0]=0.0;
        neighbor_style_->rgba[1]=1.0;
        neighbor_style_->rgba[2]=0.0;
        neighbor_style_->apply_all();  
        dbctrk_soview2D(*c_itr).draw();
      }
    }
  
  }
  if( e.type == vgui_KEY_PRESS && e.key == 't'){
    vgui_dialog topmatches_dlg("Threshold or no of top matches");
    topmatches_dlg.checkbox("Threshold(y/n)",thresh);
    topmatches_dlg.field("Threshold",threshold);
    topmatches_dlg.field("No of Top matches",numtopmatches);
    if(!topmatches_dlg.ask())
      return true;
    if(thresh)
      topmatches=false;
    else
      topmatches=true;
  }

    float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  
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

  // gives the transformation distance between two curves
  if( e.type == vgui_KEY_PRESS && e.key == 'r' && vgui_SHIFT ){
  vcl_vector<vgui_soview*> all_objects;
  all_objects = tableau_->get_selected_soviews();
  if(all_objects.size()==2)
  {
    dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
    dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)all_objects[1])->dbctrk_sptr();

    if(c1->get_best_match_prev().ptr() && c2->get_best_match_prev().ptr())
    {
      vcl_map<double,int>::iterator iter;
      for(iter=c1->neighbors_.begin();iter!=c1->neighbors_.end();iter++)
      {
        if((*iter).second==c2->get_id())
          vcl_cout<<"\n the distance between curves "<<c1->get_id() <<" and "<<c2->get_id()
              <<" is "<<(*iter).first;
      }
    }
    else
      vcl_cout<<"\n one or both of the curves do not have match";
  }
  else
  {
    vgui::out<<"requires only two curves \n";
  }
  tableau_->deselect_all();
  
  }
  int frame = view->frame();
  // draws neighbors in the transformation domain of a curve 
  if( e.type == vgui_DRAW_OVERLAY){
  if(!curr_curve_)
    return false;


  if(curr_curve_->get_best_match_prev())
    {
    vcl_map<double,int>::iterator iter;
    vcl_vector<dbctrk_tracker_curve_sptr> tc;
    storage_->get_tracked_curves(tc);
    
    if(curr_curve_->frame_number==frame)
    {
      for(unsigned int i=0;i<tc.size();i++)
      {
        dbctrk_soview2D curve(tc[i]);
        neighbor_style_->rgba[0] =0.92f;
        neighbor_style_->rgba[1] =0.92f;
        neighbor_style_->rgba[2] =0.92f;
        neighbor_style_->apply_all();  
        curve.draw();
      }
    }
    if(numtopmatches>6)
      numtopmatches=6;

    int cnt=0;
  vgui::out<<"{";
    for(iter=curr_curve_->neighbors_.begin(),cnt=0;iter!=curr_curve_->neighbors_.end() ;iter++,cnt++)
    {
      if(curr_curve_->frame_number==frame)
      {
     dbctrk_soview2D neighbors(tc[(*iter).second]);
     if((thresh && (*iter).first<threshold )|| (topmatches && cnt<numtopmatches ))
     {
       float r,g,b;
       utils::set_rank_colors(cnt-1,r,g,b);
       neighbor_style_->rgba[0] =r  ;
       neighbor_style_->rgba[1] =g  ;
       neighbor_style_->rgba[2] =b   ;
       neighbor_style_->apply_all();  
       neighbors.draw();
       vgui::out<<(*iter).first<<",";
     }
      }
      
      }
    vgui::out<<"}";
    vgui::out<<"\n";
    }
  else
  {
    vgui::out.width(6);
    vgui::out.fill(' ');
    vgui::out.precision(6);
    vgui::out<<"does not have a match\n";
  }

  }

  return false;
  


}





//: Constructor
dbctrk_foreground_background_tool::dbctrk_foreground_background_tool()
 : object_(NULL)
{
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
  top_X=0.0;
  bottom_X=0.0;
  theta_R=0.3;
  
}


//: Destructor
dbctrk_foreground_background_tool::~dbctrk_foreground_background_tool()
{
}


//: Return the name of this tool
vcl_string
dbctrk_foreground_background_tool::name() const
{
  return "See clusters"; 
}


//: Handle events
bool
dbctrk_foreground_background_tool::handle( const vgui_event & e, 
                                              const bvis1_view_tableau_sptr& view )
{
  
    
  if( !tableau_.ptr() )
    return false;

  if( e.type == vgui_KEY_PRESS && e.key == 's'){



     vcl_vector<vgui_soview*> all_objects;
     all_objects = tableau_->get_all();

     for(unsigned int i=0;i<all_objects.size();i++)
     {
     if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
     {
      if(  ((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->ismovingobject_)
      {
        neighbor_style_ = vgui_style::new_style(0.0,1.0,0.0,2.0,2.0);
 
        ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
      }
      else
      {
          neighbor_style_ = vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
          ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
      }
       }
     }
    

  return true;
  }


  
  if( e.type == vgui_KEY_PRESS && e.key == 'r'){
     vcl_vector<vgui_soview*> all_objects;
     all_objects = tableau_->get_all();
     for(unsigned int i=0;i<all_objects.size();i++)
     {
     if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
     {
        float r,g,b;
        utils::set_changing_colors( ((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->match_id_ ,r, g, b );
        neighbor_style_ = vgui_style::new_style(r,g,b,2.0,2.0);
        ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
       }
     }
     return true;
  }


  bvis1_manager::instance()->post_redraw();
 
  if( e.type == vgui_KEY_PRESS && e.key == 'l')
  {
     vcl_map<int, vcl_vector<dbctrk_soview2D*> > groupings;
     vcl_map<int, vcl_vector<dbctrk_soview2D*> >::iterator iter;
      vcl_vector<vgui_soview*> all_objects;
     all_objects = tableau_->get_all();
     for(unsigned int i=0;i<all_objects.size();i++)
     {
     if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
     {      
      neighbor_style_ = vgui_style::new_style(0.92f,0.92f,0.92f,2.0f,2.0f);
      ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
      if(  ((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->get_best_match_prev().ptr())
      {
        groupings[((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->group_id_].push_back((dbctrk_soview2D*)all_objects[i]);
      }
     
     }
    }
     vcl_vector<vcl_pair<int, vcl_vector<dbctrk_soview2D*> > > pairs;
     for(iter=groupings.begin();iter!=groupings.end();iter++)
     {
        pairs.push_back(vcl_make_pair((*iter).first,(*iter).second));
     }
     vcl_sort(pairs.begin(),pairs.end(),less_val());


     for(unsigned int i=0;i<7 && i<pairs.size();i++)
     {
        float r,g,b;
        utils::set_rank_colors(i,r,g,b);
        for(unsigned int j=0;j<pairs[i].second.size();j++)
        {
           
            neighbor_style_ = vgui_style::new_style(r,g,b,2.0,2.0);
            pairs[i].second[j]->set_style(neighbor_style_);
        }
     }
   return true;
  }


  if( e.type == vgui_KEY_PRESS && e.key == 'v')
  {
    vgui_dialog see_moving_curves_dlg("Translation");
    
    see_moving_curves_dlg.field("Lower Bound for Translation in X",bottom_X);
    see_moving_curves_dlg.field("Upper Bound for Translation in X",top_X);
    see_moving_curves_dlg.field("Upper Bound for theta ",theta_R);

    if(!see_moving_curves_dlg.ask())
      return true;

    vcl_vector<vgui_soview*> all_objects;
     all_objects = tableau_->get_all();

    for(unsigned int i=0;i<all_objects.size();i++)
     {
     if (((dbctrk_soview2D*)all_objects[i])->type_name() == "dbctrk_soview2D") 
     {
      if(((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->get_best_match_prev().ptr())
      {
        double tx=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->get_best_match_prev()->Tbar(0,0);
        double theta=vcl_acos(((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr()->get_best_match_prev()->R_(0,0));

        theta=vcl_fabs(theta);
        vcl_cout<<"\n the values are "<<tx<<" "<<top_X<<" "<<bottom_X;
        if(tx<top_X && tx>bottom_X && theta < theta_R)
        {
         neighbor_style_ = vgui_style::new_style(0.0,1.0,0.0,2.0,2.0);
         ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
        }
        else
        {
          neighbor_style_ = vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
          ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
        }

      }
      else
      {
          neighbor_style_ = vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
          ((dbctrk_soview2D*)all_objects[i])->set_style(neighbor_style_);
      }
       }
     }

  }

  bvis1_manager::instance()->post_redraw();
  return false;
  


}



