// This is Spatemp/vis/dbdet_temporal_map_tableau.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_deque.h>
#include <vcl_cstdio.h>
#include <vcl_limits.h>

#include <gl2ps/gl2ps.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_test.h>
#include "dbdet_temporal_map_tableau.h"
#include "dbdet_temporal_map_tableau_commands.h"
#include <bvis1/bvis1_manager.h>

#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <dbdet/sel/dbdet_edgel_link_graph.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>

#include <Spatemp/algo/dbdet_temporal_options.h>

#include <Spatemp/algo/dbdet_temporal_map.h>
#include <Spatemp/algo/dbdet_temporal_map_sptr.h>
#include <Spatemp/pro/dbdet_temporal_map_storage.h>
#include <Spatemp/pro/dbdet_temporal_map_storage_sptr.h>
#include <Spatemp/algo/dbdet_spatial_temporal_options.h>

#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vidpro1/vidpro1_repository.h>

#undef DRAW_GL2PS
#define DRAW_GL2PS 1
      const float col_pal[6][3] = {   {     0,      0,      0},
                                  {     0, 0.4000, 0.4000},
                                  {     0, 0.8000, 1.0000},
                                  //{     0, 1.0000,      0},
                                  {0.4000, 1.0000,      0},
                                  //{0.8000, 1.0000,      0},
                                  {1.0000, 0.8000,      0},
                                  {1.0000, 0.4000,      0}};
    
//: Constructor

  class dbdet_temporal_map_V_bundle
  {
  public:
      dbdet_temporal_map_V_bundle(dbdet_temporal_map_tableau* tab) : temporal_tab(tab) {}

      void handle(const vgui_event & e)
      {
          //draw axes
          int vp[4]; // get viewport
          glGetIntegerv(GL_VIEWPORT,(GLint*)vp);

          float xmin, xmax, ymin, ymax;//, ymin2, ymax2;
          vgui_projection_inspector().window_to_image_coordinates(vp[0]+10, vp[3]-10, xmin, ymin);
          vgui_projection_inspector().window_to_image_coordinates(vp[0]+100, vp[3]-100, xmax, ymax);

          //draw the bars
          glLineWidth(1);
          glColor3f(0.0f,0.0f,0.0f);

          //bar 1
          glBegin(GL_LINES); glVertex2f(xmin, ymin); glVertex2f(xmin, ymax); glEnd();
          glBegin(GL_LINES); glVertex2f(xmax, ymin); glVertex2f(xmax, ymax); glEnd();
          glBegin(GL_LINES); glVertex2f(xmin, ymin); glVertex2f(xmax, ymin); glEnd();
          glBegin(GL_LINES); glVertex2f(xmin, ymax); glVertex2f(xmax, ymax); glEnd();
          glBegin(GL_LINES); glVertex2f((xmin+xmax)/2, ymin); glVertex2f((xmin+xmax)/2, ymax); glEnd();
          glBegin(GL_LINES); glVertex2f(xmin, (ymin+ymax)/2); glVertex2f(xmax, (ymin+ymax)/2); glEnd();


          if ((e.type == vgui_MOTION || e.type == vgui_BUTTON_DOWN) && e.button == vgui_LEFT)
          {
              x1.clear();
              y1.clear();

              if(temporal_tab->cur_edgel)
              {
                  vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter=temporal_tab->tmap_->map_[temporal_tab->cur_edgel->id]->st_options_.begin();
                  for(;iter!=temporal_tab->tmap_->map_[temporal_tab->cur_edgel->id]->st_options_.end();iter++)
                  {
                      vcl_list<dbdet_temporal_bundle>::iterator biter=iter->second->st_bundles_.begin();
                      for(;biter!=iter->second->st_bundles_.end();biter++)
                      {

                          if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*>(biter->model_))
                          {

                              if(m->iscomputed_){
                                  x1.clear();x2.clear();
                                  y1.clear();y2.clear();
                                  m->compute_2d_map();
                                  for(unsigned i=0;i<m->f1t.size();i++)
                                  {
                                      x1.push_back((xmax+xmin)/2+(xmax-xmin)/2*m->f1p[i]);
                                      y1.push_back((ymax+ymin)/2+(ymax-ymin)/2*m->f1t[i]);
                                  }
                                  for(unsigned i=0;i<m->f2t.size();i++)
                                  {
                                      x2.push_back((xmax+xmin)/2+(xmax-xmin)/2*m->f2p[i]);
                                      y2.push_back((ymax+ymin)/2+(ymax-ymin)/2*m->f2t[i]);
                                  }
                              } 
                          }
                      }
                  }
                  temporal_tab->post_overlay_redraw();
              }
          }
          if( e.type == vgui_OVERLAY_DRAW )
          {
              glLineWidth(2);
              glColor3f(1.0f,0.0f,0.0f);

              glBegin(GL_POINTS);
              for(unsigned i=0;i<x1.size();i++)
                  glVertex2f(x1[i],y1[i]);     
              glEnd();
              glLineWidth(2);
              glColor3f(0.0f,1.0f,0.0f);
              glBegin(GL_POINTS);

              for(unsigned i=0;i<x2.size();i++)
                  glVertex2f(x2[i],y2[i]);     
              glEnd();
          }
      }
      dbdet_temporal_map_tableau *temporal_tab;
      vcl_vector<float> x1;
      vcl_vector<float> y1;
      vcl_vector<float> x2;
      vcl_vector<float> y2;

  };
dbdet_temporal_map_tableau::dbdet_temporal_map_tableau(dbdet_temporal_map_storage_sptr tmap_storage): gesture0_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true))
{
    tmap_=tmap_storage->get_temporalmap();
    EM_=tmap_->emap_;
    frame_number_=0;
    thresh_groupings_=1;
    cur_edgel=0;
    kthresh_=0;

    draw_only_current_curvelets_=false;
    draw_only_current_temoral_bundles_=false;
    draw_only_max_temoral_bundles_=false;
    draw_if_seed_=false;
    draw_dynmaic_fragment_=false;
    draw_contour_fragment_=false;
    show_potential_seeds_=false;
    show_one_sided_bundles_=false;
    display_delaunay_=false;
    current_longest_hypothesis_=false;
    current_across_and_along_hypothesis_=false;
    display_delaunay_valid_=false;
    list_hypothesis_by_nweight_=false;
    draw_layers_=false;
    neighbor_relations_=false;
    show_model_per_triangle_=false;
    draw_regions_=false;
    delaunay_thresh_=-1;
    compute_error_=false;
    display_salient_regions_=false;
    display_V_bundle=false;
    Vx=1.0;
    Vy=0.0;
    V_z=0.0;

    draw_betas_=false;
    draw_backward_betas_=false;
    all_neighbor_frames_=false;
    bundle=new dbdet_temporal_map_V_bundle(this);

}

dbdet_temporal_map_tableau::~dbdet_temporal_map_tableau()
{
}

