// This is breye1/dbctrk/vis/dbctrk_intensity_prune_tool.cxx
//:
// \file

#include "dbctrk/vis/dbctrk_intensity_prune.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_displayer.h>
#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage.h> 
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_curve_matching.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <mbl/mbl_histogram.h>
#include <dbctrk/vis/dbctrk_intensity_tool.h>
#include <vbl/vbl_array_3d.h>
#include <dbctrk/vis/dbctrk_ground_truth_tool.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>
#include <bseg/brip/brip_vil_float_ops.h>
#include <dbctrk/dbctrk_utils.h>
//: Constructor - protected
dbctrk_intensity_prune_tool::dbctrk_intensity_prune_tool()
 :  tableau_(NULL), storage_(NULL)
{
  if (!(ep = engOpen("\0"))) {
    fprintf(stderr, "\nCan't start MATLAB engine\n");
    
  }
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}
dbctrk_intensity_prune_tool::~dbctrk_intensity_prune_tool()

{
  engEvalString(ep, "close;");
}

//: Set the tableau to work with
bool
dbctrk_intensity_prune_tool::set_tableau ( const vgui_tableau_sptr& tableau )
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
dbctrk_intensity_prune_tool::set_storage ( const bpro1_storage_sptr& storage )
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

//: handle events
bool
dbctrk_intensity_prune_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
    
  /* if(!tableau_.ptr())
    return true;
  */
  if( e.type == vgui_KEY_PRESS && e.key == 's' && vgui_SHIFT)
    {
      vcl_vector<vgui_soview*> all_objects;
      all_objects = tableau_->get_selected_soviews();
      
      if(all_objects.size()<=0)
  return false;
      engEvalString(ep, "figure;"); 
      
      vcl_cout<<"\n no of selected ojects are "<<all_objects.size();
      vcl_cout<<"\n the type of the object is"<<((dbctrk_soview2D*)all_objects[0])->type_name();
       if(((dbctrk_soview2D*)all_objects[0])->type_name() == "dbctrk_soview2D") 
   {
     vcl_cout<<"\n the types matched";
     dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
     vcl_cout<<"\n the number of curves are "<<c->prev_.size();
     for(int i=0;i<c->prev_.size();i++)
       {
         double *meanp,*meann,*stdp,*stdn;
         meanp=new double(3);
         meann=new double(3);
         stdp=new double(3);
         stdn=new double(3);
         for(int j=0;j<c->prev_[i]->match_curve_set[0]->desc->meanp.size();j++)
     {
       meanp[j]=c->prev_[i]->match_curve_set[0]->desc->meanp[j];
       meann[j]=c->prev_[i]->match_curve_set[0]->desc->meann[j];
       stdp[j]=c->prev_[i]->match_curve_set[0]->desc->stdp[j];
       stdn[j]=c->prev_[i]->match_curve_set[0]->desc->stdn[j];
       vcl_cout<<meanp[j]<<" ";
       
     }
         prepare_vector("meanp",meanp,3,ep);
         prepare_vector("meann",meann,3,ep);
         prepare_vector("stdp",stdp,3,ep);
         prepare_vector("stdn",stdn,3,ep);
         engEvalString(ep, "hold on;");
         vcl_cout<<engEvalString(ep, "ellipsoid(meanp(1),meanp(2),meanp(3),stdp(1),stdp(2),stdp(3))")<<"\n";
         vcl_cout<<engEvalString(ep, "ellipsoid(meann(1),meann(2),meann(3),stdn(1),stdn(2),stdn(3))")<<"\n";
       }
   }
       
       tableau_->deselect_all();
       engEvalString(ep, "figure;"); 
       engEvalString(ep, "ellipsoid(1,1,1,2,2,2)");
    }
  if( e.type == vgui_KEY_PRESS && e.key == 'l' && vgui_SHIFT)
    {
      
      static double motion=10;
      static double tau=1.0;
      vgui_dialog motion_input("Threshold for motion");
      motion_input.field("Motion",motion);
      motion_input.field("Pruning threshold",tau);
      if(!motion_input.ask())
      return true;
      
      int framen=view->frame();
      vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();

    dbctrk_storage_sptr p1,p2;
    p1.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",0));
    p2.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",1));
    tc1.clear();
    tc2.clear();
    p1->get_tracked_curves(tc1);
    p2->get_tracked_curves(tc2);
    for(int k=0;k<tc1.size();k++)
      {
  tc1[k]->prev_.clear();
  for(int l=0;l<tc2.size();l++)
    {
      //if(k!=l)
      {
        if(dbctrk_curve_matching::sausage_intersection(tc1[k],tc2[l],motion))
    {
      match_data_sptr m=new match_data();
      m->match_curve_set.push_back(tc2[l]);
      tc1[k]->prev_.push_back(m);
        
    }
      }
    }
      }
    for(int k=0;k<tc1.size();k++)
      {
  dbctrk_tracker_curve_sptr c1=tc1[k];
  tc1[k]->next_.clear();
  for(int l=0;l<tc1[k]->prev_.size();l++)
    {

      dbctrk_tracker_curve_sptr c2=tc1[k]->prev_[l]->match_curve_set[0];
      vcl_vector<double> hist1pr=normhist(c1->desc->Prcolor,0,255,20);
      vcl_vector<double> hist1pg=normhist(c1->desc->Pgcolor,0,255,20);
      vcl_vector<double> hist1pb=normhist(c1->desc->Pbcolor,0,255,20);
      vcl_vector<double> hist1nr=normhist(c1->desc->Nrcolor,0,255,20);
      vcl_vector<double> hist1ng=normhist(c1->desc->Ngcolor,0,255,20);
      vcl_vector<double> hist1nb=normhist(c1->desc->Nbcolor,0,255,20);
      
      vcl_vector<double> hist2pr=normhist(c2->desc->Prcolor,0,255,20);
      vcl_vector<double> hist2pg=normhist(c2->desc->Pgcolor,0,255,20);
      vcl_vector<double> hist2pb=normhist(c2->desc->Pbcolor,0,255,20);
      vcl_vector<double> hist2nr=normhist(c2->desc->Nrcolor,0,255,20);
      vcl_vector<double> hist2ng=normhist(c2->desc->Ngcolor,0,255,20);
      vcl_vector<double> hist2nb=normhist(c2->desc->Nbcolor,0,255,20);
      
      double costpp=vcl_sqrt(dist2pdf(hist1pr,hist2pr)+dist2pdf(hist1pg,hist2pg)+dist2pdf(hist1pb,hist2pb));
      double costnn=vcl_sqrt(dist2pdf(hist1nr,hist2nr)+dist2pdf(hist1ng,hist2ng)+dist2pdf(hist1nb,hist2nb));
      double costpn=vcl_sqrt(dist2pdf(hist1pr,hist2nr)+dist2pdf(hist1pg,hist2ng)+dist2pdf(hist1pb,hist2nb));
      double costnp=vcl_sqrt(dist2pdf(hist1nr,hist2pr)+dist2pdf(hist1ng,hist2pg)+dist2pdf(hist1nb,hist2pb));
      double prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
      if(prunecost<tau)
        {
    c1->next_.push_back(tc1[k]->prev_[l]);
        }
    }
  
      }
    return false;
    }
  if( e.type == vgui_KEY_PRESS && e.key == 'h' && vgui_SHIFT)
    {
      motion=40;
      thetabins=6;
      r1=0.4;
      
      vgui_dialog motion_input("Thresholding");
      motion_input.field("Motion",motion);
      motion_input.field("Bins",thetabins);
      motion_input.field("R1",r1);
      if(!motion_input.ask())
  return true;
      satbins=(int)vcl_ceil(1/vcl_pow(r1,2));
      vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq=bvis1_manager::instance()->tableau_sequence();
      vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter;
      vcl_vector<vgui_soview*>  all_objects;
      vcl_vector<vgui_soview*>  selected_objects;
      vcl_map<int,vcl_vector<dbctrk_soview2D* > >::iterator curveiter;
      curr_tableau_=tableau_;
      for(unsigned int j=0;j<seq.size();j++)
  {    
    for(iter=seq[j].begin();iter!=seq[j].end();iter++)
      {
        if(set_tableau((*iter).second))
                { 
            all_objects=tableau_->get_selected_soviews();
      for(unsigned int i=0;i<all_objects.size();i++)
                    {
                      if(((dbctrk_soview2D*)all_objects[i])->type_name()=="dbctrk_soview2D" && tableau_->is_selected(all_objects[i]->get_id()))
                        {   
        tableau_->deselect(all_objects[i]->get_id());
        selected_objects.push_back(all_objects[i]);
                        }
                    }
    }
            }
  }
      
      
      vidpro1_image_storage_sptr imgstr=vidpro1_image_storage_new();
      bvis1_manager::instance()->repository()->store_data(imgstr);
      imgstr->set_name("HistImage");
      if(selected_objects.size()<=0)
  return false;
      vil_image_view<vil_rgb<vxl_byte> > histimg(200,selected_objects.size()*100+100);
      vil_rgb<vxl_byte> pixel(255,255,255);
      histimg.fill(pixel);
      int radius=50;
      displayIHSchart(histimg,radius);
      for(int i=0;i<selected_objects.size();i++)
  {
    if(((dbctrk_soview2D*)selected_objects[i])->type_name() == "dbctrk_soview2D")       {
      dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)selected_objects[i])->dbctrk_sptr();
      float r,g,b;
      utils::set_changing_colors(c->get_id(),r,g,b);
      vil_rgb<vxl_byte> color_of_hist((unsigned char)vcl_floor(r*255.f),
              (unsigned char)vcl_floor(g*255.f),
              (unsigned char)vcl_floor(b*255.f));
      float I,h,s;
      brip_vil_float_ops::rgb_to_ihs(color_of_hist,I,h,s);
      vbl_array_2d<double> chistp(thetabins,satbins,0.0);
      vbl_array_2d<double> chistn(thetabins,satbins,0.0);
      vcl_vector<vsol_spatial_object_2d_sptr > lines;
      norm2dpolar(c->desc->Prcolor,c->desc->Pgcolor,c->desc->Pbcolor,chistp,thetabins,satbins,r1);
      norm2dpolar(c->desc->Nrcolor,c->desc->Ngcolor,c->desc->Nbcolor,chistn,thetabins,satbins,r1);
      int offx,offy;
      offx=50;
      offy=(i+1)*100+50;
      hist2img(chistp,thetabins,satbins,r1,histimg,offx,offy,h,s);
      offx=150;
      hist2img(chistn,thetabins,satbins,r1,histimg,offx,offy,h,s);
      imgstr->set_image(vil_new_image_resource_of_view(histimg));
      bvis1_manager::instance()->add_to_display(imgstr);
    }
   }
    tableau_->deselect_all();
    bvis1_manager::instance()->display_current_frame();
    return false;
    }
  if( e.type == vgui_KEY_PRESS && e.key == 'm' && vgui_SHIFT)
    {
   
    motion=40;
    thetabins=6;
    r1=0.4;
    
    vgui_dialog motion_input("Thresholding");
    motion_input.field("Motion",motion);
    motion_input.field("Bins",thetabins);
    motion_input.field("R1",r1);
    if(!motion_input.ask())
      return true;
    satbins=(int)vcl_ceil(1/vcl_pow(r1,2));
    int framen=view->frame();
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();


    dbctrk_storage_sptr p1,p2;
    p1.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",0));
    p2.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",1));
    tc1.clear();
    tc2.clear();
    p1->get_tracked_curves(tc1);
    p2->get_tracked_curves(tc2);
    for(int k=0;k<tc1.size();k++)
      {
  tc1[k]->prev_.clear();
  for(int l=0;l<tc2.size();l++)
    {
      //if(k!=l)
        {
    if(dbctrk_curve_matching::sausage_intersection(tc1[k],tc2[l],motion))
      {
        match_data_sptr m=new match_data();
        m->match_curve_set.push_back(tc2[l]);
        tc1[k]->prev_.push_back(m);
      }
        }
    }
      }
    vcl_cout<<"\n motion thresholding done";
    vcl_cout<<"\n motion thresholding done";
    for(int k=0;k<tc1.size();k++)
      {
  dbctrk_tracker_curve_sptr c1=tc1[k];
  tc1[k]->next_.clear();
  //: alloctaing memory 3d array
  vbl_array_2d<double> c1histp(thetabins,satbins,0.0);
  vbl_array_2d<double> c1histn(thetabins,satbins,0.0);
  
  norm2dpolar(c1->desc->Prcolor,c1->desc->Pgcolor,c1->desc->Pbcolor,c1histp,thetabins,satbins,r1);
  norm2dpolar(c1->desc->Nrcolor,c1->desc->Ngcolor,c1->desc->Nbcolor,c1histn,thetabins,satbins,r1);
  for(int i=0;i<thetabins;i++)
    {
      for(int j=0;j<satbins;j++)
        {
    vcl_cout<<c1histp[i][j]<<" ";
        }
      vcl_cout<<"\n";
    }

  for(int l=0;l<tc1[k]->prev_.size();l++)
    {

      dbctrk_tracker_curve_sptr c2=tc1[k]->prev_[l]->match_curve_set[0];
      if(c2.ptr())
        {
    vcl_cout<<"\n id is "<<c2->get_id();
    vbl_array_2d<double> c2histp(thetabins,satbins,0.0);
    vbl_array_2d<double> c2histn(thetabins,satbins,0.0);
    
    
    norm2dpolar(c2->desc->Prcolor,c2->desc->Pgcolor,c2->desc->Pbcolor,c2histp,thetabins,satbins,r1);
    norm2dpolar(c2->desc->Nrcolor,c2->desc->Ngcolor,c2->desc->Nbcolor,c2histn,thetabins,satbins,r1);
    
        }
      /*vcl_cout<<"\n histograms computed";
      vcl_cout<<"\n";
      double costpp=dist3pdf(c1histp,c2histp,no_of_bins);
      vcl_cout<<"\n step1";
      double costnp=dist3pdf(c1histn,c2histp,no_of_bins);
      vcl_cout<<"\n step1";
      double costnn=dist3pdf(c1histn,c2histn,no_of_bins);
      vcl_cout<<"\n step1";
      double costpn=dist3pdf(c1histp,c2histn,no_of_bins);
      vcl_cout<<"\n step1";
      
      double prunecost=vcl_max(vcl_max(vcl_max(costpp,costnn),costpn),costnp);
      vcl_cout<<"\n The prune cost is "<<prunecost;
      vcl_cout<<"\n";
      if(prunecost>tau)
        {
    c1->next_.push_back(tc1[k]->prev_[l]);
    }*/
    }
  
      }
    return false;
    }
  //: motion threshlding 
    if( e.type == vgui_KEY_PRESS && e.key == 't' && vgui_SHIFT)
    {
      vgui_dialog motion_input("Thresholding");
      motion_input.field("Motion",motion);
      
      if(!motion_input.ask())
  return true;

      int framen=view->frame();
      vcl_vector<vgui_soview*> all_objects;
      all_objects = tableau_->get_all();

      
      dbctrk_storage_sptr p1,p2;
      p1.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",0));
      p2.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",1));
      tc1.clear();
      tc2.clear();
      p1->get_tracked_curves(tc1);
      p2->get_tracked_curves(tc2);
      int cnt=0;
      for(int k=0;k<tc1.size();k++)
  {
    tc1[k]->prev_.clear();
    for(int l=0;l<tc2.size();l++)
      {
    if(dbctrk_curve_matching::sausage_intersection(tc1[k],tc2[l],motion))
      {
        match_data_sptr m=new match_data();
        m->match_curve_set.push_back(tc2[l]);
        tc1[k]->prev_.push_back(m);
        cnt++;
      }
    
      }
  }
      vcl_cout<<"\n the number of possible matches are"<<cnt;
    }
    //: pruning using euclidean 3d histogram matching    
    if( e.type == vgui_KEY_PRESS && e.key == 'k' && vgui_SHIFT)
    {
   
    static double motion=10;
    static int no_of_bins=10;

    static int Lchoice=0;
    static int Hchoice=1;
    static double t=0.1;
    static int npow=6;
    static int thetabins=6;
    static double r1=0.4;
    static double v1=0.5;
    static double tau=1.0;
    int xbins,ybins,zbins;
    vgui_dialog motion_input("Threshold for motion");
    motion_input.field("Motion",motion);
    motion_input.field("Bins(Euclidean)",no_of_bins);
    motion_input.field("Pruning threshold",tau);
    motion_input.field("Threshold for polynomial function",t);
    motion_input.field("N for polynomial",npow);
    motion_input.field("Theta Bins(polar)",thetabins);
    motion_input.field("R1(polar)",r1);
    motion_input.field("V1(polar)",v1);
    vcl_vector<vcl_string> choices;
    choices.push_back("Bhat");
    choices.push_back("Chi Square");
    choices.push_back("polynomial");
    choices.push_back("KL divg");
    
    motion_input.choice("Distance",choices,Lchoice);
    motion_input.choice("Histogram","Cartesian","Polar",Hchoice);
    if(!motion_input.ask())
      return true;
    satbins=(int)vcl_ceil(1/vcl_pow(r1,2));
    int valuebins=(int)vcl_ceil(1/vcl_pow(v1,3));
   
    int framen=view->frame();
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();


    dbctrk_storage_sptr p1,p2;
    p1.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",0));
    p2.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",1));
    tc1.clear();
    tc2.clear();
    p1->get_tracked_curves(tc1);
    p2->get_tracked_curves(tc2);
    vcl_cout<<"\n curves extracted ";
    vcl_cout<<"\n curves extracted ";
    /*    for(int k=0;k<tc1.size();k++)
      {
  tc1[k]->prev_.clear();
  for(int l=0;l<tc2.size();l++)
    {
      //if(k!=l)
        {
    if(dbctrk_curve_matching::sausage_intersection(tc1[k],tc2[l],motion))
      {
        match_data_sptr m=new match_data();
        m->match_curve_set.push_back(tc2[l]);
        tc1[k]->prev_.push_back(m);
      }
        }
    }
      }
    vcl_cout<<"\n motion thresholding done";
    */
    for(int k=0;k<tc1.size();k++)
      {
  dbctrk_tracker_curve_sptr c1=tc1[k];
  c1->next_.clear();
  //: alloctaing memory 3d array
  vbl_array_3d<double> c1histp,c1histn;
  if(Hchoice==0)
    {
      xbins=ybins=zbins=no_of_bins;
      c1histp.resize(xbins,ybins,zbins);
      c1histn.resize(xbins,ybins,zbins);
      c1histp.fill(0.0);
      c1histn.fill(0.0);
      
      norm3dhist(c1->desc->Prcolor,c1->desc->Pgcolor,c1->desc->Pbcolor,c1histp,xbins);
      norm3dhist(c1->desc->Nrcolor,c1->desc->Ngcolor,c1->desc->Nbcolor,c1histn,xbins);
    }
  else
    {
      xbins=thetabins;
      ybins=satbins;
      zbins=valuebins;
      c1histp.resize(thetabins,satbins,valuebins);
      c1histn.resize(thetabins,satbins,valuebins);
      c1histp.fill(0.0);
      c1histn.fill(0.0);
      norm3dpolar(c1->desc->Prcolor,c1->desc->Pgcolor,c1->desc->Pbcolor,c1histp,thetabins,satbins,valuebins,r1,v1);
      norm3dpolar(c1->desc->Nrcolor,c1->desc->Ngcolor,c1->desc->Nbcolor,c1histn,thetabins,satbins,valuebins,r1,v1);
    }

  for(int l=0;l<tc1[k]->prev_.size();l++)
    {

      dbctrk_tracker_curve_sptr c2=tc1[k]->prev_[l]->match_curve_set[0];
      vbl_array_3d<double> c2histp,c2histn;
      if(Hchoice==0)
        {
    c2histp.resize(no_of_bins,no_of_bins,no_of_bins);
    c2histn.resize(no_of_bins,no_of_bins,no_of_bins);
    c2histp.fill(0.0);
    c2histn.fill(0.0);
    norm3dhist(c2->desc->Prcolor,c2->desc->Pgcolor,c2->desc->Pbcolor,c2histp,no_of_bins);
    norm3dhist(c2->desc->Nrcolor,c2->desc->Ngcolor,c2->desc->Nbcolor,c2histn,no_of_bins);
        }
      else
        {
    c2histp.resize(thetabins,satbins,valuebins);
    c2histn.resize(thetabins,satbins,valuebins);
    c2histp.fill(0.0);
    c2histn.fill(0.0);
    norm3dpolar(c2->desc->Prcolor,c2->desc->Pgcolor,c2->desc->Pbcolor,c2histp,thetabins,satbins,valuebins,r1,v1);
    norm3dpolar(c2->desc->Nrcolor,c2->desc->Ngcolor,c2->desc->Nbcolor,c2histn,thetabins,satbins,valuebins,r1,v1);
        }
      double prunecost=1e6;
      if(Lchoice==0){
        double costpp=dist3pdf_bhat(c1histp,c2histp,xbins,ybins,zbins);
        double costnp=dist3pdf_bhat(c1histn,c2histp,xbins,ybins,zbins);
        double costnn=dist3pdf_bhat(c1histn,c2histn,xbins,ybins,zbins);
        double costpn=dist3pdf_bhat(c1histp,c2histn,xbins,ybins,zbins);
        prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
      }
      else if(Lchoice==1)
        {
        double costpp=dist3pdf_chi_square(c1histp,c2histp,xbins,ybins,zbins);
        double costnp=dist3pdf_chi_square(c1histn,c2histp,xbins,ybins,zbins);
        double costnn=dist3pdf_chi_square(c1histn,c2histn,xbins,ybins,zbins);
        double costpn=dist3pdf_chi_square(c1histp,c2histn,xbins,ybins,zbins);
        prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
        }
      else if(Lchoice==2)
        {
        double costpp=dist3pdf_poly(c1histp,c2histp,xbins,ybins,zbins,t,npow);
        double costnp=dist3pdf_poly(c1histn,c2histp,xbins,ybins,zbins,t,npow);
        double costnn=dist3pdf_poly(c1histn,c2histn,xbins,ybins,zbins,t,npow);
        double costpn=dist3pdf_poly(c1histp,c2histn,xbins,ybins,zbins,t,npow);
        prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
        }
      else if(Lchoice==3)
        {
    double costpp=dist3pdf_KL(c1histp,c2histp,xbins,ybins,zbins);
    double costnp=dist3pdf_KL(c1histn,c2histp,xbins,ybins,zbins);
    double costnn=dist3pdf_KL(c1histn,c2histn,xbins,ybins,zbins);
    double costpn=dist3pdf_KL(c1histp,c2histn,xbins,ybins,zbins);
    
    prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
        }
      vcl_cout<<"\n the prune cost is "<<prunecost;
      if(prunecost<tau)
        {
    c1->next_.push_back(tc1[k]->prev_[l]);
        }
  
    }
  
      }
    return true;
    }
    if( e.type == vgui_KEY_PRESS && e.key == 'r' && vgui_SHIFT)
    {
   
    static double motion=10;
    static int thetabins=6;
    static double tau=1.0;
    static double r1=0.4;
    static double v1=0.5;
    static int Lchoice=0; 
    
      
    vgui_dialog motion_input("Thresholding");
    motion_input.field("Motion",motion);
    motion_input.field("Theta Bins",thetabins);
    motion_input.field("R1",r1);
    motion_input.field("V1",v1);
    motion_input.field("Pruning threshold",tau);
    motion_input.choice("Distance","Bhat","Chi Square","Mutual Info",Lchoice);

    if(!motion_input.ask())
  return true;
    vcl_cout<<"\n Lchoice ="<<Lchoice;
    satbins=(int)vcl_ceil(1/vcl_pow(r1,2));
    int valuebins=(int)vcl_ceil(1/vcl_pow(v1,3));

    int framen=view->frame();
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();


    dbctrk_storage_sptr p1,p2;
    p1.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",0));
    p2.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",1));
    tc1.clear();
    tc2.clear();
    p1->get_tracked_curves(tc1);
    p2->get_tracked_curves(tc2);

    for(int k=0;k<tc1.size();k++)
      {
  tc1[k]->prev_.clear();
  for(int l=0;l<tc2.size();l++)
    {
      //if(k!=l)
        {
    if(dbctrk_curve_matching::sausage_intersection(tc1[k],tc2[l],motion))
      {
        match_data_sptr m=new match_data();
        m->match_curve_set.push_back(tc2[l]);
        tc1[k]->prev_.push_back(m);
      }
        }
    }
      }
    vcl_cout<<"\n motion thresholding done";
    vcl_cout<<"\n motion thresholding done";
    for(int k=0;k<tc1.size();k++)
      {
  dbctrk_tracker_curve_sptr c1=tc1[k];
  c1->next_.clear();
  //: alloctaing memory 3d array
  vbl_array_3d<double> c1histp(thetabins,satbins,valuebins,0.0);
  vbl_array_3d<double> c1histn(thetabins,satbins,valuebins,0.0);

  norm3dpolar(c1->desc->Prcolor,c1->desc->Pgcolor,c1->desc->Pbcolor,c1histp,thetabins,satbins,valuebins,r1,v1);
  norm3dpolar(c1->desc->Nrcolor,c1->desc->Ngcolor,c1->desc->Nbcolor,c1histn,thetabins,satbins,valuebins,r1,v1);

  for(int l=0;l<tc1[k]->prev_.size();l++)
    {

      dbctrk_tracker_curve_sptr c2=tc1[k]->prev_[l]->match_curve_set[0];
      vbl_array_3d<double> c2histp(thetabins,satbins,valuebins,0.0);
      vbl_array_3d<double> c2histn(thetabins,satbins,valuebins,0.0);
      
      norm3dpolar(c2->desc->Prcolor,c2->desc->Pgcolor,c2->desc->Pbcolor,c2histp,thetabins,satbins,valuebins,r1,v1);
      norm3dpolar(c2->desc->Nrcolor,c2->desc->Ngcolor,c2->desc->Nbcolor,c2histn,thetabins,satbins,valuebins,r1,v1);


      double prunecost=1e6;
      if(Lchoice==0){
        double costpp=dist3pdf_bhat(c1histp,c2histp,thetabins,satbins,valuebins);
        double costnp=dist3pdf_bhat(c1histn,c2histp,thetabins,satbins,valuebins);
        double costnn=dist3pdf_bhat(c1histn,c2histn,thetabins,satbins,valuebins);
        double costpn=dist3pdf_bhat(c1histp,c2histn,thetabins,satbins,valuebins);
        prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
      }
      else if(Lchoice==1)
        {
    double costpp=dist3pdf_chi_square(c1histp,c2histp,thetabins,satbins,valuebins);
    double costnp=dist3pdf_chi_square(c1histn,c2histp,thetabins,satbins,valuebins);
    double costnn=dist3pdf_chi_square(c1histn,c2histn,thetabins,satbins,valuebins);
    double costpn=dist3pdf_chi_square(c1histp,c2histn,thetabins,satbins,valuebins);
    prunecost=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
        }
      vcl_cout<<"\n the prune cost is "<<prunecost;

      if(prunecost<tau)
        {
    c1->next_.push_back(tc1[k]->prev_[l]);
        }
  
    }
  
      }
    return false;
    }
    //: to cpmpare it against ground truth 
    if( e.type == vgui_KEY_PRESS && e.key == 'j' && vgui_SHIFT)
    {
      static vcl_string filename="";
      static vcl_string ext="*.*";

      vgui_dialog inputfile("Load Ground Truth");
      inputfile.file("Ground Truth File Name",ext,filename);
      if(!inputfile.ask())
  return true;

      vcl_cout<<"\n reading ground truth file .... ";
      vcl_cout<<"\n reading ground truth file .... ";
      vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > >gmatches;
      readcorrespondence(filename,gmatches);

      int counter=0;
      for(int i=0;i<tc1.size();i++)
  {
    int queryid=tc1[i]->get_id();
    vcl_vector<int> corr_id=find_matching_ids(queryid,gmatches);
    bool outerflag=true;
    for(int k=0;k<corr_id.size();k++)
      {
        bool innerflag=false;
        for(int l=0;l<tc1[i]->next_.size();l++)
    {
      if(corr_id[k]==tc1[i]->next_[l]->match_curve_set[0]->get_id())
        innerflag=true;
    }
        if(!innerflag)
    outerflag=false;
      }
    if(outerflag)
      counter++;
    else
      {
        //vcl_cout<<"\n the error curve is "<<queryid;
        
      }

  }
      vcl_cout<<"\n the counter is "<<counter<<" out of"<<tc1.size();
      int sumcandidi=0;
      int sumcandidm=0;
      
      for(int i=0;i<tc1.size();i++)
  {
    sumcandidi+=tc1[i]->next_.size();
    sumcandidm+=tc1[i]->prev_.size();
  }
      vcl_cout<<"\n the sum of candids intensity are "<<sumcandidi;
      vcl_cout<<"\n the sum of candids intensity are "<<sumcandidm;
      
    }
  if( e.type == vgui_MOTION){
     tableau_->motion(e.wx, e.wy);

   vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
 
  object_ = (dbctrk_soview2D*)curr_obj;
  curr_curve_ = NULL;
  if( curr_obj && curr_obj->type_name() == "dbctrk_soview2D"){   
    curr_curve_ = object_->dbctrk_sptr();
    return true;
  }
  
  /* bvis1_manager::instance()->post_overlay_redraw();*/
    
    return true;  
  }
  if( e.type == vgui_DRAW_OVERLAY){
    if(!curr_curve_)
      return false;
    
    for(int i=0;i<tc2.size();i++)
    {
    
      if(tc2[i]->frame_number=view->frame())
  {
          dbctrk_soview2D curve(tc2[i]);
          neighbor_style_->rgba[0] =0.92f;
          neighbor_style_->rgba[1] =0.92f;
          neighbor_style_->rgba[2] =0.92f;
          neighbor_style_->apply_all();  
          curve.draw();
  }
    }
    for(int i=0;i<curr_curve_->prev_.size();i++)
      {
  if(curr_curve_->prev_[i]->match_curve_set[0]->frame_number=view->frame())
  {
  dbctrk_soview2D curve(curr_curve_->prev_[i]->match_curve_set[0]);
  neighbor_style_->rgba[0] =1.0f;
  neighbor_style_->rgba[1] =0.0f;
  neighbor_style_->rgba[2] =0.0f;
  neighbor_style_->line_width=4.0;
  neighbor_style_->apply_all();  
  curve.draw();
  }
      }
    for(int i=0;i<curr_curve_->next_.size();i++)
      {
  if(curr_curve_->next_[i]->match_curve_set[0]->frame_number=view->frame())
    {
      dbctrk_soview2D curve(curr_curve_->next_[i]->match_curve_set[0]);
      neighbor_style_->rgba[0] =0.0f;
      neighbor_style_->rgba[1] =1.0f;
      neighbor_style_->rgba[2] =0.0f;
      neighbor_style_->apply_all();  
      curve.draw();
    }
      }

  }
  return false;
  
  
        }
