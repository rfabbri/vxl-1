#include<dbctrk/dbctrk_stereo_matching.h>
#include<dbctrk/dbctrk_curve_matching.h>
#include<vgl/vgl_homg_point_2d.h>
#include<vgl/algo/vgl_homg_operators_2d.h>
#include<vgl/vgl_clip.h>
#include<vgl/vgl_distance.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

dbctrk_stereo_matching::dbctrk_stereo_matching(FMatrix F)
{
  F_=F;
}

void dbctrk_stereo_matching::set_curves1(vcl_vector<dbctrk_tracker_curve_sptr> c1)
{
  curves1_.clear();
  for(unsigned int i=0;i<c1.size();i++)
  {
    curves1_.push_back(c1[i])  ;
  }
}
void dbctrk_stereo_matching::set_curves2(vcl_vector<dbctrk_tracker_curve_sptr> c2)
{
  curves2_.clear();
  for(unsigned int i=0;i<c2.size();i++)
  {
    curves2_.push_back(c2[i])  ;
  }
}

void dbctrk_stereo_matching::get_curves1(vcl_vector<dbctrk_tracker_curve_sptr> & out1)
{

   out1.clear();
   for(unsigned int i=0;i<curves1_.size();i++)
     out1.push_back(curves1_[i]);

}
void dbctrk_stereo_matching::get_curves2(vcl_vector<dbctrk_tracker_curve_sptr> & out2)
{

   out2.clear();
   for(unsigned int i=0;i<curves2_.size();i++)
     out2.push_back(curves2_[i]);

}