bool dbdet_temporal_map_tableau::handle( const vgui_event & e )
{
      if(display_V_bundle)
        bundle->handle(e);

  //apply local zoom
  //glTranslatef(-0.5, -0.5, 0);
  //glScalef(local_zoom_factor, local_zoom_factor, 1);
  //glTranslatef(0.5, 0.5, 0);


  //handle queries
  // Query 1: select an edgel
  if (gesture0_(e)) 
  {
    //draw_anchored_only_ = true;

    //float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // I) Find edgel closest to ix,iy
    cur_edgel = find_closest_edgel(ix, iy);

    //cur_link = 0; //deselect the current link
    
    // II) Display info about the selected edgel


    if (cur_edgel){

      print_edgel_stats(cur_edgel);
    
      if(list_hypothesis_by_nweight_)
          list_hypothesis_by_nweight(cur_edgel);
     if(neighbor_relations_)
          draw_neighbor_relations(cur_edgel);

     vcl_cout<< " Contour id :"<<tmap_->edge_to_one_chain[cur_edgel->id];
      post_overlay_redraw(); //for drawing the curvelets
    }
  }


  if( e.type == vgui_OVERLAY_DRAW ) {

      if(draw_only_current_curvelets_ && cur_edgel)
          draw_current_curvlets(cur_edgel);
      if(draw_only_max_temoral_bundles_ && cur_edgel)
          draw_max_temporal_bundles(cur_edgel,thresh_groupings_);
      else if(draw_only_current_temoral_bundles_ && cur_edgel)
          draw_current_temporal_bundles(cur_edgel,thresh_groupings_);
      else if(draw_if_seed_ && cur_edgel)
          draw_if_seed(cur_edgel);
      else if(current_longest_hypothesis_&& cur_edgel)
          draw_current_longest_hypothesis(cur_edgel);
      else if(current_across_and_along_hypothesis_&& cur_edgel)
          draw_current_across_and_along_hypothesis(cur_edgel);
      if(show_potential_seeds_)
          show_potential_seeds();

      
      if(display_V_bundle)
        bundle->handle(e);

      if(show_model_per_triangle_)
        print_velocity_per_triangle(ix,iy);

      get_region(ix,iy);


      if(draw_dynmaic_fragment_ && cur_edgel)
          if(tmap_->map_[cur_edgel->id]->st_options_.size()>0)
          {
              vcl_list<dbdet_temporal_bundle> * listbundles=& tmap_->map_[cur_edgel->id]->st_options_.begin()->second->list_options_;
              vcl_list<dbdet_temporal_bundle>::iterator listiter;
              int k=listbundles->size();
              if(k>0)
              {
                  vnl_random rand;

                  bool flag=false;
                  while(!flag){
                      unsigned randn=rand.lrand32(0,k-1);

                      listiter=listbundles->begin();
                      //for(unsigned t=0;t<randn && listiter!=listbundles.end();t++)
                      //    listiter++;
                      vcl_list<dbdet_temporal_bundle>::iterator founditer;
                      if(randn <listbundles->size())
                      {
                          std::advance(listiter, randn);
                          if(!listiter->used_ && listiter->bundle_.size()>1)
                          {
                              construct_contour_fragment(tmap_->map_[cur_edgel->id]->st_options_.begin()->first,&(*listiter));
                              draw_temporal_model((*listiter).model_,tmap_->map_[cur_edgel->id]->st_options_.begin()->first->ref_edgel,0,0,1);
                              flag=true;
                          }
                      }
                  }
              }
          }
 
  }
  if( e.type == vgui_DRAW ){
      draw_edges();
      if(draw_contour_fragment_)
          draw_contour_fragments();
      if(show_one_sided_bundles_)
          draw_one_sided_bundles();
      if(display_delaunay_)
          draw_delaunay();
      if(display_delaunay_valid_)
          draw_delaunay_valid();
      if(draw_layers_)
          draw_layer();
      if(draw_regions_)
          draw_regions();
      if(display_salient_regions_)
          display_salient_regions();
      if (frame_number_!=0)
          draw_neighbor_edges();

      if (compute_error_)
          tmap_->compute_motion_per_region(Vx,Vy,V_z);

      if(draw_betas_)
          draw_betas();
      if(draw_backward_betas_)
          draw_backward_betas();

      if(all_neighbor_frames_)
draw_all_neighbor_edges();
    return true;
  }

  return false;
}


void
dbdet_temporal_map_tableau::get_popup(const vgui_popup_params& /*params*/, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add("Display edges of neighboring frames", 
      new dbdet_temporal_map_tableau_set_int_command(this,"Frame number", &frame_number_));
    submenu.add( ((all_neighbor_frames_)?on:off)+"Display edges of ALL neighboring frames", 
      new dbdet_temporal_map_tableau_toggle_command(this, &all_neighbor_frames_));

  submenu.add("Set threshold for # of members in STCB", 
      new dbdet_temporal_map_tableau_set_thresh_grouping_command(this,"Thresh grouping", &thresh_groupings_));
  submenu.add( ((draw_only_current_curvelets_)?on:off)+"Draw Current Curvelets only", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_only_current_curvelets_));
  submenu.add( ((draw_only_current_temoral_bundles_)?on:off)+"Draw Current Temporal bundles only", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_only_current_temoral_bundles_));
  submenu.add( ((draw_only_max_temoral_bundles_)?on:off)+"Draw Max Temporal bundles only", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_only_max_temoral_bundles_));
  submenu.add( ((show_potential_seeds_)?on:off)+"Show Potential Seeds(high curvature) only", 
      new dbdet_temporal_map_tableau_toggle_command(this, &show_potential_seeds_));
  submenu.add( ((draw_if_seed_)?on:off)+"Draw if it is Seed(high curvature) only", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_if_seed_));
  submenu.add( ((draw_dynmaic_fragment_)?on:off)+"Draw a possible dynamic fragment", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_dynmaic_fragment_));
  submenu.add( ((show_one_sided_bundles_)?on:off)+"Show one sided bundles", 
      new dbdet_temporal_map_tableau_toggle_command(this, &show_one_sided_bundles_));
  submenu.add( ((show_model_per_triangle_)?on:off)+"Model/triangle", 
      new dbdet_temporal_map_tableau_toggle_command(this, &show_model_per_triangle_));
   submenu.add( ((draw_regions_)?on:off)+"Draw Regions", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_regions_));
   submenu.add( ((display_salient_regions_)?on:off)+"Draw Salient Regions", 
      new dbdet_temporal_map_tableau_toggle_command(this, &display_salient_regions_));

  submenu.add("Set threshold for K for potential seeds", 
      new dbdet_temporal_map_tableau_set_double_command(this,"Thresh grouping", &kthresh_));

  submenu.add("Set Vx", 
      new dbdet_temporal_map_tableau_set_double_command(this,"Vx", &Vx));
  submenu.add("Set Vy", 
      new dbdet_temporal_map_tableau_set_double_command(this,"Vy", &Vy));
  submenu.add("Set Vz", 
      new dbdet_temporal_map_tableau_set_double_command(this,"Vz", &V_z));
  submenu.add("Compute Error", 
      new dbdet_temporal_map_tableau_toggle_command(this, &compute_error_));


  submenu.add(((draw_contour_fragment_)?on:off)+"Draw Contour Hypothesis", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_contour_fragment_));
  submenu.add("Set Weight for Delaunay Edges", 
      new dbdet_temporal_map_tableau_set_double_command(this,"Thresh delaunay edges", &delaunay_thresh_));
  submenu.add(((display_delaunay_)?on:off)+"Draw Delauany", 
      new dbdet_temporal_map_tableau_toggle_command(this, &display_delaunay_));
    submenu.add(((display_delaunay_valid_)?on:off)+"Draw Delauany (Valid)", 
      new dbdet_temporal_map_tableau_toggle_command(this, &display_delaunay_valid_));
 // submenu.add(((current_longest_hypothesis_)?on:off)+"Current Longest Hypothesis", 
 //     new dbdet_temporal_map_tableau_toggle_command(this, &current_longest_hypothesis_));
 // submenu.add(((current_across_and_along_hypothesis_)?on:off)+"Current Along and Across Hypothesis", 
 //     new dbdet_temporal_map_tableau_toggle_command(this, &current_across_and_along_hypothesis_));
 // submenu.add(((neighbor_relations_)?on:off)+"Draw Neighborhood relations", 
 //     new dbdet_temporal_map_tableau_toggle_command(this, &neighbor_relations_));
 // 
 // submenu.add(((list_hypothesis_by_nweight_)?on:off)+"List Hypothesis and their weight", 
 //     new dbdet_temporal_map_tableau_toggle_command(this, &list_hypothesis_by_nweight_));
 //submenu.add(((draw_layers_)?on:off)+"Draw Layers", 
 //     new dbdet_temporal_map_tableau_toggle_command(this, &draw_layers_));

  submenu.add(((draw_betas_)?on:off)+"Draw Betas", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_betas_));
  submenu.add(((draw_backward_betas_)?on:off)+"Draw Backward Betas", 
      new dbdet_temporal_map_tableau_toggle_command(this, &draw_backward_betas_));
   submenu.add( ((display_V_bundle)?on:off)+"Display V Bundles", 
      new dbdet_temporal_map_tableau_toggle_command(this, &display_V_bundle));

  menu.add("Tableau Options", submenu);
}



