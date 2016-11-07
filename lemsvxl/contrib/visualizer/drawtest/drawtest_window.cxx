#include "drawtest_window.h"
#include "base_geometry_functions.h"
#include "command.h"

enum {
  VIS_DRAWING_TEST_TOOL = CUSTOM_ID_BASE+50
};

class DirectedLineGeom : public LineGeom {
  double _min_line_size;

  virtual void draw(SceneGraphVisitor *v) {
    LineGeom::draw(v);

    double size = euc_distance(start(), end()) * v->getScaleFactor();
    if(size > _min_line_size) {
      Point2D<double> center = .5*(start() + end()), direction = end() - start();

      v->unscale(center.getX(), center.getY());
      v->pushMatrix();
      v->multXForm(XForm::rotation(atan2(direction.getY(), direction.getX())));

      StyleSet s = v->mergeStyle(Color(RED));
      v->beginFilledPolygon();
      v->vertex(5,0);
      v->vertex(-5,5);
      v->vertex(-5,-5);
      v->end();
      v->mergeStyle(s);

      v->beginUnfilledPolygon();
      v->vertex(5,0);
      v->vertex(-5,5);
      v->vertex(-5,-5);
      v->end();

      v->popMatrix();
      v->rescale();
    }
  }

public:
  DirectedLineGeom(const Point2D<double> &pp0, const Point2D<double> &pp1, double min_line_size = 0.05)
    : LineGeom(pp0, pp1), _min_line_size(min_line_size) {}
};

class LineLabelGeom : public AffineNode {
  LineGeom *line;
  double _min_line_size;

  virtual void draw(SceneGraphVisitor *v) {
    double size = euc_distance(line->start(), line->end()) * v->getScaleFactor();
    if(size > _min_line_size) {
      Point2D<double> center = .5*(line->start() + line->end()), direction = line->end() - line->start();

      v->unscale(center.getX(), center.getY());
      v->pushMatrix();
      v->multXForm(XForm::rotation(atan2(direction.getY(), direction.getX())));

      v->beginFilledPolygon();
      v->vertex(5,0);
      v->vertex(-5,5);
      v->vertex(-5,-5);
      v->end();

      StyleSet s = v->mergeStyle(Color(BLACK));
      v->beginUnfilledPolygon();
      v->vertex(5,0);
      v->vertex(-5,5);
      v->vertex(-5,-5);
      v->end();
      v->mergeStyle(s);

      v->popMatrix();
      v->rescale();
    }
  }

public:
  LineLabelGeom(LineGeom *g, double min_line_size = 0.05)
    : line(g), _min_line_size(min_line_size) {}
};

class TestTool: public DrawingTool {
public:
  TestTool(BaseWindow *win, Group *pgroup) : 
    DrawingTool(win, pgroup), g(NULL), state(NONE) { }
private:
  enum { NONE, RUBBER_BANDING } state;
  LineGeom *g;
  LineLabelGeom *label;

  Result leftMouseDown(const wxMouseEvent *evt) {
    g = new LineGeom(
        view->unproject(Point2D<double>(evt->GetX(), evt->GetY())),
        view->unproject(Point2D<double>(evt->GetX(), evt->GetY())));
    label = new LineLabelGeom(g);
    label->mergeStyle(Color(RED));
    tmp()->addChild(g);
    tmp()->addChild(label);
    state = RUBBER_BANDING;

    return HANDLED;
  }

  Result leftMouseUp(const wxMouseEvent *evt) {
    if(state != RUBBER_BANDING) return UNHANDLED;

    g->setEnd(view->unproject(Point2D<double>(evt->GetX(), evt->GetY())));
    tmp()->removeChild(g);
    tmp()->removeChild(label);
    state = NONE;

    MultiCommand *c = new MultiCommand();
    c->addCommand(new AddGeomCommand(g, permanent_group));
    c->addCommand(new AddGeomCommand(label, permanent_group));
    g = NULL;

    return COMPLETED;
  }

  Result mouseMove(const wxMouseEvent *evt) {
    if(state == NONE) return UNHANDLED;

    g->setEnd(view->unproject(Point2D<double>(evt->GetX(), evt->GetY())));

    return HANDLED;
  }

  Result rightMouseDown(const wxMouseEvent *evt) {
    if(state == NONE) return UNHANDLED;

    state = NONE;
    tmp()->removeChild(g);
    delete g; g = NULL;

    return ABORTED;
  }

  Result rightMouseUp(const wxMouseEvent *evt) { return UNHANDLED; }
  Result middleMouseDown(const wxMouseEvent *evt) { return UNHANDLED; }
  Result middleMouseUp(const wxMouseEvent *evt) { return UNHANDLED; }
};

DrawTestWindow::DrawTestWindow(MainGUIWindow *win, BasePlugin *plugin) :
    DrawingWindow(win, "Drawing tools test", plugin) {
  //setDrawingMode(true);
}

void DrawTestWindow::addCustomMenus(wxMenuBar *menu,
        wxMenu *file, wxMenu *help, std::vector<wxAcceleratorEntry> &accels) {
  _tools_menu->Append(VIS_DRAWING_TEST_TOOL, "&Test tool");
}

GTool *DrawTestWindow::getToolFromID(int id) {
  switch(id) {
  case VIS_DRAWING_TEST_TOOL:
    return new TestTool(this, _drawing_group);

  default:
    return DrawingWindow::getToolFromID(id);
  }
}

void DrawTestWindow::menuItemSelected(wxCommandEvent &evt) {
  DrawingWindow::menuItemSelected(evt);
  switch(evt.GetId()) {
  case VIS_DRAWING_TEST_TOOL:
    OnToolSelectionEvent(evt);
    break;
  }
}
