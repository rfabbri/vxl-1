#include "dbctrk_curve_matching.h"

#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_ostream.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_tracker_curve.h>
#include <dbctrk/dbctrk_curveMatch.h>
#include <vgl/vgl_distance.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_distance.h>



dbctrk_curve_matching_params::dbctrk_curve_matching_params()
{
 motion_in_pixels=0;
 no_of_top_choices=0;
 matching_=true;
 //e_=mp.e_;
 alpha_ =   1 ;
 beta_  =   1  ;
 gamma_ =   0 ;
 delta_ =   0 ;
 eta_   =   0  ;
 zeta_  =   0 ;
 theta_ =   0 ;

onesided=false;
tauihs_=0.45f;
isintensity_=true;
    
}
 void dbctrk_curve_matching_params::operator=(dbctrk_curve_matching_params mp)
 {
     motion_in_pixels=mp.motion_in_pixels;
     no_of_top_choices=mp.no_of_top_choices;
     matching_=mp.matching_;
     e_=mp.e_;
     alpha_ =   mp.alpha_ ;
     beta_  =    mp.beta_  ;
     gamma_ =    mp.gamma_ ;
     delta_ =    mp.delta_ ;
     eta_   =    mp.eta_   ;
     zeta_  =    mp.zeta_  ;
     theta_ =    mp.theta_ ;
 
    onesided=mp.onesided;
    tauihs_=mp.tauihs_;
    isintensity_=mp.isintensity_;
    
 }
//external functions
vcl_ostream& operator<<(vcl_ostream& s, dbctrk_curve_matching_params const& params)
{
  s <<"Motion radius = "<<params.motion_in_pixels
    <<"\n Top Coarse matching Candidates ="<<params.no_of_top_choices
    <<"\n Matching ? = "<<params.matching_
    <<"\n Epipole = "<<params.e_
    <<"\n alpha (length coeff) ="<<params.alpha_
    <<"\n beta  (bending coeff) ="<<params.beta_
    <<"\n gamma (epipole coeff) ="<<params.gamma_
    <<"\n delta (stereo coeff)  ="<<params.delta_
    <<"\n eta (intensity coeff) ="<<params.eta_
    <<"\n zeta( Hue Coeff) ="<<params.zeta_
    <<"\n theta ( Saturation coeff) ="<<params.theta_
    <<"\n Onesided ? = "<<params.onesided
    <<"\n Thrshold for ihs "<<params.tauihs_;

  return s;
}
void dbctrk_curve_matching_params::print_summary(vcl_ostream &os) const
{
  os << *this;
}
struct less_val
{
  bool operator()(vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > x, 
          vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > y)
  { return x.second < y.second; }
};

dbctrk_curve_matching ::dbctrk_curve_matching()
{
 motion_in_pixels_=0;
 no_of_top_choices_=0;
}



void dbctrk_curve_matching :: initialize_matrix(vcl_vector<dbctrk_tracker_curve> new_curves,
                                              vcl_vector<dbctrk_tracker_curve> old_curves)
{
  for (unsigned int i=0;i<new_curves.size();++i)
  {
    for (unsigned int j=0;j<old_curves.size();++j)
    {
      match_data_sptr temp;
      temp=new match_data;
      temp->cost_=-1;
      temp->euc_=-1;
      temp->match_curve_set.push_back(&old_curves[j]);
      new_curves[i].prev_.push_back(temp);
    }
  }
}


void dbctrk_curve_matching :: match_prev_tail_curve(dbctrk_tracker_curve_sptr parent_curve,
                                                  dbctrk_tracker_curve_sptr &tail_curve,
                                                  vcl_vector<dbctrk_tracker_curve_sptr> * new_curves)
{
  
  for (unsigned int i=0;i<(*new_curves).size();++i)
    {
      match_data_sptr temp;
      if (!(*new_curves)[i]->ismatchedprev_)
  if (bounding_box_intersection((*new_curves)[i]->desc->cbox_, parent_curve->desc->cbox_))
    {
      temp=new match_data;
      temp->cost_=0;
      temp->match_curve_set.push_back((*new_curves)[i]);
      tail_curve->next_.push_back(temp);
    }
    }
  
  for (unsigned int i=0;i<tail_curve->next_.size();++i)
    {
    double euc,scale;
    vcl_map<int,int> mapping;
    vcl_vector<int> tail1,tail2;
    vnl_matrix<double> R1,T1,Tbar;
    vgl_point_2d<double> e;
    double matching_cost=match_DP(tail_curve->desc,
                                  tail_curve->next_[i]->match_curve_set[0]->desc,
                                  mapping,euc,R1,T1,Tbar,scale,tail1,tail2,e);
    
    tail_curve->next_[i]->cost_=euc;
    tail_curve->next_[i]->energy_=matching_cost;
    tail_curve->next_[i]->euc_=euc;
    tail_curve->next_[i]->mapping_=mapping;
    tail_curve->next_[i]->R_=R1;
    tail_curve->next_[i]->T_=T1;
    tail_curve->next_[i]->scale_=scale;
    tail_curve->next_[i]->Tbar=Tbar;
    tail_curve->next_[i]->tail1_=tail1;
    tail_curve->next_[i]->tail2_=tail2;
  }
  vcl_sort(tail_curve->next_.begin(),tail_curve->next_.end(),less_cost());

}

void dbctrk_curve_matching :: match_next_tail_curve(dbctrk_tracker_curve_sptr parent_curve,
                                                  dbctrk_tracker_curve_sptr &tail_curve,
                                                  vcl_vector<dbctrk_tracker_curve_sptr> * old_curves)
{
  for (unsigned int i=0;i<(*old_curves).size();++i)
    {
      if (!(*old_curves)[i]->ismatchednext_)
      if (bounding_box_intersection((*old_curves)[i]->desc->cbox_, parent_curve->desc->cbox_))
  {
    match_data_sptr temp=new match_data;
    temp->cost_=0;
    temp->match_curve_set.push_back((*old_curves)[i]);
    tail_curve->prev_.push_back(temp);
  }
  }
  for (unsigned int i=0;i<tail_curve->prev_.size();i++)
    {
      double scale=1.0;
      vcl_map<int,int> mapping;
      vcl_vector<int> tail1,tail2;
      vnl_matrix<double> R1,T1,Tbar;
      double matching_cost;
      if(tail_curve->desc->curve_->numPoints()>0)
  {
    double euc=match_DP(tail_curve->prev_[i]->match_curve_set[0]->desc,
            tail_curve->desc,mapping,matching_cost,R1,T1,Tbar,scale,tail1,tail2,epipole_);
    tail_curve->prev_[i]->cost_=euc;
    tail_curve->prev_[i]->euc_=euc;
    tail_curve->prev_[i]->R_=R1;
    tail_curve->prev_[i]->T_=T1;
    tail_curve->prev_[i]->scale_=scale;
    tail_curve->prev_[i]->Tbar=Tbar;
    tail_curve->prev_[i]->tail1_=tail1;
    tail_curve->prev_[i]->tail2_=tail2;
    tail_curve->prev_[i]->mapping_=mapping;
    tail_curve->prev_[i]->energy_=matching_cost;
  }
    }
  vcl_sort(tail_curve->prev_.begin(),tail_curve->prev_.end(),less_cost());
}

double dbctrk_curve_matching :: coarse_match_DP(dbctrk_curve_description * desc1,
                                              dbctrk_curve_description * desc2,
                                              vgl_point_2d<double> epi)
  
{
  
  vcl_vector<vgl_point_2d<double> > v1,v2;


  for (int i=0;i<desc1->curve_->numPoints(); i+=5)
       v1.push_back(desc1->curve_->point(i));
  for (int i=0;i<desc2->curve_->numPoints(); i+=5)
      v2.push_back(desc2->curve_->point(i));

  match_data_sptr mp=new match_data();
  vnl_matrix<double> R,T,Tbar,R1,T1,Tbar1;
  vcl_vector<int> tail_old, tail_new;
  vcl_map<int,int> alignment;
  //vgl_point_2d<double>ep1,ep2;
  //ep[1]=0;ep[2]=0;
  double dist1=curveMatch(v1,v2,mp,epipole_);
  vcl_reverse(v2.begin(),v2.end());
  double dist2=curveMatch(v1,v2,mp,epipole_);
  
  return dist1<dist2 ? dist1 : dist2;
  
}