void
dbdet_temporal_map_tableau::draw_edges()
{
    if(tmap_.ptr())
    {
        if(EM_.ptr())
        {
            glColor3f( 1.0 , 0.0 , 1.0 );
            glLineWidth(2.0);
            for(unsigned i=0;i<EM_->edgels.size();i++)
            {
                dbdet_edgel * cur_edgel=EM_->edgels[i]; 
                vgl_point_2d<double> p=cur_edgel->pt;
                glBegin( GL_LINE_STRIP );
                glVertex2d(cur_edgel->pt.x() - 0.25*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() - 0.25*vcl_sin(cur_edgel->tangent));

                glVertex2d(cur_edgel->pt.x() + 0.25*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() + 0.25*vcl_sin(cur_edgel->tangent));
                glEnd();
            }
            glColor3f( 0.0 , 0.0 , 1.0 );
            glBegin( GL_POINTS );
            for(unsigned i=0;i<EM_->edgels.size();i++)
            {
                dbdet_edgel * cur_edgel=EM_->edgels[i];             
                glVertex2d(cur_edgel->pt.x(),
                           cur_edgel->pt.y());

            }
                glEnd();
 

        }
    }
    else{
        vcl_cout<<"Temproal map is unavalaible \n";
    }
}
//: draw the neigboring edgemap one at a time .
void
dbdet_temporal_map_tableau::draw_neighbor_edges()
{

    if(tmap_.ptr())
    {
        if(tmap_->neighbor_emap_[frame_number_])       
        {
            dbdet_edgemap_sptr nmap=tmap_->neighbor_emap_[frame_number_];
            glColor3f( 0.0 , 1.0 , 1.0 );
            glLineWidth(1.0);
            for(unsigned i=0;i<nmap->edgels.size();i++)
            {
                dbdet_edgel * cur_edgel=nmap->edgels[i]; 
                vgl_point_2d<double> p=cur_edgel->pt;
                glBegin( GL_LINE_STRIP );
                glVertex2d(cur_edgel->pt.x() - 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() - 0.5*vcl_sin(cur_edgel->tangent));

                glVertex2d(cur_edgel->pt.x() + 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() + 0.5*vcl_sin(cur_edgel->tangent));
                glEnd();
            }
        }
    }
    else{
        vcl_cout<<"Neigboring Temproal map is unavalaible \n";
    }
}
//: draw the neigboring edgemap all  at the same time .
void
dbdet_temporal_map_tableau::draw_all_neighbor_edges()
{

    if(tmap_.ptr())
    {
        vcl_map<int, dbdet_edgemap_sptr>::iterator iter=tmap_->neighbor_emap_.begin();

        vnl_random rand;
        for(;iter!=tmap_->neighbor_emap_.end();iter++)       
        {
            dbdet_edgemap_sptr nmap=iter->second;
            glColor3f( rand.drand32(),rand.drand32(),rand.drand32());
            glLineWidth(1.0);
            for(unsigned i=0;i<nmap->edgels.size();i++)
            {
                dbdet_edgel * cur_edgel=nmap->edgels[i]; 
                vgl_point_2d<double> p=cur_edgel->pt;
                glBegin( GL_LINE_STRIP );
                glVertex2d(cur_edgel->pt.x() - 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() - 0.5*vcl_sin(cur_edgel->tangent));

                glVertex2d(cur_edgel->pt.x() + 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() + 0.5*vcl_sin(cur_edgel->tangent));
                glEnd();
            }
        }
    }
    else{
        vcl_cout<<"Neigboring Temproal map is unavalaible \n";
    }
}
dbdet_edgel* dbdet_temporal_map_tableau::find_closest_edgel(float ix, float iy)
{
  // I) Find edgel closest to ix,iy
  unsigned row_cell = (unsigned)iy;
  unsigned col_cell = (unsigned)ix;

  double dmin = vcl_numeric_limits<double>::infinity();
  unsigned jcell_min = 0, icell_min = 0;
  unsigned imin = 0;

  for (unsigned icell = row_cell-2; icell <= row_cell+2; ++icell)
  for (unsigned jcell = col_cell-2; jcell <= col_cell+2; ++jcell) 
  {
    if (icell < 0 || jcell < 0 || icell > EM_->nrows()-1 || jcell > EM_->ncols()-1)
        continue;

    for (unsigned i=0; i < EM_->edge_cells[icell][jcell].size(); ++i) {
      dbdet_edgel *edgel = EM_->edge_cells[icell][jcell][i];
      double dx =edgel->pt.x() - ix; 
      double dy =edgel->pt.y() - iy; 
      double d = dx*dx+dy*dy;
      if (d < dmin) {
        imin = i;
        dmin = d;
        jcell_min = jcell;
        icell_min = icell;
      }
    }
  }

  if (dmin == vcl_numeric_limits<double>::infinity())
    return 0;
  else
    return EM_->edge_cells[icell_min][jcell_min][imin];
}



void
dbdet_temporal_map_tableau::print_edgel_stats(dbdet_edgel * e)
{
  vcl_cout << "========= EDGEL =========\n";
  vcl_cout << "id: " << e->id;

    //first determine the max number of edgels before the ref edgel in all of the curvelets
    unsigned num = 0;
    //vcl_map<dbdet_curvelet*,dbdet_temporal_options* >::iterator cv_it = tmap_->map_[e->id]->st_options_.begin();
    //for ( ; cv_it!= tmap_->map_[e->id]->st_options_.end(); cv_it++){
    //  dbdet_curvelet* cvlet = cv_it->first;
    //  for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
    //    if (cvlet->edgel_chain[k]==e && k>num)
    //      num=k;
    //  }
    //}

    ////print the marker for the ref edge column
    //for (int i=0; i<=num; i++) vcl_cout << "\t";
    //vcl_cout << "***" << vcl_endl;

 

}

void dbdet_temporal_map_tableau::print_cvlet_info(dbdet_curvelet* cvlet)
{
  //pos refers to the position of the ref edgel
  if(dbdet_CC_curve_model * ccmodel1=dynamic_cast<dbdet_CC_curve_model *> (cvlet->curve_model))
      vcl_cout<<" Anchor Curvature: "<<ccmodel1->k;
    if(dbdet_CC_curve_model_3d * ccmodel1=dynamic_cast<dbdet_CC_curve_model_3d *> (cvlet->curve_model))
      vcl_cout<<" Anchor Curvature: "<<ccmodel1->k;

  vcl_cout << vcl_endl;
}

