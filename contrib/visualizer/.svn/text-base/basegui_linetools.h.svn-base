#ifndef LINETOOLS_H_INCLUDED
#define LINETOOLS_H_INCLUDED

#include "basegui_gtool.h"
#include "basegui_BaseWindow.h"

template <class T>
class LineTool: public DrawingTool 
{
public:
  LineTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), g(NULL), state(NONE) {}

protected:
  enum { NONE, POINT1, POINT2 } state;
  T *g;

  virtual Result complete()
  {

     //GfxCommand *c = new AddGeomCommand(g, permanent_group);
     //g = NULL;
     _window->glw()->Refresh();

     state = NONE;
     return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt) 
  {
    if(state == NONE) {
     g = new T
      (view->unproject(Point2D<double>(evt.GetX(), evt.GetY())),
       view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
     tmp()->addChild(g);
     state = POINT1;

     _window->glw()->Refresh();

     return HANDLED;
    } 
    else {
     return UNHANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt) 
  {
    switch(state) {
    case NONE:
     return UNHANDLED;
    
    case POINT1:
     g->setStart(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
     if(g->start()==g->end()){
      // Mouse clicked without dragging.
      state = POINT2;
      return HANDLED;
     }else{
      // Mouse dragged. Draw a line now.
      // Swap the points.
      Point2D<double> tmp = g->start();
      g->setStart(g->end());
      g->setEnd(tmp);
      return complete();
     }
    
    case POINT2:
     g->setEnd(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
     return complete();
    }
  }

  Result mouseMove(wxMouseEvent &evt) 
  {
    switch(state) {
    case NONE:
     return UNHANDLED;
    
    case POINT1:
     g->setStart(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
     _window->glw()->Refresh();
     return HANDLED;
    
    case POINT2:
     g->setEnd(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
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
     tmp()->removeChild(g);
     delete g;
    
     state = NONE;
     return ABORTED;
    }
  }

};

//////////////////////////////////
//POLY LINE TOOL

template <class T>
class PolylineTool: public DrawingTool 
{
public:
  PolylineTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), g(0), state(NONE) {}

protected:
  enum { NONE, RUBBER_BANDING } state;
  T *g; //PolylineGeom type

  virtual Result complete()
  {
    state = NONE;
    //GfxCommand *c = new AddGeomCommand(g, permanent_group);
    _window->glw()->Refresh();
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        g = new T();
        tmp()->addChild(g);
        state = RUBBER_BANDING;
        g->push_back(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
      default:
        return UNHANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(g->size()-1, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        g->push_back(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
          return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(g->size()-1, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result rightMouseDown(wxMouseEvent &evt)
  {
    if(state == NONE) 
      return UNHANDLED;
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        
        return complete();
    }
  }

  Result rightMouseUp(wxMouseEvent &evt){ return HANDLED; }

  Result abort()
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        tmp()->removeChild(g);
        delete g;

        state = NONE;
        return ABORTED;
    }
  }

};

//////////////////////////////////
//POLYGON TOOL

template <class T>
class PolygonTool: public DrawingTool 
{
public:
  PolygonTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), g(NULL), state(NONE) {}

protected:
  enum { NONE, RUBBER_BANDING } state;
  T *g; //UnfilledPolygonGeom type
  
  virtual Result complete()
  {
    state = NONE;
    //GfxCommand *c = new AddGeomCommand(g, permanent_group);
    _window->glw()->Refresh();
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        g = new T();
        tmp()->addChild(g);
        state = RUBBER_BANDING;
        g->push_back(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
      default:
        return UNHANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(g->size()-1, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        g->push_back(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
          return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(g->size()-1, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        _window->glw()->Refresh();
        return HANDLED;
    }
  }

  Result rightMouseDown(wxMouseEvent &evt)
  {
    if(state == NONE) 
      return UNHANDLED;
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(g->size()-1, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        return complete();
    }
  }

  Result rightMouseUp(wxMouseEvent &evt){ return HANDLED; }

  Result abort()
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        tmp()->removeChild(g);
        delete g;

        state = NONE;
        return ABORTED;
    }
  }

};

//////////////////////////////////
//RECTANGLE TOOL

template <class T>
class RectangleTool: public DrawingTool 
{
public:
  RectangleTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), g(NULL), state(NONE) {}

protected:
  enum { NONE, RUBBER_BANDING } state;
  T *g; //UnfilledPolygonGeom type
  Point2D<double> point1;
  
  virtual Result complete()
  {
    state = NONE;
    //GfxCommand *c = new AddGeomCommand(g, permanent_group);
    _window->glw()->Refresh();
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE:
        g = new T();
        tmp()->addChild(g);
        state = RUBBER_BANDING;
        point1 = Point2D<double>(evt.GetX(), evt.GetY());
        //top left & top right
        g->push_back(view->unproject(point1));
        g->push_back(view->unproject(point1));
        //bottom right & bottom left
        g->push_back(view->unproject(point1));
        g->push_back(view->unproject(point1));

        //original point to close the loop
        g->push_back(view->unproject(point1));
        _window->glw()->Refresh();
        return HANDLED;
      default:
        return UNHANDLED;
    }
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
        return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(1, view->unproject(Point2D<double>(evt.GetX(), point1.getY())));
        g->setPoint(2, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        g->setPoint(3, view->unproject(Point2D<double>(point1.getX(), evt.GetY())));
        return complete();
    }
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    switch(state) {
      case NONE: 
          return UNHANDLED;
      case RUBBER_BANDING:
        g->setPoint(1, view->unproject(Point2D<double>(evt.GetX(), point1.getY())));
        g->setPoint(2, view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
        g->setPoint(3, view->unproject(Point2D<double>(point1.getX(), evt.GetY())));
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
        tmp()->removeChild(g);
        delete g;

        state = NONE;
        return ABORTED;
    }
  }

};

//////////////////////////////////
//POLYLINE FREEHAND TOOL

template <class T>
class PolylineFreehandTool: public DrawingTool 
{
public:
  PolylineFreehandTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), state(NONE), g(NULL)
  {
    threshold=3;
    //if(GOptions["Base"].count("PolylineThreshold")>0){
    //  threshold = atoi(GOptions["Base"]["PolylineThreshold"].c_str());
    //}
  }

protected:
  enum { NONE, DRAWING } state;
  T *g; //PolylineGeom
  int threshold;
  Point2D<double> prevPt;

  virtual Result complete()
  {
    //AddGeomCommand *c = new AddGeomCommand(g, permanent_group);
    //tmp()->removeChild(g);
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    if(state == NONE) {
      g = new T;
      tmp()->addChild(g);

      Point2D<double> pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
      prevPt=pt;
      g->push_back(pt);

      _window->glw()->Refresh();
      state = DRAWING;
      return HANDLED;
    }
    return UNHANDLED;
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    // don't add a point here, because of X event queueing wierdness.
    // often, there are some mouse moves which are sent before the mouseup,
    // but which correspond to moves happening _after_ the mouse up (because
    // of differing latencies of the event queues). this can cause our path
    // to double back on itself, so just ignore the mouse up.
    state = NONE;

    return complete();
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    Point2D<double> pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
    if(threshold==0 || 
      sqrt(pow((pt.getX()-prevPt.getX()),2)+pow((pt.getY()-prevPt.getY()),2))
      >=(double)threshold)
    {
      g->push_back(pt);
      _window->glw()->Refresh();
      prevPt=pt;
    }
    return HANDLED;
  }


  Result abort()
  {
    if(state == NONE) return UNHANDLED;

    tmp()->removeChild(g);
    delete g;

    state = NONE;
    return ABORTED;
  }

};

//////////////////////////////////
//POLYGON FRREHAND TOOL

template <class T>
class PolygonFreehandTool: public DrawingTool 
{
public:
  PolygonFreehandTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), state(NONE), g(NULL)
  {
    threshold=3;
    //if(GOptions["Base"].count("PolylineThreshold")>0){
    //  threshold = atoi(GOptions["Base"]["PolylineThreshold"].c_str());
    //}
  }

protected:
  enum { NONE, DRAWING } state;
  T *g; //UnfilledPolygonGeom type
  int threshold;
  Point2D<double> prevPt;

  virtual Result complete()
  {
    //AddGeomCommand *c = new AddGeomCommand(g, permanent_group);
    //tmp()->removeChild(g);

    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    if(state == NONE) {
      g = new T;
      tmp()->addChild(g);

      Point2D<double> pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
      prevPt=pt;
      g->push_back(pt);

      _window->glw()->Refresh();
      state = DRAWING;
      return HANDLED;
    }
    return UNHANDLED;
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    // don't add a point here, because of X event queueing wierdness.
    // often, there are some mouse moves which are sent before the mouseup,
    // but which correspond to moves happening _after_ the mouse up (because
    // of differing latencies of the event queues). this can cause our path
    // to double back on itself, so just ignore the mouse up.
    state = NONE;

    return complete();
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    Point2D<double> pt = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));
    if(threshold==0 || 
      sqrt(pow((pt.getX()-prevPt.getX()),2)+pow((pt.getY()-prevPt.getY()),2))
      >=(double)threshold)
    {
      g->push_back(pt);
      _window->glw()->Refresh();
      prevPt=pt;
    }
    return HANDLED;
  }


  Result abort()
  {
    if(state == NONE) return UNHANDLED;

    tmp()->removeChild(g);
    delete g;

    state = NONE;
    return ABORTED;
  }

};

#endif
