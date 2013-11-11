//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_dpmatch_combined.h
//:
// \file
// \brief This is a curve matching class subclassed from dbskr_dpmatch.h
//        to match dbskr_scurve instances. Only the interval cost computation
//        is different where the width and relative orientation differences of 
//        the matched shock branches are combined 
// 
//        In the original implementation of Sebastian et al. PAMI 2006, the interval cost is given by:
//        (|d_r1 - d_r2| + |d_phi1 - d_phi2|) where d_r1 = |r1_i - r1_ip|, 
//                                                  d_r2 = |r1_j - r1_jp|, 
//                                                  d_phi1 = anglediff(phi1_i, phi1_ip), 
//                                                  d_phi2 = anglediff(phi2_j, phi2_jp)
//        and the initial costs:
//        (|r1_0 - r2_0| + |r1_end - r2_end|)/2 + (anglediff(phi1_0, phi2_0) + anglediff(phi1_end, phi2_end))/2 are added to get the shock branch deformation cost.
//
//        In this new version proposed by Kimia, Ozcanli, Tamrakar in 2006, the width and orientation differences are combined
//        in the interval cost which is given by
//        2*(|d_r1 - d_r2|) where
//        d_r1 = |r1_i*sin(phi1_i)- r1_ip*sin(phi1_ip)|, d_r2 = |r2_j*sin(phi2_j)- r2_jp*sin(phi2_jp)| 
//        and the initial costs:
//        2*|r1_0*sin(phi1_0) - r2_0*sin(phi2_0)|
//        
// 
//
// \author
//  O.C. Ozcanli - SMay 17, 2007
//
// \verbatim
//  Modifications
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dbskr_dpmatch_combined_h
#define _dbskr_dpmatch_combined_h

#include <dbskr/dbskr_dpmatch.h>

class dbskr_dpmatch_combined : public dbskr_dpmatch
{
 public:

  dbskr_dpmatch_combined() : dbskr_dpmatch() {}
  dbskr_dpmatch_combined(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2) : dbskr_dpmatch(c1, c2) {}
  //virtual ~dbskr_dpmatch_combined();
    
  //overwritten DP functions
  virtual double computeIntervalCost (int i, int ip, int j, int jp);
  virtual double computeIntervalCostPrint(int i, int ip, int j, int jp);

  //: ozge added this function
  //  it adds initial time/radius difference and alpha difference costs to the final cost of matching
  virtual float init_dr();
  virtual float init_phi();
};


#endif // _dbskr_dpmatch_combined_h