void dbdet_temporal_map_tableau::draw_curvelet(dbdet_curvelet* cvlet, float r, float g, float b)
{
  glColor3f( r, g, b );
  glLineWidth (2.0);
  //gl2psLineWidth(2.0);
  glBegin( GL_LINE_STRIP );
  for (unsigned j=0; j<cvlet->edgel_chain.size(); j++){
    glVertex2f(cvlet->edgel_chain[j]->pt.x(), cvlet->edgel_chain[j]->pt.y());
  }
  glEnd();

}
void dbdet_temporal_map_tableau::draw_edgel(dbdet_edgel* e, float r, float g, float b)
{
    glColor3f( r, g, b );
    glLineWidth (2.0);
    //gl2psLineWidth(2.0);
    glBegin( GL_LINES );
    glVertex2d(e->pt.x() - 0.5*vcl_cos(e->tangent),e->pt.y() - 0.5*vcl_sin(e->tangent));
    glVertex2d(e->pt.x() + 0.5*vcl_cos(e->tangent),e->pt.y() + 0.5*vcl_sin(e->tangent));
    glEnd();

}
void dbdet_temporal_map_tableau::draw_current_curvlets(dbdet_edgel * e)
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
    {
        vnl_random rand;
        draw_curvelet(cv_iter->first,rand.drand32(0.0,1.0),rand.drand32(0.0,1.0),rand.drand32(0.0,1.0));
    }
}
void dbdet_temporal_map_tableau::draw_current_temporal_bundles(dbdet_edgel * e, int thresh)
{
    const float col_pal[6][3] = {   {     0,      0,      0},
    {     0, 0.4000, 0.4000},
    {     0, 0.8000, 1.0000},
    //{     0, 1.0000,      0},
    {0.4000, 1.0000,      0},
    //{0.8000, 1.0000,      0},
    {1.0000, 0.8000,      0},
    {1.0000, 0.4000,      0}};
    int tri_id=point_inside(e->pt.x(),e->pt.y());
    int region_id_=-1;
    for(unsigned k=0;k<tmap_->derivatives_map_[e]->models_.size();k++)
    {
        tmap_->derivatives_map_[e]->models_[k].print_model();
        vcl_cout<<e->pt<<","<<e->tangent<<"\n";
        if(dbdet_second_order_velocity_model * m=dynamic_cast<dbdet_second_order_velocity_model * >(&(tmap_->derivatives_map_[e]->models_[k])))
        {
            //m->computeV(e)->print();
            double b=m->b_;
            double vmax=4.00;
            double amax=vcl_sqrt(vmax*vmax-b*b);
            for(double a=-amax;a<=amax;)
            {
                double alphas=m->alphas(a);
                double alphat=m->alphat(a);
                vcl_cout<<"Alpha ="<<a<<"\n";
                dbdet_curvelet * refc=tmap_->derivatives_map_[e]->refcs_[0];
                vcl_cout<<m->compute_V_by_rho(e,320,a)<<"\n";
                for(unsigned i=0;i<refc->edgel_chain.size();i++)
                {
                    dbdet_edgel * e1=refc->edgel_chain[i];
                    double ds=refc->compute_ds(i);
                    double ai=a+alphas*ds;
                    if(dbdet_second_order_velocity_model * m1=dynamic_cast<dbdet_second_order_velocity_model * >(&(tmap_->derivatives_map_[e1]->models_[0])))
                    {
                        vcl_cout<<m1->compute_V_by_rho(e1,320,ai)<<"\n";
                    }
                }
                int frame=bvis1_manager::instance()->repository()->current_frame();
                //: in different frame.
                for(int t=-3;t<=3;t++)
                {
                    if(t==0)
                     continue;
                        
                    double at=a+alphat*t;
                    double newx=e->pt.x()+(at*vcl_cos(e->tangent)-b*vcl_sin(e->tangent))*t;
                    double newy=e->pt.y()+(at*vcl_sin(e->tangent)+b*vcl_cos(e->tangent))*t;


                    dbdet_temporal_map_storage_sptr p;
                    p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("temporalmap",frame+t));

                    dbdet_temporal_map_sptr tmapt=p->get_temporalmap();

                    dbdet_edgel* et=compute_closes_edge(newx,newy,tmapt->emap_);

                    if(et)
                    {
                    dbdet_curvelet * refct=tmapt->derivatives_map_[et]->refcs_[0];
                
                    for(unsigned it=0;it<refct->edgel_chain.size();it++)
                    {
                        dbdet_edgel * e1t=refct->edgel_chain[it];
                        double ds=refc->compute_ds(it);
                        double ait=at+alphas*ds;
                        if(tmapt->derivatives_map_[e1t]->models_.size()>0)
                        if(dbdet_second_order_velocity_model * m1=dynamic_cast<dbdet_second_order_velocity_model * >(&(tmapt->derivatives_map_[e1t]->models_[0])))
                        {
                            vcl_cout<<m1->compute_V_by_rho(e1t,320,ait)<<"\n";
                        }
                    }
                    }
                        
                }
                  
                a+=0.1;
            }
            //if(region_id_!=-1)
            //    vcl_cout<<"Error "<<tmap_->compute_geometric_error(tmap_->regions_[region_id_],m->computeV(e))<<"\n";
        }
        vcl_cout<<"\n";
    }
    if(tri_id>=0)
    {
        for(unsigned i=0;i<tmap_->regions_.size();i++)
        {
            for(unsigned k=0;k<tmap_->regions_[i].triangles_.size();k++)
                if(tmap_->regions_[i].triangles_[k]==tri_id)
                {
                    vcl_map<int, vnl_vector<float> >::iterator iter;

                    tmap_->regions_[i].Vdist->print();
                    for(iter=tmap_->v_modes_.begin();iter!=tmap_->v_modes_.end();iter++)
                    {
                        //vcl_cout<<tmap_->computer_error_per_region(tmap_->regions_[i],iter->second[0],iter->second[1],iter->second[2]);
                        vcl_cout<<" ";
                    }vcl_cout<<"\n error ";
                    if(compute_error_)
                    {
                        vcl_vector<float> xs;
                        vcl_vector<float> ys;
                       //vcl_cout<<tmap_->computer_error_per_region(tmap_->regions_[i],Vx,Vy,V_z,xs,ys)<<"\n";
                        glBegin(GL_POINTS);
                        for(unsigned i=0;i<xs.size();i++)
                        {
                            glVertex2f(xs[i],ys[i]);
                        }
                        glEnd();

                    }
                }                                                
        }                                                        
    }

    vcl_cout<<"\n";
}





bool compare_temporal_bundle_weight(dbdet_temporal_bundle  b1,dbdet_temporal_bundle  b2)
{
    if (b1.weight_>b2.weight_)
        return true;
    else 
        return false;
}
void dbdet_temporal_map_tableau::draw_max_temporal_bundles(dbdet_edgel * e, int thresh)
{

    
      vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
      for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
      {

          if(cv_iter->second->list_options_.size()>0)
          {
              vcl_list<dbdet_temporal_bundle>::iterator list_iter;

              print_cvlet_info(cv_iter->first);
              //: iterating through all the bundles
              double max_weight=0;
              vcl_list<dbdet_temporal_bundle>::iterator max_list_iter=cv_iter->second->list_options_.end();
              for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
              {
                  vcl_map<int,dbdet_curvelet * > b=list_iter->bundle_;
                  if(b.size()>thresh)
                  {
                      
                      if(list_iter->weight_>max_weight)
                      {
                          max_list_iter=list_iter;
                          max_weight=list_iter->weight_;
                      }
                  }
              }
         
          if(max_list_iter!=cv_iter->second->list_options_.end())
          {
              max_list_iter->model_->print_model();
              draw_temporal_model(max_list_iter->model_,cv_iter->first->ref_edgel,0,0,1);
              //: iterating over edgels from different frames
              vcl_map<int, dbdet_curvelet* >::iterator biter;
              vcl_cout<<"Curvatures: ";
              for (biter=max_list_iter->bundle_.begin();biter!=max_list_iter->bundle_.end();biter++)
              {
                  draw_curvelet(biter->second,col_pal[biter->first+2][0],col_pal[biter->first+2][1],col_pal[biter->first+2][2]);
                  if(dbdet_CC_curve_model * ccmodel1=dynamic_cast<dbdet_CC_curve_model *> (biter->second->curve_model))
                      vcl_cout<<ccmodel1->k;
                  if(dbdet_CC_curve_model_3d * ccmodel1=dynamic_cast<dbdet_CC_curve_model_3d *> (biter->second->curve_model))
                      vcl_cout<<ccmodel1->k;

                  vcl_cout<<"\n Weight = "<<max_list_iter->weight_<<"\n";
              }
              vcl_cout<<"\n";
          }
          }

      }
}


void dbdet_temporal_map_tableau::draw_temporal_model(dbdet_temporal_model * m, dbdet_edgel * refc, double r, double g, double b )
{
    if(dbdet_temporal_curvature_velocity_model * cm=dynamic_cast<dbdet_temporal_curvature_velocity_model*>(m))
    {
        glColor3f( r, g, b );
        glLineWidth (2.0);
        glBegin( GL_LINE_STRIP );
        for(int t=-2;t<=2;t++)
            glVertex2d(refc->pt.x()+cm->meanV()[0]*t,refc->pt.y()+cm->meanV()[1]*t);
        glEnd();
    }
    if(dbdet_temporal_normal_velocity_model * cm=dynamic_cast<dbdet_temporal_normal_velocity_model*>(m))
    {
        vnl_random rand;
        rand.reseed(100);
        glColor3f( rand.drand32(0,1), rand.drand32(0,1), rand.drand32(0,1) );
        glPointSize (4.0);

        double x0=refc->pt.x();
        double y0=refc->pt.y();
        double theta0=refc->tangent;

        double nx=-vcl_sin(theta0);
        double ny=vcl_cos(theta0);

        double tx=vcl_cos(theta0);
        double ty=vcl_sin(theta0);

        glColor3f( 1, 0, 0 );
        glBegin( GL_LINE_STRIP );
        glVertex2d(x0,y0);
        glVertex2d(x0+cm->w1*tx+cm->b0*nx,y0+cm->w1*ty+cm->b0*ny);
        glEnd();
        glColor3f( 0, 1, 0 );
        glBegin( GL_LINE_STRIP );
        glVertex2d(x0,y0);
        glVertex2d(x0+cm->w2*tx+cm->b0*nx,y0+cm->w2*ty+cm->b0*ny);        
        glEnd();


            
    }
}





