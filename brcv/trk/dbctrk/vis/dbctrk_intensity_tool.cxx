#include <dbctrk/vis/dbctrk_intensity_tool.h>
#include <bvis1/bvis1_manager.h>
#include <dbctrk/vis/dbctrk_soview2D.h>
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
#include <dbctrk/dbctrk_curveMatch.h> 
#include <bvis1/bvis1_view_tableau.h>
#include <mbl/mbl_histogram.h>


dbctrk_intensity_tool::dbctrk_intensity_tool()
{

     if (!(ep = engOpen("\0"))) {
         fprintf(stderr, "\nCan't start MATLAB engine\n");
         
     }
    
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}

dbctrk_intensity_tool::~dbctrk_intensity_tool()
{
   engEvalString(ep, "close;");

}

bool
dbctrk_intensity_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_intensity_tool::set_storage ( const bpro1_storage_sptr& storage)
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
dbctrk_intensity_tool::tableau()
{
  return tableau_;
}

dbctrk_storage_sptr
dbctrk_intensity_tool::storage()
{
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage_);
  return dbctrk_storage;
}

bool
dbctrk_intensity_tool::handle(const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  int frame = view->frame();
 //: tool to write rgb or IHS profile of a curve to a file.
  if (e.type == vgui_KEY_PRESS && e.key == 'w' ) {
    vgui_dialog rgb_dlg("Output file for Intesnity profile");
    static vcl_string file_name = "";
    static vcl_string ext = "*.*";
    rgb_dlg.file("File:", ext, file_name);
    if( !rgb_dlg.ask())
         return true;
    vcl_ofstream ofile(file_name.c_str());
    if(!ofile)
        return false;
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_selected_soviews();
    if(all_objects.size()==1)
    {
        for (int i = 0; i<all_objects.size(); i++) {
           if(((dbctrk_soview2D*)all_objects[i])->type_name()=="dbctrk_soview2D")
           {
               dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr();
               for(i=0;i<c->desc->Prcolor.size();i++)
               {
                ofile<<c->desc->Prcolor[i]<<"\t"<<c->desc->Pgcolor[i]<<"\t"
                     <<c->desc->Pbcolor[i]<<"\t"<<c->desc->Nrcolor[i]<<"\t"
                     <<c->desc->Ngcolor[i]<<"\t"<<c->desc->Nbcolor[i]<<"\n";
               }
            }
         }
        ofile.close();
    }
  /* Free memory, close MATLAB engine */
    tableau_->deselect_all();
  }
  //: tool to plot a intensity profile of a curve using matlab
  if (e.type == vgui_KEY_PRESS && e.key == 'v' && vgui_SHIFT) {
    engEvalString(ep, "figure;"); 
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_selected_soviews();
    if(all_objects.size()==1)
    {
        for (int i = 0; i<all_objects.size(); i++) {
           if(((dbctrk_soview2D*)all_objects[i])->type_name()=="dbctrk_soview2D")
           {
               dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr();
               double *prdata=new double[c->desc->Prcolor.size()];
               double *pgdata=new double[c->desc->Pgcolor.size()];
               double *pbdata=new double[c->desc->Pbcolor.size()];

               double *nrdata=new double[c->desc->Nrcolor.size()];
               double *ngdata=new double[c->desc->Ngcolor.size()];
               double *nbdata=new double[c->desc->Nbcolor.size()];

               double *pldata=new double[c->desc->Prcolor.size()];
               double *padata=new double[c->desc->Pgcolor.size()];
               double *plbdata=new double[c->desc->Pbcolor.size()];

               double *nldata=new double[c->desc->Nrcolor.size()];
               double *nadata=new double[c->desc->Ngcolor.size()];
               double *nlbdata=new double[c->desc->Nbcolor.size()];


               for(i=0;i<c->desc->Prcolor.size();i++)
               {
                utils::IHS2lab(c->desc->Prcolor[i],c->desc->Pgcolor[i],c->desc->Pbcolor[i],pldata[i],padata[i],plbdata[i]);
                prdata[i]=c->desc->Prcolor[i];
                pgdata[i]=c->desc->Pgcolor[i];
                pbdata[i]=c->desc->Pbcolor[i];
                utils::IHS2lab(c->desc->Nrcolor[i],c->desc->Ngcolor[i],c->desc->Nbcolor[i],nldata[i],nadata[i],nlbdata[i]);    
                nrdata[i]=c->desc->Nrcolor[i];
                ngdata[i]=c->desc->Ngcolor[i];
                nbdata[i]=c->desc->Nbcolor[i];
                }
                prepare_vector("prdata",prdata,c->desc->Prcolor.size(),ep);
                prepare_vector("pgdata",pgdata,c->desc->Prcolor.size(),ep);
                prepare_vector("pbdata",pbdata,c->desc->Prcolor.size(),ep);

                prepare_vector("nrdata",nrdata,c->desc->Nrcolor.size(),ep);
                prepare_vector("ngdata",ngdata,c->desc->Nrcolor.size(),ep);
                prepare_vector("nbdata",nbdata,c->desc->Nrcolor.size(),ep);

                prepare_vector("pldata",pldata,c->desc->Prcolor.size(),ep);
                prepare_vector("padata",padata,c->desc->Prcolor.size(),ep);
                prepare_vector("plbdata",plbdata,c->desc->Prcolor.size(),ep);

                prepare_vector("nldata",nldata,c->desc->Nrcolor.size(),ep);
                prepare_vector("nadata",nadata,c->desc->Nrcolor.size(),ep);
                prepare_vector("nlbdata",nlbdata,c->desc->Nrcolor.size(),ep);

                
    engEvalString(ep,"pr = mean(prdata);");
    engEvalString(ep,"pg = mean(pgdata);");
    engEvalString(ep,"pb = mean(pbdata);");

    engEvalString(ep,"spr = std(prdata);");
    engEvalString(ep,"spg = std(pgdata);");
    engEvalString(ep,"spb = std(pbdata);");
    
    engEvalString(ep,"nr = mean(nrdata);");
    engEvalString(ep,"ng = mean(ngdata);");
    engEvalString(ep,"nb = mean(nbdata);");

    engEvalString(ep,"snr = std(nrdata);");
    engEvalString(ep,"sng = std(ngdata);");
    engEvalString(ep,"snb = std(nbdata);");
    /*  engEvalString(ep,"pl = mean(pldata);");
     engEvalString(ep,"pa = mean(padata);");
    engEvalString(ep,"pb = mean(pbdata);");
    */

                engEvalString(ep, "subplot(2,3,1);");
                engEvalString(ep, "plot(prdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep,"meanpr(1:length(prdata))=pr;");
    engEvalString(ep,"plot(meanpr,'r');");
    engEvalString(ep, "pstdpr(1:length(prdata))=pr+spr;");
    engEvalString(ep, "nstdpr(1:length(prdata))=pr-spr;");
    engEvalString(ep, "plot(pstdpr,'g');");
    engEvalString(ep, "plot(nstdpr,'g');");

                engEvalString(ep, "subplot(2,3,2);");
                engEvalString(ep, "plot(pgdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "meanpg(1:length(pgdata))=pg;");
    engEvalString(ep, "plot(meanpg,'r');");
    engEvalString(ep, "pstdpg(1:length(pgdata))=pg+spg;");
    engEvalString(ep, "nstdpg(1:length(pgdata))=pg-spg;");
    engEvalString(ep, "plot(pstdpg,'g');");
    engEvalString(ep, "plot(nstdpg,'g');");


                engEvalString(ep, "subplot(2,3,3);");
                engEvalString(ep, "plot(pbdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "meanpb(1:length(pbdata))=pb;");
    engEvalString(ep, "plot(meanpb,'r');");
    engEvalString(ep, "pstdpb(1:length(pbdata))=pb+spb;");
    engEvalString(ep, "nstdpb(1:length(pbdata))=pb-spb;");
    engEvalString(ep, "plot(pstdpb,'g');");
    engEvalString(ep, "plot(nstdpb,'g');");


                engEvalString(ep, "subplot(2,3,4);");
                engEvalString(ep, "plot(nrdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "meannr(1:length(nrdata))=nr;");
    engEvalString(ep, "plot(meannr,'r');");
    engEvalString(ep, "pstdnr(1:length(nrdata))=nr+snr;");
    engEvalString(ep, "nstdnr(1:length(nrdata))=nr-snr;");
    engEvalString(ep, "plot(pstdnr,'g');");
    engEvalString(ep, "plot(nstdnr,'g');");

                engEvalString(ep, "subplot(2,3,5);");
                engEvalString(ep, "plot(ngdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "meanng(1:length(ngdata))=ng;");
    engEvalString(ep, "plot(meanng,'r');");
    engEvalString(ep, "pstdng(1:length(ngdata))=ng+sng;");
    engEvalString(ep, "nstdng(1:length(ngdata))=ng-sng;");
    engEvalString(ep, "plot(pstdng,'g');");
    engEvalString(ep, "plot(nstdng,'g');");

                engEvalString(ep, "subplot(2,3,6);");
                engEvalString(ep, "plot(nbdata);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "meannb(1:length(nbdata))=nb;");
    engEvalString(ep, "plot(meannb,'r');");
    engEvalString(ep, "pstdnb(1:length(nbdata))=nb+snb;");
    engEvalString(ep, "nstdnb(1:length(nbdata))=nb-snb;");
    engEvalString(ep, "plot(pstdnb,'g');");
    engEvalString(ep, "plot(nstdnb,'g');");
    /*
                engEvalString(ep, "subplot(4,3,7);");
                engEvalString(ep, "plot(pldata);");
                engEvalString(ep, "subplot(4,3,8);");
                engEvalString(ep, "plot(padata);");
                engEvalString(ep, "subplot(4,3,9);");
                engEvalString(ep, "plot(plbdata);");

                engEvalString(ep, "subplot(4,3,10);");
                engEvalString(ep, "plot(nldata);");
                engEvalString(ep, "subplot(4,3,11);");
                engEvalString(ep, "plot(nadata);");
                engEvalString(ep, "subplot(4,3,12);");
                engEvalString(ep, "plot(nlbdata);");
    */
                printf("Hit return to exit\n\n");
                fgetc(stdin); 
            }
         }
    }
  
  /* Free memory, close MATLAB engine */
    tableau_->deselect_all();
  }
    //: to check the distribution of intensity along the curve 
  if (e.type == vgui_KEY_PRESS && e.key == 'h' ) {
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_selected_soviews();
    if(all_objects.size()>=1)
      {
  if(((dbctrk_soview2D*)all_objects[0])->type_name() == "dbctrk_soview2D")
    {
      dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
      double *prdata=new double[c->desc->Prcolor.size()];
               double *pgdata=new double[c->desc->Pgcolor.size()];
               double *pbdata=new double[c->desc->Pbcolor.size()];

               double *nrdata=new double[c->desc->Nrcolor.size()];
               double *ngdata=new double[c->desc->Ngcolor.size()];
               double *nbdata=new double[c->desc->Nbcolor.size()];



               for(int i=0;i<c->desc->Prcolor.size();i++)
               {

                prdata[i]=c->desc->Prcolor[i];
                pgdata[i]=c->desc->Pgcolor[i];
                pbdata[i]=c->desc->Pbcolor[i];

                nrdata[i]=c->desc->Nrcolor[i];
                ngdata[i]=c->desc->Ngcolor[i];
                nbdata[i]=c->desc->Nbcolor[i];
         }
         prepare_vector("prdata",prdata,c->desc->Prcolor.size(),ep);
         prepare_vector("pgdata",pgdata,c->desc->Prcolor.size(),ep);
         prepare_vector("pbdata",pbdata,c->desc->Prcolor.size(),ep);
         
         prepare_vector("nrdata",nrdata,c->desc->Nrcolor.size(),ep);
         prepare_vector("ngdata",ngdata,c->desc->Nrcolor.size(),ep);
         prepare_vector("nbdata",nbdata,c->desc->Nrcolor.size(),ep);

         engEvalString(ep, "hold on;");
         engEvalString(ep, "X=1:255;");
         
         engEvalString(ep, "subplot(2,3,1);");
         engEvalString(ep, "hpr=hist(prdata,255);");
         engEvalString(ep, "hold on;");
         engEvalString(ep, "plot(hpr./length(prdata),'b-')");
         engEvalString(ep, "plot(normpdf(X,mean(prdata),std(prdata )),'r-')");

    engEvalString(ep, "subplot(2,3,2);");
    engEvalString(ep, "hpg=hist(pgdata,255);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "plot(hpg./length(pgdata),'b-')");
    engEvalString(ep, "plot(normpdf(X,mean(pgdata),std(pgdata)),'r-')");    
    engEvalString(ep, "subplot(2,3,3);");
    engEvalString(ep, "hpb=hist(pbdata,255);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "plot(hpb./length(pbdata),'b-')");
    engEvalString(ep, "plot(normpdf(X,mean(pbdata),std(pbdata)),'r-')");
    
    engEvalString(ep, "subplot(2,3,4);");
    engEvalString(ep, "hnr=hist(nrdata,255);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "plot(hnr./length(nrdata),'b-')");
    engEvalString(ep, "plot(normpdf(X,mean(nrdata),std(nrdata)),'r-')");

    engEvalString(ep, "subplot(2,3,5);");
    engEvalString(ep, "hng=hist(ngdata,255);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "plot(hng./length(ngdata),'b-')");
    engEvalString(ep, "plot(normpdf(X,mean(ngdata),std(ngdata)),'r-')");    
    engEvalString(ep, "subplot(2,3,6);");
    engEvalString(ep, "hnb=hist(nbdata,255);");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "plot(hnb./length(nbdata),'b-')");
    engEvalString(ep, "plot(normpdf(X,mean(nbdata),std(nbdata)),'r-')");
    printf("Hit return to exit\n\n");
                fgetc(stdin); 
    
    }
      }
    tableau_->deselect_all();
  }
    if (e.type == vgui_KEY_PRESS && e.key == 'j' && vgui_SHIFT) {
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_selected_soviews();
    vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq=bvis1_manager::instance()->tableau_sequence();
    vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter;
    vcl_vector<vgui_soview*>  selected_objects;
    vcl_map<int,vcl_vector<dbctrk_soview2D* > >::iterator curveiter;
    curr_tableau_=tableau_;
    for(int j=0;j<seq.size();j++)
       {    
            for(iter=seq[j].begin();iter!=seq[j].end();iter++)
            {
                if(set_tableau((*iter).second))
                { 
                    all_objects=tableau_->get_selected_soviews();
                    for(int i=0;i<all_objects.size();i++)
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

   
    engEvalString(ep, "style1='b-'");
    engEvalString(ep, "style2='r-'");
    engEvalString(ep, "hold on;");
    engEvalString(ep, "numbin=20;");
    engEvalString(ep, "cnt=1;");
    engEvalString(ep, "X=1:numbin;");
    engEvalString(ep, "pr(1:2,X)=0;");
    engEvalString(ep, "pg(1:2,X)=0;");
    engEvalString(ep, "pb(1:2,X)=0;");
    engEvalString(ep, "nr(1:2,X)=0;");
    engEvalString(ep, "ng(1:2,X)=0;");
    engEvalString(ep, "nb(1:2,X)=0;");
    if(selected_objects.size()==2)
      {
  for(int i=0;i<selected_objects.size();i++)
    {
  if(((dbctrk_soview2D*)selected_objects[i])->type_name() == "dbctrk_soview2D")
    {
      dbctrk_tracker_curve_sptr c=((dbctrk_soview2D*)selected_objects[i])->dbctrk_sptr();
      double *prdata=new double[c->desc->Prcolor.size()];
      double *pgdata=new double[c->desc->Pgcolor.size()];
      double *pbdata=new double[c->desc->Pbcolor.size()];

      double *nrdata=new double[c->desc->Nrcolor.size()];
      double *ngdata=new double[c->desc->Ngcolor.size()];
      double *nbdata=new double[c->desc->Nbcolor.size()];
      vcl_cout<<"\n the size of the color array is "<<c->desc->Prcolor.size();
      vcl_cout<<"\n";
      engEvalString(ep, "clear prdata pgdata pbdata nrdata ngdata nbdata;");
      for(int i=0;i<c->desc->Prcolor.size();i++)
        {
     
                prdata[i]=c->desc->Prcolor[i];
                pgdata[i]=c->desc->Pgcolor[i];
                pbdata[i]=c->desc->Pbcolor[i];

                nrdata[i]=c->desc->Nrcolor[i];
                ngdata[i]=c->desc->Ngcolor[i];
                nbdata[i]=c->desc->Nbcolor[i];
        }
      prepare_vector("prdata",prdata,c->desc->Prcolor.size(),ep);
      prepare_vector("pgdata",pgdata,c->desc->Prcolor.size(),ep);
      prepare_vector("pbdata",pbdata,c->desc->Prcolor.size(),ep);
         
      prepare_vector("nrdata",nrdata,c->desc->Nrcolor.size(),ep);
      prepare_vector("ngdata",ngdata,c->desc->Nrcolor.size(),ep);
      prepare_vector("nbdata",nbdata,c->desc->Nrcolor.size(),ep);


      engEvalString(ep, "subplot(2,3,1);");
      engEvalString(ep, "hpr=hist(prdata,numbin);");
      engEvalString(ep, "pr(cnt,X)=hpr./length(prdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hpr./length(prdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(prdata)./numbin,std(prdata )./numbin),style2);");

      engEvalString(ep, "subplot(2,3,2);");
      engEvalString(ep, "hpg=hist(pgdata,numbin);");
      engEvalString(ep, "pg(cnt,X)=hpg./length(pgdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hpg./length(pgdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(pgdata)./numbin,std(pgdata)./numbin),style2);");    

      engEvalString(ep, "subplot(2,3,3);");
      engEvalString(ep, "hpb=hist(pbdata,numbin);");
      engEvalString(ep, "pb(cnt,X)=hpb./length(pbdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hpb./length(pbdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(pbdata)./numbin,std(pbdata)./numbin),style2);");
      
      engEvalString(ep, "subplot(2,3,4);");
      engEvalString(ep, "hnr=hist(nrdata,numbin);");
      engEvalString(ep, "nr(cnt,X)=hnr./length(nrdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hnr./length(nrdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(nrdata)./numbin,std(nrdata)./numbin),style2);");
      
      engEvalString(ep, "subplot(2,3,5);");
      engEvalString(ep, "hng=hist(ngdata,numbin);");
      engEvalString(ep, "ng(cnt,X)=hng./length(ngdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hng./length(ngdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(ngdata)./numbin,std(ngdata)./numbin),style2);");    
      engEvalString(ep, "subplot(2,3,6);");
      engEvalString(ep, "hnb=hist(nbdata,numbin);");
      engEvalString(ep, "nb(cnt,X)=hnb./length(nbdata);");
      engEvalString(ep, "hold on;");
      engEvalString(ep, "plot(hnb./length(nbdata),style1);");
      engEvalString(ep, "plot(normpdf(X,mean(nbdata)./numbin,std(nbdata)./numbin),style2);");
      
      engEvalString(ep, "style1='g-';");
      engEvalString(ep, "style2='c-';");
      engEvalString(ep, "cnt=cnt+1;");
      

      delete[] prdata;
      delete[] pgdata;
      delete[] pbdata;

      delete[] nrdata;
      delete[] ngdata;
      delete[] nbdata;

    
    }

      }
      }


     if(selected_objects.size()==2)
      {
  if(((dbctrk_soview2D*)selected_objects[0])->type_name() == "dbctrk_soview2D" &&
     ((dbctrk_soview2D*)selected_objects[1])->type_name() == "dbctrk_soview2D")
    {
      dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)selected_objects[0])->dbctrk_sptr();
      dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)selected_objects[1])->dbctrk_sptr();
      
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

      
      vcl_cout<<" Dpr = "<<dist2pdf(hist1pr,hist2pr)
        <<" Dpg = "<<dist2pdf(hist1pg,hist2pg)
        <<" Dpb = "<<dist2pdf(hist1pb,hist2pb)
        <<" Dnr = "<<dist2pdf(hist1nr,hist2nr)
        <<" Dnb = "<<dist2pdf(hist1ng,hist2ng)
        <<" Dng = "<<dist2pdf(hist1nb,hist2nb);
      vcl_cout<<"\n";
    }
      }
    tableau_->deselect_all();
  }
  //: tool to plot a intensity profile of matched curves using matlab
  if (e.type == vgui_KEY_PRESS && e.key == 's' ) {
    vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq=bvis1_manager::instance()->tableau_sequence();
    vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter;
    vcl_vector<vgui_soview*>  all_objects;
    vcl_vector<vgui_soview*>  selected_objects;
    vcl_map<int,vcl_vector<dbctrk_soview2D* > >::iterator curveiter;
    curr_tableau_=tableau_;
    for(int j=0;j<seq.size();j++)
       {    
            for(iter=seq[j].begin();iter!=seq[j].end();iter++)
            {
                if(set_tableau((*iter).second))
                { 
                    all_objects=tableau_->get_selected_soviews();
                    for(int i=0;i<all_objects.size();i++)
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

   
    vgui_dialog m_dlg("Match curves");
    static bool reverse=false;
    static bool onesided=false;
    static int alpha=0;
    static int beta=0;
    static int eta=1;
    static int zeta=1;
    static int theta=1;
    static int gamma=1;
    static int delta=1;
    m_dlg.checkbox("Reverse",reverse);
    m_dlg.checkbox("Onesided",onesided);
    m_dlg.field("alpha(length)",alpha);
    m_dlg.field("beta(Curvature)",beta);
    m_dlg.field("Gamma (epipole)",gamma);
    m_dlg.field("deltaa (Stereo)",delta);
    m_dlg.field("eta(I)",eta);
    m_dlg.field("zeta(H)",zeta);
    m_dlg.field("theta(S)",theta);


    if(!m_dlg.ask())
        return false;

    vcl_map<int,int> mapping;
    vnl_matrix <double> R;
    vnl_matrix <double> Tbar;              
    tableau_=curr_tableau_;

    if(selected_objects.size()==2)
    {
           if(((dbctrk_soview2D*)selected_objects[0])->type_name()=="dbctrk_soview2D" &&
              ((dbctrk_soview2D*)selected_objects[1])->type_name()=="dbctrk_soview2D")
           {
               dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)selected_objects[0])->dbctrk_sptr();
               dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)selected_objects[1])->dbctrk_sptr();
         vgl_point_2d<double> e;
         match_data_sptr m;
         curveMatch(c1->desc,c2->desc,e,m,reverse,alpha,beta,gamma,delta,eta,zeta,theta,onesided);

                if(!reverse)
                {
                  vcl_vector<vgl_point_2d<double> > points= c2->desc->curve_->pointarray(); 
      vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(points);
                    bgui_vsol_soview2D_edgel_curve pcurve1(dc1);
                    neighbor_style_->rgba[0] =1 ;
                    neighbor_style_->rgba[1] =1;
                    neighbor_style_->rgba[2] =0;

                    neighbor_style_->apply_all();
                    tableau_->add_edgel_curve(dc1);
                
                    vcl_map<int,int>::iterator iter;
                    for(iter=mapping.begin();iter!=mapping.end();iter++)

                   {
                    vsol_point_2d_sptr p1=new vsol_point_2d(c1->desc->curve_->point((*iter).first));
                    vsol_point_2d_sptr p2=new vsol_point_2d(c2->desc->curve_->point((*iter).second));

                    vsol_line_2d_sptr l= new vsol_line_2d(p1,p2);
                    vgui_style_sptr sty=vgui_style::new_style(0.0,0.0,0.0,2.0,3.0);
                    tableau_->add_vsol_line_2d(l,sty);
                   }
                tableau_->post_redraw();
                }
   
            }
           else 
           {
               tableau_->deselect_all();
               return false;
           }   
          
      }
 
    tableau_->deselect_all();
  }

  if (e.type == vgui_KEY_PRESS && e.key == 'm' && vgui_SHIFT ) {
       vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq=bvis1_manager::instance()->tableau_sequence();
       vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter;
       vcl_vector<vgui_soview*>  all_objects;
       vcl_vector<vgui_soview*>  selected_objects;
       vcl_map<int,vcl_vector<dbctrk_soview2D* > >::iterator curveiter;
       curr_tableau_=tableau_;
   
       for(int j=0;j<seq.size();j++)
       {    
            for(iter=seq[j].begin();iter!=seq[j].end();iter++)
            {
                if(set_tableau((*iter).second))
                { 
                    all_objects=tableau_->get_selected_soviews();
                    for(int i=0;i<all_objects.size();i++)
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

   
    vgui_dialog m_dlg("Match curves");

    static bool reverse=false;
    static int alpha=0;
    static int beta=0;
    static int eta=1;

    m_dlg.checkbox("Reverse",reverse);
    m_dlg.field("alpha",alpha);
    m_dlg.field("beta",beta);
    m_dlg.field("eta",eta);

    if(!m_dlg.ask())
        return false;
    vcl_map<int,int> mapping;
    vnl_matrix <double> R;
    vnl_matrix <double> Tbar;              
    tableau_=curr_tableau_;
    if(selected_objects.size()==2)
    {
           if(((dbctrk_soview2D*)selected_objects[0])->type_name()=="dbctrk_soview2D" &&
              ((dbctrk_soview2D*)selected_objects[1])->type_name()=="dbctrk_soview2D")
           {
               dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)selected_objects[0])->dbctrk_sptr();
               dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)selected_objects[1])->dbctrk_sptr();
//               IcurveMatch(c1,c2,mapping,R,Tbar,scale,alpha,beta,eta,reverse);

                if(!reverse)
                {
              
                    double *prdata1=new double[mapping.size()]; double *prdata2=new double[mapping.size()];
                    double *pgdata1=new double[mapping.size()]; double *pgdata2=new double[mapping.size()];
                    double *pbdata1=new double[mapping.size()]; double *pbdata2=new double[mapping.size()];

                    double *nrdata1=new double[mapping.size()]; double *nrdata2=new double[mapping.size()];
                    double *ngdata1=new double[mapping.size()]; double *ngdata2=new double[mapping.size()];
                    double *nbdata1=new double[mapping.size()]; double *nbdata2=new double[mapping.size()];

                    vcl_map<int,int>::iterator iter;
                    int i=0;
                    for(iter=mapping.begin();iter!=mapping.end();iter++,i++)
                    {
                      prdata1[i]=c1->desc->Prcolor[(*iter).first]; prdata2[i]=c2->desc->Prcolor[(*iter).second];
                      pgdata1[i]=c1->desc->Pgcolor[(*iter).first]; pgdata2[i]=c2->desc->Pgcolor[(*iter).second];
                      pbdata1[i]=c1->desc->Pbcolor[(*iter).first]; pbdata2[i]=c2->desc->Pbcolor[(*iter).second];

                      nrdata1[i]=c1->desc->Nrcolor[(*iter).first];  nrdata2[i]=c2->desc->Nrcolor[(*iter).second];
                      ngdata1[i]=c1->desc->Ngcolor[(*iter).first];  ngdata2[i]=c2->desc->Ngcolor[(*iter).second];
                      nbdata1[i]=c1->desc->Nbcolor[(*iter).first];  nbdata2[i]=c2->desc->Nbcolor[(*iter).second];
                    }
                    
                    prepare_vector("prdata1",prdata1,mapping.size(),ep);prepare_vector("prdata2",prdata2,mapping.size(),ep);
                    prepare_vector("pgdata1",pgdata1,mapping.size(),ep);prepare_vector("pgdata2",pgdata2,mapping.size(),ep);
                    prepare_vector("pbdata1",pbdata1,mapping.size(),ep);prepare_vector("pbdata2",pbdata2,mapping.size(),ep);
                                                                        
                    prepare_vector("nrdata1",nrdata1,mapping.size(),ep);prepare_vector("nrdata2",nrdata2,mapping.size(),ep);
                    prepare_vector("ngdata1",ngdata1,mapping.size(),ep);prepare_vector("ngdata2",ngdata2,mapping.size(),ep);
                    prepare_vector("nbdata1",nbdata1,mapping.size(),ep);prepare_vector("nbdata2",nbdata2,mapping.size(),ep);

                    engEvalString(ep, "figure;"); 
                   
                   engEvalString(ep, "subplot(2,3,1);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(prdata1,'r');");
                   engEvalString(ep, "plot(prdata2,'b');");
                   engEvalString(ep, "subplot(2,3,2);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(pgdata1,'r');");
                   engEvalString(ep, "plot(pgdata2,'b');");
                   engEvalString(ep, "subplot(2,3,3);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(pbdata1,'r');");
                   engEvalString(ep, "plot(pbdata2,'b');");

                   engEvalString(ep, "subplot(2,3,4);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(nrdata1,'r');");
                   engEvalString(ep, "plot(nrdata2,'b');");
                   engEvalString(ep, "subplot(2,3,5);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(ngdata1,'r');");
                   engEvalString(ep, "plot(ngdata2,'b');");
                   engEvalString(ep, "subplot(2,3,6);");
                   engEvalString(ep, "hold on;");
                   engEvalString(ep, "plot(nbdata1,'r');");
                   engEvalString(ep, "plot(nbdata2,'b');");
                }
                else
                {
                    
                }
   
            }
           else 
           {
               tableau_->deselect_all();
               return false;
           }   
          
      }
    tableau_->deselect_all();
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
      return true;
  }

  if( e.type == vgui_DRAW_OVERLAY){
  
  if(!curr_curve_)
    return false;


  if(curr_curve_->frame_number==frame)
  {
    
    
    bgui_vsol_soview2D_digital_curve pcurve(curr_curve_->desc->curvepos);
    bgui_vsol_soview2D_digital_curve ncurve(curr_curve_->desc->curveneg);
    neighbor_style_->rgba[0] =1  ;
    neighbor_style_->rgba[1] =0;
    neighbor_style_->rgba[2] =0;

    neighbor_style_->apply_all();
    pcurve.draw();
    neighbor_style_->rgba[0] =0  ;
    neighbor_style_->rgba[1] =1;
    neighbor_style_->rgba[2] =0;

    neighbor_style_->apply_all();
    ncurve.draw();

  /*  for(int i=0;i<curr_curve_->desc->maskpos.size();i++)
    {
        for(int j=0;j<curr_curve_->desc->maskpos[i].size();j++)
        {
            bgui_vsol_soview2D_point p(curr_curve_->desc->maskpos[i][j]);
            vgui_style_sptr temp_style_ = vgui_style::new_style(0.0, 1.0, 0.0, 2.0, 2.0);
            tableau_->add_vsol_point_2d(curr_curve_->desc->maskpos[i][j],temp_style_);
        }
    }
    */

  }

    
  
 }
 
 return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_intensity_tool::name() const
{

  return "Intesnity inspector";

}

vcl_vector<double>  normhist(vcl_vector<double> data,double x_lo, double x_hi, int n_bins)
{
  mbl_histogram hist(x_lo,x_hi,n_bins);
  vcl_vector<double> normpdf;
  if(data.size()<=0)
    return normpdf;
  for(int i=0;i<data.size();i++)
    {
      hist.obs(data[i]);
    }
  vcl_vector<int> frequency=hist.frequency();
  for(int i=0;i<frequency.size();i++)
    {
      normpdf.push_back(double(frequency[i])/data.size());
    }
  return normpdf;
}



double dist2pdf(vcl_vector<double> pdf1, vcl_vector<double> pdf2)
{
  if(pdf1.size()!=pdf2.size())
    return -1;
  double sum=0;
  for(int i=0;i<pdf1.size();i++)
    {
      sum+=vcl_sqrt(pdf1[i]*pdf2[i]);
    }
  return vcl_pow(vcl_fabs(vcl_log(sum)),2);
}
void dbctrk_intensity_tool::prepare_vector(char matlab_name[], double data[], int dim1,Engine *ep) {
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
