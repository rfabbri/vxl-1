#ifndef curve_Match_h_
#define curve_Match_h_




#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_2.h>
#include <vcl_utility.h>
#include<vcl_algorithm.h>


#include "dbctrk/dbctrk_DPMatch.h"
#include "dbctrk/dbctrk_curve.h"
#include <dbctrk/dbctrk_tracker_curve.h>
#define SUCCESS 1
#define FAILURE 0

inline
double curveMatch(double &euc_dist,                        //!< out
                  vcl_vector<vcl_pair<double,double> > v1, //!< in
                  vcl_vector<vcl_pair<double,double> > v2, //!< in
                  vcl_map<int,int> & mapping,              //!< out
                  vnl_matrix <double> & R_,                //!< out
                  vnl_matrix <double> & T_,                //!< out
                  vnl_matrix <double> & Tbar,              //!< out
                  vcl_vector<int> &tail1,
                  vcl_vector<int> &tail2,
                  double &scale,vgl_point_2d<double>  & e)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0
  bool endPointMatch=false;

  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

 
  dbctrk_DPMatch d1(c1,c2,e);
  //DPMatch d1(c1,c2);
  d1.gamma=1;
  d1.delta=0;
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  //euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  T_       = d1.T;
  d1.detect_tail(tail1,tail2);
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  return d1.normfinalCost();
}

inline
double curveMatch(Curve * c1, Curve * c2, 
                  vgl_point_2d<double>  & e,match_data_sptr  m,
                  bool reverse=false,
                  int alpha=1,int beta=1,int gamma=0,
                  int delta=0,int eta=0,int zeta=0,int theta=0,bool onesided=false)     
{
  dbctrk_DPMatch d1(*c1,*c2,e);
  d1.alpha=alpha;
  d1.beta=beta;
  d1.gamma=gamma;
  d1.delta=delta;
  d1.eta=eta;
  d1.zeta=zeta;
  d1.theta=theta;
  d1.reverse=reverse;
  d1.onesided=onesided;

  d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  m->euc_       = d1.transformed_euclidean_distance();
  m->mapping_   = d1.alignment;
  m->R_         = d1.R;
  m->T_         = d1.T;
  d1.detect_tail(m->tail1_,m->tail2_);
  m->Tbar       = d1.Tbar;
  m->scale_     = d1.scale;

  return d1.normfinalCost();
}
inline
double curveMatch(dbctrk_curve_description *  c1,
      dbctrk_curve_description *  c2, 
                  vgl_point_2d<double>  & e,match_data_sptr  m,
                  bool reverse=false,
                  int alpha=1,int beta=1,int gamma=0,
                  int delta=0,int eta=0,int zeta=0,int theta=0,bool onesided=false)     
{

  dbctrk_DPMatch d1(*c1->curve_,*c2->curve_,e);

  d1.alpha=alpha;
  d1.beta=beta;
  d1.gamma=gamma;
  d1.delta=delta;
  d1.eta=eta;
  d1.zeta=zeta;
  d1.theta=theta;
  d1.reverse=reverse;
  d1.onesided=onesided;

 d1.I1pos.push_back(c1->Prcolor);
  d1.I1pos.push_back(c1->Pgcolor);
  d1.I1pos.push_back(c1->Pbcolor);

  d1.I1neg.push_back(c1->Nrcolor);
  d1.I1neg.push_back(c1->Ngcolor);
  d1.I1neg.push_back(c1->Nbcolor);

  d1.I2pos.push_back(c2->Prcolor);
  d1.I2pos.push_back(c2->Pgcolor);
  d1.I2pos.push_back(c2->Pbcolor);

  d1.I2neg.push_back(c2->Nrcolor);
  d1.I2neg.push_back(c2->Ngcolor);
  d1.I2neg.push_back(c2->Nbcolor);
  
  //   vcl_cout<<"\n cost of matching is "<<d1.normfinalCost();
  d1.match();
  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  m->euc_       = d1.transformed_euclidean_distance();
  m->mapping_   = d1.alignment;
  m->R_         = d1.R;
  m->T_         = d1.T;
  d1.detect_tail(m->tail1_,m->tail2_);
  m->Tbar       = d1.Tbar;
  m->scale_     = d1.scale;

  return d1.normfinalCost();
}
inline
double curveMatch(vcl_vector<vgl_point_2d<double> > v1, //!< in
                  vcl_vector<vgl_point_2d<double> > v2, //!< in
                  match_data_sptr  m,vgl_point_2d<double>  & e,int alpha=1,int beta=1,int gamma=1)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0
  bool endPointMatch=false;

  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

 
  dbctrk_DPMatch d1(c1,c2,e);
  d1.alpha=alpha;
  d1.beta=beta;
  d1.gamma=gamma;
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  m->euc_ = d1.transformed_euclidean_distance();
  m->mapping_  = d1.alignment;
  m->R_       = d1.R;
  m->T_       = d1.T;
  d1.detect_tail(m->tail1_,m->tail2_);
  m->Tbar     = d1.Tbar;
  m->scale_    = d1.scale;
  return d1.normfinalCost();
}
inline
double FcurveMatch(double &euc_dist,                        //!< out
                  vcl_vector<vcl_pair<double,double> > v1, //!< in
                  vcl_vector<vcl_pair<double,double> > v2, //!< in
                  vcl_map<int,int> & mapping,              //!< out
                  vnl_matrix <double> & R_,                //!< out
                  vnl_matrix <double> & T_,                //!< out
                  vnl_matrix <double> & Tbar,              //!< out
                  vcl_vector<int> &tail1,
                  vcl_vector<int> &tail2,
                  double &scale,FMatrix F)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0
  bool endPointMatch=false;

  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

 
  dbctrk_DPMatch d1(c1,c2,F);
  //dbctrk_DPMatch d1(c1,c2);
  d1.delta=1;
  d1.gamma=0;
  d1.alpha=1;
  d1.beta=1;

  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  //euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  T_       = d1.T;
  d1.detect_tail(tail1,tail2);
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  return d1.normfinalCost();
}