void dbdet_temporal_map_tableau::show_potential_seeds()
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    for(unsigned i=0;i<EM_->edgels.size();i++)
    {
        dbdet_edgel * e=EM_->edgels[i]; 
        for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
        {
            if(dbdet_CC_curve_model * ccmodel1=dynamic_cast<dbdet_CC_curve_model *> (cv_iter->first->curve_model))
            {
                if(ccmodel1->k>kthresh_ || ccmodel1->k<-kthresh_)
                {
                    draw_curvelet(cv_iter->first,0,0,0);
                }
            }
            if(dbdet_CC_curve_model_3d * ccmodel1=dynamic_cast<dbdet_CC_curve_model_3d *> (cv_iter->first->curve_model))
            {
                if(ccmodel1->k>kthresh_ || ccmodel1->k<-kthresh_)
                    draw_curvelet(cv_iter->first,0,0,0);
            }
        }
    }

    //: also draw the path infulenced by the seeds


}



void dbdet_temporal_map_tableau::draw_if_seed(dbdet_edgel * e)
{
        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
        for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
        {
                    draw_curvelet(cv_iter->first,0,0,0);
                    vcl_vector<int> path=tmap_->find_path(cv_iter->first);
                    for(unsigned k=0;k<path.size();k++)
                        draw_edgel(tmap_->emap_->edgels[path[k]],0,0,1);
        }
    

}




//: draw the contour fragments
void dbdet_temporal_map_tableau::draw_contour_fragments()
{
    vnl_random rand;
    for(unsigned i=0;i<tmap_->cf_list.size();i++)
    {
        double r=rand.drand32(0,1);
        double g=rand.drand32(0,1);
        double b=rand.drand32(0,1);
        draw_temporal_model(tmap_->cf_list[i].tb_->model_,tmap_->cf_list[i].tb_->refc->ref_edgel,r,g,b);

        vcl_list<dbdet_temporal_bundle *>::iterator iterlist;
        glColor3f(r,g,b);
        double offset=rand.drand32(0,0.2);
        glBegin(GL_LINE_STRIP);
        for(iterlist=tmap_->cf_list[i].chain_.begin();iterlist!=tmap_->cf_list[i].chain_.end();iterlist++)
           glVertex2d((*iterlist)->refc->ref_edgel->pt.x()+offset,(*iterlist)->refc->ref_edgel->pt.y()+offset);          
        glEnd();
    }
}
void dbdet_temporal_map_tableau::construct_contour_fragment(dbdet_curvelet * refc, dbdet_temporal_bundle  * b)
{
    int id=refc->ref_edgel->id;
    double err_epsilon=0.35; 
    if(dbdet_temporal_curvature_velocity_model * tmodel=dynamic_cast<dbdet_temporal_curvature_velocity_model *> (b->model_))
    {   
        tmodel->print_model();
        bool flag=true;

        int curr_id=id;
        while(flag)
        {
            dbdet_link_list clinks=tmap_->elg_.cLinks[curr_id];
            if(clinks.size()>0 && clinks.size()<=1)
            {
                int edgeid=(*clinks.begin())->ce->id;
                vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter_hyp;
                bool anymatch=false;

                for(iter_hyp=tmap_->map_[edgeid]->st_options_.begin();iter_hyp!=tmap_->map_[edgeid]->st_options_.end();iter_hyp++)
                {    
                    vcl_list<dbdet_temporal_bundle>::iterator biter;
                    for(biter=iter_hyp->second->list_options_.begin();biter!=iter_hyp->second->list_options_.end();biter++)
                    {
                        if(dbdet_temporal_curvature_velocity_model * tmodel1
                            =dynamic_cast<dbdet_temporal_curvature_velocity_model *> (biter->model_))
                        {   
                            if(tmodel->is_model_intersect(tmodel1, err_epsilon) && !biter->used_ && biter->bundle_.size()>1)
                            {
                                anymatch=true;
                                //: draw the edges beloging to the dynamic curve fragment
                                vcl_map<int, dbdet_curvelet*>::iterator belements;
                                draw_edgel(iter_hyp->first->ref_edgel,col_pal[2][0],col_pal[2][1],col_pal[2][2]);

                                for(belements=biter->bundle_.begin();belements!=biter->bundle_.end();belements++)
                                {
                                    int framenum=belements->first;
                                    draw_edgel(belements->second->ref_edgel,col_pal[framenum+2][0],col_pal[framenum+2][1],col_pal[framenum+2][2]);
                                }

                            }
                        }
                    }
                }
                if(!anymatch)
                    flag=false;

                curr_id=edgeid;
            }
            else
                flag=false;

        }
         flag=true;

         curr_id=id;
        while(flag)
        {

            dbdet_link_list plinks=tmap_->elg_.pLinks[curr_id];
            if(plinks.size()>0 && plinks.size()<=1)
            {
                int edgeid=(*plinks.begin())->pe->id;
                vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter_hyp;
                                    bool anymatch=false;

                for(iter_hyp=tmap_->map_[edgeid]->st_options_.begin();iter_hyp!=tmap_->map_[edgeid]->st_options_.end();iter_hyp++)
                {    
                    vcl_list<dbdet_temporal_bundle>::iterator biter;
                    for(biter=iter_hyp->second->list_options_.begin();biter!=iter_hyp->second->list_options_.end();biter++)
                    {
                        if(dbdet_temporal_curvature_velocity_model * tmodel1
                            =dynamic_cast<dbdet_temporal_curvature_velocity_model *> (biter->model_))
                        {   
                            if(tmodel->is_model_intersect(tmodel1, err_epsilon) && !biter->used_ && biter->bundle_.size()>1)
                            {
                                anymatch=true;
                                //: draw the edges beloging to the dynamic curve fragment
                                vcl_map<int, dbdet_curvelet*>::iterator belements;
                                draw_edgel(iter_hyp->first->ref_edgel,col_pal[2][0],col_pal[2][1],col_pal[2][2]);
                                for(belements=biter->bundle_.begin();belements!=biter->bundle_.end();belements++)
                                {
                                    int framenum=belements->first;
                                    draw_edgel(belements->second->ref_edgel,col_pal[framenum+2][0],col_pal[framenum+2][1],col_pal[framenum+2][2]);
                                }

                            }
                        }
                    }
                }
                if(!anymatch)
                    flag=false;

                curr_id=edgeid;
            }
            else
                flag=false;

        }
    }


}


void dbdet_temporal_map_tableau::draw_one_sided_bundles()
{

    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    for(unsigned i=0;i<tmap_->map_.size();i++)
    {
        for(cv_iter=tmap_->map_[i]->st_options_.begin();cv_iter!=tmap_->map_[i]->st_options_.end();cv_iter++)
        {
                        vcl_list<dbdet_temporal_bundle>::iterator list_iter;
            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
            {   
                if(!list_iter->used_ && list_iter->bundle_.size()>1 && list_iter->one_sided_)
                {
                    draw_temporal_model(list_iter->model_,cv_iter->first->ref_edgel,0,0,1);
                }
            }
        }
    }

}