void dbctrk_stereo_matching::stereo_matching()
{
  dbctrk_curve_matching mc;
  for(unsigned int i=0;i<curves2_.size();i++)
  {
    for(unsigned int j=0;j<curves1_.size();j++)
    {
      if(check_bounding_box_intersection(curves1_[j],curves2_[i]))
      {

           double euc,scale;
        vcl_map<int,int> mapping;
        vcl_vector<int> tail_old;
        vcl_vector<int> tail_new;
        vnl_matrix<double> R1,T1,Tbar;
        //vgl_point_2d<double> epipole_(0,0);
        double matching_cost=mc.match_DP(curves1_[j]->desc,curves2_[i]->desc,
                         mapping,euc,R1,T1,Tbar,scale,tail_old,
                         tail_new,F_);
        match_data_sptr temp=new match_data;
            temp->energy_=matching_cost;
            temp->cost_=matching_cost;
            temp->euc_=euc;
            temp->mapping_=mapping;
            temp->R_=R1;
            temp->T_=T1;
            temp->Tbar=Tbar;
            temp->tail1_=tail_old;
            temp->tail2_=tail_new;
            temp->scale_=scale;
            temp->match_curve_set.push_back(curves1_[j]);
             curves2_[i]->prev_.push_back(temp);
      }
    }
  }

  vcl_cout<<"\n In best matches";
  //mc.best_matches(&curves2_,&curves1_);
  bootstrap_matches(10);

}
void dbctrk_stereo_matching::bootstrap_matches(int level_curves)
{

  compute_spatial_distance(curves1_,spatiald1_);
  compute_spatial_distance(curves2_,spatiald2_);

  
  // building a distance matrix
  int I=curves2_.size();
  int A=curves1_.size();

  vcl_map<int,dbctrk_tracker_curve_sptr> c1;
  vcl_map<int,dbctrk_tracker_curve_sptr> c2;

  vcl_vector<int> seed1;
  vcl_vector<int> seed2;


  for(unsigned int i=0;i<curves2_.size();i++)
    c2[curves2_[i]->get_id()]=curves2_[i];

  for(unsigned int i=0;i<curves1_.size();i++)
    c1[curves1_[i]->get_id()]=curves1_[i];




  double **d = new double * [I];
  double **s = new double * [I];
  for(int i=0;i<A;i++)
  {
    d[i]= new double[A];
    s[i]= new double[A];
  }
  // initializing X matrix
  for(int i=0;i<I;i++)
  {
    for(int a=0;a<A;a++)
    {
      d[i][a]=1e5;
      s[i][a]=1e5;
    }
  }
  //vcl_cout<<"\n ....................";
  vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;

  iter_new=curves2_.begin();
  for (int i=0; iter_new!=curves2_.end(); iter_new++,i++)
    {
      for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
        {
      int a=(*iter_old)->match_curve_set[0]->get_id();
      d[i][a]=(*iter_old)->cost_;
    }
  }

  double minCost=1e5;
  int mini,mina;
  bool selection=true;
  int cnt=0;
  //for(int l=0;l<12;l++)

  while(selection)
  {
    mini=-1;
    mina=-1;
    minCost=1e5;
    selection=false;
    for(int i=0;i<I;i++)
    {
      for(int a=0;a<A;a++)
      {
        if(d[i][a]<minCost)
        {
          minCost=d[i][a];
          mini=i;
          mina=a;
        }

      }
    }

    if(minCost<1e4 && mini>=0 && mina>=0)
    {

      for(int k=0;k<A;k++)
        d[mini][k]=1e5;
      for(int k=0;k<I;k++)
        d[k][mina]=1e5;

      c2[mini]->ismatchedprev_=true;
            c1[mina]->ismatchednext_=true;

      seed1.push_back(mina);
      seed2.push_back(mini);

            selection=true;

      if (c1[mina]->match_id_>-1)
                c2[mini]->match_id_ = c1[mina]->match_id_;
            else
             {
                c1[mina]->match_id_=c2[mini]->get_id();
                c2[mini]->match_id_=c1[mina]->match_id_;
              }


      for (iter_old=curves2_[mini]->prev_.begin();iter_old!=curves2_[mini]->prev_.end();iter_old++)
      {
        if((*iter_old)->match_curve_set[0]->get_id()==mina)
        {
          curves2_[mini]->set_best_match_prev(*iter_old);
          match_data_sptr temp=new match_data(curves2_[mini],*iter_old);
          curves1_[mina]->set_best_match_next(temp);
        }
      }

    }
    cnt++;
  if(cnt==10)
  {

  for(unsigned int l=0;l<curves2_.size();l++)
  {
    double max=0;
    curves2_[l]->spatialsig.clear();
    for(unsigned int p=0;p<seed2.size();p++)
    {
      /*curves2_[l]->spatialsig[seed2[p]]=spatial_euclidean_dist(curves2_[l],curves2_[seed2[p]]);
      if(curves2_[l]->spatialsig[seed2[p]]>max)
        max=curves2_[l]->spatialsig[seed2[p]];*/
      curves2_[l]->spatialsig.push_back(spatial_euclidean_dist(curves2_[l],curves2_[seed2[p]]));
      if(curves2_[l]->spatialsig[p]>max)
        max=curves2_[l]->spatialsig[p];
    }
    if(max>0)
      for(unsigned int p=0;p<curves2_[l]->spatialsig.size();p++)
      {
        curves2_[l]->spatialsig[p]/=max;
      }
  }

  for(unsigned int l=0;l<curves1_.size();l++)
  {
    double max=0;
    curves1_[l]->spatialsig.clear();
    for(unsigned int p=0;p<seed1.size();p++)
    {
      /*curves1_[l]->spatialsig[seed1[p]]=spatial_euclidean_dist(curves1_[l],curves1_[seed1[p]]);
      if(curves1_[l]->spatialsig[seed1[p]]>max)
        max=curves1_[l]->spatialsig[seed1[p]];*/
      curves1_[l]->spatialsig.push_back(spatial_euclidean_dist(curves1_[l],curves1_[seed1[p]]));
      if(curves1_[l]->spatialsig[p]>max)
        max=curves1_[l]->spatialsig[p];
    }
    if(max>0)
      for(unsigned int p=0;p<curves1_[l]->spatialsig.size();p++)
      {
        curves1_[l]->spatialsig[p]/=max;
      }
  }


  // updating distance matrix
  double maxs=1e5;
  for(int i=0;i<I;i++)
    {
      for(int a=0;a<A;a++)
      {
        if(d[i][a]<1e5)
        {
          s[i][a]=spatial_sig_dist(curves2_[i],curves1_[a]);
          if(maxs>s[i][a])
            maxs=s[i][a];
        }

      }
    }
  for(int i=0;i<I;i++)
    {
      for(int a=0;a<A;a++)
      {
        if(d[i][a]<1e5)
        {
          d[i][a]=d[i][a]*(1+s[i][a]);
        }

      }
    }

  }
 }
  for (unsigned int i=0;i<curves2_.size();++i)
  {
    if (!curves2_[i]->ismatchedprev_)
    {
      curves2_[i]->match_id_ =-1;
      curves2_[i]->group_id_ =-1;
    }
  }


  free(d);
  free(s);

}
bool dbctrk_stereo_matching::check_bounding_box_intersection(dbctrk_tracker_curve_sptr c1,
                               dbctrk_tracker_curve_sptr c2)
{
  vsol_box_2d_sptr box1=c1->desc->cbox_;
  vsol_box_2d_sptr box2=c2->desc->cbox_;

  bool one2two=false;
  bool two2one=false;

  vgl_homg_point_2d<double> pl(box1->get_min_x(),box1->get_min_y());
  vgl_homg_line_2d<double> l21 = F_.image2_epipolar_line(pl);

  vgl_homg_point_2d<double> p2(box1->get_max_x(),box1->get_min_y());
  vgl_homg_line_2d<double> l22 = F_.image2_epipolar_line(p2);

  vgl_homg_point_2d<double> p3(box1->get_min_x(),box1->get_max_y());
  vgl_homg_line_2d<double> l23 = F_.image2_epipolar_line(p3);

  vgl_homg_point_2d<double> p4(box1->get_max_x(),box1->get_max_y());
  vgl_homg_line_2d<double> l24 = F_.image2_epipolar_line(p4);

  if(line_box_intersection(l21,box2)||line_box_intersection(l22,box2)||
     line_box_intersection(l23,box2)||line_box_intersection(l24,box2))
     one2two=true;

  vgl_homg_point_2d<double> p21(box2->get_min_x(),box2->get_min_y());
  vgl_homg_line_2d<double> l11 = F_.image1_epipolar_line(p21);

  vgl_homg_point_2d<double> p22(box2->get_max_x(),box2->get_min_y());
  vgl_homg_line_2d<double> l12 = F_.image1_epipolar_line(p22);

  vgl_homg_point_2d<double> p23(box2->get_min_x(),box2->get_max_y());
  vgl_homg_line_2d<double> l13 = F_.image1_epipolar_line(p23);

  vgl_homg_point_2d<double> p24(box2->get_max_x(),box2->get_max_y());
  vgl_homg_line_2d<double> l14 = F_.image1_epipolar_line(p24);

  if(line_box_intersection(l11,box1)||line_box_intersection(l12,box1)||
     line_box_intersection(l13,box1)||line_box_intersection(l14,box1))
     two2one=true;

  if(one2two || two2one)
    return true;
  else
    return false;


}
bool dbctrk_stereo_matching::line_box_intersection(vgl_homg_line_2d<double> l,
                        vsol_box_2d_sptr box)
{
//int sum=0;
double bx,by,ex,ey;
if(vgl_clip_line_to_box<double>(l.a(), l.b(), l.c(),
   box->get_min_x(),box->get_min_y(),box->get_max_x(),box->get_max_y(),bx, by, ex, ey))
              return true;

/*if(line_segment_intersection(l,box->get_min_x(),box->get_min_x(),box->get_min_y(),box->get_max_y()))
   sum+=1;
if(line_segment_intersection(l,box->get_min_x(),box->get_max_x(),box->get_min_y(),box->get_min_y()))
   sum+=1;
if(line_segment_intersection(l,box->get_min_x(),box->get_max_x(),box->get_max_y(),box->get_max_y()))
   sum+=1;
if(line_segment_intersection(l,box->get_max_x(),box->get_max_x(),box->get_min_y(),box->get_max_y()))
   sum+=1;
*/
//  if(sum==2)
//    return true;
  else
    return false;


}