//: function to observe the top n closures
bool dbctrk_curve_matching :: best_topn_matches(int n,
                vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                vcl_vector<dbctrk_tracker_curve_sptr> * old_curves,
                vcl_map<int,vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > > &transitives_)
{
  vcl_vector<match_data_sptr>::iterator minpiter;
  vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;
  transitives_.clear();
  // simply choosing best corresponding matches
  // giving preference to low cost matches
  
  if(n<0)
    return false;
  
  
  for(unsigned int i=0;i<(*new_curves).size();i++)
    {
      (*new_curves)[i]->isprevclosure_=-1;
    }
  for(unsigned int i=0;i<(*old_curves).size();i++)
  {
    (*old_curves)[i]->isnextclosure_=-1;
  }
  
  vcl_vector<vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > >  matchlist;
  vcl_vector<vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > >::iterator iterlist;
  
  bool selection=true;
  while (selection)
    {
      double min_cost=1e6;
      vcl_vector<dbctrk_tracker_curve_sptr>::iterator min_iter_new;
      dbctrk_tracker_curve_sptr min_iter_old;
      for (iter_new=(*new_curves).begin();iter_new!=(*new_curves).end();iter_new++)
     {
       if ((*iter_new)->isprevclosure_<0)
         for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
     {
       if ((*iter_old)->match_curve_set[0]->isnextclosure_<0)
             if (min_cost>(*iter_old)->cost_)
         {
     min_cost=(*iter_old)->cost_;
     min_iter_new=iter_new;
     min_iter_old=(*iter_old)->match_curve_set[0];
     minpiter=iter_old;
         }
     }
     }
      if (min_iter_new == (*new_curves).end() || ! ((bool)min_iter_old) )
  {
    selection=false;
  }
      else
  {
    
          (*min_iter_new)->isprevclosure_=1;
    min_iter_old->isnextclosure_=1;
    matchlist.push_back(vcl_make_pair(vcl_make_pair((*min_iter_new),min_iter_old),min_cost));
    
  }
    }
  
  vcl_sort(matchlist.begin(),matchlist.end(),less_val());
  
  for(int k=0;k<n;k++)
  {
    for(iterlist=matchlist.begin();iterlist!=matchlist.end();)
      {
  dbctrk_tracker_curve_sptr c1=(*iterlist).first.first;
  dbctrk_tracker_curve_sptr c2=(*iterlist).first.second;
  int flag=0;
  for( int i=0;i<(int)c1->prev_.size() && i<=k;i++)
    {
      if(c1->prev_[i]->match_curve_set[0]->get_id()==c2->get_id())
        {
    flag=1;
    transitives_[k].push_back(vcl_make_pair(c1,c2));
        }
       
    }
  if(flag)
    matchlist.erase(iterlist);
  else
    iterlist++;
      }
  }
  return true;
  
  
}
bool dbctrk_curve_matching :: greedy_and_closure(int n,
                 vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                 vcl_vector<dbctrk_tracker_curve_sptr> * old_curves)
{
  
  //: code to find the assignment using greedy and closure propoerty among pair of curves
  vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;
  
  if(n<0)
    return false;

  for(unsigned int i=0;i<(*new_curves).size();i++)
  {
   (*new_curves)[i]->isprevclosure_=-1;
  }
  for(unsigned int i=0;i<(*old_curves).size();i++)
  {
   (*old_curves)[i]->isnextclosure_=-1;
  }
  vcl_vector<vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > >  matchlist;
  vcl_vector<vcl_pair<vcl_pair<dbctrk_tracker_curve_sptr,dbctrk_tracker_curve_sptr>, double > >::iterator iterlist;

  for (iter_new=(*new_curves).begin();iter_new!=(*new_curves).end();iter_new++)
     {
         for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
         {
        matchlist.push_back(vcl_make_pair(vcl_make_pair((*iter_new),(*iter_old)->match_curve_set[0]),(*iter_old)->cost_));
         }
     }


  vcl_sort(matchlist.begin(),matchlist.end(),less_val());


  for(int k=0;k<n;k++)
  {
      vcl_cout<<"\n step "<<k;

    for(iterlist=matchlist.begin();iterlist!=matchlist.end();)
     {
       dbctrk_tracker_curve_sptr c1=(*iterlist).first.first;
       dbctrk_tracker_curve_sptr c2=(*iterlist).first.second;
       int flag=0;
       if(c1->isprevclosure_<0 &&  c2->isnextclosure_<0)
       {
        for(unsigned int i=0;i<c1->prev_.size() && i<=(unsigned int)k;i++)
        {
         if(c1->prev_[i]->match_curve_set[0]->get_id()==c2->get_id())
         {
            flag++;
         }
       }
        for(unsigned int i=0;i<c2->next_.size() && i<=(unsigned int)k;i++)
       {
        if(c2->next_[i]->match_curve_set[0]->get_id()==c1->get_id())
         {
            flag++;
         }

       }

       
       if(flag==2)
        {
         c1->isprevclosure_=1;
         c2->isnextclosure_=1;
         //: assigning the best match 
         if (c2->match_id_>-1)
                c1->match_id_ = c2->match_id_;
          else
               {
                   c1->match_id_=c1->get_id();
                   c2->match_id_=c1->match_id_;
                }
          for(unsigned int j=0;j<c1->prev_.size();j++)
          {
           if(c1->prev_[j]->match_curve_set[0]->get_id()==c2->get_id())
           {
             c1->set_best_match_prev(c1->prev_[j]);
             match_data_sptr temp=new match_data(c1,c1->prev_[j]);
             c2->set_best_match_next(temp);
           }
          }
         
         }
       }
        if(flag==2)
        {
            matchlist.erase(iterlist);
           }
         else
         {
            iterlist++;
         }
            }
         }
       

 
    
  
  return true;
}
//: function to observe the top n closures
bool dbctrk_curve_matching :: best_n_matches(int n,
                       vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                       vcl_vector<dbctrk_tracker_curve_sptr> * old_curves,
                       vcl_map<int,vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > > &transitives_)
{
  vcl_vector<match_data_sptr>::iterator minpiter;
  vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;
  transitives_.clear();
  // simply choosing best corresponding matches
  // giving preference to low cost matches

  if(n<0)
    return false;

  for(unsigned int i=0;i<(*new_curves).size();i++)
  {
  (*new_curves)[i]->isprevclosure_=-n-1;
  }
  for(unsigned int i=0;i<(*old_curves).size();i++)
  {
  (*old_curves)[i]->isnextclosure_=-n-1;
  }
  for(int k=0;k<n;k++)
  {
   bool selection=true;
   while (selection)
   {
     double min_cost=1e6;
     vcl_vector<dbctrk_tracker_curve_sptr>::iterator min_iter_new;
     dbctrk_tracker_curve_sptr min_iter_old;
     for (iter_new=(*new_curves).begin();iter_new!=(*new_curves).end();iter_new++)
     {
         if ((*iter_new)->isprevclosure_<k-n)
         for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
         {
           if ((*iter_old)->match_curve_set[0]->isnextclosure_<k-n)
             if (min_cost>(*iter_old)->cost_)
             {
               min_cost=(*iter_old)->cost_;
               min_iter_new=iter_new;
               min_iter_old=(*iter_old)->match_curve_set[0];
               minpiter=iter_old;
             }
         }
     }
     if (min_iter_new == (*new_curves).end() || ! ((bool)min_iter_old) )
     {
       selection=false;
     }
     else
     {
       
       bool flag=false;
         for(unsigned int i=0;i<(*min_iter_new)->prev_.size() && i<=(unsigned int)k;i++)
       {
       if((*min_iter_new)->prev_[i]->match_curve_set[0]->get_id()==(*minpiter)->match_curve_set[0]->get_id())
       {
        (*min_iter_new)->isprevclosure_=k;
        min_iter_old->isnextclosure_=k;
        transitives_[k].push_back(vcl_make_pair((*min_iter_new),min_iter_old));
        flag=true;
       }
      

       }
       if(!flag)
       {
      (*min_iter_new)->isprevclosure_++;
      min_iter_old->isnextclosure_++;
       }
     }
   }
  }

  return true;
 
}
double dbctrk_curve_matching :: match_DP(dbctrk_curve_description * desc1,
                                       dbctrk_curve_description * desc2,vcl_map<int,int> &alignment,
                                       double & cost, vnl_matrix<double> &R,vnl_matrix<double> &T,
                                       vnl_matrix<double> & Tbar,double & scale,vcl_vector<int> & tail1,
                                       vcl_vector<int> & tail2,vgl_point_2d<double> & e)
{
  double dist = -1; // dummy initialisation, to avoid compiler warning
  
  vcl_vector<vcl_pair<double,double> > v1,v2;

  // getting points from curves into vectors
  for (int i=0;i<desc1->curve_->numPoints();++i)
  {
    vcl_pair<double,double> coordinate;
    coordinate.first=desc1->curve_->point(i).x();
    coordinate.second=desc1->curve_->point(i).y();
    v1.push_back(coordinate);
  }
  for (int i=0;i<desc2->curve_->numPoints();++i)
  {
    vcl_pair<double,double> coordinate;
    coordinate.first=desc2->curve_->point(i).x();
    coordinate.second=desc2->curve_->point(i).y();
    v2.push_back(coordinate);
  }
  double sign11=0,sign12=0,sign21=0,sign22=0; // initialise to harmless value, to avoid compiler warning

  if (v1.size()>3)
  {
    double dx1=v1[1].first-v1[0].first;
    double dy1=v1[1].second-v1[0].second;
    double theta1=desc1->angles_[0]*vnl_math::pi/180;
    sign11=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    double dx2=v1[v1.size()-2].first-v1[v1.size()-1].first;
    double dy2=v1[v1.size()-2].second-v1[v1.size()-1].second;
    double theta2=desc1->angles_[v1.size()-1]*vnl_math::pi/180;
    sign12=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
  }
  if (v2.size()>3)
  {
    double dx1=v2[1].first-v2[0].first;
    double dy1=v2[1].second-v2[0].second;
    double theta1=desc2->angles_[0]*vnl_math::pi/180;
    sign21=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    double dx2=v2[v2.size()-2].first-v2[v2.size()-1].first;
    double dy2=v2[v2.size()-2].second-v2[v2.size()-1].second;
    double theta2=desc2->angles_[v2.size()-1]*vnl_math::pi/180;
    sign22=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
 }
  if (sign11*sign12<0 && sign21*sign22<0)
  {
    if (sign11*sign21>0) // hence sign12*sign22>0
    {
      alignment.clear();
      dist = curveMatch(cost,v1,v2,alignment,R,T,Tbar,tail1,tail2,scale,epipole_);
    }
    else // i.e., sign11*sign22>0 && sign12*sign21>0
    {
      vcl_reverse(v2.begin(),v2.end());
      vcl_map<int,int> alignment2;
      vcl_vector<int> tail_reversed;
      dist = curveMatch(cost,v1,v2,alignment2,R,T,Tbar,tail1,tail_reversed,scale,epipole_);
      unsigned int sizeofv2=v2.size();
      vcl_map<int,int>::iterator iter;
      alignment.clear();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment[(*iter).first]=sizeofv2-(*iter).second-1;
        
      }
      for (unsigned int i=0;i<tail_reversed.size();i++)
      {
        tail2.push_back(sizeofv2-tail_reversed[i]-1);
      }
    }
  }
  else
  {
    double             euc1,             euc2;
    vcl_map<int,int>   alignment1,       alignment2;
    double             scale1,           scale2;
    vnl_matrix<double> R1,T1,Tbar1,      R2,T2,Tbar2;
    vcl_vector<int> tail_old1,tail_new1, tail_old2,tail_new2;

    double dist1 = curveMatch(euc1,v1,v2,alignment1,R1,T1,Tbar1,tail_old1,tail_new1,scale1,epipole_);
    vcl_reverse(v2.begin(),v2.end());
    double dist2 = curveMatch(euc2,v1,v2,alignment2,R2,T2,Tbar2,tail_old2,tail_new2,scale2,epipole_);

    if (dist1<dist2)
    {
      alignment.clear();
      alignment=alignment1;
      cost=euc1;
      dist=dist1;
      R=R1;
      T=T1;
      Tbar=Tbar1;
      tail1=tail_old1;
      tail2=tail_new1;
      scale=scale1;
    }
    else
    {
      cost=euc2;
      dist=dist2;
      unsigned int sizeofv2=v2.size();
      vcl_map<int,int>::iterator iter;
      alignment.clear();
      alignment.clear();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment[(*iter).first]=sizeofv2-(*iter).second-1;
      }
      for (unsigned int i=0;i<tail_new2.size();i++)
      {
        tail2.push_back(sizeofv2-tail_new2[i]-1);
      }
      R=R2;
      T=T2;
      Tbar=Tbar2;
      tail1=tail_old2;
      scale=scale2;
    }
  }
  vcl_cout<<". ";
  return dist;
}



