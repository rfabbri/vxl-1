#include "dbrl_match_set_tableau.h"
#include <vgui/vgui_command.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_projection_inspector.h>


class dbrl_match_set_tableau_toggle_command : public vgui_command
{
 public:
 dbrl_match_set_tableau_toggle_command(dbrl_match_set_tableau* tab, 
                                       const void* boolref) : 
       match_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    match_tableau->post_redraw(); 
  }

  dbrl_match_set_tableau *match_tableau;
  bool* bref;
};

dbrl_match_set_tableau::dbrl_match_set_tableau(const dbrl_match_set_sptr &dms):dms_(dms),gesture0_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true))

    {
        display_correspondence_=true;
        display_superimpose_=false;
        correspondence_inspector_=true;
        point_inspector_=false;
        show_velocity_flow_=false;
    }
dbrl_match_set_tableau::~dbrl_match_set_tableau()
    {

    }


vcl_string dbrl_match_set_tableau::type_name() const
    {
        return "dbrl_match_set_tableau";
    }
void dbrl_match_set_tableau::draw_correspondence()
    {
    
        if(dms_.ptr())
        {
            //: code to draw the original set of points 
            // along with alignment lines
            glBegin( GL_POINTS );
            glColor3f(1.0,0.0,0.0);
            glPointSize( 3.0);
            for(int i=0;i<dms_->size_set1();i++)
            {
                draw_a_feature(dms_->feature_set1_i(i));
            }
            glColor3f(0.0,0.0,1.0);
            for(int i=0;i<dms_->size_set2();i++)
            {
                draw_a_feature(dms_->feature_set2_i(i));
            }

            glEnd();
            float r=0.5,g=0.5,b=0.0;

            glColor3f(r,g,b);
            for(int i=0;i<dms_->size_set1();i++)
            {
                draw_a_correspondence(dms_->feature_set1_i(i),
                    dms_->correspondence1to2(i));

            }
        }

}
void dbrl_match_set_tableau::draw_superimpose()
    {
        if(dms_.ptr())
            {
                //: code to draw the mapped set of points 
                  
                glBegin( GL_POINTS );
                glColor3f(0.0,0.5,0.5);
                glPointSize( 3.0);
                for(int i=0;i<dms_->size_set1();i++)
                {
                    draw_a_feature(dms_->xformed_feature_set1_i(i).ptr());
                    //if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(dms_->xformed_feature_set1_i(i).ptr()))
                    //    glVertex2f(pt->location()[0], pt->location()[1]);
                }
                glColor3f(0.5,0.0,0.5);
                glPointSize( 2.0);
                for(int i=0;i<dms_->size_set2();i++)
                    draw_a_feature(dms_->xformed_feature_set2_i(i).ptr());
                glEnd();
            }
         
           
    }
void dbrl_match_set_tableau::draw_flow()
    {
        if(dms_.ptr())
            {
                //: code to draw the mapped set of points 
                  
                for(int i=0;i<dms_->size_set2();i++)
                {
                    glColor3f(0.5,0.0,0.5);
                    draw_a_feature(dms_->feature_set2_i(i).ptr());
                    glColor3f(0.0,0.5,0.5);
                    draw_a_feature(dms_->xformed_feature_set2_i(i).ptr());
                    glColor3f(0.5,0.5,0.0);
                    draw_a_correspondence(dms_->feature_set2_i(i).ptr(),dms_->xformed_feature_set2_i(i).ptr());
                }
            }
         
           
    }