inline
double computesplicecost(vcl_vector<vgl_point_2d<double> > v)                           //!< out
{
  #if 0 // these variables are not used
    double lambda1=1.0;
    double R=8.0;
    int writeFiles=0;
    double sampleSize=0.1;
    int localize=0;
    bool endPointMatch=false;
  #endif // 0
   vcl_vector<vcl_pair<double,double> > points;
   for(unsigned int i=0;i<v.size();i++)
    points.push_back(vcl_make_pair(v[i].x(),v[i].y()));
  Curve c1; c1.readDataFromVector(points);

 
  dbctrk_DPMatch d1(c1);
  return d1.SpliceCost(c1);
}
inline
double tempcurveMatch(vcl_vector<vcl_pair<double,double> > v1, //!< in
            vcl_vector<vcl_pair<double,double> > v2, //!< in
            vcl_map<int,int> & mapping,              //!< out
            vnl_matrix <double> & R_,                //!< out
                      vnl_matrix <double> & Tbar,              //!< out
                      double &scale,vgl_point_2d<double>  & e,
            int alpha, int beta, int gamma,int delta, double mu, bool backward)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0

  vcl_vector<int> tail1;
  vcl_vector<int> tail2;
  bool endPointMatch=false;


 // vcl_cout<<"\n size of input curve is "<<v1.size()<<"\t"<<v2.size();
  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

  dbctrk_DPMatch d1(c1,c2,e);
 
  //dbctrk_DPMatch d1(c1,c2);
  
  d1.alpha=alpha;
  d1.beta=beta;
  d1.gamma=gamma;
  d1.delta=delta;
  d1.mu=mu;
  d1.backward=backward;
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  //double euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  d1.detect_tail(tail1,tail2);
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  return d1.normfinalCost();
}


inline
double tempcurveMatch(vcl_vector<vgl_point_2d<double> > v1, //!< in
            vcl_vector<vgl_point_2d<double> > v2, //!< in
            vcl_map<int,int> & mapping,              //!< out
            vnl_matrix <double> & R_,                //!< out
                      vnl_matrix <double> & Tbar,              //!< out
                      double &scale,vgl_point_2d<double>  & e,
                      int alpha, int beta, int gamma, double mu, bool backward)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0

  vcl_vector<int> tail1;
  vcl_vector<int> tail2;
  bool endPointMatch=false;


 // vcl_cout<<"\n size of input curve is "<<v1.size()<<"\t"<<v2.size();
  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

  dbctrk_DPMatch d1(c1,c2,e);
  //dbctrk_DPMatch d1(c1,c2);
  
  d1.alpha=alpha;
  d1.beta=beta;
  d1.gamma=gamma;
  d1.mu=mu;
  d1.backward=backward;
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  //double euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  d1.detect_tail(tail1,tail2);
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  //vcl_cout<<"\n"<<d1.normfinalCost();
  return d1.normfinalCost();
}