//: Return the name of this tool
vcl_string
dbctrk_intensity_prune_tool::name() const
{
  return "Intensity pruner"; 
}

bool dbctrk_intensity_prune_tool::disjoint(vbl_bounding_box<double,3> const &a,
           vbl_bounding_box<double,3> const &b)
{
  for (int i=0; i<3; ++i)
    if (a.min()[i] > b.max()[i] || a.max()[i] < b.min()[i])
      return true;
  return false;
}
bool dbctrk_intensity_prune_tool::norm2dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_2d<double> &polarhist,int thetabins,int rbins,double r1)
{
  if(dim1.size()!=dim2.size() && dim1.size()!=dim3.size() && dim1.size()>0)
    return false;
  
  for(int i=0;i<dim1.size();i++)
    {
      float h,s;
      if(dim2[i]<0.0 || dim2[i]>255.0)
  h=0.0;
      else
  h=(float)dim2[i];
      if(dim3[i]<0.0 || dim3[i]>255.0)
  s=0.0;
      else
  s=(float)dim3[i];


      int thetaindex=(int)vcl_floor((h*360/255)/(360/thetabins));
      int satindex=(int)vcl_floor(vcl_pow((s/255)/r1,2));

      polarhist[thetaindex][satindex]=polarhist[thetaindex][satindex]+1;
    }
    for(int i=0;i<thetabins;i++)
    {
      for(int j=0;j<rbins;j++)
  {
    polarhist[i][j]= polarhist[i][j]/dim1.size();
  
  }
      
    }/*
      vcl_cout<<"\n";
      vcl_cout<<"\n";*/
  return true;
}
bool dbctrk_intensity_prune_tool::norm3dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_3d<double> &polarhist,int thetabins,int rbins,int valuebins,double r1,double v1)
{
   if(dim1.size()!=dim2.size() && dim1.size()!=dim3.size() && dim1.size()>0)
    return false;
  
  for(int i=0;i<dim1.size();i++)
    {
      float h,s,v;
      if(dim1[i]<0.0 || dim1[i]>255.0)
  v=0.0;
      else
  v=(float)dim1[i];
      if(dim2[i]<0.0 || dim2[i]>255.0)
  h=0.0;
      else
  h=(float)dim2[i];
      if(dim3[i]<0.0 || dim3[i]>255.0)
  s=0.0;
      else
  s=(float)dim3[i];


      int thetaindex=(int)vcl_floor((h*360/255)/(360/thetabins));
      int satindex=(int)vcl_floor(vcl_pow((s/255)/r1,2));
      int valindex=(int)vcl_floor(vcl_pow((v/255)/v1,3));
      if(thetaindex>=thetabins)
  thetaindex=thetabins-1;
      if(satindex>=rbins)
  satindex=rbins-1;
      if(valindex>=valuebins)
  valindex=valuebins-1;
      polarhist(thetaindex,satindex,valindex)= polarhist(thetaindex,satindex,valindex)+1;
    }
    for(int i=0;i<thetabins;i++)
    {
      for(int j=0;j<rbins;j++)
  {
    for(int k=0;k<valuebins;k++)
      {
        polarhist(i,j,k)= polarhist(i,j,k)/dim1.size();
        
      }
  } 
    }
}
bool dbctrk_intensity_prune_tool::hist2img(vbl_array_2d<double> hist, int thetabins,int satbins, double r1,vil_image_view<vil_rgb<vxl_byte> > &polarimg, int offx, int offy,float h,float s)
{
  
  int R=50;
  vcl_cout<<"\n entering the hist2img";
  for(int i=0;i<hist.rows();i++)
    {
      for(int j=0;j<hist.cols();j++)
  {
    vcl_cout<<255.0*hist(i,j)<<" ";
    for(double theta=(i*2*vnl_math::pi)/hist.rows();theta<((i+1)*2*vnl_math::pi)/hist.rows();)
      {
        for(double rad=vcl_sqrt((double)j)*r1;rad < vcl_sqrt((double)j+1)*r1 && rad<1.0;)
    {
      int index_i=(int)vcl_floor((rad*vcl_cos(theta))*(double)R)+offx;
      int index_j=(int)vcl_floor((rad*vcl_sin(theta))*(double)R)+offy;
      if(index_j>=offy+R)
        index_j=offy+R-1;
      if(index_i>=offx+R)
        index_i=offx+R-1;
      if(index_j<offy-R)
        index_j=offy-R;
      if(index_i<offx-R)
        index_i=offx-R;
      vil_rgb<vxl_byte> pixel((unsigned char)vcl_floor(255.0*hist(i,j)),
            (unsigned char)vcl_floor(255.0*hist(i,j)),
            (unsigned char)vcl_floor(255.0*hist(i,j)));
      brip_vil_float_ops::ihs_to_rgb(pixel,(unsigned char)vcl_floor(255.0*hist(i,j)),h,s);
      polarimg(index_i,index_j)=pixel;
      rad+=0.01;
    }
        theta+=0.01;
      }
  }
    }
 return true;
}
bool dbctrk_intensity_prune_tool::norm3dhist(vcl_vector<double> dim1,vcl_vector<double> dim2,vcl_vector<double> dim3, vbl_array_3d<double> & hist, int numbins)
{

  if(dim1.size()!=dim2.size() && dim1.size()!=dim3.size() && dim1.size()>0)
    return false;

  for(int i=0;i<dim1.size();i++)
    {
      if(dim1[i]<0.0 || dim1[i]>255.0)
   dim1[i]=0;
      if(dim2[i]<0.0 || dim2[i]>255.0)
  dim2[i]=0;
      if(dim3[i]<0.0 || dim3[i]>255.0)
  dim3[i]=0;
      
      double range=vcl_ceil(255.0/(double)numbins);
      int ind1=(int)vcl_floor(dim1[i]/range);
      int ind2=(int)vcl_floor(dim2[i]/range);
      int ind3=(int)vcl_floor(dim3[i]/range);
      if(ind1>=numbins)
  ind1=numbins-1;
      if(ind2>=numbins)
  ind2=numbins-1;
      if(ind3>=numbins)
  ind3=numbins-1;
      
      hist(ind1,ind2,ind3)=hist(ind1,ind2,ind3)+1;

    }
  for(int i=0;i<numbins;i++)
    {
      for(int j=0;j<numbins;j++)
  {
    for(int k=0;k<numbins;k++)
      {
      hist(i,j,k)=hist(i,j,k)/dim1.size();

      }

  
  }

    }

    
  return true;
}
double dbctrk_intensity_prune_tool::dist3pdf(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int numbins)
{
  double dist=0;
  for(int i=0;i<numbins;i++)
      for(int j=0;j<numbins;j++)
  for(int k=0;k<numbins;k++)
    {
      dist+=vcl_sqrt(hist1[i][j][k]*hist2[i][j][k]);
    }
  return dist;
}
double dbctrk_intensity_prune_tool::dist3pdf_bhat(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins)
{
  double dist=0;
  for(int i=0;i<xbins;i++)
      for(int j=0;j<ybins;j++)
  for(int k=0;k<zbins;k++)
    {
      dist+=vcl_sqrt(hist1[i][j][k]*hist2[i][j][k]);
    }
  if(dist!=0.0)
    return (-vcl_log(dist));
  else
    return 1e6;

}
double dbctrk_intensity_prune_tool::dist3pdf_KL(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins)
{
  double dist=0;
  for(int i=0;i<xbins;i++)
      for(int j=0;j<ybins;j++)
  for(int k=0;k<zbins;k++)
    {
      if(hist2[i][j][k]>0 && hist1[i][j][k]>0)
        dist+=hist1[i][j][k]*vcl_log(hist1[i][j][k]/hist2[i][j][k])+
        hist2[i][j][k]*vcl_log(hist2[i][j][k]/hist1[i][j][k]);
      else
        dist+=hist1[i][j][k]+hist2[i][j][k];
    }
  return dist/2;
}
double dbctrk_intensity_prune_tool::dist3pdf_chi_square(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins)
{
  double dist=0;
  for(int i=0;i<xbins;i++)
      for(int j=0;j<ybins;j++)
  for(int k=0;k<zbins;k++)
    {
      if((hist1[i][j][k]+hist2[i][j][k])>0)
        dist+=vcl_pow((hist1[i][j][k]-hist2[i][j][k]),2)/(hist1[i][j][k]+hist2[i][j][k]);
    }
  return dist;
}
double dbctrk_intensity_prune_tool::dist3pdf_poly(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins,double tau,int n)
{
  double dist=0;
  int cnt=0;
  for(int i=0;i<xbins;i++)
      for(int j=0;j<ybins;j++)
  for(int k=0;k<zbins;k++)
    {
      double f1=hist1[i][j][k];
      double f2=hist2[i][j][k];
      if(f1>0.0 && f2 >0.0)  
        {
    dist+=(vcl_pow((f1-f2),n+1)-vcl_pow(tau,n+1))/(vcl_pow((f1-f2),n)+vcl_pow(tau,n));
    ++cnt;
        }
      
    }
  if(cnt>0)
    return dist/cnt;
  else
    return dist;
}
void dbctrk_intensity_prune_tool::prepare_vector(char matlab_name[], double data[], int dim1,Engine *ep) {
  /* Create matlab variables for our data */
  mxArray *matlab_data = NULL;
  matlab_data = mxCreateDoubleMatrix(1, dim1, mxREAL);
  /* Give the variable a name in Matlab */
  memcpy((void *)mxGetPr(matlab_data), (void *)data, dim1*sizeof(*data));
  /* The previous line is quite tricky to understand. In c arrays are converted */
  /* to pointers. The last parameter of memcpy must be the size of the array */
  /* and NOT the pointer. */

  engPutVariable(ep,matlab_name,matlab_data);
  /* Free the memory */
  mxDestroyArray(matlab_data);
}

