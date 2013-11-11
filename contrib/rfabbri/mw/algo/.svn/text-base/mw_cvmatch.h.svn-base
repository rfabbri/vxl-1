#ifndef _mw_cvmatch_h
#define _mw_cvmatch_h

//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/mw_cvmatch.h
//:
// \file
// \brief closed curve matching using even sampling on both of the curves
//
//
// \author
//  O.C. Ozcanli - April 27, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------


#include <vcl_ctime.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>

#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_clsd_cvmatch_even.h>

#include <dbdif/dbdif_rig.h>

class mw_cvmatch : public dbcvr_clsd_cvmatch_even
{

protected:

  dbdif_rig *rig_;

  virtual double computeIntervalCost(int i, int ip, int j, int jp);

  double epipolar_error_threshold_;

public:
  mw_cvmatch();
  mw_cvmatch( const dbsol_interp_curve_2d_sptr c1, 
              const dbsol_interp_curve_2d_sptr c2, 
              int n1,
              int n2,
              double R, 
              int template_size,
              double epipolar_error_threshold)
    : dbcvr_clsd_cvmatch_even (c1,c2,n1,n2,R,template_size),
      epipolar_error_threshold_(epipolar_error_threshold)
    {}
//  mw_cvmatch(const dbsol_interp_curve_2d_sptr c1, 
//                          const dbsol_interp_curve_2d_sptr c2, 
//                          int n1,
//                          int n2,
//                          double R, 
//                          int template_size);
  void set_rig (dbdif_rig *rig) {rig_ = rig;}

};

#endif
