#include <dbctrk/vis/dbctrk_patch_inspector.h>
#include <bvis1/bvis1_manager.h>
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
#include <dbctrk/dbctrk_curveMatch.h> 
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_selector_tableau.h>
#include <vgui/vgui_selector_tableau_sptr.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
//#include <brct/brct_algos.h>

dbctrk_patch_inspector::dbctrk_patch_inspector()
{

    
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}

dbctrk_patch_inspector::~dbctrk_patch_inspector()
{


}

bool
dbctrk_patch_inspector::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_patch_inspector::set_storage ( const bpro1_storage_sptr& storage)
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
dbctrk_patch_inspector::tableau()
{
  return tableau_;
}

dbctrk_storage_sptr
dbctrk_patch_inspector::storage()
{
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage_);
  return dbctrk_storage;
}

bool
dbctrk_patch_inspector::handle(const vgui_event & e, 
           const bvis1_view_tableau_sptr& view  )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  
  if (e.type == vgui_KEY_PRESS && e.key == 's' ) {
     vgui_tableau_sptr t=view->selector()->get_tableau("image0");
    vcl_cout<<t->type_name();
    vgui_image_tableau_sptr imgt;
    imgt.vertical_cast(t);
    vil_image_view_base_sptr img_base=imgt->get_image_view();
    vil_image_view<unsigned char> img(img_base);
    vcl_cout<<"\n"<<img_base->ni()<<","<<img_base->nj();
    vcl_cout<<"\n"<<img.ni()<<","<<img.nj();
    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_selected_soviews();
    if(all_objects.size()>=1)
      {
  if(((dbctrk_soview2D*)all_objects[0])->type_name() == "dbctrk_soview2D")
    {
      curr_curve_=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();

      double min_dist =1e6;
      int min_index=-1;
      for(int i=0;i<curr_curve_->desc->curve_->numPoints();i++)
        {
    double dist=vcl_pow(curr_curve_->desc->curve_->point(i).x()-ix,2)+
      vcl_pow(curr_curve_->desc->curve_->point(i).y()-iy,2);
    if(dist<min_dist)
      {
        min_dist=dist;
        min_index=i;
      }
        }
      if(min_index>=0)
        {
    vcl_vector<vsol_point_2d_sptr> pos_points;
    vcl_vector<vsol_point_2d_sptr> neg_points;
    
    vcl_vector<vnl_double_3> pqL;
    vcl_vector<vnl_double_3> pqI;
    for(unsigned int i=0;i< curr_curve_->desc->maskpos[min_index].size();i++)
      {
        if(curr_curve_->desc->isvalidmaskpos[min_index][i])
          {
      vgui_style_sptr p=vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
      tableau_->add_vsol_point_2d(curr_curve_->desc->maskpos[min_index][i],p);

      int r=(int)img((int)curr_curve_->desc->maskpos[min_index][i]->x(), 
               (int)curr_curve_->desc->maskpos[min_index][i]->y(),0);
      int g=(int)img((int)curr_curve_->desc->maskpos[min_index][i]->x(),
               (int)curr_curve_->desc->maskpos[min_index][i]->y(),1);
      int b=(int)img((int)curr_curve_->desc->maskpos[min_index][i]->x(),
             (int)curr_curve_->desc->maskpos[min_index][i]->y(),2);
      double l,a,B;
      double i,h,s;
      
      utils::rgb2lab(((double)r),((double)g),((double)b),l,a,B);
      utils::rgb_to_ihs(((double)r),((double)g),((double)b),i,h,s);
      vcl_cout<<"RGB="<<r<<","<<g<<","<<b<<"\t"
              <<"IHS="<<i<<","<<h<<","<<s<<"\t"
        <<"LAB="<<l<<","<<a<<","<<B<<"\n";
      pqL.push_back(vnl_double_3(l,a,B));
      pqI.push_back(vnl_double_3(i,h,s));
          }
      }
    vnl_double_3 PmeanL=mean3d(pqL);
    vnl_double_3 PstdL=std3d(pqL);
    vecmeanL.push_back(PmeanL);
    vecstdL.push_back(PstdL);
    vnl_double_3 PmeanI=mean3d(pqI);
    vnl_double_3 PstdI=std3d(pqI);
    vecmeanI.push_back(PmeanI);
    vecstdI.push_back(PstdI);
    vectype.push_back("pos");
    vcl_cout<<"\n";
      pqL.clear();
      pqI.clear();
    for(unsigned int i=0;i< curr_curve_->desc->maskneg[min_index].size();i++)
      {
        if(curr_curve_->desc->isvalidmaskneg[min_index][i])

          {
      vgui_style_sptr p=vgui_style::new_style(0.0,1.0,0.0,2.0,2.0);
      tableau_->add_vsol_point_2d(curr_curve_->desc->maskneg[min_index][i],p);

      int r=(int)img((int)curr_curve_->desc->maskneg[min_index][i]->x(), 
               (int)curr_curve_->desc->maskneg[min_index][i]->y(),0);
      int g=(int)img((int)curr_curve_->desc->maskneg[min_index][i]->x(),
               (int)curr_curve_->desc->maskneg[min_index][i]->y(),1);
      int b=(int)img((int)curr_curve_->desc->maskneg[min_index][i]->x(),
             (int)curr_curve_->desc->maskneg[min_index][i]->y(),2);
      double l,a,B;
      double i,h,s;
      
      utils::rgb2lab(((double)r),((double)g),((double)b),l,a,B);
      utils::rgb_to_ihs(((double)r),((double)g),((double)b),i,h,s);
      vcl_cout<<"RGB="<<r<<","<<g<<","<<b<<"\t"
              <<"IHS="<<i<<","<<h<<","<<s<<"\t"
        <<"LAB="<<l<<","<<a<<","<<B<<"\n";
      pqL.push_back(vnl_double_3(l,a,B));
      pqI.push_back(vnl_double_3(i,h,s));
          }
      }
    vnl_double_3 NmeanL=mean3d(pqL);
    vnl_double_3 NstdL=std3d(pqL);
    vecmeanL.push_back(NmeanL);
    vecstdL.push_back(NstdL);
    vnl_double_3 NmeanI=mean3d(pqI);
    vnl_double_3 NstdI=std3d(pqI);
    vecmeanI.push_back(NmeanI);
    vecstdI.push_back(NstdI);
    vectype.push_back("neg");

    vcl_map<vcl_string,vcl_vector<vnl_double_3> > ::iterator itr;
    vcl_cout<<"\n IHS: query is p: "<<PmeanI<<" n: "<<NmeanI;
    for(itr=maptypeI.begin();itr!=maptypeI.end();itr++)
      {
        vnl_double_3 classmean=mean3d((*itr).second);
        vnl_double_3 classstd=std3d((*itr).second);
        vcl_cout<<"\n class "<<(*itr).first<<" mean "<<classmean<<" std "<<classstd;
        if(doesbelong(classmean,classstd,NmeanI))
        {
          vcl_cout<<"\n Negative side belongs to class in IHS"<<(*itr).first;
        }
        if(doesbelong(classmean,classstd,PmeanI))
        {
          vcl_cout<<"\n Positive side belongs to class in IHS"<<(*itr).first;
        }
           
           
      }

    vcl_cout<<"\n LAB: query is p: "<<PmeanL<<" n: "<<NmeanL;
    for(itr=maptypeL.begin();itr!=maptypeL.end();itr++)
      {
        vnl_double_3 classmean=mean3d((*itr).second);
        vnl_double_3 classstd=std3d((*itr).second);
        vcl_cout<<"\n class "<<(*itr).first<<" mean "<<classmean<<" std "<<classstd;
        if(doesbelong(classmean,classstd,NmeanL))
        {
          vcl_cout<<"\n Negative side belongs to class in LAB"<<(*itr).first;
        }
        if(doesbelong(classmean,classstd,PmeanL))
        {
          vcl_cout<<"\n Positive side belongs to class in LAB"<<(*itr).first;
        }
           
           
      }

    tableau_->deselect_all();
    return true;
    }
      }
      }
    
    tableau_->deselect_all();
  }

  //: develop the database
 if (e.type == vgui_KEY_PRESS && e.key == 'l' && vgui_SHIFT) {
   
   vgui_dialog filedlg("Path for patches dir");
   static vcl_string directory="";
   static vcl_string ext="";

   filedlg.file("Path: ",ext,directory);
   if(!filedlg.ask())
     return true;
   if(! vul_file::is_directory(directory))
    {
      vcl_cout<<"\n The input directory does not exist";
      return true;
    }
   vcl_string filenames=directory+"/*.txt";
   vcl_cout<<filenames;
   vecmeanI.clear();
   vecstdI.clear();
   vecmeanL.clear();
   vecstdL.clear();
   vectype.clear();
   maptypeL.clear();
   maptypeI.clear();

   for (vul_file_iterator fn=filenames; fn; ++fn) {

     vcl_cout<<"\n "<<fn();
     vcl_string type;
     vnl_double_3 meanI,meanL;
     vnl_double_3 stdI,stdL;
     readpatchinfo(fn(),type,meanI, stdI,true);
     vcl_cout<<"\n meanI"<<meanI;
     readpatchinfo(fn(),type,meanL, stdL,false);
     vecmeanI.push_back(meanI);
     vecstdI.push_back(stdI);
     vecmeanL.push_back(meanL);
     vecstdL.push_back(stdL);
     vectype.push_back(type);
     if(maptypeL.find(type)!=maptypeL.end())
       maptypeL[type].push_back(meanL);
     else
       {
   vcl_vector<vnl_double_3> temp;
   maptypeL[type]=temp;
   maptypeL[type].push_back(meanL);
       }

     if(maptypeI.find(type)!=maptypeI.end())
       maptypeI[type].push_back(meanI);
     else
       {
   vcl_vector<vnl_double_3> temp;
   maptypeI[type]=temp;
   maptypeI[type].push_back(meanI);
       }
     
   }
 }

  return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_patch_inspector::name() const
{

  return "Patch inspector";
 
}