void dbdet_temporal_map_tableau::draw_layer()
{
    
if(tmap_.ptr() && tmap_->label_map_.size()>0)
    {
        if(EM_.ptr())
        {
            glColor3f( 1.0 , 0.0 , 1.0 );
            glLineWidth(1.0);
            for(unsigned i=0;i<EM_->edgels.size();i++)
            {
                if(tmap_->label_map_[i]>=0)
                    glColor3f( col_pal[ tmap_->label_map_[i]][0], col_pal[ tmap_->label_map_[i]][1] ,col_pal[ tmap_->label_map_[i]][2]);
                dbdet_edgel * cur_edgel=EM_->edgels[i]; 
                vgl_point_2d<double> p=cur_edgel->pt;
                glBegin( GL_LINE_STRIP );
                glVertex2d(cur_edgel->pt.x() - 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() - 0.5*vcl_sin(cur_edgel->tangent));

                glVertex2d(cur_edgel->pt.x() + 0.5*vcl_cos(cur_edgel->tangent),
                           cur_edgel->pt.y() + 0.5*vcl_sin(cur_edgel->tangent));
                glEnd();
            }
        }
    }
    else{
        vcl_cout<<"Temproal map is unavalaible \n";
    }
}
void dbdet_temporal_map_tableau::draw_delaunay()
{
    
    for(unsigned i=0;i<tmap_->dt_->triangles_.size();i++)
    {
        int id1=tmap_->dt_->triangles_[i].node1_id_;
        int id2=tmap_->dt_->triangles_[i].node2_id_;
        int id3=tmap_->dt_->triangles_[i].node3_id_;

        glLineWidth(2.0);
        glBegin(GL_TRIANGLES );
        glVertex2f(tmap_->emap_->edgels[id1]->pt.x(),tmap_->emap_->edgels[id1]->pt.y());
        glVertex2f(tmap_->emap_->edgels[id2]->pt.x(),tmap_->emap_->edgels[id2]->pt.y());
        glVertex2f(tmap_->emap_->edgels[id3]->pt.x(),tmap_->emap_->edgels[id3]->pt.y());

        glEnd();
        
    }
}
void dbdet_temporal_map_tableau::draw_delaunay_valid()
{
 for(unsigned i=0;i<tmap_->emap_->edgels.size();i++)
    {
        vcl_vector<int> ids=tmap_->valid_neighbor_map_[i];
        glColor3f(1,1,0);
        for(unsigned j=0;j<ids.size();j++)
        {
            glBegin(GL_LINES);
            glVertex2f(tmap_->emap_->edgels[i]->pt.x(),tmap_->emap_->edgels[i]->pt.y());
            glVertex2f(tmap_->emap_->edgels[ids[j]]->pt.x(),tmap_->emap_->edgels[ids[j]]->pt.y());
            glEnd();
        }
    }
}

void dbdet_temporal_map_tableau::draw_current_longest_hypothesis(dbdet_edgel * e)
{
    int max_length_index=-1;
    int max_length=0;
    for(unsigned i=0;i<tmap_->cf_list.size();i++)
    {
        vcl_vector<int>::iterator iter=vcl_find(tmap_->cf_list[i].memids.begin(),tmap_->cf_list[i].memids.end(),e->id);
        if(iter!=tmap_->cf_list[i].memids.end())
        {
           if( tmap_->cf_list[i].memids.size()>max_length)
           {
               max_length=tmap_->cf_list[i].memids.size();
               max_length_index=i;
           }
        }
    }


    if(max_length_index>0)
    {
        vnl_random rand;
        double r=rand.drand32(0,1);
        double g=rand.drand32(0,1);
        double b=rand.drand32(0,1);
        draw_temporal_model(tmap_->cf_list[max_length_index].tb_->model_,tmap_->cf_list[max_length_index].tb_->refc->ref_edgel,r,g,b);

        vcl_list<dbdet_temporal_bundle *>::iterator iterlist;
        glColor3f(r,g,b);
        double offset=rand.drand32(0,0.2);
        glBegin(GL_LINE_STRIP);
        for(iterlist=tmap_->cf_list[max_length_index].chain_.begin();iterlist!=tmap_->cf_list[max_length_index].chain_.end();iterlist++)
            glVertex2d((*iterlist)->refc->ref_edgel->pt.x()+offset,(*iterlist)->refc->ref_edgel->pt.y()+offset);          
        glEnd();
    }
}


void dbdet_temporal_map_tableau::draw_current_across_and_along_hypothesis(dbdet_edgel * e)
{
    int max_length_index=-1;
    int max_length=0;
    vnl_random rand;

    for(unsigned i=0;i<tmap_->cf_list.size();i++)
    {
        //: found a segment for current edge and display all hypotheses.
        vcl_vector<int>::iterator iter=vcl_find(tmap_->cf_list[i].memids.begin(),tmap_->cf_list[i].memids.end(),e->id);
        if(iter!=tmap_->cf_list[i].memids.end())
        {
            double r=rand.drand32(0,1);
            double g=rand.drand32(0,1);
            double b=rand.drand32(0,1);
            double offset=rand.drand32(0,0.2);
            glColor3f(r,g,b);
            vcl_vector<int>::iterator iterlist;
            //: find the segements for the memberid.
            bool flag=false;
            for(unsigned j=0;j<tmap_->cf_list[i].memids.size();j++)
            {
                //: get the delaunay neighbors
                vcl_vector<int> nids=tmap_->dt_->neighbor_map_[tmap_->cf_list[i].memids[j]];
                for(unsigned k=0;k<nids.size();k++)
                {
                    for(unsigned l=0;l<tmap_->cf_list.size();l++)
                    {
                        if(i!=l)
                        {
                            vcl_vector<int>::iterator sub_iter=vcl_find(tmap_->cf_list[l].memids.begin(),tmap_->cf_list[l].memids.end(),nids[k]);
                            if(sub_iter!=tmap_->cf_list[l].memids.end() && tmap_->bundles_intersect(*tmap_->cf_list[l].tb_,*tmap_->cf_list[i].tb_ ) )
                            {
                                if(tmap_->cf_list[l].memids.size()>1)
                                {
                                    flag =true;
                                    glBegin(GL_LINE_STRIP);
                                    for(iterlist=tmap_->cf_list[l].memids.begin();iterlist!=tmap_->cf_list[l].memids.end();iterlist++)
                                        glVertex2d(tmap_->emap_->edgels[*iterlist]->pt.x()+offset,tmap_->emap_->edgels[*iterlist]->pt.y()+offset);          
                                    glEnd();
                                }
                            }
                        }
                    }
                }
            }
            if(flag)
            {
                draw_temporal_model(tmap_->cf_list[i].tb_->model_,tmap_->cf_list[i].tb_->refc->ref_edgel,r,g,b);
                glBegin(GL_LINE_STRIP);
                for(iterlist=tmap_->cf_list[i].memids.begin();iterlist!=tmap_->cf_list[i].memids.end();iterlist++)
                    glVertex2d(tmap_->emap_->edgels[*iterlist]->pt.x()+offset,tmap_->emap_->edgels[*iterlist]->pt.y()+offset);          
                glEnd();
            }
            else
            {
                draw_temporal_model(tmap_->cf_list[i].tb_->model_,tmap_->cf_list[i].tb_->refc->ref_edgel,0,0,0);
                glColor3f(0,0,0);
                glBegin(GL_LINE_STRIP);
                for(iterlist=tmap_->cf_list[i].memids.begin();iterlist!=tmap_->cf_list[i].memids.end();iterlist++)
                    glVertex2d(tmap_->emap_->edgels[*iterlist]->pt.x()+offset,tmap_->emap_->edgels[*iterlist]->pt.y()+offset);          
                glEnd();
            }
        }
    }
}
void dbdet_temporal_map_tableau::list_hypothesis_by_nweight(dbdet_edgel * e)
{

    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
        cv_iter->second->print();
}