bool dbctrk_stereo_matching::line_segment_intersection(vgl_homg_line_2d<double> l,
                             double xs,double xe,
                             double ys, double ye)
{
  vgl_homg_point_2d<double> p1(xs,ys);
  vgl_homg_point_2d<double> p2(xe,ye);

  vgl_homg_line_2d<double> l2(p1,p2);

  vgl_homg_point_2d<double> p= vgl_homg_operators_2d<double> ::intersection(l,l2);

  if(p.x()/p.w()>=xs && p.x()/p.w()<=xe && p.y()/p.w()>=ys && p.y()/p.w()<=ye)
  {
    return true;
  }
  else
    return false;


}


double dbctrk_stereo_matching::spatial_euclidean_dist(dbctrk_tracker_curve_sptr c1,dbctrk_tracker_curve_sptr c2)
{
  double min=1e5;
  for(unsigned int i=0;i<c1->desc->cms_.size();i++)
  {
    for(unsigned int j=0;j<c2->desc->cms_.size();j++)
    {
      double dist=vgl_distance<double>(c1->desc->cms_[i],
                       c2->desc->cms_[j]);
      if(min>dist)
        min=dist;
    }
  }

  return min;
}
void dbctrk_stereo_matching::compute_spatial_distance(vcl_vector<dbctrk_tracker_curve_sptr> curves,
                        vcl_map<vcl_pair<int,int>, double > & spatiald)
{
  spatiald.clear();
  for(unsigned int i=0;i<curves.size()-1;i++)
  {
    for(unsigned int j=i+1;j<curves.size();j++)
    {
      spatiald[vcl_make_pair(i,j)]=spatial_euclidean_dist(curves[i],curves[j]);

    }
  }
}

double dbctrk_stereo_matching::spatial_sig_dist(dbctrk_tracker_curve_sptr c1,dbctrk_tracker_curve_sptr c2)
{
  double dist=0;
  for(unsigned int i=0;i<c1->spatialsig.size()  && i<c2->spatialsig.size();i++)
  {
    dist+=(c1->spatialsig[i]-c2->spatialsig[i])*(c1->spatialsig[i]-c2->spatialsig[i]);
  }

  return vcl_sqrt(dist);
}
