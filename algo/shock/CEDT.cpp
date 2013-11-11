#include "CEDT.h"
#include <vcl_ostream.h>
#include <vnl/vnl_math.h>

//Contour-based Distance Transform (CEDT)
CEDT::CEDT (Boundary* bnd, Point2D<int> topLeft, int width, int height):
  _boundary(bnd), _topLeft(topLeft), _width(width), _height(height)
{
  int i,j;

  //setup the grid structure
  _grid.resize(_width+1);
  for (i=0; i<=_width; i++){
    _grid[i].resize(_height+1);
  }

  //initialize the Discrete Grid points
  for (i=0; i<=_width; i++){
    for (j=0; j<=_height; j++){
      _grid[i][j] = new DiscreteGridPoint(i+_topLeft.getX(), j+_topLeft.getY());
    }
  }

  //initialize the Ordered Wavefront list
  _OWFList.clear();
  _WFcount = 0;

  update_list.clear();
}

CEDT::~CEDT ()
{
  //clear the ordered wavefront list
  OWFList()->clear();

  //delete the grid
  for (int i=0; i<=_width; i++){
    for (int j=0; j<=_height; j++){
      delete _grid[i][j];
    }
    _grid[i].clear();
  }
  _grid.clear();

}

//grid access functions
DiscreteGridPoint* CEDT:: Grid(int x, int y)
{
  return _grid[x-_topLeft.getX()][y-_topLeft.getY()];
}

//methods
void CEDT::InitializeWaveFronts()
{
  //if there exist any wavefrontsm then this operation is not required
  if (_WFcount)
    return;

  //go through the boundary elements and initialize discrete waves from them
  BElmListIterator elmPtr = _boundary->BElmList.begin();
  for (; elmPtr != _boundary->BElmList.end(); elmPtr++) {
    BElement* current = (elmPtr->second);

    switch (current->type()) {
      case BPOINT:
      {
        BPoint* bp = (BPoint*)current;
        InitializeWaveFrontsFromAnElement(bp);
        break;
      }
      case BLINE:
      {
        BLine* bl = (BLine*)current;
        InitializeWaveFrontsFromAnElement(bl);
        break;
      }
      case BARC:
      {
        BArc* ba = (BArc*)current;
        InitializeWaveFrontsFromAnElement(ba);
        break;
      }
    }
  }
}

void CEDT::PropagateWaveFronts()
{
  //keep propagating the wavefronts until all have been quenched
  while (PropagateNextActiveWaveFront());
}