void dbdet_temporal_map_tableau::draw_neighbor_relations(dbdet_edgel *e)
{
 if(tmap_->dt_)
 {
     vcl_vector<int> nids=tmap_->dt_->get_neighbors(e->id);
     vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
     vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter2;
     for(cv_iter=tmap_->map_[e->id]->st_options_.begin();cv_iter!=tmap_->map_[e->id]->st_options_.end();cv_iter++)
     {
         vcl_list<dbdet_temporal_bundle>::iterator list_iter;
         vcl_list<dbdet_temporal_bundle>::iterator list_iter2;
         print_cvlet_info(cv_iter->first);
         //: iterating through all the bundles
         for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
         {
             int count_members=0;
             vcl_map<int,dbdet_curvelet * > b=list_iter->bundle_;
             if(!list_iter->used_ && list_iter->weight_>0.05)
             {
                 list_iter->model_->print_model();
                 draw_temporal_model(list_iter->model_,cv_iter->first->ref_edgel,0,0,1);
                 //: iterating over edgels from different frames
                 for(unsigned i=0;i<nids.size();i++)
                 {
                     for(cv_iter2=tmap_->map_[nids[i]]->st_options_.begin();cv_iter2!=tmap_->map_[nids[i]]->st_options_.end();cv_iter2++)
                     {
                         for(list_iter2=cv_iter2->second->list_options_.begin();list_iter2!=cv_iter2->second->list_options_.end();list_iter2++)
                         {
                             if(!list_iter2->used_)
                             {
                                 
                                 if(tmap_->bundles_intersect( *list_iter, *list_iter2))
                                 {
                                    list_iter2->model_->print_model();
                                    vcl_cout<<"YES \n";
                                 }
                             }
                         }
                     }
                 }
                 vcl_cout<<" Weight = "<<list_iter->weight_<<"\n";
             }
         }
     }
 }
}
void
dbdet_temporal_map_tableau::print_velocity_per_triangle(float ix, float iy)
{
    //int tri_id=point_inside(ix,iy);
    //if(tri_id<0)
    //{
    //    vcl_cout<<"\n not locatd in any of triangles";return;
    //}
    //vcl_list<dbdet_3D_velocity_model>::iterator model_iter;
    //for(model_iter=tmap_->motion_triangle_[tri_id].begin();
    //     model_iter!=tmap_->motion_triangle_[tri_id].end();model_iter++)
    //{
    //    vcl_cout<<model_iter->theta<<","<<model_iter->phi<<"\t";

    //    if(model_iter==tmap_->motion_triangle_[tri_id].begin())
    //    {
    //        vcl_cout<<"("<<model_iter->phi<<","<<model_iter->theta<<")\n";
    //        draw_edges_agreed_with_model(*model_iter);
    //    }

    //}
    //vcl_cout<<"\n";

}

//: returns the triangle id in which the point lies
int dbdet_temporal_map_tableau::point_inside(float ix, float iy)
{
    int tri_id=-1;
    for(unsigned i=0;i<tmap_->dt_->triangles_.size();i++)
    {
        dbdet_delaunay_triangle t=   tmap_->dt_->triangles_[i];
        
        if( vgl_triangle_test_inside<float>(tmap_->emap_->edgels[t.node1_id_]->pt.x(),
                                           tmap_->emap_->edgels[t.node1_id_]->pt.y(),
                                           tmap_->emap_->edgels[t.node2_id_]->pt.x(),
                                           tmap_->emap_->edgels[t.node2_id_]->pt.y(),
                                           tmap_->emap_->edgels[t.node3_id_]->pt.x(),
                                           tmap_->emap_->edgels[t.node3_id_]->pt.y(),ix,iy))
        {
            i=tmap_->dt_->triangles_.size();
            tri_id=t.id_;
        }

    }
    return tri_id;
}
void
dbdet_temporal_map_tableau::draw_edges_agreed_with_model(dbdet_3D_velocity_model &c)
{
    for(unsigned i=0;i<tmap_->map_.size();i++)
    {
        glColor3f( 1.0 , 0.0 , 0.0 );
        glPointSize(5.0);

        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter_hyp;
        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator link_iter_hyp;
        vcl_list<dbdet_temporal_bundle> ::iterator iter_options;
        vcl_map<int,vcl_list<dbdet_temporal_bundle> >::iterator link_iter_options;

        for(iter_hyp=tmap_->map_[i]->st_options_.begin();iter_hyp!=tmap_->map_[i]->st_options_.end();iter_hyp++)
        {    
            vcl_list<dbdet_temporal_bundle>::iterator biter;
            //: any arbitrary starting edge
            for(biter=iter_hyp->second->st_bundles_.begin();biter!=iter_hyp->second->st_bundles_.end();biter++)
            {
                float x1, y1;
                tmap_->confirm_3D_model(c,(*biter),x1,y1);
                {
                    glBegin(GL_LINES);

                    glVertex2d(biter->refc->ref_edgel->pt.x(),biter->refc->ref_edgel->pt.y());
                    glVertex2d(x1,y1);
                    glEnd();                   

                }
            }
        }

    }
}

void
dbdet_temporal_map_tableau::draw_regions()
{
    vnl_random ran;
    const float col_pal[6][3] = {   {     0,      0,      0},
                                    {     0, 0.4000, 0.4000},
                                    {     0, 0.8000, 1.0000},
                                    {0.4000, 1.0000,      0},
                                    {1.0000, 0.8000,      0},
                                    {1.0000, 0.4000,      0}};

    for(unsigned i=0;i<tmap_->regions_.size();i++)
    {
        
        int id= i%6;
        glColor3f(col_pal[id][0],col_pal[id][1],col_pal[id][2]);
        
        glPointSize(5.0);
        glBegin(GL_TRIANGLES);  
        for( unsigned k=0;k<tmap_->regions_[i].triangles_.size();k++)
        {
            int id=tmap_->regions_[i].triangles_[k];
            dbdet_delaunay_triangle t=tmap_->dt_->triangles_[id];

            glVertex2d(tmap_->emap_->edgels[t.node1_id_]->pt.x(),tmap_->emap_->edgels[t.node1_id_]->pt.y());
            glVertex2d(tmap_->emap_->edgels[t.node2_id_]->pt.x(),tmap_->emap_->edgels[t.node2_id_]->pt.y());
            glVertex2d(tmap_->emap_->edgels[t.node3_id_]->pt.x(),tmap_->emap_->edgels[t.node3_id_]->pt.y());

        }
        glEnd();
    }        

}
void dbdet_temporal_map_tableau::get_region(float ix, float iy)
{
    int tri_id=point_inside(ix,iy);
    if(tri_id>=0)
    {
        for(unsigned i=0;i<tmap_->regions_.size();i++)
        {
            for(unsigned k=0;k<tmap_->regions_[i].triangles_.size();k++)
                if(tmap_->regions_[i].triangles_[k]==tri_id)
                {
                    vcl_vector<double> xs; 
                    vcl_vector<double> ys;
                    vcl_cout<<"\n";
                    vcl_cout<<tmap_->regions_[i].Vdist;
//                   vcl_cout<<"\nError "<<tmap_->compute_probabilistic_error(tmap_->regions_[i],Vx,Vy,V_z);
//  
//                    //: see which w gives least error
//                    vcl_map<int,int>::iterator iter_option=tmap_->regions_[i].ids_ws.begin();
//                    vcl_map<int,dbdet_temporal_model*>::iterator iter_model=tmap_->regions_[i].ids_model.begin();
//                    for(;iter_option!=tmap_->regions_[i].ids_ws.end();iter_option++,iter_model++)
//                    {
//                            if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*>(iter_model->second))
//                            {
//                                        double x0=m->ref_curvelet->ref_edgel->pt.x();
//                                        double y0=m->ref_curvelet->ref_edgel->pt.y();
//                                        double theta0=m->ref_curvelet->ref_edgel->tangent;
//
//                                        double nx=-vcl_sin(theta0);
//                                        double ny=vcl_cos(theta0);
//
//                                        double tx=vcl_cos(theta0);
//                                        double ty=vcl_sin(theta0);
//                                        
//                                        glColor3f( 1, 0, 0 );
//                                        glBegin( GL_LINE_STRIP );
//                                        glVertex2d(x0,y0);
//                                        if(iter_option->second==1)
//                                            glVertex2d(x0+m->w1*tx+m->b0*nx,y0+m->w1*ty+m->b0*ny);
//                                        else if(iter_option->second==2)
//                                            glVertex2d(x0+m->w2*tx+m->b0*nx,y0+m->w2*ty+m->b0*ny);        
//                                        glEnd();
//                            }
//                    }
//                    vcl_cout<<"v1 ="<<tmap_->regions_[i].v1<<" v2="<<tmap_->regions_[i].v2<<"\n";
//                    vcl_cout<<"("<<tmap_->regions_[i].l1<<","<<tmap_->regions_[i].l2<<","<<tmap_->regions_[i].l3<<")\n";
//                    vcl_cout<<tmap_->regions_[i].v1v1<<","<<tmap_->regions_[i].v1v2<<","<<tmap_->regions_[i].v2v2<<"\n";
//
//                    dbdet_region r=tmap_->regions_[i];
///*                    for(float phi=r.phi_min;phi<=r.phi_max;)
//                    {
//                                vnl_vector<float> vmean=r.v1*vcl_cos(phi)+r.v2*vcl_sin(phi);
//
//                        vnl_matrix<float> cov(3,3);
//                        cov=vcl_cos(phi)*vcl_cos(phi)*r.v1v1+
//                            2*vcl_cos(phi)*vcl_sin(phi)*r.v1v2+
//                            vcl_sin(phi)*vcl_sin(phi)*r.v2v2;
//
//                        vnl_vector<float> lambdas(3);
//                        vnl_matrix<float> Vs(3,3);
//
//                        if(!vnl_symmetric_eigensystem_compute(cov,Vs,lambdas))
//                            return ;
//
//                        vcl_cout<<"phi ="<<phi<<" mean ="<<vmean<<" "<<Vs<<lambdas;
//                        phi+=0.2;
//                    }
//  */                  vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter
//                        =tmap_->map_[cur_edgel->id]->st_options_.begin();
//                    for(;iter!=tmap_->map_[cur_edgel->id]->st_options_.end();iter++)
//                    {
//                        vcl_list<dbdet_temporal_bundle>::iterator biter=iter->second->st_bundles_.begin();
//                        for(;biter!=iter->second->st_bundles_.end();biter++)
//                        {
//                            if(dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*>(biter->model_))
//                            {
//                                if(m->iscomputed_){
//                                    vcl_cout<<"\n the error is "<<tmap_->compute_geometric_error_for_family(tmap_->regions_[i],m->v1_1,m->v1_2,xs,ys,m->phi1_min,m->phi1_max)<<" ";
//
//                                    glColor3f( 1, 0, 0 );
//                                    glBegin(GL_POINTS);
//                                    for(unsigned l=0;l<xs.size();l++)
//                                        glVertex2d(xs[l],ys[l]);
//                                    glEnd();
//                                    xs.clear();ys.clear();
//                                    vcl_cout<<tmap_->compute_geometric_error_for_family(tmap_->regions_[i],m->v2_1,m->v2_2,xs,ys,m->phi2_min,m->phi2_max)<<"\n";
//                                    glColor3f( 0, 1, 0 );
//                                    glBegin(GL_POINTS);
//                                    for(unsigned l=0;l<xs.size();l++)
//                                        glVertex2d(xs[l],ys[l]);
//                                    glEnd();
//
//                                } 
//                            }
//                        }
//                    }
                }
        }
    }

}