double dbctrk_curve_matching :: match_DP(dbctrk_curve_description * desc1,
                                       dbctrk_curve_description * desc2,
                                       match_data_sptr m,
                                       vgl_point_2d<double> & e)
{
 match_data_sptr m1=new match_data();
 match_data_sptr m2=new match_data();


 double cost1=curveMatch(desc1,desc2,e,m1,false,mp_.alpha_,mp_.beta_,mp_.gamma_,mp_.delta_,mp_.eta_,mp_.zeta_,mp_.theta_,mp_.onesided);
 double cost2=curveMatch(desc1,desc2,e,m2,true,mp_.alpha_,mp_.beta_,mp_.gamma_,mp_.delta_,mp_.eta_,mp_.zeta_,mp_.theta_,mp_.onesided);
  if(cost1>cost2)
  {
    m->R_=m2->R_;
    m->T_=m2->T_;
    m->Tbar=m2->Tbar;
    m->scale_=m2->scale_;
    m->cost_ =cost2;
    m->euc_=m2->euc_;
    m->mapping_=m2->mapping_;
    m->tail1_=m2->tail1_;
    m->tail2_=m2->tail2_;    

    return cost2;
  }
  else
  {
    m->R_=m1->R_;
    m->T_=m1->T_;
    m->Tbar=m1->Tbar;
    m->scale_=m1->scale_;
    m->cost_ =cost1;
    m->euc_=m1->euc_;
    m->mapping_=m1->mapping_;
    m->tail1_=m1->tail1_;
    m->tail2_=m1->tail2_;    
    return cost1;
  }

}



/*double dbctrk_curve_matching :: match_DP(dbctrk_curve_description * desc1,
                                       dbctrk_curve_description * desc2,
                                       match_data_sptr m,
                                       vgl_point_2d<double> & e)
{
  double dist = -1; // dummy initialisation, to avoid compiler warning
  
 
  double dist1=curveMatch(desc1->curve_,desc2->curve_,mdata1,e);
  double dist2=curveMatch(desc1->curve_,desc2->curve_,mdata2,e,true);
  
  if(dist1<dist2)
  {
    m=mdata1;
    return dist1;
  }
  else
  {
    m=mdata2;
    return dist2;
  }
}
*/