void dbctrk_patch_inspector::readpatchinfo(char const* filename, vcl_string &type, vnl_double_3 & mean, vnl_double_3 &std,bool istrue)
{
  vcl_cout<<"\n reading file";
  vcl_ifstream ifile(filename);
  int cx,cy;
  vcl_string parentfilename;
  char buffer1[100],buffer2[100];
  ifile>>buffer1>>cx>>cy>>buffer2;
  double r,g,b;
  vcl_vector<vnl_double_3> rgbs;
  vcl_vector<vnl_double_3> ihss;
  vcl_vector<vnl_double_3> labs;
  type=vcl_string(buffer1);
  while(!ifile.eof())
    {
      ifile>>r>>g>>b;
      rgbs.push_back(vnl_double_3(r,g,b));
      double i,h,s;
      utils::rgb_to_ihs( r, g, b,i,h,s);
      ihss.push_back(vnl_double_3(i,h,s));
      //vcl_cout<<"IHS"<<i<<","<<h<<","<<s<<"\n";
      //vcl_cout<<"RGB"<<r<<","<<g<<","<<b<<"\n";
      double L=0.0,A=0.0,B=0.0;
      utils::rgb2lab( r, g,b,L,A,B);
      labs.push_back(vnl_double_3(L,A,B));

    }
  if(istrue)
    {
      mean=mean3d(ihss);
      std=std3d(ihss);
      vcl_cout<<"\n IHS";
    }
  else
    {
      mean=mean3d(labs);
      std=std3d(labs);
      vcl_cout<<"\n LAb";
    }
  vcl_cout<<"\n the mean is"<<mean<<"\t"<<std;
  ifile.close();
  
}

