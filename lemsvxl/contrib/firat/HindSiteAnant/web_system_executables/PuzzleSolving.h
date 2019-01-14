#ifndef PUZZLESOLVING_H
#define PUZZLESOLVING_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <bxml/bxml_document.h>

#include "bfrag_curve.h"
#include "Point.h"
#include "DPMatch.h"
#include "FineDPMatch.h"
#include "xform.h"
#include "utils.h"
#include "extern_params.h"

using namespace std;

typedef std::vector<std::pair<int,int> > intMap;
typedef std::pair<double,std::vector<std::pair<int,int> > > map_with_cost;
typedef std::vector<std::pair<double, int> > indexedMeasures;

class map_cost_less
{
public:
  bool operator() (std::pair<double,std::vector<std::pair<int,int> > > map1, 
                   std::pair<double,std::vector<std::pair<int,int> > > map2) 
  { return map1.first < map2.first; }
};

class cost_ind_less 
{
public:
  bool operator()(std::pair<double,int> c1, std::pair<double,int> c2){ return c1.first<c2.first; }
};

//essentially a struct, but i need to define operator(s) on it
class pairwiseMatch
{
	//So this is an object which is actually a pair of fragments. it holds the cost of the pair, has an index
public:
  // DATA
  double               cost;         //old DOULBE from cost vector (the actual cost)
  int                  myIndex;      //old  INT   from cost vector (which numMatch am i)
  intMap               pointMap;     //old _maps
  XForm3x3             xForm;        //old _xForms
  std::pair<int,int>    whichCurves;  //old _pairs

  void write_out(std::ofstream &out);
  void read_in(std::ifstream &in);
    
  pairwiseMatch(){};
  //i use this operator explicity in sort b/c otherwise it seems sort will compare
  //the memory location of the pointers, and not use the pointers' operators
  bool operator() (pairwiseMatch *a, pairwiseMatch *b){ return a->cost < b->cost; }
};

class pairwiseMatchSort2 
{
public:
  bool operator()(pairwiseMatch a, pairwiseMatch b) { return a.cost < b.cost; }
};

class pairwiseMatchSort 
{
public:
//  bool operator()(pairwiseMatch a, pairwiseMatch b) { return a.cost < b.cost; }
  bool operator()(std::pair <int, double> a, std::pair <int, double> b) { return a.second < b.second; }//So basically returns true if cost of 1 is less than the other.
};

class searchState
{
public:
  // Added for helping with visualizing issue, has nothing to do with the algorithm
  // -1 is not assigned
  int state_id_;
/*/  std::vector<bfrag_curve> _cList;

  //pointer style
//  std::vector< pairwiseMatch* > _matches;
  //non pointer style
//  std::vector< pairwiseMatch > _matches;
*/  // transformation applied to the global curves in this state so far
  std::vector< vnl_matrix_fixed<double, 3, 3> > transform_list_;
  // rotation angle applied to the global curves in this state so far
  std::vector<double> rot_ang_list_;

  // the first element is the match index
  // the second element is the cost
  // this is done for efficiency
  std::vector< std::pair <int, double> > matches_ez_list_;
  
  int _numMatch;
  int _num_new;
  bfrag_curve _merged;
  std::vector<bfrag_curve> _constr;

  void sortPairwiseMatches();

  bool active;

  //List of the indices of the pieces added to the puzzle.
  std::vector<int> process;

  int nProcess;
  double tCost;
  double sCost;
  std::vector<std::pair<std::vector<int>,vgl_point_2d<double> > > open_junc;
  std::vector<std::pair<std::vector<int>,vgl_point_2d<double> > > closed_junc;
  std::vector<std::pair<int,int> > old_edges;
  std::vector<std::pair<int,int> > new_edges;
  std::pair<int,int> new_edge;

  searchState(){ state_id_ = -1; };
  searchState(std::vector<bfrag_curve> 
	  );//{ state_id_ = -1; std::cout<<"new state created thorugh contours"<<std::endl;};
  ~searchState()
  {
//    _cList.clear();
//    _matches.clear();
    _constr.clear();
    process.clear();
    open_junc.clear();
    closed_junc.clear();
    old_edges.clear();
    new_edges.clear();
  };

  bool operator==(searchState state2);
  void merge(bfrag_curve *cv, intMap &map);
  void updateCost();
  void structure();
  void comboMatch();
  void addMatch(std::vector<std::pair<int,int> > map, double cst, int c1, int c2);
  void purge(); 