double dbctrk_curve_matching :: match_DP(dbctrk_curve_description * desc1,
                                       dbctrk_curve_description * desc2,vcl_map<int,int> &alignment,
                                       double & cost, vnl_matrix<double> &R,vnl_matrix<double> &T,
                                       vnl_matrix<double> & Tbar,double & scale,vcl_vector<int> & tail1,
                                       vcl_vector<int> & tail2,FMatrix F)
{
  double dist = -1; // dummy initialisation, to avoid compiler warning
  
  vcl_vector<vcl_pair<double,double> > v1,v2;

  // getting points from curves into vectors
  for (int i=0;i<desc1->curve_->numPoints();++i)
  {
    vcl_pair<double,double> coordinate;
    coordinate.first=desc1->curve_->point(i).x();
    coordinate.second=desc1->curve_->point(i).y();
    v1.push_back(coordinate);
  }
  for (int i=0;i<desc2->curve_->numPoints();++i)
  {
    vcl_pair<double,double> coordinate;
    coordinate.first=desc2->curve_->point(i).x();
    coordinate.second=desc2->curve_->point(i).y();
    v2.push_back(coordinate);
  }
  double sign11=0,sign12=0,sign21=0,sign22=0; // initialise to harmless value, to avoid compiler warning

  if (v1.size()>3)
  {
    double dx1=v1[1].first-v1[0].first;
    double dy1=v1[1].second-v1[0].second;
    double theta1=desc1->angles_[0]*vnl_math::pi/180;
    sign11=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    double dx2=v1[v1.size()-2].first-v1[v1.size()-1].first;
    double dy2=v1[v1.size()-2].second-v1[v1.size()-1].second;
    double theta2=desc1->angles_[v1.size()-1]*vnl_math::pi/180;
    sign12=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
  }
  if (v2.size()>3)
  {
    double dx1=v2[1].first-v2[0].first;
    double dy1=v2[1].second-v2[0].second;
    double theta1=desc2->angles_[0]*vnl_math::pi/180;
    sign21=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    double dx2=v2[v2.size()-2].first-v2[v2.size()-1].first;
    double dy2=v2[v2.size()-2].second-v2[v2.size()-1].second;
    double theta2=desc2->angles_[v2.size()-1]*vnl_math::pi/180;
    sign22=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
 }
  if (sign11*sign12<0 && sign21*sign22<0)
  {
    if (sign11*sign21>0) // hence sign12*sign22>0
    {
      alignment.clear();
      dist = FcurveMatch(cost,v1,v2,alignment,R,T,Tbar,tail1,tail2,scale,F);
    }
    else // i.e., sign11*sign22>0 && sign12*sign21>0
    {
      vcl_reverse(v2.begin(),v2.end());
      vcl_map<int,int> alignment2;
      vcl_vector<int> tail_reversed;
      dist = FcurveMatch(cost,v1,v2,alignment2,R,T,Tbar,tail1,tail_reversed,scale,F);
      unsigned int sizeofv2=v2.size();
      vcl_map<int,int>::iterator iter;
      alignment.clear();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment[(*iter).first]=sizeofv2-(*iter).second-1;
        
      }
      for (unsigned int i=0;i<tail_reversed.size();i++)
      {
        tail2.push_back(sizeofv2-tail_reversed[i]-1);
      }
    }
  }
  else
  {
    double             euc1,             euc2;
    vcl_map<int,int>   alignment1,       alignment2;
    double             scale1,           scale2;
    vnl_matrix<double> R1,T1,Tbar1,      R2,T2,Tbar2;
    vcl_vector<int> tail_old1,tail_new1, tail_old2,tail_new2;

    double dist1 = FcurveMatch(euc1,v1,v2,alignment1,R1,T1,Tbar1,tail_old1,tail_new1,scale1,F);
    vcl_reverse(v2.begin(),v2.end());
    double dist2 = FcurveMatch(euc2,v1,v2,alignment2,R2,T2,Tbar2,tail_old2,tail_new2,scale2,F);

    if (dist1<dist2)
    {
      alignment.clear();
      alignment=alignment1;
      cost=euc1;
      dist=dist1;
      R=R1;
      T=T1;
      Tbar=Tbar1;
      tail1=tail_old1;
      tail2=tail_new1;
      scale=scale1;
    }
    else
    {
      cost=euc2;
      dist=dist2;
      unsigned int sizeofv2=v2.size();
      vcl_map<int,int>::iterator iter;
      alignment.clear();
      alignment.clear();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment[(*iter).first]=sizeofv2-(*iter).second-1;
      }
      for (unsigned int i=0;i<tail_new2.size();i++)
      {
        tail2.push_back(sizeofv2-tail_new2[i]-1);
      }
      R=R2;
      T=T2;
      Tbar=Tbar2;
      tail1=tail_old2;
      scale=scale2;
    }
  }
  vcl_cout<<". ";
  return dist;
}
/*///double dbctrk_curve_matching :: match_stat(dbctrk_curve_description * desc1,
                                         dbctrk_curve_description  * desc2)
{
  double dist=0;
  double image_scale=1;
  double angle_scale=.31416;
  double grad_scale=10.0;

  dist = 1.0*vcl_sqrt( vnl_math_sqr( (desc1->center_.x()-desc2->center_.x())/image_scale )
                     +vnl_math_sqr( (desc1->center_.y()-desc2->center_.y())/image_scale ) );
  dist+= 0.5*vnl_math_abs( (desc1->length_-desc2->length_)/image_scale );
  dist+= 0.5*vnl_math_abs( (desc1->curvature_-desc2->curvature_)/image_scale );

  dist+= 1.0*vnl_math_abs( (desc1->gradient_mean_val_-desc2->gradient_mean_val_)/grad_scale);
  dist+= 1.0*vnl_math_abs( (desc1->gradient_mean_dir_-desc2->gradient_mean_dir_)/angle_scale );
  dist+= 1.0*vnl_math_abs( (desc1->gradient_std_val_-desc2->gradient_std_val_)/grad_scale);
  dist+= 1.0*vnl_math_abs( (desc1->gradient_std_dir_-desc2->gradient_std_dir_)/angle_scale );

  return dist;
}
*/
//
// Minkowski sausage bounding region
bool dbctrk_curve_matching ::sausage_intersection(dbctrk_tracker_curve_sptr c1,dbctrk_tracker_curve_sptr c2,double motion_in_pixels)
{
  for(int i=0;i<c1->desc->coarser_curve_->numPoints();i++)
  {
    for(int j=0;j<c2->desc->coarser_curve_->numPoints();j++)
    {
      double dist=vgl_distance<double>(c1->desc->coarser_curve_->point(i),
                                       c2->desc->coarser_curve_->point(j));
      
       if(dist<motion_in_pixels)
        return true;
    }
  }
  return false;
}

bool dbctrk_curve_matching :: bounding_box_intersection(vsol_box_2d_sptr box1,
                                                      vsol_box_2d_sptr box2)
{
  return box2->get_min_x()<=(box1->get_max_x()+mp_.motion_in_pixels) &&
         box2->get_max_x()>=(box1->get_min_x()-mp_.motion_in_pixels) &&
         box2->get_min_y()<=(box1->get_max_y()+mp_.motion_in_pixels) &&
         box2->get_max_y()>=(box1->get_min_y()-mp_.motion_in_pixels);
}

