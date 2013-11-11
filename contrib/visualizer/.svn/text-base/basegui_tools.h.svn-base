#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include "basegui_gtool.h"
#include "basegui_pointtools.h"
#include "basegui_linetools.h"
#include "algo_arctools.h"

#include <wx/wx.h>
#include <wx/event.h>

class PanTool: public GTool {
public:
  PanTool(BaseWindow* win);
private:
  enum { NONE, PAN, ROTATE, ZOOM } state;
  int old_x, old_y, mousedown_y, center_x, center_y;

  Result leftMouseDown(wxMouseEvent &evt);
  Result leftMouseUp(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);
  Result rightMouseDown(wxMouseEvent &evt);
  Result rightMouseUp(wxMouseEvent &evt);

  Result abort();
};

class GetInfoTool: public GTool {
public:
  GetInfoTool(BaseWindow* win);
private:

  Result leftMouseDown(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);

  Result abort();
};

//Delete Tool
class DeleteTool: public GTool {
public:
  DeleteTool(BaseWindow* win);
  //cheating::need to improve defn so that this can be private as it should be
  enum { NONE, DELETING } state;

private:

  Result leftMouseDown(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);
  Result leftMouseUp(wxMouseEvent &evt);

  Result abort();
};

class EulerSpiralTool;
class BiArcTool;

class ArrowTool: public DrawingTool {
  friend class EulerSpiralTool;
  friend class BiArcTool;
 public:
  ArrowTool(BaseWindow* win, Group *pgroup);

private:
  enum { NONE, RUBBER_BANDING } state;
  ArrowGeom *arrow;

  Result leftMouseDown(wxMouseEvent &evt);
  Result leftMouseUp(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);
  Result rightMouseDown(wxMouseEvent &evt);
  Result rightMouseUp(wxMouseEvent &evt);
  Result abort();
};
   
class EulerSpiralTool :public DrawingTool {
 public:
  EulerSpiralTool(BaseWindow* win, Group *pgroup);        
  ~EulerSpiralTool();
              
 private:
  enum  {NONE, SPIRALING} state;
  EulerSpiralSplineGeom *_esSpline;
  ArrowTool *_arrowTool;
  ArrowGeom *_curArrow;
  

  Result leftMouseDown(wxMouseEvent &evt);
  Result leftMouseUp(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);
  Result rightMouseDown(wxMouseEvent &evt);
  Result rightMouseUp(wxMouseEvent &evt);

  Result abort();
};

class BiArcTool :public DrawingTool {
 public:
  BiArcTool(BaseWindow* win, Group *pgroup);
  ~BiArcTool();
 private:
  enum  {NONE, BIARCING} state;
  BiArcSplineGeom *_baSpline;
  ArrowTool *_arrowTool;
  ArrowGeom *_curArrow;

  Result leftMouseDown(wxMouseEvent &evt);
  Result leftMouseUp(wxMouseEvent &evt);
  Result mouseMove(wxMouseEvent &evt);
  Result rightMouseDown(wxMouseEvent &evt);
  Result rightMouseUp(wxMouseEvent &evt);

  Result abort();
};

#endif
