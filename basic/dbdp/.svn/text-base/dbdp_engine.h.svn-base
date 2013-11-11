#ifndef _dbdp_dbdp_engine_h
#define _dbdp_dbdp_engine_h
//---------------------------------------------------------------------
// This is basic/dbdp/dbdp_engine.h
//:
// \file
// \brief Dynamic programming engine (based on Sebastien's code)
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-15
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <assert.h>
#include <vbl/vbl_ref_count.h>
#include <dbdp/dbdp_cost.h>
#include <vcl_cfloat.h>

#define DP_VERY_LARGE_COST DBL_MAX

typedef vcl_vector< vcl_vector<double> > DPCostType;
typedef vcl_vector< vcl_vector< vcl_pair <int,int> > > DPMapType;
typedef vcl_vector< vcl_pair<int,int> > FinalMapType;

class dbdp_engine : public vbl_ref_count
{
  //functions
public:
  dbdp_engine(int grid_w, int grid_h, int template_size, dbdp_cost *cost_fnc);
  virtual ~dbdp_engine();
  //Result of matching:
  DPCostType *DPCost() { return &DPCost_; }
  DPMapType *DPMap() { return &DPMap_; }
  FinalMapType *finalMap() { return &finalMap_; }
  vcl_vector<double> *finalMapCost() { return &finalMapCost_; }
  double finalCost() { return finalCost_; }

  int getFMapFirst(int i) 
  {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond(int i) 
  {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  void setFinalMap(FinalMapType map) { finalMap_ = map; }
  void setFinalMap(int i, int first, int second)
  {
    finalMap_[i].first = first;
    finalMap_[i].second = second;
  }

  void RunDP();

protected:
  void setTemplateSize(int size);
  void initializeDPCosts();
  double computeIntervalCost(int i, int ip, int j, int jp);
  void computeDPCosts();
  void findDPCorrespondence();
  //variables
public:

protected:
  DPCostType DPCost_;               //DPMap of cost: n*m array of double
  DPMapType DPMap_;                 //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  FinalMapType finalMap_;           //minimizing path
  vcl_vector<double> finalMapCost_; //cost of minimizing path
  double finalCost_;                //final cost

  // number of samples on the grid along each of the two directions
  int grid_w_, grid_h_;
  // template size
  int template_size_;
  // pointer to cost computation class
  dbdp_cost *cost_fnc_;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;
};

#endif