void dbdet_temporal_map_tableau::display_salient_regions()
{
    vcl_set<int>::iterator iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_list<dbdet_temporal_bundle>::iterator list_iter;

    for(unsigned i=0;i<tmap_->regions_.size();i++)
    {
        for(iter=tmap_->regions_[i].node_ids_.begin();iter!=tmap_->regions_[i].node_ids_.end();iter++)
        {
            for(cv_iter=tmap_->map_[*iter]->st_options_.begin();cv_iter!=tmap_->map_[*iter]->st_options_.end();cv_iter++)
            {
                if(dbdet_CC_curve_model_3d * ccmodel=dynamic_cast<dbdet_CC_curve_model_3d *> (cv_iter->first->curve_model))
                {
                    if(vcl_fabs(ccmodel->k)>kthresh_)
                    {
                        for(list_iter=cv_iter->second->st_bundles_.begin();list_iter!=cv_iter->second->st_bundles_.end();list_iter++)
                        {
                            if(list_iter->model_->isvalid())
                            {
                                glColor3f(0,0,0 );
                                glPointSize(5.0);
                                glBegin(GL_TRIANGLES);  
                                for( unsigned k=0;k<tmap_->regions_[i].triangles_.size();k++)
                                {
                                    int id=tmap_->regions_[i].triangles_[k];
                                    dbdet_delaunay_triangle t=tmap_->dt_->triangles_[id];

                                    glVertex2d(tmap_->emap_->edgels[t.node1_id_]->pt.x(),tmap_->emap_->edgels[t.node1_id_]->pt.y());
                                    glVertex2d(tmap_->emap_->edgels[t.node2_id_]->pt.x(),tmap_->emap_->edgels[t.node2_id_]->pt.y());
                                    glVertex2d(tmap_->emap_->edgels[t.node3_id_]->pt.x(),tmap_->emap_->edgels[t.node3_id_]->pt.y());

                                }
                                glEnd();
                            }
                        }
                    }
                }
            }
        }
    }
}


void dbdet_temporal_map_tableau::draw_betas()
{

    vcl_map<dbdet_edgel*,vcl_vector<float> >::iterator iter;
    for(iter=tmap_->beta_edge_map.begin();iter!=tmap_->beta_edge_map.end();iter++)
    {
        double x=iter->first->pt.x();
        double y=iter->first->pt.y();
        double theta=iter->first->tangent;

        double nx=-vcl_sin(theta);
        double ny=vcl_cos(theta);

        for(unsigned i=0;i<iter->second.size();i++)
        {
            double b0=iter->second[i];
        glColor3f( 0, 1, 0 );
        glBegin( GL_LINES );
        glVertex2d(x,y);
        glVertex2d(x+b0*nx,y+b0*ny);
        glEnd();
        }

    }   
}

void dbdet_temporal_map_tableau::draw_backward_betas()
{

    vcl_map<dbdet_edgel*,vcl_vector<float> >::iterator iter;
    vcl_map<dbdet_edgel*, vcl_vector<vgl_vector_2d<double> > >::iterator iterv;
    iter=tmap_->backward_beta_edge_map.begin();

    for(iterv=tmap_->backward_vector_edge_map.begin();iterv!=tmap_->backward_vector_edge_map.end();iterv++,iter++)
    {
        double x=iterv->first->pt.x();
        double y=iterv->first->pt.y();
        double theta=iterv->first->tangent;

        double nx=-vcl_sin(theta);
        double ny=vcl_cos(theta);

        for(unsigned i=0;i<iterv->second.size();i++)
        {
            glColor3f( 1, 0, 0 );
            glBegin( GL_LINES );
            glVertex2d(x,y);
            glVertex2d(x+iterv->second[i].x(),y+iterv->second[i].y());
            glEnd();
            glColor3f( 1, 0.5, 1 );
            glBegin(GL_POINTS);
            glVertex2d(x+iterv->second[i].x(),y+iterv->second[i].y());
                glEnd();
        }

    }   
}





dbdet_edgel* dbdet_temporal_map_tableau::compute_closes_edge(double x, double y, dbdet_edgemap_sptr emap)
{
    int winsize=2;
    dbdet_edgel * min_edge=0;
    if(x<emap->ncols() && x>=0 && y<emap->nrows() && y>=0 )
    {
        int indexi=vcl_floor(x);
        int indexj=vcl_floor(y);
        double minerr=1e5;
        for(unsigned k=vcl_max<int>(0,indexi-winsize);k<=vcl_min<int>(indexi+winsize, emap->ncols()-1);k++)
        {
            for(unsigned l=vcl_max<int>(0,indexj-winsize);l<=vcl_min<int>(indexj+winsize, emap->nrows()-1);l++)
            {
                vcl_vector<dbdet_edgel *> *es=&(emap->edge_cells(l,k));
                if(es->size()>0)
                {
                    dbdet_edgel * e=(*es)[0];
                    double err=((e->pt.x()-x)*(e->pt.x()-x)+(e->pt.y()-y)*(e->pt.y()-y));
                    if(err<minerr)
                    {
                        minerr=err;
                        min_edge=e;
                    }
                }
            }
        }
    }
    else
        return 0;

    //: check if the tangents are enormously different.
    if(min_edge)
        return min_edge;
    else
    return 0;

   
}


