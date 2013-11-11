#ifndef _dbcvr_cvmatch_h
#define _dbcvr_cvmatch_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_cvmatch.h
//:
// \file
// \brief open curve matching with even spacing on both curves (input curve class is interp_curve_2d)
//
//
// \author
//  Ozge Can Ozcanli
//
// \verbatim
//  Modifications
//   Anil Usumezbas June 30th, 2010: Added a function to return the size of the final map
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include "dbcvr_cvmatch.h"

#include <vbl/vbl_ref_count.h>

class dbcvr_cvmatch_even : public vbl_ref_count
{
protected:

  dbsol_interp_curve_2d_sptr    _curve1;
  dbsol_interp_curve_2d_sptr    _curve2;

  int _n1;
  int _n2;

  double _delta_s1;
  double _delta_s2;

  DPCostType        _DPCost;          //DPMap of cost: n*m array of double
  DPMapType        _DPMap;          //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  FinalMapType      _finalMap;        //alignment curve
  vcl_vector<double>  _finalMapCost;      //cost on alignment curve
  
  //: compute tangents and arclengths at each sample on the curves
  vcl_vector<double>  _tangents_curve1;
  vcl_vector<double>  _tangents_curve2;

  vcl_vector<double>  _lengths_curve1;
  vcl_vector<double>  _lengths_curve2;

  double          _finalCost;        //final cost

  double  _R;
  bool _normalized_stretch_cost;
  int _template_size;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

public:

  void clear(); 

  //Result of matching:
  DPCostType*        DPCost()      { return &_DPCost; }
  DPMapType*        DPMap()      { return &_DPMap; }
  FinalMapType*      finalMap()    { return &_finalMap; }
  vcl_vector<double>*  finalMapCost() { return &_finalMapCost; }
  double          finalCost()    { return _finalCost; }
  unsigned        finalMapSize() { return _finalMap.size();}

  int getFMapFirst (int i) {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  vcl_string    _fileName1, _fileName2;
  dbsol_interp_curve_2d_sptr curve1() { return _curve1; }
  dbsol_interp_curve_2d_sptr curve2() { return _curve2; }
  void setFinalMap (FinalMapType map) {
    _finalMap = map;
  }
  void setFinalMap (int i, int first, int second) {
    _finalMap[i].first = first;
    _finalMap[i].second = second;
  }
  void setStretchCostFlag (bool flag) {
        _normalized_stretch_cost = flag;
  }
  void setTemplateSize (int size);

  dbcvr_cvmatch_even ();
  dbcvr_cvmatch_even (dbsol_interp_curve_2d_sptr c1, 
                      dbsol_interp_curve_2d_sptr c2, 
                      int n1, 
                      int n2, 
                      double R, 
                      int template_size);

  virtual ~dbcvr_cvmatch_even ();

  virtual void initializeDPCosts();
  virtual double computeIntervalCost (int i, int ip, int j, int jp);

  void computeDPCosts ();
  void findDPCorrespondence ();

  void Match ();
};

#endif

