#ifndef ARC_TOOLS_H_INCLUDED
#define ARC_TOOLS_H_INCLUDED

#include <basegui_gtool.h>
#include <algo_arc.h>

#include <basegui_basewindow.h>

//////////////////////////////////
//ARC TOOLS

template <class T>
class ArcTool: public DrawingTool 
{
public:
  ArcTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), screenLine(NULL), screenPt1(NULL),
    screenPt2(NULL),screenPt3(NULL), state(NONE) {}

protected:
  enum { NONE, POINT1, POINT2, POINT3 } state;
  Point2D<double> point1, point2, point3;
  LineGeom *screenLine;
  PointGeom *screenPt1, *screenPt2, *screenPt3;
  T* arc; //ArcGeom

  virtual Result complete()
  {
    _window->glw()->Refresh();
    //GfxCommand *c = new AddGeomCommand(ellipse, permanent_group);
    state = NONE;
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt){
    switch(state) {
      case NONE:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenLine = new LineGeom(point1, point1);
        screenPt1 = new PointGeom(point1);
        tmp()->addChild(screenLine);
        tmp()->addChild(screenPt1);
        state = POINT1;
        _window->glw()->Refresh();
        return HANDLED;
      default:
        return HANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt){
    switch(state) {
      case NONE:
        return UNHANDLED;
      case POINT1:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenLine->setStart(point1);
        screenLine->setEnd(point1);
        screenPt1->setPoint(point1);

        screenPt2 = new PointGeom(point1); // create new onscreen point for point 2
        tmp()->addChild(screenPt2);

        _window->glw()->Refresh();
        state = POINT2;
        return HANDLED;
      case POINT2:
        point2 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenPt2->setPoint(point2);

        tmp()->removeChild(screenLine);
        delete screenLine;

        arc = new T(0,0,0,0,0,0);
        tmp()->addChild(arc);

        screenPt3 = new PointGeom(point2);
        tmp()->addChild(screenPt3);

        _window->glw()->Refresh();
        state = POINT3;
        return HANDLED;
      case POINT3:
        point3 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        solve();

        _window->glw()->Refresh();
        tmp()->removeChild(screenPt1);
        delete screenPt1;
        tmp()->removeChild(screenPt2);
        delete screenPt2;
        tmp()->removeChild(screenPt3);
        delete screenPt3;

        return complete();
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        return UNHANDLED;

      case POINT1:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenPt1->setPoint(point1);
        screenLine->setStart(point1);
        screenLine->setEnd(point1);
        _window->glw()->Refresh();
        return HANDLED;

      case POINT2:
        point2 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenPt2->setPoint(point2);
        screenLine->setEnd(point2);
        _window->glw()->Refresh();
        return HANDLED;

      case POINT3:
        point3 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        screenPt3->setPoint(point3);
        solve();
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result abort()
  {
    switch(state) {
      case NONE:
        return UNHANDLED;

      default:
        //tmp()->eraseAllChildren();
        state = NONE;
        return ABORTED;
    }
  }

  void solve()
  {
    Point2D<double> center;
    double radius;
    ARC_NUD nud;
    threePointsToArc(point1, point2, point3, center, radius, nud);
    arc->setParameters (point1.getX(), point1.getY(), point3.getX(), point3.getY(),
      center.getX(), center.getY(), radius, nud);
    _window->glw()->Refresh();
  }

};

//////////////////////////////////
//CIRCLE TOOL

template <class T>
class CircleTool: public DrawingTool 
{
public:
  CircleTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), arc(NULL),state(NONE) {}

protected:
  enum { NONE, RUBBER_BANDING } state;
  Point2D<double> point1;
  std::vector<T *> arcs; //ArcGeom
  T *arc;
  int N; //number of arc pieces that will make up the circle
  
  virtual Result complete()
  {
    state = NONE;
    //GfxCommand *c = new AddGeomCommand(g, permanent_group);
    arcs.clear();

    _window->glw()->Refresh();
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    int i;
    double arc_segment;

    switch(state) {
      case NONE:
        N = 16; //16 pieces for now
        point1 = Point2D<double>(evt.GetX(), evt.GetY());
        state = RUBBER_BANDING;

        arcs.clear();
        //now add the null arcgeoms to make up the circle
        //USING N segments here
        arc_segment = 2*M_PI/N;
        for (i=0; i<N; i++){
          arc = new T(0,0,0,0,0,0);
          tmp()->addChild(arc);
          arcs.push_back(arc);
        }

        _window->glw()->Refresh();
        return HANDLED;
      default:
        return UNHANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    int i;
    double arc_segment;

    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        //get the current radius
        Point2D<double> edge_pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        Point2D<double> cen = view->unproject(Point2D<double>(point1.getX(), point1.getY()));
        double R = euc_distance(cen, edge_pt);

        Point2D<double> lastpt = cen + Point2D<double>(R, 0);
        //update the arcs based on the current radius
        //USING 16 segments here
        arc_segment = 2*M_PI/N;
        for (i=1; i<=N; i++){
          Point2D<double> pt = cen + Point2D<double>(R*cos(arc_segment*i), R*sin(arc_segment*i));
          arcs[i-1]->setParameters (lastpt.getX(), lastpt.getY(), pt.getX(), pt.getY(),
            cen.getX(), cen.getY(), R, ARC_NUD_CCW);
          lastpt = pt;
        }

        return complete();
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
          return UNHANDLED;
      case RUBBER_BANDING:
        //get the current radius
        Point2D<double> edge_pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        Point2D<double> cen = view->unproject(Point2D<double>(point1.getX(), point1.getY()));
        double R = euc_distance(cen, edge_pt);

        Point2D<double> lastpt = cen + Point2D<double>(R, 0);
        //update the arcs based on the current radius
        //USING 16 segments here
        double arc_segment = 2*M_PI/N;
        for (int i=1; i<=N; i++){
          Point2D<double> pt = cen + Point2D<double>(R*cos(arc_segment*i), R*sin(arc_segment*i));
          arcs[i-1]->setParameters (lastpt.getX(), lastpt.getY(), pt.getX(), pt.getY(),
            cen.getX(), cen.getY(), R, ARC_NUD_CCW);
          lastpt = pt;
        }

        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result rightMouseDown(wxMouseEvent &evt)  { return HANDLED; }
  Result rightMouseUp(wxMouseEvent &evt)    { return HANDLED; }

  Result abort()
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        arcs.clear();
        state = NONE;
        return ABORTED;
    }
  }

};

//////////////////////////////////
//ARC SPLINE TOOL

template <class T>
class ArcSplineTool: public DrawingTool 
{
public:
  ArcSplineTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), line(NULL), arc(NULL), state(NONE) {}

protected:
  enum { NONE, POINT1, POINT2, POINT3, POINTN } state;
  Point2D<double> point1, point2, point3, pointnm1, pointn;
  double thetanm1;
  LineGeom *line;
  std::vector<T *> arcs; //ArcGeom
  T *arc;

  virtual Result complete()
  {
    //GfxCommand *c = new AddGeomCommand(ellipses_group, permanent_group);
    arcs.clear();

    state = NONE;
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        line = new LineGeom(point1, point1);
        tmp()->addChild(line);

        state = POINT1;
        _window->glw()->Refresh();
        return HANDLED;

      default:
        return HANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        return UNHANDLED;

      case POINT1:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        line->setStart(point1);
        line->setEnd(point1);

        state = POINT2;
        _window->glw()->Refresh();
        return HANDLED;

      case POINT2:
        point2 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
      
        tmp()->removeChild(line);
        delete line;

        arc = new T(0,0,0,0,0,0);
        tmp()->addChild(arc);
        arcs.push_back(arc);

        state = POINT3;
        _window->glw()->Refresh();
        return HANDLED;

      case POINT3:
        point3 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        solve();

        arc = new T(0,0,0,0,0,0);
        tmp()->addChild(arc);
        arcs.push_back(arc);

        pointnm1 = point3;

        state = POINTN;
        _window->glw()->Refresh();
        return HANDLED;

      case POINTN:
        pointn = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        thetanm1 = solve_tangent();

        arc = new T(0,0,0,0,0,0);
        tmp()->addChild(arc);
        arcs.push_back(arc);

        pointnm1 = pointn;
        state = POINTN;
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        return UNHANDLED;

      case POINT1:
        point1 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        line->setStart(point1);
        line->setEnd(point1);
        _window->glw()->Refresh();
        return HANDLED;

      case POINT2:
        point2 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        line->setEnd(point2);
        _window->glw()->Refresh();
        return HANDLED;

      case POINT3:
        point3 = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        solve();
        _window->glw()->Refresh();
        return HANDLED;

      case POINTN:
        pointn = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        solve_tangent();
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result rightMouseDown(wxMouseEvent &evt){ return UNHANDLED; }

  Result rightMouseUp(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        return UNHANDLED;
      default:
        pointn = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
        solve_tangent();
        _window->glw()->Refresh();

        return complete();
    }
  }

  Result abort()
  {
    switch(state) {
    case NONE: 
      return UNHANDLED;
    default:

      arcs.clear();

      state = NONE;
      return ABORTED;
    }
  }

  void solve()
  {
    Point2D<double> center;
    double radius;
    ARC_NUD nud;
    
    threePointsToArc(point1, point2, point3, center, radius, nud);
    arc->setParameters (point1.getX(), point1.getY(), point3.getX(), point3.getY(),
      center.getX(), center.getY(), radius, nud);

    thetanm1 = getTangentOfArc(center, point1, point3, nud);
  }

  double solve_tangent()
  {
    Point2D<double> center;
    double radius;
    ARC_NUD nud;
    
    pointTangentPointToArc(pointnm1, thetanm1, pointn, center, radius, nud);

    arc->setParameters (pointnm1.getX(), pointnm1.getY(), pointn.getX(), pointn.getY(),
      center.getX(), center.getY(), radius, nud);

    return getTangentOfArc(center, pointnm1, pointn, nud);
  }

};

#endif