inline
double IcurveMatch(dbctrk_tracker_curve_sptr dbctrk1, //!< in
                   dbctrk_tracker_curve_sptr dbctrk2, //!< in
                   vcl_map<int,int> & mapping,              
                   vnl_matrix <double> & R_,                
                   vnl_matrix <double> & Tbar,              
                   double &scale,
                   int alpha, int beta, double eta,
                   int gamma, double zeta, double theta,
                   bool onesided,bool reverse,int L)                         
{

  vcl_vector<int> tail1;
  vcl_vector<int> tail2;
  bool endPointMatch=false;
  Curve c1; 
  Curve c2; 
//  if(!reverse)
//  {
  ///c1.readDataFromVector(dbctrk1->desc->points_);
  ///c2.readDataFromVector(dbctrk2->desc->points_);
  c1=(*dbctrk1->desc->curve_);
  c2=(*dbctrk2->desc->curve_);


  dbctrk_DPMatch d1(c1,c2);

  d1.I1pos.push_back(dbctrk1->desc->Prcolor);
  d1.I1pos.push_back(dbctrk1->desc->Pgcolor);
  d1.I1pos.push_back(dbctrk1->desc->Pbcolor);

  d1.I1neg.push_back(dbctrk1->desc->Prcolor);
  d1.I1neg.push_back(dbctrk1->desc->Pgcolor);
  d1.I1neg.push_back(dbctrk1->desc->Pbcolor);
  
  d1.I2pos.push_back(dbctrk2->desc->Prcolor);
  d1.I2pos.push_back(dbctrk2->desc->Pgcolor);
  d1.I2pos.push_back(dbctrk2->desc->Pbcolor);

  d1.I2neg.push_back(dbctrk2->desc->Prcolor);
  d1.I2neg.push_back(dbctrk2->desc->Pgcolor);
  d1.I2neg.push_back(dbctrk2->desc->Pbcolor);
  
 /* if(reverse)
  {
    for(int i=0;i<d1.I1pos.size();i++)
        vcl_reverse(d1.I1pos[i].begin(),d1.I1pos[i].end());
    for(int i=0;i<d1.I1neg.size();i++)
        vcl_reverse(d1.I1neg[i].begin(),d1.I1neg[i].end());
    
    
  }*/
  d1.alpha=alpha;
  d1.beta=beta;
  d1.eta=eta;
  d1.gamma=gamma;
  d1.zeta=zeta;
  d1.theta=theta;
  d1.onesided=onesided;
  d1.reverse=reverse;
  d1.Lchoice=L;

  if(!onesided)
    {
      vcl_cout<<"\n normal match";
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
  //transformed and then computed euclidean distance
  //double euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  d1.detect_tail(tail1,tail2);
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  return d1.normfinalCost();
    }
  else
    {
      d1.pos=1;
      d1.neg=0;
   
      if (endPointMatch)
        d1.endPointMatch();
      else
  d1.match();
  
  //transformed and then computed euclidean distance
      
  vcl_map<int,int> mapping1  = d1.alignment;
  vnl_matrix<double> R1_       = d1.R;
  d1.detect_tail(tail1,tail2);
  vnl_matrix<double> Tbar1     = d1.Tbar;
  double scale1   = d1.scale;
  double cost1=d1.normfinalCost();

  d1.neg=1;
  d1.pos=0;
  vcl_map<int,int> mapping2  = d1.alignment;
  vnl_matrix<double> R2_       = d1.R;
  d1.detect_tail(tail1,tail2);
  vnl_matrix<double> Tbar2     = d1.Tbar;
  double scale2   = d1.scale;
  double cost2=d1.normfinalCost();
  if(cost1<cost2)
    {
      mapping=mapping1;
      R_=R1_;
      Tbar=Tbar1;
      scale=scale1;
      vcl_cout<<"\n Positive side";
      return cost1;
    }
  else
    {
      mapping=mapping2;
      R_=R2_;
      Tbar=Tbar2;
      scale=scale2;
      vcl_cout<<"\n Negative side";

      return cost2;
    }
  
    }
}
#endif