vnl_double_3 dbctrk_patch_inspector::mean3d(vcl_vector<vnl_double_3> pts)
{
  vnl_double_3 mean(0.0,0.0,0.0);
  for(unsigned int i=0;i<pts.size();i++)
    {
      mean[0]+=pts[i][0];
      mean[1]+=pts[i][1];
      mean[2]+=pts[i][2];
    }
  if(pts.size()>0)
    {
      mean[0]/=pts.size();
      mean[1]/=pts.size();
      mean[2]/=pts.size();
    }
  return mean;
}
vnl_double_3 dbctrk_patch_inspector::std3d(vcl_vector<vnl_double_3> pts)
{
  vnl_double_3 mean=mean3d(pts);
  vnl_double_3 std(0.0,0.0,0.0);
  for(unsigned int i=0;i<pts.size();i++)
    {
      vnl_double_3 temp=(pts[i]-mean);
      temp[0]*=temp[0];
      temp[1]*=temp[1];
      temp[2]*=temp[2];
      
      std+=temp;
      if(pts.size()>0)
   {
     std[0]/=pts.size();
     std[1]/=pts.size();
     std[2]/=pts.size();
  }
      std[0]=vcl_sqrt(std[0]);
      std[1]=vcl_sqrt(std[1]);
      std[2]=vcl_sqrt(std[2]);

    }
 return std;
}
bool dbctrk_patch_inspector::doesbelong(vnl_double_3 cmean,vnl_double_3 cstd,vnl_double_3 p)
{
  if(vcl_fabs(p[0]-cmean[0])<3*cstd[0] &&
     vcl_fabs(p[1]-cmean[1])<3*cstd[1] &&
     vcl_fabs(p[2]-cmean[2])<3*cstd[2] )
    {
      return true;
    }
  return false;
}
int dbctrk_patch_inspector::return_color(vcl_string type)
{


  if(type=="leaves")
      return 0;
  else if(type=="road")
      return 1;
  else if(type=="treebark")
      return 2;
  else if(type=="grass")
    return 3;
  else if(type=="sidewalk")
    return 4;
  else if(type=="pos")
    return 5;
  else if(type=="neg")
    return 6;
  else if(type=="brick")
    return 7;
  else
    return -1;
}