void dbctrk_curve_matching :: match(vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                                  vcl_vector<dbctrk_tracker_curve_sptr> * old_curves)
{
  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    
    for (unsigned int j=0;j<(*old_curves).size();++j)
    {
      //if (bounding_box_intersection((*new_curves)[i]->desc->cbox_,(*old_curves)[j]->desc->cbox_))
      if(sausage_intersection((*new_curves)[i],(*old_curves)[j],mp_.motion_in_pixels))
      {
          // if pruning using intensity is applied
          if(mp_.isintensity_)
          {
          int xbins=(*new_curves)[i]->desc->chistp.get_row1_count();
          int ybins=(*new_curves)[i]->desc->chistp.get_row2_count();
          int zbins=(*new_curves)[i]->desc->chistp.get_row3_count();
          
          double costpp=utils::dist3pdf_bhat((*new_curves)[i]->desc->chistp,(*old_curves)[j]->desc->chistp,xbins,ybins,zbins);
          double costnn=utils::dist3pdf_bhat((*new_curves)[i]->desc->chistn,(*old_curves)[j]->desc->chistn,xbins,ybins,zbins);
          double costnp=utils::dist3pdf_bhat((*new_curves)[i]->desc->chistn,(*old_curves)[j]->desc->chistp,xbins,ybins,zbins);
          double costpn=utils::dist3pdf_bhat((*new_curves)[i]->desc->chistp,(*old_curves)[j]->desc->chistn,xbins,ybins,zbins);

          double prunedistIHS=vcl_min(vcl_min(vcl_min(costpp,costnn),costpn),costnp);
   
          if(prunedistIHS<mp_.tauihs_)
          {
            double matching_cost=coarse_match_DP((*new_curves)[i]->desc,(*old_curves)[j]->desc,epipole_);
            match_data_sptr temp=new match_data;
            temp->cost_=matching_cost;
            temp->energy_=matching_cost;
            temp->match_curve_set.push_back((*old_curves)[j]);
            (*new_curves)[i]->prev_.push_back(temp);
          }
          }
          else
          {
            double matching_cost=coarse_match_DP((*new_curves)[i]->desc,(*old_curves)[j]->desc,epipole_);
            match_data_sptr temp=new match_data;
            temp->cost_=matching_cost;
            temp->energy_=matching_cost;
            temp->match_curve_set.push_back((*old_curves)[j]);
            (*new_curves)[i]->prev_.push_back(temp);
          }
      }
      (*old_curves)[j]->next_.clear();
    }
    
    // sorting the matches w.r.t elastic energy cost
    vcl_sort((*new_curves)[i]->prev_.begin(),(*new_curves)[i]->prev_.end(),less_cost());
    if (no_of_top_choices_>0)
    {
      vcl_vector<match_data_sptr>::iterator piter=(*new_curves)[i]->prev_.begin();
      if ((*new_curves)[i]->prev_.size()>=(unsigned int)no_of_top_choices_)
      {
        piter+=no_of_top_choices_;
        (*new_curves)[i]->prev_.erase(piter,(*new_curves)[i]->prev_.end());
      }
    }
  }

  vnl_matrix<double> R1,T1,Tbar;

  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    vcl_map<int,int> mapping;
    vcl_vector<int> tail_old;
    vcl_vector<int> tail_new;
    //vcl_cout<<"\n curve no "<<(*new_curves)[i]->get_id();
    if ((*new_curves)[i]->prev_.size()> 0)
    {

      vcl_vector<match_data_sptr>::iterator piter=(*new_curves)[i]->prev_.begin();
      for (unsigned int j=0;j<(*new_curves)[i]->prev_.size(); ++j,++piter)
      {
          

        double matching_cost=match_DP((*new_curves)[i]->prev_[j]->match_curve_set[0]->desc,(*new_curves)[i]->desc,(*new_curves)[i]->prev_[j],epipole_);
           
 

        match_data_sptr temp=new match_data;
        temp->euc_=(*new_curves)[i]->prev_[j]->euc_;
        temp->cost_=(*new_curves)[i]->prev_[j]->cost_;
        temp->energy_=(*new_curves)[i]->prev_[j]->energy_;
        temp->mapping_=(*new_curves)[i]->prev_[j]->mapping_;
        temp->R_=(*new_curves)[i]->prev_[j]->R_;
        temp->T_=(*new_curves)[i]->prev_[j]->T_;
        temp->Tbar=(*new_curves)[i]->prev_[j]->Tbar;
        temp->scale_=(*new_curves)[i]->prev_[j]->scale_;
        temp->match_curve_set.push_back((*new_curves)[i]);

        //insering the next match information
        (*piter)->match_curve_set[0]->next_.push_back(temp);
      }

    vcl_sort((*new_curves)[i]->prev_.begin(),(*new_curves)[i]->prev_.end(),less_cost());
    }
  }
  // sorting the next matches of the curves
  for (unsigned int i=0;i<(*old_curves).size();++i)
  {
    vcl_sort((*old_curves)[i]->next_.begin(),(*old_curves)[i]->next_.end(),less_cost());
  }
  vcl_cout<<"\n fine curve matching done";
  // computing the first best match
  //best_matches(new_curves,old_curves);
}

