#ifndef _curve_match_2d_h
#define _curve_match_2d_h

#include <cmath>
#include <string>
#include <vector>
#include <utility>

#include "bsol/bsol_intrinsic_curve_2d.h"
#include "bsol/bsol_intrinsic_curve_2d_sptr.h"
#include "curve_dpmatch_2d.h"
#include <vbl/vbl_ref_count.h>
#define DP_VERY_LARGE_COST 1E10

//value in (-Pi,Pi]
double curve_fixAngleMPiPi (double a);

//Does a1-a2, value in (-Pi,Pi]
double curve_angleDiff (double a1, double a2);

typedef std::vector< std::vector<double> >            DPCostType;
typedef std::vector< std::vector< std::pair <int,int> > >  DPMapType;
typedef std::vector< std::pair<int,int> >            FinalMapType;

class curve_dpmatch_2d : public vbl_ref_count
{
protected:

  bsol_intrinsic_curve_2d_sptr    _curve1;
  bsol_intrinsic_curve_2d_sptr    _curve2;

  DPCostType        _DPCost;          //DPMap of cost: n*m array of double
  DPMapType        _DPMap;          //DPMap of prev point std::map: n*m array of std::pair of index
  FinalMapType      _finalMap;        //alignment curve
  std::vector<double>  _finalMapCost;      //cost on alignment curve
  double          _finalCost;        //final cost

  double  _R;

public:

  //Result of matching:
  DPCostType*        DPCost()      { return &_DPCost; }
  DPMapType*        DPMap()      { return &_DPMap; }
  FinalMapType*      finalMap()    { return &_finalMap; }
  std::vector<double>*  finalMapCost() { return &_finalMapCost; }
  double          finalCost()    { return _finalCost; }

  int getFMapFirst (int i) {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  std::string    _fileName1, _fileName2;
  bsol_intrinsic_curve_2d_sptr curve1() { return _curve1; }
  bsol_intrinsic_curve_2d_sptr curve2() { return _curve2; }
  void setCurve1 (bsol_intrinsic_curve_2d_sptr c1) {
    _curve1 = c1;
    //_curve1.curveMatch = this;
  }
  void setCurve2 (bsol_intrinsic_curve_2d_sptr c2) {
    _curve2 = c2;
    //_curve2.curveMatch = this;
  }
  void setFinalMap (FinalMapType map) {
    _finalMap = map;
  }

  double stretchCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
    return curve->arcLength(i) - curve->arcLength(ip);
  }
  double bendCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
    return curve_angleDiff (curve->angle(i), curve->angle(ip));
  }

  curve_dpmatch_2d ();
  curve_dpmatch_2d (bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2);
  virtual ~curve_dpmatch_2d ();

  void initializeDPCosts();
  virtual double computeIntervalCost (int i, int ip, int j, int jp, std::vector<double> &py);

  void computeDPCosts ();
  void findDPCorrespondence ();

  void Match ();

  void GetAverageCurveFromAMatch (bsol_intrinsic_curve_2d_sptr AverageCurve);
  void GetSumCurveFromAMatch (bsol_intrinsic_curve_2d_sptr SumCurve);
  void GetMorphingCurvesFromAMatch (bsol_intrinsic_curve_2d_sptr MorphingCurves, int NumMorphs);

  //Debug Print...
  void ListDPTable (void);
  void ListAlignCurve (void);
  void SaveDPTable (void);
  void SaveAlignCurve (void);
};

#endif

