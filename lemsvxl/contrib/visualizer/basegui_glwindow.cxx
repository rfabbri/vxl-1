#include "basegui_glwindow.h"
#include "basegui_glgraphics.h"
#include "basegui_scenegraph.h"
#include "basegui_visitor.h"
#include "basegui_basewindow.h"

#include <wx/wx.h>
#include <wx/dcclient.h>

// massive hack: wxwindows doesn't like to initialize the glwindow to a
// reasonable size until it sends a few draw and resize events with
// the wrong size first. so, to hack this, we initialize the window 
// to 42x42, and don't do our stuff until the window is resized to
// a size other than that
const int MAGIC_WINDOW_SIZE = 42;

// ---------------------------------------------------------------------------
// GLWindow
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GLWindow, wxGLCanvas)
    EVT_SIZE(GLWindow::OnSize)
    EVT_PAINT(GLWindow::OnPaint)
    EVT_ERASE_BACKGROUND(GLWindow::OnEraseBackground)
    EVT_LEFT_DOWN(GLWindow::OnLeftMouseDown)
    EVT_LEFT_UP(GLWindow::OnLeftMouseUp)
    EVT_MIDDLE_DOWN(GLWindow::OnMiddleMouseDown)
    EVT_MIDDLE_UP(GLWindow::OnMiddleMouseUp)
    EVT_RIGHT_DOWN(GLWindow::OnRightMouseDown)
    EVT_RIGHT_UP(GLWindow::OnRightMouseUp)
    EVT_MOTION(GLWindow::OnMouseMove)
    EVT_IDLE(GLWindow::OnIdle)
  EVT_KEY_DOWN(GLWindow::OnKeyDown)
  EVT_KEY_UP(GLWindow::OnKeyUp)
  EVT_CHAR(GLWindow::OnChar)
END_EVENT_TABLE()

GLWindow::GLWindow(BaseWindow *parent, const char *name, SceneGraph *graph)
    : wxGLCanvas((wxWindow*)parent,(wxGLCanvas*)NULL,-1,
           wxDefaultPosition,wxSize(MAGIC_WINDOW_SIZE, MAGIC_WINDOW_SIZE),
         wxSUNKEN_BORDER, name,
           NULL,wxNullPalette),
      _win(parent), _glinitialized(false), _completelyCreated(false)
{
  _scene_graph = graph;
  _view = new GLGraphicsView(_scene_graph);

  // this is needed to cause key events to get sent to the GL window
  SetFocus();
}

GLWindow::~GLWindow(){
  delete _view;
}

void GLWindow::OnPaint(wxPaintEvent &event) 
  {
  /* must always be here */
  wxPaintDC dc(this);
  if(_glinitialized)
    RefreshNow();
}

void GLWindow::RefreshNow() {
  assert(_glinitialized);
  if (!GetContext()) return;
  SetCurrent();

  _view->draw();

  SwapBuffers();
}

void GLWindow::OnSize(wxSizeEvent& event) {
  int width, height;
  GetClientSize(&width, &height);

  if(width != MAGIC_WINDOW_SIZE || height != MAGIC_WINDOW_SIZE) {
    if(!GetContext()) return;
    SetCurrent();

    if(!_glinitialized) {
      _glinitialized=true;
      _view->init();
    }

    _view->resize(width, height);
  }
}

void GLWindow::OnMouseMove( wxMouseEvent& event )
{
  _win->mouseMove(event); // relay through BaseWindow
}

void GLWindow::OnLeftMouseDown( wxMouseEvent& event )  { _win->leftMouseDown(event); }
void GLWindow::OnLeftMouseUp( wxMouseEvent& event )    { _win->leftMouseUp(event); }
void GLWindow::OnRightMouseDown( wxMouseEvent& event ) { _win->rightMouseDown(event); }
void GLWindow::OnRightMouseUp( wxMouseEvent& event )   { _win->rightMouseUp(event); }
void GLWindow::OnMiddleMouseDown( wxMouseEvent& event ){ _win->middleMouseDown(event); }
void GLWindow::OnMiddleMouseUp( wxMouseEvent& event )  { _win->middleMouseUp(event); }

void GLWindow::OnKeyDown(wxKeyEvent &event)        { _win->keyDown(event); }
void GLWindow::OnKeyUp(wxKeyEvent &event)            { _win->keyUp(event); }
void GLWindow::OnChar(wxKeyEvent &event)             { _win->charUp(event); }

void GLWindow::Refresh(bool eraseBackground, const wxRect *rect) {
  wxGLCanvas::Refresh(eraseBackground, rect);
  if(eraseBackground)
    vcl_cerr << "Warning: GLWindow::Refresh was called with eraseBackground = true\n"
      << "This may cause the display to flicker; use Refresh(false)" << vcl_endl;
}

void GLWindow::OnEraseBackground(wxEraseEvent& event)
{   
    /* Do nothing, to avoid flashing on MSW */
}

void GLWindow::CompletelyCreated() { }
void GLWindow::OnIdle(wxIdleEvent& event) {
  if(_glinitialized && !_completelyCreated) {
    _win->CompletelyCreated();
    CompletelyCreated();
    _completelyCreated = true;
  }
}