void dbctrk_curve_matching::best_matches(vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                                       vcl_vector<dbctrk_tracker_curve_sptr> * old_curves)
{
  vcl_vector<match_data_sptr>::iterator minpiter;
  vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;
  // simply choosing best corresponding matches
  // giving preference to low cost matches
  bool selection=true;
  while (selection)
  {
    double min_cost=1e6;
    vcl_vector<dbctrk_tracker_curve_sptr>::iterator min_iter_new;
    bool min_iter_new_flag=false;
    dbctrk_tracker_curve_sptr min_iter_old;
    for (iter_new=(*new_curves).begin();iter_new!=(*new_curves).end();iter_new++)
    {
      
        if (!(*iter_new)->ismatchedprev_)
        for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
        {
          if (!(*iter_old)->match_curve_set[0]->ismatchednext_)
            if (min_cost>(*iter_old)->cost_)
            {
              min_cost=(*iter_old)->cost_;
              min_iter_new=iter_new;
              min_iter_old=(*iter_old)->match_curve_set[0];
              minpiter=iter_old;
              min_iter_new_flag=true;
            }
          }
        }

        if (!min_iter_new_flag/*min_iter_new == (*new_curves).end()*/ || ! ((bool)min_iter_old) )
        {
          selection=false;
        }
        else
        {
          // check if the tail exists
          selection=false;
          // tail in the previous frame

          //double s;
         
           /* if ((*minpiter)->tail1_.size()>4 && (*minpiter)->tail1_.size()>(*minpiter)->tail2_.size())
          {
            match_data_sptr  best_match_for_tail;
            dbctrk_tracker_curve_sptr  prev_tail_curve=new dbctrk_tracker_curve;
            prev_tail_curve->init_set((*minpiter)->match_curve_set[0],(*minpiter)->tail1_,0);
            vnl_matrix<double> R1,T1;
            // matching the tail
            match_prev_tail_curve((*minpiter)->match_curve_set[0], prev_tail_curve,new_curves);
            if (prev_tail_curve->next_[0]->match_curve_set[0]==(*min_iter_new))
            {
              if (prev_tail_curve->next_.size()>1)
                best_match_for_tail=prev_tail_curve->next_[1];
              else
                best_match_for_tail=NULL;
            }
            else
            {
              best_match_for_tail=prev_tail_curve->next_[0];
              int id=prev_tail_curve->next_[0]->match_curve_set[0]->get_id();
            }
            // check the best match of the tail
           if (best_match_for_tail)
              if (best_match_for_tail->match_curve_set[0]->prev_.size()>0 )
                if (best_match_for_tail->energy_ <
                    best_match_for_tail->match_curve_set[0]->prev_[0]->energy_)
                {
                  R1=(*minpiter)->R_;
                  T1=(*minpiter)->Tbar;
                  //vcl_cout<<"\n "<<T1(0,0)<<"\t"<<T1(1,0);
                  s=(*minpiter)->scale_;
                  prev_tail_curve->set_best_match_next(best_match_for_tail);
                  double tail_euc_dist=dbctrk_curve_algs::compute_transformed_euclidean_distance(prev_tail_curve,
                                                             best_match_for_tail->match_curve_set[0],
                                                             R1,T1,s,best_match_for_tail->mapping_);
                  // confirm it using transformation
                  R1=best_match_for_tail->R_;
                  T1=best_match_for_tail->T_;
                  //vcl_cout<<"\n "<<T1(0,0)<<"\t"<<T1(1,0);
                  s=best_match_for_tail->scale_;
                  // to compute the euc dist
                  double parent_euc_dist=dbctrk_curve_algs::
                      compute_transformed_euclidean_distance
                      ((*minpiter)->match_curve_set[0],(*min_iter_new),R1,T1,s,
                      (*minpiter)->mapping_);
                  // join the two curves and add it to the new curves;
                  if (vcl_fabs(2*(tail_euc_dist-parent_euc_dist)/(tail_euc_dist+parent_euc_dist))<1.0
                     && (tail_euc_dist+parent_euc_dist)/2<10)
                  {
                    //merged two curves to form a virtual curve
                    dbctrk_tracker_curve_sptr c=new dbctrk_tracker_curve;
                    merge_curves((*min_iter_new),best_match_for_tail->match_curve_set[0],c);
                    c->set_id((*new_curves).size());
                    // fill in the values of best_match_prev for each fragment
                    match_data_sptr temp1= new match_data;
                    temp1->match_curve_set.push_back( (*min_iter_new));
                    temp1->match_curve_set.push_back( best_match_for_tail->match_curve_set[0]);

                    temp1->curve_set=c;
                    
                    match_data_sptr temp= new match_data;
                    temp->match_curve_set.push_back((*minpiter)->match_curve_set[0]);
                    double cst=match_DP(c->desc,temp->match_curve_set[0]->desc,
                                        temp->mapping_, temp->euc_,temp->R_,temp->T_,
                                       temp->Tbar,temp->scale_,temp->tail1_,temp->tail2_,epipole_);
                    // filling the best next match
                    temp1->mapping_=temp->mapping_;
                    temp1->R_=temp->R_;
                    temp1->T_=temp->T_;
                    
                    temp1->Tbar=temp->Tbar;
                    temp1->cost_=temp->cost_;
                    temp1->energy_=temp->energy_;
                    temp1->tail1_=temp->tail1_;
                    temp1->tail2_=temp->tail2_;
                    temp1->euc_=temp->euc_;
                    temp1->scale_=temp->scale_;
                    (*minpiter)->match_curve_set[0]->set_best_match_next(temp1);

                    temp->energy_=cst;
                    temp->cost_=cst;
                    // keeping the original mapping for parent match
                    temp->mapping_=(*minpiter)->mapping_;
                    (*min_iter_new)->set_best_match_prev((*minpiter));
                    // obtaining the original mapping for tail
                    vcl_map<int,int>::iterator itermap;vcl_map<int,int> tailmap;
                    for(itermap=best_match_for_tail->mapping_.begin();
                        itermap!=best_match_for_tail->mapping_.end();
                        itermap++)
                        {
                            tailmap[(*minpiter)->tail1_[(*itermap).first]]=(*itermap).second;
                            
                        }
                        temp->mapping_=tailmap;
                        match_data_sptr temp3=new match_data((*minpiter)->match_curve_set[0], best_match_for_tail);
                        
                        temp3->mapping_=tailmap;
                       best_match_for_tail->match_curve_set[0]->set_best_match_prev(temp3);
                    
                    
                     // set the match_ids
                    best_match_for_tail->match_curve_set[0]->match_id_=min_iter_old->match_id_;
                    (*min_iter_new)->match_id_=min_iter_old->match_id_;

                    best_match_for_tail->match_curve_set[0]->ismatchedprev_=true;
                    (*min_iter_new)->ismatchedprev_=true;
                    c->ismatchedprev_=true;
                    c->ismatchednext_=true;
                    c->isreal_=false;
                    min_iter_old->ismatchednext_=true;

                    selection=true;
                    new_curves->push_back(c);
                  }
                }
          }

          // tail in the current frame
         
   
          else if ((*minpiter)->tail2_.size()>4
                    && (*minpiter)->tail2_.size()>(*minpiter)->tail1_.size())
          {
            dbctrk_tracker_curve_sptr  next_tail_curve = new dbctrk_tracker_curve;
               next_tail_curve->init_set((*min_iter_new),(*minpiter)->tail2_,0);
            vnl_matrix<double> R2,T2,Tbar2;
            // matching the tail

            match_next_tail_curve((*min_iter_new),next_tail_curve,old_curves);
            match_data_sptr best_match_for_tail=0;
            if (next_tail_curve->prev_[0]->match_curve_set[0]==min_iter_old)
            {
              if (next_tail_curve->prev_.size()>1)
                best_match_for_tail=next_tail_curve->prev_[1];
              else
                best_match_for_tail=NULL;
            }
            else
              best_match_for_tail=next_tail_curve->prev_[0];

            if (best_match_for_tail)
              if (best_match_for_tail->match_curve_set[0]->next_.size()>0 )
                if (best_match_for_tail->cost_   <
                    best_match_for_tail->match_curve_set[0]->next_[0]->cost_)
                {
                  
                  R2=(*minpiter)->R_;
                  T2=(*minpiter)->Tbar;
                  
                  s=(*minpiter)->scale_;
                  next_tail_curve->set_best_match_prev(best_match_for_tail);
                  
                  double tail_euc_dist=dbctrk_curve_algs::compute_transformed_euclidean_distance
                                       (best_match_for_tail->match_curve_set[0],next_tail_curve,
                                       R2,T2,s,best_match_for_tail->mapping_);

                  //computing parent eulidean distance
                  R2=best_match_for_tail->R_;
                  T2=best_match_for_tail->Tbar;
                  //vcl_cout<<"\n"<<best_match_for_tail->Tbar(0,0)
                //          <<"\t"<<best_match_for_tail->Tbar(1,0);
                  s=best_match_for_tail->scale_;
                  double parent_euc_dist=dbctrk_curve_algs::
                      compute_transformed_euclidean_distance((*minpiter)->match_curve_set[0],(*min_iter_new),R2,T2,s,(*minpiter)->mapping_);
                  // join the two curves and add it to the new curves;
                  if (vcl_fabs(2*(tail_euc_dist-parent_euc_dist)/(tail_euc_dist+parent_euc_dist))<1.0
                     && (tail_euc_dist+parent_euc_dist)/2<10)
                  {
                    //merged two curves to form a virtual curve
                    dbctrk_tracker_curve_sptr c=new dbctrk_tracker_curve;
                    merge_curves((*minpiter)->match_curve_set[0],
                                 best_match_for_tail->match_curve_set[0],c);
                    c->set_id((*old_curves).size());
                    c->isreal_=false;

                    match_data_sptr temp= new match_data;
                    temp->match_curve_set.push_back(*min_iter_new);
                    
                    double cst=match_DP(c->desc,temp->match_curve_set[0]->desc,
                                        temp->mapping_,temp->euc_,temp->R_,temp->T_,temp->Tbar,
                                        temp->scale_,temp->tail1_,temp->tail2_,epipole_);
                    temp->cost_=cst;
                    temp->energy_=cst;
                    
                    // fill in the values of best_match_next
                    (*minpiter)->match_curve_set[0]->set_best_match_next(temp);
                    best_match_for_tail->match_curve_set[0]->set_best_match_next(temp);

                    // fill in the values of the best_match_prev
                    match_data_sptr temp1= new match_data;
                    temp1->match_curve_set.push_back((*minpiter)->match_curve_set[0]);
                    temp1->match_curve_set.push_back(best_match_for_tail->match_curve_set[0]);
                    temp1->curve_set=c;
                    
                    temp1->mapping_=(*minpiter)->mapping_;
                    temp1->energy_=temp->energy_;
                    temp1->cost_=temp->cost_;
                    temp1->R_=((*minpiter)->R_+best_match_for_tail->R_)/2;
                    temp1->Tbar=((*minpiter)->Tbar+best_match_for_tail->Tbar)/2;
                    temp1->scale_=((*minpiter)->scale_+best_match_for_tail->scale_)/2;
                    temp1->euc_=((*minpiter)->euc_+best_match_for_tail->euc_)/2;
                    temp1->T_=temp->T_;
                    temp1->tail1_=temp->tail1_;
                    temp1->tail2_=temp->tail2_;
                    
                    
                    (*min_iter_new)->set_best_match_prev(temp1);

                    //set_the_match_ids
                    c->ismatchedprev_=true;
                    c->ismatchednext_=true;
                    (*min_iter_new)->ismatchedprev_=true;
                    (*minpiter)->match_curve_set[0]->ismatchednext_=true;
                    best_match_for_tail->match_curve_set[0]->ismatchednext_=true;
                    (*min_iter_new)->match_id_=temp1->match_curve_set[1]->match_id_;
                    temp1->match_curve_set[0]->match_id_=temp1->match_curve_set[1]->match_id_;;
                    selection=true;
                  }
                }
          }
          */
          // add the curve
          if (!selection)
          {
            (*min_iter_new)->ismatchedprev_=true;
            min_iter_old->ismatchednext_=true;
            //previous_id[min_cost_previous_id]=1;
            selection=true;
            if (min_cost >= 1e6)
            {
              (*min_iter_new)->match_id_ = -1;
            }
            else
            {
              if (min_iter_old->match_id_>-1)
                (*min_iter_new)->match_id_ = min_iter_old->match_id_;
              else
              {
                min_iter_old->match_id_=min_iter_old->get_id();
                (*min_iter_new)->match_id_=min_iter_old->match_id_;
              }

              (*min_iter_new)->set_best_match_prev((*minpiter));

              match_data_sptr temp=new match_data((*min_iter_new),(*minpiter));
              min_iter_old->set_best_match_next(temp);
            }
          }
        }
  }

  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    if (!(*new_curves)[i]->ismatchedprev_)
    {
      (*new_curves)[i]->match_id_ =-1;
      (*new_curves)[i]->group_id_ =-1;
    }
  }
}


