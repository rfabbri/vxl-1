//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_dpmatch.h
//:
// \file
// \brief This is a curve matching class subclassed from dbcvr_cvmatch_even
//        to match dbskr_scurve instances
//        Dynamic programming core is the same with dbcvr curve matching
//        interval cost computation is different and specific to shock curves.
//
// \author
//  O.C. Ozcanli - September 20, 2005
//
// \verbatim
//  Modifications
//  Ozge C. Ozcanli - March 28, 07 -- added coarse cost method which returns
//                                    the cost of the diagonal in the DP match table
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dbskr_dpmatch_h
#define _dbskr_dpmatch_h

#include <dbcvr/dbcvr_cvmatch_even.h>
#include <dbskr/dbskr_scurve_sptr.h>

class dbskr_dpmatch : public dbcvr_cvmatch_even 
{
 public:

  dbskr_dpmatch();
  dbskr_dpmatch(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2);
  dbskr_dpmatch(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2, double R1, vcl_vector<double>& lambda, int num_cost_elems);
  virtual ~dbskr_dpmatch();
    
  //overwritten DP functions
  virtual void initializeDPCosts();
  virtual double computeIntervalCost (int i, int ip, int j, int jp);
  virtual double computeIntervalCostPrint(int i, int ip, int j, int jp);

  //: ozge added this function to find an approximate cost before giving DP solution
  //  needed by tree-edit shock curve deformation cost computation to prune some high cost matchings
  float approx_cost();

  //: ozge added this function
  //  it adds initial time/radius difference and alpha difference costs to the final cost of matching
  virtual float init_dr();
  virtual float init_phi();

  double recompute_final_cost();

  //: ozge added this function to be used for coarse tree matching (includes init_dr and init_phi)
  float coarse_match();

  void set_scurves(dbskr_scurve_sptr sc1, dbskr_scurve_sptr sc2);
  void set_R(double R) { R1_ = R; }
  double get_R(double R) { return R1_; }

 protected:

  //shock curves to match
  dbskr_scurve_sptr scurve1_;
  dbskr_scurve_sptr scurve2_;

  int num_cost_elems_;
  double R1_;

  vcl_vector<double> lambda_;

  // temp arrays to use for cost computation
  vcl_vector <double> ds1_;
  vcl_vector <double> ds2_;
  vcl_vector <double> dt1_;
  vcl_vector <double> dt2_;
};


#endif // _dbskr_dpmatch_h
