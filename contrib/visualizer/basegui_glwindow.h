#ifndef GLWINDOW_H_INCLUDED
#define GLWINDOW_H_INCLUDED

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <basegui_xpgl.h>

class Interactor;
class GLGraphicsView;
class SceneGraph;
class BaseWindow;

#ifndef NDEBUG
#define DEBUG_GL_DRAWING 1
#endif

class GLWindow: public wxGLCanvas {
 private:
  BaseWindow* _win;
public:
  GLWindow(BaseWindow *parent, const char *name, SceneGraph *graph);
  ~GLWindow();

  //TAKA
  GLGraphicsView* view() {return _view;}
  virtual void CompletelyCreated();
  void RefreshNow();

protected:
  GLGraphicsView *_view;
  SceneGraph *_scene_graph;

  void initializeGL();
  bool _glinitialized;
  bool _completelyCreated;

  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);

  // mouse
  void OnMouseMove( wxMouseEvent& event );
  void OnLeftMouseDown( wxMouseEvent& event );
  void OnLeftMouseUp( wxMouseEvent& event );
  void OnMiddleMouseDown( wxMouseEvent& event );
  void OnMiddleMouseUp( wxMouseEvent& event );
  void OnRightMouseDown( wxMouseEvent& event );
  void OnRightMouseUp( wxMouseEvent& event );

  void OnIdle(wxIdleEvent& event);
  void OnKeyDown(wxKeyEvent &evt);
  void OnKeyUp(wxKeyEvent &evt);
  void OnChar(wxKeyEvent &evt);

public:
  // override the wxWindow Refresh method, to warn you if it gets called
  // with eraseBackground == true
  virtual void Refresh(bool eraseBackground = false, const wxRect *rect = NULL);

   DECLARE_EVENT_TABLE()
};

#endif