double dbctrk_curve_matching::compute_euc_dist(dbctrk_tracker_curve_sptr a,
                                             dbctrk_tracker_curve_sptr b)
{
  vnl_matrix<double> Ri,Ti;
  double x2,y2;
  double cost=0;
  vcl_map<int,int>::iterator iter;
  for (iter=a->get_best_match_prev()->mapping_.begin();
       iter!=a->get_best_match_prev()->mapping_.end();
       iter++)
  {
    double point[2];
    point[0]=a->get_best_match_prev()->match_curve_set[0]->desc->curve_->point((*iter).first).x();
    point[1]=a->get_best_match_prev()->match_curve_set[0]->desc->curve_->point((*iter).first).y();
  
    Ri=b->get_best_match_prev()->R_;
    Ti=b->get_best_match_prev()->T_;
  
    vnl_matrix<double> P (point, 2, 1);
    vnl_matrix<double> Q = Ri*P + Ti;

    x2=a->desc->curve_->point((*iter).second).x();
    y2=a->desc->curve_->point((*iter).second).y();

    cost+=vcl_sqrt((Q(0,0)-x2)*(Q(0,0)-x2)+(Q(1,0)-y2)*(Q(1,0)-y2));
  }
  cost/=a->get_best_match_prev()->mapping_.size();
  return cost;
}

void dbctrk_curve_matching::merge_curves(dbctrk_tracker_curve_sptr cs1,
                                       dbctrk_tracker_curve_sptr cs2,
                                       dbctrk_tracker_curve_sptr &outcs)
{
  outcs=new dbctrk_tracker_curve;
  outcs->set_curve(cs1->get_curve());
  outcs->set_curve(cs2->get_curve());
  int id = cs1->get_id();

  unsigned int s1=cs1->desc->curve_->numPoints();
  unsigned int s2=cs2->desc->curve_->numPoints();

  vcl_vector<vgl_point_2d<double> > f;
  vcl_vector<vgl_point_2d<double> > s;

  for (unsigned int i=0;i<s1;++i)
    f.push_back(cs1->desc->curve_->point(i));

  for (unsigned int i=0;i<s2;++i)
    s.push_back(cs1->desc->curve_->point(i));

  double x11=cs1->desc->curve_->point(0).x();
  double y11=cs1->desc->curve_->point(0).y();
  double x12=cs1->desc->curve_->point(s1-1).x();
  double y12=cs1->desc->curve_->point(s1-1).y();

  double x21=cs2->desc->curve_->point(0).x();
  double y21=cs2->desc->curve_->point(0).y();
  double x22=cs2->desc->curve_->point(s2-1).x();
  double y22=cs2->desc->curve_->point(s2-1).y();

  vcl_pair<int,int> p;

  vcl_map<double,vcl_pair<int,int> > dist;

  p.first=0;p.second=0;
  double e11=vcl_sqrt((x11-x21)*(x11-x21)+(y11-y21)*(y11-y21));
  dist[e11]=p;

  p.first=0;p.second=s2-1;
  double e12=vcl_sqrt((x11-x22)*(x11-x22)+(y11-y22)*(y11-y22));
  dist[e12]=p;

  p.first=s1-1;p.second=s2-1;
  double e22=vcl_sqrt((x12-x22)*(x12-x22)+(y12-y22)*(y12-y22));
  dist[e22]=p;

  p.first=s1-1;p.second=0;
  double e21=vcl_sqrt((x12-x21)*(x12-x21)+(y12-y21)*(y12-y21));
  dist[e21]=p;


  p=((*dist.begin()).second);
  if (p.first>0 )
  {
    if (p.second>0)
      {
        vcl_reverse(s.begin(),s.end());
        f.insert(f.end(),s.begin(),s.end());
      }
    else
      {
        f.insert(f.end(),s.begin(),s.end());
      }
  }
  else
  {
    if (p.second>0)
    {
      f.insert(f.begin(),s.begin(),s.end());
    }
    else
    {
      vcl_reverse(s.begin(),s.end());
      f.insert(f.begin(),s.begin(),s.end());
    }
  }
  outcs->init_set(f,id);
}


double dbctrk_curve_matching::compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0;i<t.size();++i)
    sum+=t[i];
  return sum/=t.size();
}


double dbctrk_curve_matching::compute_std(vcl_vector<double> t)
{
  double sum=compute_mean(t);
  double std=0;
  for (unsigned int i=0;i<t.size();++i)
    std+=(t[i]-sum)*(t[i]-sum);
  return vcl_sqrt(std/t.size());
}
//: function to match the curves with transitive closure over a given window size assuming all matches have been done
//bool dbctrk_curve_matching::best_matches_tc(vcl_vector< vcl_vector<dbctrk_tracker_curve_sptr> > curve_set ,int winsize )
//{
// if(curve_set.size()<=winsize || curve_set.size()<=0)
//   return false;
//
// vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter,min_iter_current;
// match_data_sptr min_i_data;
// match_data_sptr min_k_data;
// double dji,djk,dki;
// bool selection=true;
//
// for(iter=curve_set[curve_set.size()-1].begin();iter!=curve_set[curve_set.size()-1].end();iter++)
// {
//           
// }
//
//
//
// 
//
//}
void dbctrk_curve_matching::best_matches_tc(vcl_vector<dbctrk_tracker_curve_sptr> * current_curves,
                                          vcl_vector<dbctrk_tracker_curve_sptr> * past_curves,
                                          vcl_vector<dbctrk_tracker_curve_sptr> * future_curves)

