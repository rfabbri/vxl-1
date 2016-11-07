#ifndef _CEDT_H
#define _CEDT_H

#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_map.h>

//CEDT: Curvature-based Euclidean Disvcl_tance Transform
class CEDT;
class DiscreteGridPoint;
class WaveFront;

#include "boundary.h"

//data structure for the entire grid
  //every location has to be accessible by its coordinates

typedef vcl_vector<vcl_vector<DiscreteGridPoint*> > DiscreteGrid;

//data structure to order the wavefronts by time(distance)
  //time ordered list of active wavefronts only
  //need id to discriminate the ones with the same distance

typedef vcl_pair<double, int> T_ID_pair;
typedef vcl_map<T_ID_pair, WaveFront*> OrderedWFList;
typedef vcl_pair<T_ID_pair, WaveFront*> T_ID_WF_pair;

typedef enum 
{
  BOGUS_DIR=-1,
  EAST=0,
  EASTNORTHEAST=1,
  NORTHEAST=2,
  NORTHNORTHEAST=3,
  NORTH=4,
  NORTHNORTHWEST=5,
  NORTHWEST=6,
  WESTNORTHWEST=7,
  WEST=8,
  WESTSOUTHWEST=9,
  SOUTHWEST=10,
  SOUTHSOUTHWEST=11,
  SOUTH=12,
  SOUTHSOUTHEAST=13,
  SOUTHEAST=14,
  EASTSOUTHEAST=15,
} WAVE_FRONT_DIRECTION;

typedef enum
{
  BOGUS_LOC,
  GRID=1,
  CELL=2,
  VERT_GRID=3,
  HORIZ_GRID=4,
}POINT_LOCATION;

//CEDT data structure
  //will depend on the boundary data structure for source models
  //will house the discrete grid points
  //will house the wavefronts
  //will house the structures to manage the wavefronts

class CEDT 
{
protected:
  DiscreteGrid  _grid;
  Boundary*    _boundary;
  OrderedWFList  _OWFList;

  //for locating the grid
  Point2D<int>  _topLeft;
  int        _width;
  int        _height;
  long        _WFcount;

public:
  CEDT (Boundary* bnd, Point2D<int> topLeft, int width, int height);
  ~CEDT ();

  vcl_map<int, WaveFront*> update_list; //keeps the elements that need to be updated

  //general accessibility functions
  Boundary* BND(){return _boundary;}
  OrderedWFList* OWFList(){return &(_OWFList);}
  int Width(){return _width;}
  int Height(){return _height;}
  Point2D<int> TopLeft(){return _topLeft;}
  long GetNextAvailableID(){_WFcount++; return _WFcount;}

  //grid access functions
  DiscreteGridPoint* Grid(int x, int y);

  //methods
  void InitializeWaveFronts();
    void InitializeWaveFrontsFromAnElement(BPoint* bp);
    void InitializeWaveFrontsFromAnElement(BLine* bl);
    void InitializeWaveFrontsFromAnElement(BArc* ba);
    POINT_LOCATION LocationOfAPoint(Point2D<double> pt);
    WAVE_FRONT_DIRECTION convertToDiscreteInitDir(double dir);
  void PropagateWaveFronts();
    WAVE_FRONT_DIRECTION convertToDiscreteDir(double dir);
  int PropagateNextActiveWaveFront();
  void ClearAllWaveFrontsInTheGrid();

  //methods
  void AddAWaveFront(int x, int y, int src_label, double d, int dir, bool _init_prop=false);
  bool IsWaveFrontOnBoundaryLimit(WaveFront* WF);
  bool IsLocationOutsideTheGrid(int x, int y);

};

//data structure for discrete grid points
  //wavefronts live at the discrete gridpoints
  //needs to be able to store information about multiple wavefronts 
  // that will reach any particular discrete location

class DiscreteGridPoint
{
protected:
  Point2D<int>    _loc; //extrinsic location
  vcl_vector<WaveFront*> _fronts;
public:
  DiscreteGridPoint(); //default constructor
  DiscreteGridPoint(int x, int y);
  ~DiscreteGridPoint();

  //accesibility functions
  int numofWaveFronts(){return _fronts.size();}
  WaveFront* getWaveFront(int i){return _fronts[i];}
  vcl_vector<WaveFront*>* Fronts(){return &_fronts;}

  //methods
  WaveFront* AddAWaveFront(int id, int src_label, double d, int dir, bool _init_prop=false);
  void AddAWaveFront(WaveFront* newWF);
  void clearAllWaveFronts();
};

//data structure for wave fronts
  //only exist at discrete grid points
  //carries the information about the particular geometric model(source) that caused it
  //analytic distance value from the source at the current grid point
  //initial discrete directions of propagation
  //flag to identify it as the initial propagation of a wavefront

class WaveFront: public BaseGUIGeometry
{
protected:
  int            _id; //unique id for this wavefront
  Point2D<int>      _loc; //extrinsic location
  int            _label; //id of the source
  double          _d; //distance from source
  WAVE_FRONT_DIRECTION  _dir; //direction of propagation
  bool            _init_prop; //is it the first propagation ?
  bool            _active; //is it still active and so can propagate ?
public:
  WaveFront(int x, int y, int id, int src_label, double d, int dir, bool init_prop=false);
  ~WaveFront();

  //accesibility functions
  int ID(){return _id;}
  Point2D<int> Loc(){return _loc;}
  int Label(){return _label;}
  double Dist(){return _d;}
  WAVE_FRONT_DIRECTION Dir(){return _dir;}
  bool IsInitProp(){return _init_prop;}
  bool IsActive(){return _active;}
  bool IsQuenched(){ return !_active;} //it has been quenched or it reached the propagation limits
  void Deactivate(){_active = false;}
  void Reactivate(){_active = true;}

  virtual void getInfo (vcl_ostream& ostrm);
};

#endif
