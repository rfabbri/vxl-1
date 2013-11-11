#ifndef dbsk3dr_dpmatch_h_
#define dbsk3dr_dpmatch_h_
//---------------------------------------------------------------------
// This is brcv/rec/dbsk3dr/dbsk3dr_dpmatch.h
//:
// \file
// \brief open curve matching adapted from original source code of Thomas Sebastian
//
//
// \author
//  MingChing Chang 
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_utility.h>
#include <dbsk3dr/dbsk3dr_ms_curve.h>

#define DP_VERY_LARGE_COST 1E10

class dbsk3dr_dpmatch
{
protected:
  bool                use_radius_DP_;
  dbsk3dr_ms_curve*   _curve1;
  dbsk3dr_ms_curve*   _curve2;

  //: DPMap of cost: n*m array of double
  vcl_vector< vcl_vector<double> >                _DPCost;          

  //: DPMap of prev point vcl_map: n*m array of vcl_pair of index
  vcl_vector< vcl_vector< vcl_pair <int,int> > >  _DPMap;          

  //: alignment curve
  vcl_vector< vcl_pair<int,int> >                 _finalMap;        

  //: cost on alignment curve
  vcl_vector<double>                              _finalMapCost;      

  //: final cost
  double          _finalCost;        

  bool            _normalized_stretch_cost;
  int             _template_size;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

public:

  //###### Constructor & Destructor ######  
  dbsk3dr_dpmatch (const bool use_radius = true);

  virtual ~dbsk3dr_dpmatch ();

  void clear () {
    _curve1 = NULL;
    _curve2 = NULL;
    _DPCost.clear();
    _DPMap.clear();
    _finalMap.clear();
    _finalMapCost.clear();
    ///XOFFSET.clear();
    ///YOFFSET.clear();
  }

  //###### Data Access Functions ######
  const bool use_radius_DP () const {
    return use_radius_DP_;
  }
  void set_use_radius_DP (const bool use_radius) {
    use_radius_DP_ = use_radius;
  }
  //Result of matching:
  vcl_vector< vcl_vector<double> >* DPCost() { 
    return &_DPCost; 
  }
  vcl_vector< vcl_vector< vcl_pair <int,int> > >* DPMap() { 
    return &_DPMap; 
  }
  vcl_vector< vcl_pair<int,int> >* finalMap() { 
    return &_finalMap; 
  }
  vcl_vector<double>* finalMapCost() { 
    return &_finalMapCost; 
  }
  double finalCost() { 
    return _finalCost; 
  }

  int getFMapFirst (int i) {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  dbsk3dr_ms_curve* curve1() { return _curve1; }
  dbsk3dr_ms_curve* curve2() { return _curve2; }
  void setCurve1 (dbsk3dr_ms_curve* c1) {
    _curve1 = c1;
  }
  void setCurve2 (dbsk3dr_ms_curve* c2) {
    _curve2 = c2;
  }

  void setStretchCostFlag (bool flag) {
        _normalized_stretch_cost = flag;
  }
  void setTemplateSize (int size);

  //###### Functions for matching ######
  double stretch_cost (dbsk3dr_ms_curve* curve, int i, int ip);
  double bend_cost (dbsk3dr_ms_curve* curve, int i, int ip);
  double radius_cost (dbsk3dr_ms_curve* curve, int i, int ip);

  void initializeDPCosts();
  virtual double computeIntervalCost (int i, int ip, int j, int jp);

  void computeDPCosts ();
  void findDPCorrespondence ();

  void Match ();

  void GetAverageCurveFromAMatch (dbsk3dr_ms_curve* AverageCurve);
  void GetSumCurveFromAMatch (dbsk3dr_ms_curve* SumCurve);
  void GetMorphingCurvesFromAMatch (dbsk3dr_ms_curve* MorphingCurves, int NumMorphs);

  //###### Debug Print ######
  void ListDPTable (void);
  void ListDPTable_full (void);
  void ListAlignCurve (void);
  void ListAlignCurve_full (void);

  void SaveDPTable (vcl_string& _fileName1, vcl_string& _fileName2);
  void SaveAlignCurve (vcl_string& _fileName1, vcl_string& _fileName2);
};


//value in (-Pi,Pi]
double dbsk3dr_angle_mpipi (double a);

//Does a1-a2, value in (-Pi,Pi]
double dbsk3dr_angle_diff (double a1, double a2);

double dbsk3dr_max3 (double a, double b, double c);

#endif


