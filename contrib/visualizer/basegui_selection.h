#ifndef SELECTION_H_INCLUDED
#define SELECTION_H_INCLUDED

#include "basegui_tools.h"
#include "basegui_view.h"
#include "basegui_style.h"
#include "basegui_scenegraph.h"
#include "basegui_geomnodes.h"
#include "command.h"
#include <math.h>

template <class polygon_type>
class RectangleGeom : public polygon_type {
 protected:
  Point2D<double> _originPos;
  double _width;
  double _height;
 private:
  bool _flag(int data, int bit){
    return (data & ((int)1<<bit));
  }
 public:
  enum OriginType {UP_LEFT=0,UP_RIGHT,DOWN_LEFT,DOWN_RIGHT,CENTER};
  OriginType originType;

  RectangleGeom
    (const Point2D<double>& originPos_, double width_, double height_,
     OriginType originType_)
    : polygon_type(originPos_), _originPos(originPos_), 
    _width(width_), _height(height_), originType(originType_)
    {
      calcCoords();
    }

  void calcCoords(){
    if(_points.size()>0)
      _points.clear();
    
    if(originType==CENTER){
      push_back(_originPos.getX()-(_width/2), _originPos.getY()-(_height/2));
      push_back(_originPos.getX()+(_width/2), _originPos.getY()-(_height/2));
      push_back(_originPos.getX()+(_width/2), _originPos.getY()+(_height/2));
      push_back(_originPos.getX()-(_width/2), _originPos.getY()+(_height/2));
      push_back(_originPos.getX()-(_width/2), _originPos.getY()-(_height/2));
    }else if(originType==UP_LEFT){
      push_back(_originPos.getX(), _originPos.getY());
      push_back(_originPos.getX()+_width, _originPos.getY());
      push_back(_originPos.getX()+_width, _originPos.getY()+_height);
      push_back(_originPos.getX(), _originPos.getY()+_height);
      push_back(_originPos.getX(), _originPos.getY());
    }
  }

};

class HandleGeom : public RectangleGeom<FilledPolygonGeom> {
 private:
  bool _selected;

 public:
  HandleGeom(const Point2D<double>& origin_);

  const Point2D<double>& origin();
  void origin(const Point2D<double>& origin_);
  bool selected();
  void selected(bool selected_);
};

class Selection{
 protected:
  DrawingTool* _tool;
  GraphicsNode *_selectedGeom;
  Group *_handleGroup;  
 public:
  Selection(DrawingTool* tool_,GraphicsNode *selectedGeom_, 
      Group* handleGroup_);
  ~Selection(){};

  GraphicsNode* selectedGeom(){return _selectedGeom;}
  void selectedGeom(GraphicsNode*rh){_selectedGeom=rh;}
};

class GroupSelection : public Selection {
 private:
  enum {ROTATION_HANDLE_OFFSET=3};
  double _corners[4];
  double _cornersBR[4];
  XForm _prevXF;
  XForm _origXF;
  Point2D<double> _origHandleOrigs[2];

  RectangleGeom<UnfilledPolygonGeom> *_BR;
  enum{HANDLE_SCALE=0,HANDLE_ROTATION};
  HandleGeom* _handles[2];

  double _mag;
  double _angle;
  Point2D<double> _translation;

  void _deleteGeom(GraphicsNode* geom_);
  Point2D<double> _xfPt(const XForm& xf_, const Point2D<double>& pt_);
  Point2D<double> _invPt(const XForm& xf_, const Point2D<double>& pt_);

 public:
  GroupSelection(DrawingTool* tool_,
     GraphicsNode *selectedGeom_, Group* handleGroup_);
  ~GroupSelection();
  
  void _initHandleFromGeom();
  void moveHandles
    (const Point2D<double>& offset_,const Point2D<double>& mousePos_,
     HandleGeom*selectedHandle_);
  void moveGeom(const Point2D<double>& offset_);
  void moveScaleHandle(const Point2D<double>& mousePos_);
  void moveRotationHandle(const Point2D<double>& mousePos_);
  void refreshXForm();
};

class DirectSelection : public Selection{
 private:
 public:
  DirectSelection(DrawingTool* tool_,
      GraphicsNode *selectedGeom_, Group* handleGroup_);
  ~DirectSelection();
  
  vcl_vector<HandleGeom*> handles;
  void setFlag(bool flag,const HandleGeom* hg_=NULL);
  bool selected();
  void moveHandles(const Point2D<double>& offset_);
  void _initHandleFromGeom();
  void propagateHandleToGeom();
};

class SelectionTool : public DrawingTool {
 protected:
  enum { NONE, GEOM_SELECTED, HANDLE_DRAGGED } state;
  const wxMouseEvent* _evt;
  Point2D<double> _prevMousePos;
  Group *_handleGroup;
  Selection *_selection;

  virtual void _initSelectionObj(GraphicsNode*);
 public:
  SelectionTool(BaseWindow *win, Group *pgroup);

  Result _selectGeom(const Point2D<double>& coord_);
  Result leftMouseUp(const wxMouseEvent *evt);
  Result rightMouseDown(const wxMouseEvent *evt);
  Result middleMouseDown(const wxMouseEvent *evt);
  Result middleMouseUp(const wxMouseEvent *evt);
  Result abort();
};

class GroupSelectionTool : public SelectionTool {
 private:
  HandleGeom *_selectedHandle;
  GroupSelection *selection(){return (GroupSelection*)_selection;}

  virtual void _initSelectionObj(GraphicsNode*);
  Result _selectHandles(const Point2D<double>& coord_);

 public:
  GroupSelectionTool(BaseWindow *win, Group *pgroup);

  virtual Result keyDown(wxKeyEvent &evt);
  Result leftMouseDown(const wxMouseEvent *evt);
  Result mouseMove(const wxMouseEvent *evt);
  Result rightMouseUp(const wxMouseEvent *evt);
};

class DirectSelectionTool : public SelectionTool {
private:
  DirectSelection *selection(){return (DirectSelection*)_selection;}

  virtual void _initSelectionObj(GraphicsNode*);
  Result _selectHandles(const Point2D<double>& coord_);

public:
  DirectSelectionTool(BaseWindow *win, Group *pgroup);

  Result keyDown( wxKeyEvent *evt);
  Result leftMouseDown(const wxMouseEvent *evt);
  Result mouseMove(const wxMouseEvent *evt);
  Result rightMouseUp(const wxMouseEvent *evt);
};

#endif
