#include "basegui_scenegraph.h"
#include "basegui_visitor.h"
#include "basegui_bounds.h"

SceneGraph::SceneGraph(const StyleSet &defaultStyle) {
  _root_node = new Group();
  _root_node->pushStyle(defaultStyle);
  _master_style = _root_node->getLocalStyleNode();
}

SceneGraph::~SceneGraph() {
  delete _master_style;
}

const Group *SceneGraph::rootGroup() const { return _root_node; }
Group *SceneGraph::rootGroup() { return _root_node; }

void SceneGraph::setMasterStyle(const StyleSet &s) {
  _master_style->mergeStyle(s);
}
const StyleSet &SceneGraph::masterStyle() const {
  return _master_style->localStyleSet();
}

// the scene graph is actually rooted at the _master_style node
void SceneGraph::traverse(SceneGraphVisitor *g) {
  VisitResult res = g->visitSceneGraph(this, VISIT_PRE);
  if(res == VISIT_ABORT || res == VISIT_SKIP) return;

  _master_style->traverse(g);
  g->visitSceneGraph(this, VISIT_POST);
}

void SceneGraph::getWorldBoundingRect(GraphicsNode *g,
      double &x1, double &y1, double &x2, double &y2) {
  getBoundingRect(XForm::identity(), g, x1, y1, x2, y2);
}

void SceneGraph::getBoundingRect(const XForm &space_xform, GraphicsNode *g,
      double &x1, double &y1, double &x2, double &y2) {
  BoundsVisitor visitor(space_xform * g->getXForm());
  g->traverse(&visitor);
  visitor.getRect(x1, y1, x2, y2);
}
