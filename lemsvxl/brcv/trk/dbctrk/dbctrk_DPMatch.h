
#ifndef __TBS_dbctrk_DPMatch_DEF__
#define __TBS_dbctrk_DPMatch_DEF__
#include <vcl_map.h>
#include <vcl_vector.h> 
#include <vcl_utility.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_2.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <mvl/HMatrix2D.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#define DP_VERY_LARGE_COST 1E10
#include <dbctrk/dbctrk_curve.h>
#include <mvl/FMatrix.h>

class dbctrk_DPMatch
{
 public:
  dbctrk_DPMatch();
  dbctrk_DPMatch(Curve &c);
  dbctrk_DPMatch(Curve &c1, Curve &c2);
  dbctrk_DPMatch(Curve &c1, Curve &c2,vgl_point_2d<double> & e);
  dbctrk_DPMatch(Curve &c1, Curve &c2,FMatrix F);
  dbctrk_DPMatch(vcl_vector<vcl_pair<double,double> > v1,
          vcl_vector<vcl_pair<double,double> > v2,
      vnl_double_2 & e);
  void init();
  ~dbctrk_DPMatch(){};
  //access functions
  double finalCost(){return finalCost_;}
  double normfinalCost();
  double finalCost(double cost){finalCost_=cost; return finalCost_;};
  vcl_vector <vcl_pair <int,int> > finalMap(){return finalMap_;};
  vcl_vector< double > finalMapCost(){return finalMapCost_;};
  int n(){return n_;};
  int m(){return m_;};

  void lambda(vcl_vector<double> l){lambda_=l;};

  void printCost();
  void writeCost(vcl_string f);
  void printMap();
  void printFinalMap();

  Curve curve1(){return curve1_;};
  Curve curve2(){return curve2_;};

  void match();
  void endPointMatch();
  void detect_tail(vcl_vector<int> &tail1,vcl_vector<int> &tail2);
  double SpliceCost(Curve c);
  vnl_matrix<double> R;
  vnl_matrix<double> T;
  vnl_matrix<double> Tbar;
  double scale;
  vcl_map<int,int> alignment;
  vcl_map<int,int> refine_mapping();

  double computeEpipolarCost(int i, int ip, int j, int jp);
  double computeForwardEpipolarCost(int i, int ip, int j, int jp);
  double ComputeFmatrixCost(int i, int ip, int j, int jp);
  double computeIntensityL1(int i,int ip, int j, int  jp);
  double computeIntensityL2(int i,int ip, int j, int  jp);
  double computeIntensityLinf(int i,int ip,int j,int  jp);
  double computeIntensitySecondOrder(int i,int ip, int j, int  jp);

  double euclidean_distance(vnl_matrix<double> R,vnl_matrix<double> T,double s);
  double transformed_euclidean_distance();
  void deformation_cost();
  double matched_len1;
  double matched_len2;
  
  double ds1_;
  double ds2_;
  double dt1_;
  double dt2_;
  Curve curve1_;
  Curve curve2_;

  int alpha, beta , gamma,delta;
  double eta,zeta,theta;
  bool onesided;
  bool reverse;
  int Lchoice;
  double pos,neg;
  double mu;
  bool backward;
  // RGB or IHS or Lab data
  vcl_vector<vcl_vector<double> > I1pos;
  vcl_vector<vcl_vector<double> > I2pos;
  vcl_vector<vcl_vector<double> > I1neg;
  vcl_vector<vcl_vector<double> > I2neg;

 protected:
  //Data
  vdgl_digital_curve_sptr dc1;
  vdgl_digital_curve_sptr dc2;


  vcl_vector< vcl_vector<double> > cost_;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > map_;
  vcl_vector< vcl_pair <int,int> > finalMap_;
  double finalCost_;
  vcl_vector< double > finalMapCost_;
  int n_;
  int m_;
  bool flip_;
  vnl_double_2 ep_;
  vgl_homg_point_2d<double> ep_pt;
  int numCostElems_;
  double R1_;
  double R2_;
  vcl_vector<double> lambda_;
  FMatrix F_;
  //Functions
  void initializeDPCosts(bool reverse);
  void computeDPCosts();
  void computeDPCosts(bool reverse);

  void findDPCorrespondence();
  void findDPCorrespondence(int n, int m);
  void findDPCorrespondence(bool reverse);

  void findEndPoint();
  double computeIntervalCost(int i, int ip, int j, int jp);
  double computeIntervalCost(int i, int ip, int j, int jp,bool reverse);

  
};

#endif