bool
dbrl_match_set_tableau::
handle(vgui_event const &e)
{
  if (gesture0_(e)) 
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    
    if(correspondence_inspector_)
    {
        double mindist=1e5;
        minindex=-1;
        for(int i=0;i<dms_->size_set1();i++)
        {
            double dist=dist_from_feature(dms_->feature_set1_i(i),ix,iy);
            if(dist<mindist)
            {
                mindist=dist;
                minindex=i;
            }

        }
        //cur_feature=dms_->feature_set1_i(minindex);
      post_overlay_redraw();
        }
    
  
  else if(point_inspector_)
  {
      double mindist=1e5;

      int set_index=1;
      minindex=-1;
      for(int i=0;i<dms_->size_set1();i++)
      {
          double dist=dist_from_feature(dms_->feature_set1_i(i),ix,iy);
          if(dist<mindist)
          {
              mindist=dist;
              minindex=i;
          }
      }
      for(int i=0;i<dms_->size_set2();i++)
      {
          double dist=dist_from_feature(dms_->feature_set2_i(i),ix,iy);
          if(dist<mindist)
          {
              mindist=dist;
              minindex=i;
              set_index=2;
          }
      }
      vcl_cout<<"Feature Set "<<set_index<<"\n";
      if(set_index==1 && minindex>-1)
      {
          vcl_cout<<" Id is  "<<dms_->feature_set1_i(minindex)->id<<"\n";
      }
            if(set_index==2 && minindex>-1)
      {
          vcl_cout<<" Id is  "<<dms_->feature_set2_i(minindex)->id<<"\n";
      }



  }
  }
    if( e.type == vgui_OVERLAY_DRAW ) {
    //if closest feature selected display the top 5 correspondences
    if (minindex>=0)
    {
        vcl_map<double,dbrl_feature_sptr> f=dms_->correspondences1to2(minindex);
        vcl_map<double,dbrl_feature_sptr>::iterator iter;
        int cnt=0;
        vcl_vector<vcl_string> colornames;
        colornames.push_back("aqua");
        colornames.push_back("mediumorchid");
        colornames.push_back("darkorange");
        colornames.push_back("brown");
        colornames.push_back("chartreuse");
        if (dbrl_feature_point_tangent_curvature_groupings *pt
            =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>(dms_->feature_set1_i(minindex).ptr()))
        {
            draw_a_point_tangent_curvature_groupings(pt,true,dms_->get_feature_set1());
        }

        for(iter=f.begin();iter!=f.end() && cnt<5;iter++,cnt++)
        {
            float r,g,b;
            set_changing_colors(cnt,r,g,b);
            glColor3f(r,g,b);
            draw_a_correspondence(dms_->feature_set1_i(minindex),(*iter).second);
            if(cnt==0)
            {
                if (dbrl_feature_point_tangent_curvature_groupings *pt
                    =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>((*iter).second.ptr()))
                {
                    draw_a_point_tangent_curvature_groupings(pt,true,dms_->get_feature_set2());
                    //else
                    //    draw_a_point_tangent_curvature_groupings(pt,false,dms_->get_feature_set2());
                }
            }
            vcl_cout<<cnt<< " : "<<1-(*iter).first<<" "<<colornames[cnt]<<"\n";
        }
        vcl_cout<<"\n";
    }

    //return true;

  }
   return vgui_tableau::handle(e);
}

bool dbrl_match_set_tableau::draw()
    {
    if(display_correspondence_)
        draw_correspondence();
    if(display_superimpose_)
        draw_superimpose();
     if(show_velocity_flow_)
        draw_flow();
    return true;
    }   
void dbrl_match_set_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
    {
    vgui_menu submenu;
    vcl_string on = "[x] ", off = "[ ] ";

    submenu.add( ((display_correspondence_)?on:off)+"Show Correspondence ", 
        new dbrl_match_set_tableau_toggle_command(this, &display_correspondence_));

    submenu.add( ((display_superimpose_)?on:off)+"Draw Superimpose", 
        new dbrl_match_set_tableau_toggle_command(this, &display_superimpose_));

    submenu.add( ((correspondence_inspector_)?on:off)+"Inspect Correspondence ", 
        new dbrl_match_set_tableau_toggle_command(this, &correspondence_inspector_));

    submenu.add( ((point_inspector_)?on:off)+"Inspect Points ", 
        new dbrl_match_set_tableau_toggle_command(this, &point_inspector_));

    submenu.add( ((show_velocity_flow_)?on:off)+"Show velocity flow ", 
        new dbrl_match_set_tableau_toggle_command(this, &show_velocity_flow_));

  //add this submenu to the popup menu
  menu.add("RPM Tableau Options", submenu);
    }