//: function to return the corresponding ids to an id from ground truth data
vcl_vector<int> find_matching_ids(int query,vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > > gmatches)
{
  bool flag=false;
  vcl_vector<int> ids;
  ids.clear();
 for(int i=0;i<gmatches.size();i++)
    {
      //: looping over the ids of agroup
      for(int j=0;j<gmatches[i].first.size();j++)
  {
    if(query==gmatches[i].first[j])
      flag=true;
  }
      if(flag)
  {
   for(int j=0;j<gmatches[i].second.size();j++)
   {
     if(query==302)
       vcl_cout<<" the "<<gmatches[i].second[j];
     if(gmatches[i].second[j]>=0)
       ids.push_back(gmatches[i].second[j]);
   }
  }
      flag=false;
    }
 return ids;
}
bool dbctrk_intensity_prune_tool::displayIHSchart(vil_image_view<vil_rgb<vxl_byte> > &histimg,int radius)
{

    for(double r=0;r<1.0;)
      {    
        for(double theta=0;theta<2*vnl_math::pi;)
    {

      int index_i=(int)vcl_floor(r*vcl_cos(theta)*radius) + radius;
      int index_j=(int)vcl_floor(r*vcl_sin(theta)*radius) + radius;
      if(index_j>=2*radius)
        index_j=2*radius-1;
      if(index_i>=2*radius)
        index_i=2*radius-1;
      if(index_j<0)
        index_j=0;
      if(index_i<0)
        index_i=0;
      

      vil_rgb<vxl_byte> rgbvals;
      brip_vil_float_ops::ihs_to_rgb(rgbvals,255.0,theta*180*255/(360*vnl_math::pi),r*255.0);

      histimg(index_i,index_j)=rgbvals;

      theta+=0.01;
    }
        r+=0.01;
      }
    
  return true;
}
bool dbctrk_intensity_prune_tool::buildprior(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2,vbl_array_2d<double> pxy)
{
  if(hist1.get_row1_count()==hist2.get_row1_count() &&
     hist1.get_row2_count()==hist2.get_row2_count() &&
     hist1.get_row3_count()==hist2.get_row3_count() )
    {
      double *iter1=hist1.begin();
      double *iter2=hist2.begin();
      
      for(int i=0;i<hist1.size();i++)
  {
    for(int j=0;j<hist2.size();j++)
      {
        double fx=*(iter1+1);
        double fy=*(iter2+1);
        pxy(i,j)+=fx*fy;
      }
  }
      double total=0.0;
      for(int i=0;i<pxy.rows();i++)
    for(int j=0;j<pxy.cols();j++)
        total+=pxy(i,j);
      for(int i=0;i<pxy.rows();i++)
  for(int j=0;j<pxy.cols();j++)
    pxy(i,j)/=total;

      
      return true;
    }
   else
    return false;
}
