#ifndef PUZZLESOLVING_H
#define PUZZLESOLVING_H

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <bxml/bxml_document.h>

#include "bfrag_curve.h"
#include "Point.h"
#include "DPMatch.h"
#include "FineDPMatch.h"
#include "xform.h"
#include "utils.h"
#include "extern_params.h"

using namespace std;

typedef vcl_vector<vcl_pair<int,int> > intMap;
typedef vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map_with_cost;
typedef vcl_vector<vcl_pair<double, int> > indexedMeasures;

class map_cost_less
{
public:
  bool operator() (vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map1, 
                   vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map2) 
  { return map1.first < map2.first; }
};

class cost_ind_less 
{
public:
  bool operator()(vcl_pair<double,int> c1, vcl_pair<double,int> c2){ return c1.first<c2.first; }
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
  vcl_pair<int,int>    whichCurves;  //old _pairs

  void write_out(vcl_ofstream &out);
  void read_in(vcl_ifstream &in);
    
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
  bool operator()(vcl_pair <int, double> a, vcl_pair <int, double> b) { return a.second < b.second; }//So basically returns true if cost of 1 is less than the other.
};

class searchState
{
public:
  // Added for helping with visualizing issue, has nothing to do with the algorithm
  // -1 is not assigned
  int state_id_;
/*/  vcl_vector<bfrag_curve> _cList;

  //pointer style
//  vcl_vector< pairwiseMatch* > _matches;
  //non pointer style
//  vcl_vector< pairwiseMatch > _matches;
*/  // transformation applied to the global curves in this state so far
  vcl_vector< vnl_matrix_fixed<double, 3, 3> > transform_list_;
  // rotation angle applied to the global curves in this state so far
  vcl_vector<double> rot_ang_list_;

  // the first element is the match index
  // the second element is the cost
  // this is done for efficiency
  vcl_vector< vcl_pair <int, double> > matches_ez_list_;
  
  int _numMatch;
  int _num_new;
  bfrag_curve _merged;
  vcl_vector<bfrag_curve> _constr;

  void sortPairwiseMatches();

  bool active;

  //List of the indices of the pieces added to the puzzle.
  vcl_vector<int> process;

  int nProcess;
  double tCost;
  double sCost;
  vcl_vector<vcl_pair<vcl_vector<int>,vgl_point_2d<double> > > open_junc;
  vcl_vector<vcl_pair<vcl_vector<int>,vgl_point_2d<double> > > closed_junc;
  vcl_vector<vcl_pair<int,int> > old_edges;
  vcl_vector<vcl_pair<int,int> > new_edges;
  vcl_pair<int,int> new_edge;

  searchState(){ state_id_ = -1; };
  searchState(vcl_vector<bfrag_curve> 
	  );//{ state_id_ = -1; vcl_cout<<"new state created thorugh contours"<<vcl_endl;};
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
  void addMatch(vcl_vector<vcl_pair<int,int> > map, double cst, int c1, int c2);
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
  void write_out(vcl_string fname);
  void read_in(vcl_string fname);
  void write_frag_pairs_in_xml(vcl_string fname);

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
  vcl_vector<vcl_vector<int> > _pairMatchesByPiece;

  //my new, do nothing constructor
  PuzzleSolving() 
  { 
    _nPieces = 0;
    tot_num_iters_ = 0;
  };
  
  ~PuzzleSolving() {};

  //--spinner
  void setTopContours(const vcl_vector<bfrag_curve> &contoursIN );
  void setBotContours(const vcl_vector<bfrag_curve> &BotcontoursIN );
  void preProcessCurves();
  //--spinner

  searchState pairMatch();
  vcl_vector<searchState> search(vcl_vector<searchState> states);
  vcl_vector<searchState> iterateSearch(searchState this_state, int f);

  void write_experiment_search_state_and_puzzle_solving_objects(vcl_string fname, vcl_vector<searchState> &states, bool write_matches);
  void read_experiment_search_state_and_puzzle_solving_objects(vcl_string fname, vcl_vector<searchState> &states, bool read_matches);
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

  void write_out(vcl_string fname);
  void write_out_matches(vcl_string fname);
  void write_out_frags(vcl_string fname);
  void read_in(vcl_string fname);
  void read_in_matches(vcl_string fname);
  void read_in_frags(vcl_string fname);

  void write_frag_assemblies_in_xml(vcl_string fname, vcl_vector<searchState> &states);
  unsigned tot_num_iters_;
};

// Global Functions
vcl_vector<map_with_cost> testDP(bfrag_curve &c1, bfrag_curve &c2);
map_with_cost fineScaleMatch(bfrag_curve c1i, bfrag_curve c2, vcl_vector<vcl_pair<int,int> > fmap);
intMap localReg(bfrag_curve *cv, bfrag_curve *mg, double* dis,double* len, double* dgn, int num_iter);

int edgeCheck(bfrag_curve *c1, bfrag_curve *c2, intMap map);
bool distanceCheck(bfrag_curve *c1, bfrag_curve *c2, int checkIndex1, int checkIndex2);
//Returns true if angles are close to one another
bool anglesAreClose(double angle1, double angle2);
bool inPolygon(double x, double y, bfrag_curve *c);
vcl_pair<double,int> ptDist(double x, double y, bfrag_curve *c);
vcl_pair<double,int> new_ptDist(double x, double y, bfrag_curve *c, unsigned index);
double detectOverlap(bfrag_curve *c1, bfrag_curve *c2);
XForm3x3 regContour(bfrag_curve *c1, bfrag_curve *c2, vcl_vector<vcl_pair<int,int> > map, bool flip=0, XForm3x3 *out = 0);
double flat(bfrag_curve *c);
vcl_pair<double,double> center(bfrag_curve *c);

#endif  /*  PUZZLESOLVING_H */