void dbrl_match_set_tableau::draw_a_feature(dbrl_feature_sptr f)
{
    if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(f.ptr()))
        draw_a_point(pt);
    else if(dbrl_feature_point_tangent *pt=dynamic_cast<dbrl_feature_point_tangent *>(f.ptr()))
        draw_a_point_tangent(pt);
    else if(dbrl_feature_point_tangent_curvature_groupings *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>(f.ptr()))
        draw_a_point_tangent_curvature(pt);
    else if (dbrl_feature_point_tangent_curvature *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature *>(f.ptr()))
        draw_a_point_tangent_curvature(pt);

}
void dbrl_match_set_tableau::draw_a_point(dbrl_feature_point *pt)
{
    glBegin( GL_POINTS );
    glVertex2f(pt->location()[0], pt->location()[1]);
    glEnd();

}
void dbrl_match_set_tableau::draw_a_point_tangent(dbrl_feature_point_tangent *pt)
{
    glBegin( GL_POINTS );
    glVertex2f(pt->location()[0], pt->location()[1]);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(pt->location()[0]+0.5*vcl_cos(pt->dir()), 
               pt->location()[1]+0.5*vcl_sin(pt->dir()));
    glVertex2f(pt->location()[0]-0.5*vcl_cos(pt->dir()), 
               pt->location()[1]-0.5*vcl_sin(pt->dir()));
    glEnd();

}
void dbrl_match_set_tableau::draw_a_point_tangent_curvature(dbrl_feature_point_tangent_curvature *pt)
{
    glBegin( GL_POINTS );
    glVertex2f(pt->location()[0], pt->location()[1]);
    glEnd();
    if(pt->k_flag())
    {
        //dbgl_circ_arc carc=pt->get_arc();
        //glBegin(GL_LINE_STRIP);
        //// Display the list
        //for( double i = 0; i<1.0; ) {
        //    vgl_point_2d<double> p=carc.point_at(i);
        //    glVertex2f(p.x(),p.y());
        //}
        //glEnd();  
    }
    else
    {
        glPointSize(1.0);
        glBegin(GL_LINES);
        glVertex2f(pt->location()[0]+0.5*vcl_cos(pt->dir()), 
                   pt->location()[1]+0.5*vcl_sin(pt->dir()));
        glVertex2f(pt->location()[0]-0.5*vcl_cos(pt->dir()), 
                   pt->location()[1]-0.5*vcl_sin(pt->dir()));
        glEnd();
        glPointSize(2.0);
    }

}
void dbrl_match_set_tableau::draw_a_point_tangent_curvature_groupings(dbrl_feature_point_tangent_curvature_groupings *pt,
                                                                      bool flag,vcl_vector<dbrl_feature_sptr> allpts)
{
    glBegin( GL_POINTS );
    glVertex2f(pt->location()[0], pt->location()[1]);
    glEnd();
    if(pt->k_flag())
    {
        //dbgl_circ_arc carc=pt->get_arc();
        //glBegin(GL_LINE_STRIP);
        //// Display the list
        //for( double i = 0; i<1.0; ) {
        //    vgl_point_2d<double> p=carc.point_at(i);
        //    glVertex2f(p.x(),p.y());
        //    i+=0.1;
        //}
        //glEnd();  
    }
    else
    {
        glPointSize(1.0);
        glBegin(GL_LINES);
        glVertex2f(pt->location()[0]+0.5*vcl_cos(pt->dir()),pt->location()[1]+0.5*vcl_sin(pt->dir()));
        glVertex2f(pt->location()[0]-0.5*vcl_cos(pt->dir()),pt->location()[1]-0.5*vcl_sin(pt->dir()));
        glEnd();
        glPointSize(2.0);
    }

    if(flag)
    {
        glColor3f(0.0,1.0,0.0);
        glPointSize(4.0);
        for(unsigned i=0;i<pt->feature_neighbor_map_.size();i++)
        {
            draw_a_feature(allpts[pt->feature_neighbor_map_[i]]);
        }
    }
}
void dbrl_match_set_tableau::draw_a_correspondence(dbrl_feature_sptr f1,dbrl_feature_sptr f2)
{
    double x0,y0,x1,y1;
    if(!f1.ptr() || !f2.ptr())
        return;
    if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(f1.ptr()))
    {        x0=pt->location()[0]; y0=pt->location()[1];}
    else if(dbrl_feature_point_tangent *pt=dynamic_cast<dbrl_feature_point_tangent *>(f1.ptr()))
    {x0=pt->location()[0]; y0=pt->location()[1];}
    else if(dbrl_feature_point_tangent_curvature *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature *>(f1.ptr()))
    {x0=pt->location()[0]; y0=pt->location()[1];}
    else if (dbrl_feature_point_tangent_curvature_groupings *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>(f1.ptr()))
    {x0=pt->location()[0]; y0=pt->location()[1];}
    if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(f2.ptr()))
    {   x1=pt->location()[0]; y1=pt->location()[1];}
    else if(dbrl_feature_point_tangent *pt=dynamic_cast<dbrl_feature_point_tangent *>(f2.ptr()))
    {x1=pt->location()[0]; y1=pt->location()[1];}
    else if(dbrl_feature_point_tangent_curvature *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature *>(f2.ptr()))
    {   x1=pt->location()[0]; y1=pt->location()[1];}
    else if (dbrl_feature_point_tangent_curvature_groupings *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>(f2.ptr()))
    {x1=pt->location()[0]; y1=pt->location()[1];}
    glBegin(GL_LINES);
        glVertex2f(x0,y0);
        glVertex2f(x1,y1);
    glEnd(); 

}