{
    vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter,min_iter_current;
    match_data_sptr min_i_data;
    match_data_sptr min_k_data;
    double dji,djk,dki;
    bool selection=true;
    while (selection)
    {
        double min_cost=1e6;
        double mink=-1;
        // iterating over j
        for(iter=(*current_curves).begin();iter!=(*current_curves).end();iter++)
        {
            // iterating over i
            dji=0;
            if(!(*iter)->ismatchedprev_)
            {
                for(unsigned int i=0;i<(*iter)->prev_.size();i++)
                {
                    if(!(*iter)->prev_[i]->match_curve_set[0]->ismatchednext_)
                    {
                    dji=(*iter)->prev_[i]->energy_;
                    djk=0;
                    dki=0;
                    // iterating over k
                    int minl=-1;
                    for(unsigned int k=0;k<(*future_curves).size();k++)
                    {
                        if(!(*future_curves)[k]->ismatchedprev_)
                        {
                            for(unsigned int l=0;l<(*iter)->next_.size();l++)
                            {
                                if((*iter)->next_[l]->match_curve_set[0]->get_id()==(*future_curves)[k]->get_id())
                                {
                                    djk=(*iter)->next_[l]->energy_;
                                    minl=l;
                                }
                            }
                            for(unsigned int m=0;m<(*future_curves)[k]->prev_.size();m++)
                            {
                                if((*iter)->prev_[i]->match_curve_set[0]->get_id()==(*future_curves)[k]->prev_[m]->match_curve_set[0]->get_id())
                                {
                                    dki=(*future_curves)[k]->prev_[m]->energy_;
                                }
                            }
                        }
                        if(dji>0 && djk>0 && dki > 0)
                        {
                            double cost=vcl_max(dji,vcl_max(djk,dki));
                            if(min_cost>cost)
                            {
                                min_cost=cost;
                                min_iter_current=iter;
                                min_i_data=(*iter)->prev_[i];
                                min_k_data=(*iter)->next_[minl];
                                mink=k;
                            }
                        }
                    }
                }
            }
        }
        }

    if(min_cost==1e6)
        selection=false;
    else
    {
        // filling the next and before for j
        (*min_iter_current)->set_best_match_prev(min_i_data);
        (*min_iter_current)->ismatchedprev_=true;
        (*min_iter_current)->match_id_=min_i_data->match_curve_set[0]->match_id_;
        // filling the next for i
        match_data_sptr temp1=new match_data((*min_iter_current),min_i_data);
        min_i_data->match_curve_set[0]->set_best_match_next(temp1);
        min_i_data->match_curve_set[0]->ismatchednext_=true;
        //filling the info for virtual future curves
        (*future_curves)[(unsigned int)mink]->ismatchedprev_=true;
    }

    }

}





void dbctrk_curve_matching::softmax(vcl_vector<dbctrk_tracker_curve_sptr> * new_curves,
                                  vcl_vector<dbctrk_tracker_curve_sptr> * old_curves)
{

    double * X=new double[(new_curves->size()+1)*(old_curves->size()+1)];// adding 1 for slack variables
    double * M=new double[(new_curves->size()+1)*(old_curves->size()+1)];
    double * Mo=new double[(new_curves->size()+1)*(old_curves->size()+1)];
    double * S=new double[(new_curves->size()+1)*(old_curves->size()+1)];
    // euclidean distance
    double * E=new double[(new_curves->size()+1)*(old_curves->size()+1)];

    int I=new_curves->size();
    int A=old_curves->size();

    int i=0;
    // initializing X matrix
    for( i=0;i<I+1;i++)
    {
        for(int a=0;a<A+1;a++)
        {
            X[i*(A+1)+a]=0;
            S[i*(A+1)+a]=100;
            M[i*(A+1)+a]=1.005;
            E[i*(A+1)+a]=100;

        }
    }
    //vcl_cout<<"\n ....................";
    vcl_vector<dbctrk_tracker_curve_sptr>::iterator iter_new;
    vcl_vector<match_data_sptr>::iterator iter_old;

    vcl_ofstream of("c:\\similarity_matrix.txt");
    // initialize X(data) matrix
    for (iter_new=(*new_curves).begin(),i=0 ;iter_new!=(*new_curves).end();iter_new++,i++)
    {
          for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
        {
            int a=(*iter_old)->match_curve_set[0]->get_id();
            X[i*(A+1)+a]=1/(0.0001+(*iter_old)->cost_);
            //S[i*(A+1)+a]=(*iter_old)->cost_;
            S[i*(A+1)+a]=(*iter_old)->cost_;
            E[i*(A+1)+a]=(*iter_old)->euc_;
        }
    }
    // slack column
    for (iter_new=(*new_curves).begin(),i=0 ;iter_new!=(*new_curves).end();iter_new++,i++)
    {
        //S[i*(A+1)+(A)]=1/max((*new_curves).size(),max((*old_curves).size(),0));
        S[i*(A+1)+(A)]=(*iter_new)->desc->splice_cost_;
        E[i*(A+1)+(A)]=0;

    }
    int a=0;
    // slack row
    for (iter_new=(*old_curves).begin(),a=0;iter_new!=(*old_curves).end();iter_new++,a++)
    {
        S[(I)*(A+1)+a]=(*iter_new)->desc->splice_cost_;
        //S[i*(A+1)+(A)]=1/maxof((*new_curves).size(),(*old_curves).size(),0);
        E[(I)*(A+1)+a]=0;
    }


    for(i=0;i<I+1;i++)
    {
        for(int a=0;a<A+1;a++)
        {
            of<<S[i*(A+1)+a]<<" ";

        }
        of<<"\n";
    }

    of.close();


 for(int beta=100;beta<101;beta++)
 {
    for( i=0;i<I;i++)
    {
        for(int a=0;a<A;a++)
        {
            M[i*A+a]=X[i*A+a];/*exp(beta*X[i*A+a]);*/
            Mo[i*A+a]=X[i*A+a];/*exp(beta*X[i*A+a]);*/
        }
    }
    double error=1e6;

    for( i=0;i<I;i++)
    {
        for(int a=0;a<A;a++)
        {
        //    vcl_cout<<" "<<M[i*A+a];
        }
    //    vcl_cout<<"\n";
    }
    while(error>0.05)
    {
        for( i=0;i<I;i++)
        {
            double sumI=0;
            for(int a=0;a<A;a++)
            {

                sumI+=M[i*A+a];
            }
            for(int a=0;a<A;a++)
            {

                M[i*A+a]/=sumI;
            }
        }

        for( a=0;a<A;a++)
        {
            double sumA=0;
            for( i=0;i<I;i++)
            {

                sumA+=M[i*A+a];
            }
            for( i=0;i<I;i++)
            {

                M[i*A+a]/=sumA;
            }
        }


        error=0;
        for( i=0;i<I;i++)
        {
            for( a=0;a<A;a++)
            {
                error+=vcl_fabs(Mo[i*A+a]-M[i*A+a]);
                Mo[i*A+a]=M[i*A+a];
            }
        }


    }

 }

    for( i=0;i<I;i++)
    {
        double best_match=0;
        int best_match_a=-1;
        for( a=0;a<A;a++)
        {
            if(M[i*A+a]>best_match)
            {
                best_match_a=a;
            }
        }
        if(best_match_a>=0)
        {
        for (iter_old=(*new_curves)[i]->prev_.begin();iter_old!=(*new_curves)[i]->prev_.end();iter_old++)
        {
            if((*iter_old)->match_curve_set[0]->get_id()==best_match_a)
            {
                    (*new_curves)[i]->set_best_match_prev(*iter_old);
                    (*new_curves)[i]->match_id_=(*iter_old)->match_curve_set[0]->match_id_;
            }
            //vcl_cout<<" "<<M[i*A+a];
        }
        }
    //vcl_cout<<"\n";
    }

/*    for(int i=0;i<I;i++)
    {
        for(int a=0;a<A;a++)
        {
            vcl_cout<<" "<<X[i*A+a];
        }
        vcl_cout<<"\n";
    }
*/
/*    for(int i=0;i<I;i++)
    {
        for(int a=0;a<A;a++)
        {
            vcl_cout<<" "<<M[i*A+a];
        }
        vcl_cout<<"\n";
    }

*/


}
void dbctrk_curve_matching::write_transformations(vcl_vector<dbctrk_tracker_curve_sptr> * new_curves)
{


}
double dbctrk_curve_matching::spatial_euclidean_dist(dbctrk_tracker_curve_sptr c1,dbctrk_tracker_curve_sptr c2)
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
void dbctrk_curve_matching::compute_spatial_distance(vcl_vector<dbctrk_tracker_curve_sptr> curves,
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