int CEDT::PropagateNextActiveWaveFront()
{
  double dist;
  int dir;
  int i;

  //no wavefronts to propagate
  if (OWFList()->size()==0)
    return 0;

  //propagate the wavefront with the least time
  WaveFront* curWF = OWFList()->begin()->second;

  //first remove the current waveFront form the Active wavefront list
  OWFList()->erase(OWFList()->begin());//T_ID_pair(curWF->Dist(), curWF->ID())
  //and deactivate it
  curWF->Deactivate();

  //its current location on the grid
  Point2D<int> curLoc = curWF->Loc();

  //the boundary element causingthis wavefront
  BElement* belm = _boundary->BElmList[curWF->Label()];

  bool init_prop = ( curWF->Dir() == NORTHEAST ||
               curWF->Dir() == NORTHWEST ||
               curWF->Dir() == SOUTHWEST ||
               curWF->Dir() == SOUTHEAST );

  //Then, propagate this wavefront to neighboring grid locations according to CEDT rules
  if (init_prop){//curWF->IsInitProp()
    //CEDT rules for first time propagation of wavefront
    int num_of_updates[16] = {1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0};
    int update_dx[16][3] = {{1,0,0}, {0,0,0}, {1,0,1}, {0,0,0}, {0,0,0}, {0,0,0}, {-1,0,-1}, {0,0,0},
                    {-1,0,0}, {0,0,0}, {-1,0,-1}, {0,0,0}, {0,0,0}, {0,0,0}, {1,0,1}, {0,0,0}};
    int update_dy[16][3] = {{0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {1,0,0}, {0,0,0}, {0,1,1}, {0,0,0},
                    {0,0,0}, {0,0,0}, {0,-1,-1}, {0,0,0}, {-1,0,0}, {0,0,0}, {0,-1,-1}, {0,0,0}};

    for (i=0; i<num_of_updates[curWF->Dir()]; i++){
      Point2D<int> newLoc( curLoc.getX()+update_dx[curWF->Dir()][i],
                    curLoc.getY()+update_dy[curWF->Dir()][i]);

      //compute the distance from the source to the grid point
      dist = belm->distFromPoint(Point(newLoc.getX(), newLoc.getY()));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(belm->dirFromPoint(Point(newLoc.getX(), newLoc.getY())));
      AddAWaveFront(newLoc.getX(), newLoc.getY(), belm->id(), dist, dir, true);//false
    }
  }
  else {
    //CEDT Rules for regular propagation of fronts
    int num_of_updates[16] = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    int update_dx[16][2] = {{1,0}, {1,0}, {1,0}, {1,0}, {0,0}, {0,-1}, {-1,0}, {-1,-1},
                    {-1,0}, {-1,-1}, {-1,0}, {-1,0}, {0,0}, {0,1}, {1,0}, {1,1}};
    int update_dy[16][2] = {{0,0}, {0,1}, {1,0}, {1,1}, {1,0}, {1,1}, {1,0}, {0,1},
                    {0,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,0}, {-1,-1}, {-1,0}, {-1,0}};

    for (i=0; i<num_of_updates[curWF->Dir()]; i++){
      Point2D<int> newLoc( curLoc.getX()+update_dx[curWF->Dir()][i],
                    curLoc.getY()+update_dy[curWF->Dir()][i]);

      //compute the distance from the source to the grid point
      dist = belm->distFromPoint(Point(newLoc.getX(), newLoc.getY()));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(belm->dirFromPoint(Point(newLoc.getX(), newLoc.getY())));
      AddAWaveFront(newLoc.getX(), newLoc.getY(), belm->id(), dist, dir, false);
    }
  }

  //successful propagation of the wavefront
  return 1;
}

void CEDT::ClearAllWaveFrontsInTheGrid()
{
  for (int i=0; i<=_width; i++){
    for (int j=0; j<=_height; j++){
      _grid[i][j]->clearAllWaveFronts();
    }
  }

  //clear the WFcount variable
  _WFcount=0;
}

void CEDT::AddAWaveFront(int x, int y, int src_label, double d, int dir, bool init_prop)
{
  WaveFront* existing_WF = NULL;

  //do not initialize wavefronts outside the grid
  if (IsLocationOutsideTheGrid(x,y))
    return;

  //temp for experimentation only
  if (d==ISHOCK_DIST_HUGE)
    return;

  //first check to see if there is already a wavefront at the current grid location
  DiscreteGridPoint* curGridPt = Grid(x,y);
  if (curGridPt->numofWaveFronts())
    existing_WF = curGridPt->Fronts()->back();

  //if there is
    //1. if it has the same label we don't have to initialize a new one at that location
    //2. we need to perform a distance comparison
    //   the smaller distance wavefront gets to stay
  if (existing_WF){
    if (existing_WF->Label() == src_label)
      return;
    else {
      if (d > existing_WF->Dist()){
        //no need to place this wavefront, another one has made it
        //need to trigger a source computation

        //slight change in CEDT rules neccesitated by boundary definition
        //if distance is greater but direction is the same allow it to stay for init
        bool same_dir = ( (existing_WF->Dir()==NORTHEAST && dir==NORTHNORTHEAST) ||
                    (existing_WF->Dir()==NORTHEAST && dir==EASTNORTHEAST) ||
                    (existing_WF->Dir()==NORTHWEST && dir==NORTHNORTHWEST) ||
                    (existing_WF->Dir()==NORTHWEST && dir==WESTNORTHWEST) ||
                    (existing_WF->Dir()==SOUTHWEST && dir==WESTSOUTHWEST) ||
                    (existing_WF->Dir()==SOUTHWEST && dir==SOUTHSOUTHWEST) ||
                    (existing_WF->Dir()==SOUTHEAST && dir==SOUTHSOUTHEAST) ||
                    (existing_WF->Dir()==SOUTHEAST && dir==EASTSOUTHEAST));

        same_dir = same_dir || (existing_WF->Dir() == dir);

        if (!(init_prop && same_dir))//
          return;
      }
      else {
        //the existing WF is not valid

        //slight change in CEDT rules neccesitated by boundary definition
        //if distance is greater but direction is the same allow it to stay for init

        bool same_dir = ( (existing_WF->Dir()==NORTHEAST && dir==NORTHNORTHEAST) ||
                    (existing_WF->Dir()==NORTHEAST && dir==EASTNORTHEAST) ||
                    (existing_WF->Dir()==NORTHWEST && dir==NORTHNORTHWEST) ||
                    (existing_WF->Dir()==NORTHWEST && dir==WESTNORTHWEST) ||
                    (existing_WF->Dir()==SOUTHWEST && dir==WESTSOUTHWEST) ||
                    (existing_WF->Dir()==SOUTHWEST && dir==SOUTHSOUTHWEST) ||
                    (existing_WF->Dir()==SOUTHEAST && dir==SOUTHSOUTHEAST) ||
                    (existing_WF->Dir()==SOUTHEAST && dir==EASTSOUTHEAST));

        same_dir = same_dir || (existing_WF->Dir() == dir);

        if (!(init_prop && same_dir)){//
          //remove the earlier WF and replace it with the new one
          //also remove it from the OWFList
          OWFList()->erase(T_ID_pair(existing_WF->Dist(), existing_WF->ID()));
          curGridPt->Fronts()->pop_back();
          update_list.erase(existing_WF->ID());
          delete existing_WF;
        }
      }
    }
  }

  //put the new wavefront at the appropriate grid location
  WaveFront* newWF = new WaveFront(x, y, GetNextAvailableID(), src_label, d, dir, init_prop);
  _grid[x-_topLeft.getX()][y-_topLeft.getY()]->AddAWaveFront(newWF);

  //put the newly created wavefront into the OWFList for further propagation
  OWFList()->insert(T_ID_WF_pair(T_ID_pair(newWF->Dist(), newWF->ID()), newWF));

  //put the new wavefront into the update list
  update_list.insert(vcl_pair<int, WaveFront*>(newWF->ID(), newWF));
}

bool CEDT::IsWaveFrontOnBoundaryLimit(WaveFront* WF)
{
  if (WF->Loc().getX() == _topLeft.getX() ||
     WF->Loc().getX() == _topLeft.getX()+_width ||
     WF->Loc().getY() == _topLeft.getY() ||
     WF->Loc().getY() == _topLeft.getY()+_height)
    return true;
  else
    return false;

}

bool CEDT::IsLocationOutsideTheGrid(int x, int y)
{
  if (x < _topLeft.getX() ||
     x > _topLeft.getX()+_width ||
     y < _topLeft.getY() ||
     y > _topLeft.getY()+_height)
     return true;
  else
    return false;
}

POINT_LOCATION CEDT::LocationOfAPoint(Point2D<double> pt)
{
  //compute distances to nearest grid line
  double dx = vcl_fabs(pt.getX() - round(pt.getX()));
  double dy = vcl_fabs(pt.getY() - round(pt.getY()));

  if (dx<EI && dy<EI)
    return GRID;
  else if (dx<EI)
    return VERT_GRID;
  else if (dy<EI)
    return HORIZ_GRID;
  else
    return CELL;
}

WAVE_FRONT_DIRECTION CEDT::convertToDiscreteInitDir(double dir)
{
  //directions are only defined in 45 degree directions for the init propagation
  //if (AisEq(dir, 0*M_PI/180) || AisEq(dir, 360*M_PI/180))
  //  return EAST;
  //else if (AisEq(dir, 90*M_PI/180))
  //  return NORTH;
  //else if (AisEq(dir, 180*M_PI/180))
  //  return WEST;
  //else if (AisEq(dir, 270*M_PI/180))
  //  return SOUTH;
  //else

  if (dir>=0*M_PI/180 && dir<90*M_PI/180)
    return NORTHEAST;
  else if (dir>=90*M_PI/180 && dir<180*M_PI/180)
    return NORTHWEST;
  else if (dir>=180*M_PI/180 && dir<270*M_PI/180)
    return SOUTHWEST;
  else if (dir>=270*M_PI/180 && dir<360*M_PI/180)
    return SOUTHEAST;

  return BOGUS_DIR;
}

void CEDT::InitializeWaveFrontsFromAnElement(BPoint* bp)
{
  int i,j;
  double dist;
  int dir;

  Point2D<double> pt(bp->pt().x, bp->pt().y);

  //get the location of the source relative to the grid
  POINT_LOCATION cur_grid_relative_loc = LocationOfAPoint(pt);

  //depending on the relative grid location initialize
  //wavefronts at various surrounding grid locations
  switch (cur_grid_relative_loc){
    case CELL:
      for (i= (int)vcl_floor (pt.getX()); i<= (int)vcl_ceil (pt.getX()); i++){
        for (j= (int)vcl_floor (pt.getY()); j<= (int)vcl_ceil (pt.getY()); j++){
          //compute the distance from the source to the grid point
          dist = bp->distFromPoint(Point(i, j));
          //compute the initial discrete direction of the ray
          dir = convertToDiscreteInitDir(bp->dirFromPoint(Point(i, j)));
          AddAWaveFront(i, j, bp->id(), dist, dir, true);
        }
      }
      break;
    case HORIZ_GRID:
      for (i= (int)vcl_floor (pt.getX()); i<= (int)vcl_ceil (pt.getX()); i++){
        for (j= (int)vcl_floor (pt.getY())-1; j<= (int)vcl_floor (pt.getY())+1; j++){
          //compute the distance from the source to the grid point
          dist = bp->distFromPoint(Point(i, j));
          //compute the initial discrete direction of the ray
          dir = convertToDiscreteInitDir(bp->dirFromPoint(Point(i, j)));
          AddAWaveFront(i, j, bp->id(), dist, dir, true);
        }
      }
      break;
    case VERT_GRID:
      for (i= (int)vcl_floor (pt.getX())-1; i<= (int)vcl_floor (pt.getX())+1; i++){
        for (j= (int)vcl_floor (pt.getY()); j<= (int)vcl_ceil (pt.getY()); j++){
          //compute the distance from the source to the grid point
          dist = bp->distFromPoint(Point(i, j));
          //compute the initial discrete direction of the ray
          dir = convertToDiscreteInitDir(bp->dirFromPoint(Point(i, j)));
          AddAWaveFront(i, j, bp->id(), dist, dir, true);
        }
      }
      break;
    case GRID:
      for (i= (int)vcl_floor (pt.getX())-1; i<= (int)vcl_floor (pt.getX())+1; i++){
        for (j= (int)vcl_floor (pt.getY())-1; j<= (int)vcl_floor (pt.getY())+1; j++){
          if (!(i== vcl_floor (pt.getX()) && j== vcl_floor (pt.getY()))){
            //compute the distance from the source to the grid point
            dist = bp->distFromPoint(Point(i, j));
            //compute the initial discrete direction of the ray
            dir = convertToDiscreteInitDir(bp->dirFromPoint(Point(i, j)));
            AddAWaveFront(i, j, bp->id(), dist, dir, true);
          }
        }
      }
      break;
  }
}

void CEDT::InitializeWaveFrontsFromAnElement(BLine* bl)
{
  int i,j;
  double dist;
  int dir;

  Point2D<double> s_pt (bl->s_pt()->pt().x, bl->s_pt()->pt().y);
  Point2D<double> e_pt (bl->e_pt()->pt().x, bl->e_pt()->pt().y);

  //depending on the relative grid location initialize
  //wavefronts at various surrounding grid locations

  double sx, ex, sy, ey;
  sx =  vnl_math_min( s_pt.getX(), e_pt.getX());
  ex =  vnl_math_max( s_pt.getX(), e_pt.getX());

  sy =  vnl_math_min( s_pt.getY(), e_pt.getY());
  ey =  vnl_math_max( s_pt.getY(), e_pt.getY());

  double cos_theta = vcl_cos(bl->U());
  double sin_theta = vcl_sin(bl->U());

  if (cos_theta!=0){
    for (i= (int)vcl_floor (sx); i<= (int)vcl_ceil (ex); i++){
      double k = (i-s_pt.getX())/cos_theta;
      //now compute y corrdinate at this x grid line
      double y = k*sin_theta + s_pt.getY();

      //now init above and below the current y

      //compute the distance from the source to the grid point
      dist = bl->distFromPoint(Point(i,  vcl_floor (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(bl->dirFromPoint(Point(i,  vcl_floor (y))));
      AddAWaveFront(i, (int)vcl_floor (y), bl->id(), dist, dir, true);

      dist = bl->distFromPoint(Point(i,  vcl_ceil (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(bl->dirFromPoint(Point(i,  vcl_ceil (y))));
      AddAWaveFront(i, (int)vcl_ceil (y), bl->id(), dist, dir, true);

    }
  }

  if (sin_theta!=0){
    for (j= (int)vcl_floor (sy); j<= (int)vcl_ceil (ey); j++){
      double k = (j-s_pt.getY())/sin_theta;
      //now compute y corrdinate at this x grid line
      double x = k*cos_theta + s_pt.getX();

      //now init left and right the current x

      //compute the distance from the source to the grid point
      dist = bl->distFromPoint(Point( vcl_floor (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(bl->dirFromPoint(Point( vcl_floor (x), j)));
      AddAWaveFront( (int)vcl_floor (x), j, bl->id(), dist, dir, true);

      dist = bl->distFromPoint(Point( vcl_ceil (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(bl->dirFromPoint(Point( vcl_ceil (x), j)));
      AddAWaveFront( (int)vcl_ceil (x), j, bl->id(), dist, dir, true);

    }
  }

}

void CEDT::InitializeWaveFrontsFromAnElement(BArc* ba)
{
  int i,j;
  double dist;
  int dir;

  double R = ba->R();
  Point C = ba->center();
  double s_angle = ba->_CCWStartVector;
  double e_angle = ba->_CCWEndVector;

  //find the extent of the arc in x and y directions
  double sx = ba->center().x - R;
  double ex = ba->center().x + R;
  double sy = ba->center().y - R;
  double ey = ba->center().y + R;

  for (i= (int)vcl_floor (sx); i<= (int)vcl_ceil (ex); i++){
    double ratio = (i-C.x)/R;
    if (vcl_fabs(ratio)>1) continue;

    //two possible solutions
    double theta1 = angle02Pi(vcl_acos(ratio));
    double theta2 = angle02Pi(-vcl_acos(ratio));

    if (_validStartEnd0To2Pi(theta1, s_angle, e_angle)){
      //now compute y corrdinate at this x grid line
      double y = R*vcl_sin(theta1) + C.y;

      //now init above and below the current y

      //compute the distance from the source to the grid point
      dist = ba->distFromPoint(Point(i,  vcl_floor (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point(i,  vcl_floor (y))));
      AddAWaveFront(i,  (int)vcl_floor (y), ba->id(), dist, dir, true);

      dist = ba->distFromPoint(Point(i,  vcl_ceil (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point(i,  vcl_ceil (y))));
      AddAWaveFront(i,  (int)vcl_ceil (y), ba->id(), dist, dir, true);
    }

    if (_validStartEnd0To2Pi(theta2, s_angle, e_angle)){
      //now compute y corrdinate at this x grid line
      double y = R*vcl_sin(theta2) + C.y;

      //now init above and below the current y

      //compute the distance from the source to the grid point
      dist = ba->distFromPoint(Point(i,  vcl_floor (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point(i,  vcl_floor (y))));
      AddAWaveFront(i, (int)vcl_floor (y), ba->id(), dist, dir, true);

      dist = ba->distFromPoint(Point(i,  vcl_ceil (y)));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point(i,  vcl_ceil (y))));
      AddAWaveFront(i, (int)vcl_ceil (y), ba->id(), dist, dir, true);
    }
  }

  for (j= (int)vcl_floor (sy); j<= (int)vcl_ceil (ey); j++){
    double ratio = (j-C.y)/R;
    if (vcl_fabs(ratio)>1) continue;

    //two possible solutions
    double theta1 = angle02Pi(vcl_asin(ratio));
    double theta2 = angle02Pi(M_PI-vcl_asin(ratio));

    if (_validStartEnd0To2Pi(theta1, s_angle, e_angle)){
      //now compute y corrdinate at this x grid line
      double x = R*vcl_cos(theta1) + C.x;

      //now init left and right the current x

      //compute the distance from the source to the grid point
      dist = ba->distFromPoint(Point( vcl_floor (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point( vcl_floor (x), j)));
      AddAWaveFront( (int)vcl_floor (x), j, ba->id(), dist, dir, true);

      dist = ba->distFromPoint(Point( vcl_ceil (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point( vcl_ceil (x), j)));
      AddAWaveFront( (int)vcl_ceil (x), j, ba->id(), dist, dir, true);
    }

    if (_validStartEnd0To2Pi(theta2, s_angle, e_angle)){
      //now compute y corrdinate at this x grid line
      double x = R*vcl_cos(theta2) + C.x;

      //now init left and right the current x

      //compute the distance from the source to the grid point
      dist = ba->distFromPoint(Point( vcl_floor (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point( vcl_floor (x), j)));
      AddAWaveFront( (int)vcl_floor (x), j, ba->id(), dist, dir, true);

      dist = ba->distFromPoint(Point( vcl_ceil (x), j));
      //compute the initial discrete direction of the ray
      dir = convertToDiscreteDir(ba->dirFromPoint(Point( vcl_ceil (x), j)));
      AddAWaveFront( (int)vcl_ceil (x), j, ba->id(), dist, dir, true);
    }


  }

}

WAVE_FRONT_DIRECTION CEDT::convertToDiscreteDir(double dir)
{
  //directions are only defined in 22.5 degree directions for regular propagation
  //if (AisEq(dir, 0*M_PI/180) || AisEq(dir, 360*M_PI/180))
  //  return EAST;
  //else if (AisEq(dir, 45*M_PI/180))
  //  return NORTHEAST;
  //else if (AisEq(dir, 90*M_PI/180))
  //  return NORTH;
  //else if (AisEq(dir, 135*M_PI/180))
  //  return NORTHWEST;
  //else if (AisEq(dir, 180*M_PI/180))
  //  return WEST;
  //else if (AisEq(dir, 225*M_PI/180))
  //  return SOUTHWEST;
  //else if (AisEq(dir, 270*M_PI/180))
  //  return SOUTH;
  //else if (AisEq(dir, 315*M_PI/180))
  //  return SOUTHEAST;
  //else

  if (dir>=0*M_PI/180 && dir<45*M_PI/180)
    return EASTNORTHEAST;
  else if (dir>=45*M_PI/180 && dir<90*M_PI/180)
    return NORTHNORTHEAST;
  else if (dir>=90*M_PI/180 && dir<135*M_PI/180)
    return NORTHNORTHWEST;
  else if (dir>=135*M_PI/180 && dir<180*M_PI/180)
    return WESTNORTHWEST;
  else if (dir>=180*M_PI/180 && dir<225*M_PI/180)
    return WESTSOUTHWEST;
  else if (dir>=225*M_PI/180 && dir<270*M_PI/180)
    return SOUTHSOUTHWEST;
  else if (dir>=270*M_PI/180 && dir<315*M_PI/180)
    return SOUTHSOUTHEAST;
  else if (dir>=315*M_PI/180 && dir<360*M_PI/180)
    return EASTSOUTHEAST;

  return BOGUS_DIR;
}

// Discrete Grid Point
//default constructor
DiscreteGridPoint::DiscreteGridPoint()
{
  //set the coordinates of the grid point
  _loc.set(-1,-1);
  _fronts.clear();
}

DiscreteGridPoint::DiscreteGridPoint(int x, int y)
{
  //set the coordinates of the grid point
  _loc.set(x,y);
  _fronts.clear();
}

DiscreteGridPoint::~DiscreteGridPoint()
{
  //clear all the wavefronts
  clearAllWaveFronts();
}

//methods
WaveFront* DiscreteGridPoint::AddAWaveFront(int id, int src_label, double d, int dir, bool init_prop)
{
  WaveFront* newWF = new WaveFront(_loc.getX(), _loc.getY(), id, src_label, d, dir, init_prop);
  _fronts.push_back(newWF);

  return newWF;
}

void DiscreteGridPoint::AddAWaveFront(WaveFront* newWF)
{
  _fronts.push_back(newWF);
}

void DiscreteGridPoint::clearAllWaveFronts()
{
  //delete all the waveFronts residing at this grid point
  while (_fronts.size()>0){
    delete _fronts.back();
    _fronts.pop_back();
  }
}

//Discrete Wavefronts
WaveFront::WaveFront(int x, int y, int id, int src_label, double d, int dir, bool init_prop)
{
  _id = id;
  //set the coordinates
  _loc.set(x,y);
  _label = src_label;
  _d = d;
  _dir = WAVE_FRONT_DIRECTION(dir);
  _init_prop = init_prop;
  _active = true;
}

WaveFront::~WaveFront(){}

void WaveFront::getInfo (vcl_ostream& ostrm)
{
  /*
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.sprintf ("WaveFront\n \n"); buf+=s;
   s.sprintf ("ID: %d\n", _id); buf+=s;
   s.sprintf ("Position: (%d, %d)\n", _loc.getX(), _loc.getY()); buf+=s;
  s.sprintf ("Boundary Label: %d\n", _label); buf+=s;
  s.sprintf ("Dist: %.4f\n", _d); buf+=s;
   s.sprintf ("Active: %s\n", _active ? "yes" : "no"); buf+=s;
  s.sprintf ("Initial WF: %s\n", _init_prop ? "yes" : "no"); buf+=s;
   s.sprintf ("DIR: "); buf+=s;

  switch (_dir){
  case EAST:        s.sprintf ("EAST = (0)"); break;
  case EASTNORTHEAST:  s.sprintf ("EASTNORTHEAST = (1)"); break;
  case NORTHEAST:    s.sprintf ("NORTHEAST = (2)"); break;
  case NORTHNORTHEAST: s.sprintf ("NORTHNORTHEAST = (3)"); break;
  case NORTH:        s.sprintf ("NORTH = (4)"); break;
  case NORTHNORTHWEST: s.sprintf ("NORTHNORTHWEST = (5)"); break;
  case NORTHWEST:    s.sprintf ("NORTHWEST = (6)"); break;
  case WESTNORTHWEST:  s.sprintf ("WESTNORTHWEST = (7)"); break;
  case WEST:        s.sprintf ("WEST = (8)"); break;
  case WESTSOUTHWEST:  s.sprintf ("WESTSOUTHWEST = (9)"); break;
  case SOUTHWEST:    s.sprintf ("SOUTHWEST = (10)"); break;
  case SOUTHSOUTHWEST: s.sprintf ("SOUTHSOUTHWEST = (11)"); break;
  case SOUTH:        s.sprintf ("SOUTH = (12)"); break;
  case SOUTHSOUTHEAST: s.sprintf ("SOUTHSOUTHEAST = (13)"); break;
  case SOUTHEAST:    s.sprintf ("SOUTHEAST = (14)"); break;
  case EASTSOUTHEAST:  s.sprintf ("EASTSOUTHEAST = (15)"); break;
  }
  buf+=s;

  ostrm << buf;*/
}