double dbrl_match_set_tableau::dist_from_feature(dbrl_feature_sptr f1,float x, float y)
{

    double x1=0.0;
    double y1=0.0;

    if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(f1.ptr()))
    {
        x1=pt->location()[0];
        y1=pt->location()[1];
    }
    else if(dbrl_feature_point_tangent *pt=dynamic_cast<dbrl_feature_point_tangent *>(f1.ptr()))
    {
        x1=pt->location()[0];
        y1=pt->location()[1];
    }
    else if(dbrl_feature_point_tangent_curvature *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature *>(f1.ptr()))
    {
        x1=pt->location()[0];
        y1=pt->location()[1];
    }
    else if (dbrl_feature_point_tangent_curvature_groupings *pt
        =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings *>(f1.ptr()))
    {
        x1=pt->location()[0];
        y1=pt->location()[1];
    }

    double dist=vcl_sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));

    return dist;
}




void dbrl_match_set_tableau::set_changing_colors(int id,float &r,float &g, float &b)
{
  int pattern  = id%16;
  switch(pattern)
  {
    //aqua
  case 0 : (r)=0 ;(g)=255;(b)=255;break;
    //mediumorchid BA 55 D3
  case 1 : (r)=255;(g)=140;(b)=0;break;
    //darkorange
  case 2 : (r)=138 ;(g)=43;(b)=224;break;
    //brown
  case 3 : (r)=165 ;(g)=42;(b)=42;  break;
    //chartreuse
  case 4 : (r)=127 ;(g)=255;(b)=0;  break;
    //CHOCLATE BROWN
  case 5 : (r)=210 ;(g)=105;(b)=30;break;
    //CRIMSON
  case 6 : (r)=220 ;(g)=20;(b)=60;break;
    //darkgoldenrod
  case 7 : (r)=184;(g)=134;(b)=11;break;
    //darkmagenta
  case 8 : (r)=139;(g)=0;(b)=139;break;
    //blueviolet
  case 9 : (r)=186;(g)=85;(b)=211;break;
    //darkslategray
  case 10 :(r)=47;(g)=79;(b)=79;break;
    //deeppink
  case 11 : (r)=255;(g)=20;(b)=147;break;
    //gold
  case 12 : (r)=255;(g)=215;(b)=0;break;
    //greenyellow
  case 13 : (r)=173;(g)=255;(b)=47;break;
    //indianred  CD 5C 5C
  case 14 : (r)=192;(g)=92;(b)=92;break;
    //MAROON  80 00 0
  case 15 : (r)=128;(g)=0;(b)=0;break;
  
    default: (r) = 0; (g) = 0; (b) = 0; break; // this will never happen
  }

   (r)/=255;
   (g)/=255;
   (b)/=255;


  return  ;


}