  bfrag_curve *piece(int index){return &_cList[index];} //keeper
  bfrag_curve *merged(){return &_merged;}
  bfrag_curve *constr(int index){return &_constr[index];}
  bool is_constr()
  {
    if(_constr.size() == 0)
      return false;
    else
      return true;
  }

  unsigned numMatch(){return _numMatch;}
  unsigned numPiece(){return _cList.size();}
  unsigned numCon(){return _constr.size();}

  // for the web system
  void write_out(std::string fname);
  void read_in(std::string fname);
  void write_frag_pairs_in_xml(std::string fname);

  void load_state_curves_list();
};

class search_state_sort 
{
public:
//  bool operator()(pairwiseMatch a, pairwiseMatch b) { return a.cost < b.cost; }
  bool operator()(searchState a, searchState b) { return a.tCost < b.tCost; }
};

class PuzzleSolving
{
public:
  std::vector<std::vector<int> > _pairMatchesByPiece;

  //my new, do nothing constructor
  PuzzleSolving() 
  { 
    _nPieces = 0;
    tot_num_iters_ = 0;
  };
  
  ~PuzzleSolving() {};

  //--spinner
  void setTopContours(const std::vector<bfrag_curve> &contoursIN );
  void setBotContours(const std::vector<bfrag_curve> &BotcontoursIN );
  void preProcessCurves();
  //--spinner

  searchState pairMatch();
  std::vector<searchState> search(std::vector<searchState> states);
  std::vector<searchState> iterateSearch(searchState this_state, int f);

  void write_experiment_search_state_and_puzzle_solving_objects(std::string fname, std::vector<searchState> &states, bool write_matches);
  void read_experiment_search_state_and_puzzle_solving_objects(std::string fname, std::vector<searchState> &states, bool read_matches);
  // Here is how the parameter system works when separate web executables for pairwise matching(PWM) and puzzle solving
  // iteration (PSI) are run
  // The user specifies values for parameters used for PWM and then executes the program
  // Some of these parameters are not saved because they are used only for PWM and we do PWM only once for an experiment
  // Some of these parameters are common to PWM and PSI; these are saved since we will reuse them when PSI is run
  // When PSI is executed for the first time, the user specifies values for parameters only used for PSI because up to this point,
  // we do not know what these parameter values are. For the first PSI execution, parameters that were common with PWM
  // are loaded from file
  // For executions of PSI after the first time, both common parameters and PSI-only parameters are loaded from file, the user
  // only specifies N (number of iterations)

//private:
public:
  int _nPieces;

  void write_out(std::string fname);
  void write_out_matches(std::string fname);
  void write_out_frags(std::string fname);
  void read_in(std::string fname);
  void read_in_matches(std::string fname);
  void read_in_frags(std::string fname);

  void write_frag_assemblies_in_xml(std::string fname, std::vector<searchState> &states);
  unsigned tot_num_iters_;
};

// Global Functions
std::vector<map_with_cost> testDP(bfrag_curve &c1, bfrag_curve &c2);
map_with_cost fineScaleMatch(bfrag_curve c1i, bfrag_curve c2, std::vector<std::pair<int,int> > fmap);
intMap localReg(bfrag_curve *cv, bfrag_curve *mg, double* dis,double* len, double* dgn, int num_iter);

int edgeCheck(bfrag_curve *c1, bfrag_curve *c2, intMap map);
bool distanceCheck(bfrag_curve *c1, bfrag_curve *c2, int checkIndex1, int checkIndex2);
//Returns true if angles are close to one another
bool anglesAreClose(double angle1, double angle2);
bool inPolygon(double x, double y, bfrag_curve *c);
std::pair<double,int> ptDist(double x, double y, bfrag_curve *c);
std::pair<double,int> new_ptDist(double x, double y, bfrag_curve *c, unsigned index);
double detectOverlap(bfrag_curve *c1, bfrag_curve *c2);
XForm3x3 regContour(bfrag_curve *c1, bfrag_curve *c2, std::vector<std::pair<int,int> > map, bool flip=0, XForm3x3 *out = 0);
double flat(bfrag_curve *c);
std::pair<double,double> center(bfrag_curve *c);

#endif  /*  PUZZLESOLVING_H */
