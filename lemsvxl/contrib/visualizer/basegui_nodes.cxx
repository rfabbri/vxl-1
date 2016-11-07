
#include <algorithm>
#include <math.h>
#include <assert.h>

#include "basegui_nodes.h"
#include "basegui_scenegraph.h"
#include "basegui_visitor.h"

GraphicsNode::~GraphicsNode() {
  _parent = NULL;
}

void GraphicsNode::deleteSelf() {
  assert(parent());
  parent()->removeChild(this);
  delete this;
}

Group *GraphicsNode::parent() {
  GraphicsNode *cp = _parent;
  while(cp && dynamic_cast<LocalPropertyNode*>(cp)) {
    cp = cp->_parent;
  }
  if(cp) assert(dynamic_cast<Group*>(cp));

  return dynamic_cast<Group*>(cp);
}
GraphicsNode *GraphicsNode::child(int i) {
  GraphicsNode *cc = localChild(i);
  while(cc && dynamic_cast<LocalPropertyNode*>(cc))
    cc = cc->child(0);
  assert(cc);
  return cc;
}

StyleNode *GraphicsNode::getLocalStyleNode() {
  GraphicsNode *p = this;

  if(!dynamic_cast<LocalPropertyNode*>(p))
    p = p->_parent;
  while(p && dynamic_cast<LocalPropertyNode*>(p)) {
    if(StyleNode *style = dynamic_cast<StyleNode*>(p))
      return style;
    p = p->_parent;
  }
  return NULL;
}

XFormNode *GraphicsNode::getLocalXFormNode() {
  GraphicsNode *p = this;

  if(!dynamic_cast<LocalPropertyNode*>(p))
    p = p->_parent;
  while(p && dynamic_cast<LocalPropertyNode*>(p)) {
    if(XFormNode *style = dynamic_cast<XFormNode*>(p))
      return style;
    p = p->_parent;
  }
  return NULL;
}

GraphicsNode *GraphicsNode::getLocalParent() { return _parent; }
void GraphicsNode::setLocalParent(GraphicsNode *p) { _parent = p; }

void GraphicsNode::pushLocalNodeAbove(LocalPropertyNode *new_parent) {
  GraphicsNode *old_parent = _parent;

  _parent = NULL; // placate warning in setChild, which bitches if
                  // then node is already attached
  new_parent->setLocalChild(this);

  if(old_parent)
    old_parent->spliceLocalChild(this, new_parent);
  _parent = new_parent;
}

void GraphicsNode::spliceLocalChild(GraphicsNode *cur_child, GraphicsNode *new_child) {
  assert("spliceLocalChild" == "called on an inappropriate object");
}

StyleSet GraphicsNode::getStyle() const {
  if(!_parent)
    return StyleSet();
  return _parent->getStyle();
}

XForm GraphicsNode::getXForm() const {
  if(!_parent)
    return XForm::identity();
  return _parent->getXForm();
}

void GraphicsNode::loadStyle(const StyleSet &p) {
  StyleNode *local_style = getLocalStyleNode();

  if(!_parent || !local_style) {
    // no preexisting style parent
    pushLocalNodeAbove(new StyleNode(p));
  } else {
    // there was a preexisting style parent
    local_style->loadStyle(p);
  }
}

void GraphicsNode::mergeStyle(const StyleSet &p) {
  StyleNode *local_style = getLocalStyleNode();

  if(!_parent || !local_style) {
    pushLocalNodeAbove(new StyleNode(p));
  } else {
    // there was a preexisting style parent
    local_style->mergeStyle(p);
  }
}

void GraphicsNode::pushStyle(const StyleSet &p) {
  pushLocalNodeAbove(new StyleNode(p));
}

void GraphicsNode::loadXForm(const XForm &x) {
  XFormNode *local_xform = getLocalXFormNode();

  if(!_parent || !local_xform) {
    // no preexisting xform parent
    pushLocalNodeAbove(new XFormNode(x));
  } else {
    // there was a preexisting xform parent
    local_xform->xform() = x;
  }
}

void GraphicsNode::multXForm(const XForm &x) {
  XFormNode *local_xform = getLocalXFormNode();

  if(!_parent || !local_xform) {
    // no preexisting xform parent
    pushLocalNodeAbove(new XFormNode(x));
  } else {
    // there was a preexisting xform parent
    local_xform->xform() = local_xform->xform() * x;
  }
}

void GraphicsNode::pushXForm(const XForm &x) {
  pushLocalNodeAbove(new XFormNode(x));
}

void GraphicsNode::pop() {
  assert(_parent);
  LocalPropertyNode *node_above = 
    dynamic_cast<LocalPropertyNode*>(_parent);

  if(node_above) {
    // there was a preexisting properties parent
    node_above->removeSelf();
    delete node_above;
  } else {
    vcl_cerr << "warning: pop called, but no local properties node to pop" << vcl_endl;
  }
}

int GraphicsNode::getPoints_(vcl_vector<Point2D<double> >& pts_){
  return 0;
}
bool GraphicsNode::setPoint_(int id_, const Point2D<double> pt_) {
  return false;
}
bool GraphicsNode::supportSelect_(){
  vcl_vector<Point2D<double> > pts;
  return getPoints_(pts)>0;
}

Group::Group() { }
Group::~Group() {
  for(children_t::iterator i = _children.begin(); i != _children.end(); ++i)
    delete *i;
}

int Group::numChildren() const {
  return _children.size();
}

GraphicsNode *Group::localChild(int i) {
  assert(i >= 0 && i < (int)_children.size());
  return _children[i];
}

GraphicsNode *Group::addChild(GraphicsNode *child) {
  return addChild(child, numChildren());
}

GraphicsNode *Group::addChild(GraphicsNode *child, int location) {
  assert(location >= 0 && location <= numChildren());

#ifdef SCENEGRAPH_DEBUG
  GraphicsNode *child_tmp = child;
  while(child_tmp->getLocalParent() != NULL) {
    if(dynamic_cast<LocalPropertyNode*>(child_tmp->getLocalParent()) == NULL)
      vcl_cerr << "warning: addChild called, but child seems to already be in a tree\n"
        "it has a parent which isn't a LocalPropertyNode" << vcl_endl;
    child_tmp = child_tmp->getLocalParent();
  }
#endif

  while(child->getLocalParent() != NULL)
    child = child->getLocalParent();

  _children.insert(_children.begin() + location, child);
  child->setLocalParent(this);
  return child;
}

void Group::removeChild(GraphicsNode *child) {
  // because there are local nodes (xforms, style) between the group and the
  // actual gfx node, move up the tree until we get to ourself
  while(child && child->getLocalParent() != this) {
    child = child->getLocalParent();
#ifdef SCENEGRAPH_DEBUG
    if(child != this && dynamic_cast<LocalPropertyNode*>(child) == NULL)
      vcl_cerr << "warning: Group::removeChild called, but child doesn't appear to belong to "
        "this group" << vcl_endl;
#endif
  }

  if(!child) {
    vcl_cerr << "warning: Group::removeChild called, but child doesn't appear to belong to "
      "this group" << vcl_endl;
    return;
  }

  children_t::iterator
      i = std::find(_children.begin(), _children.end(), child);
  if(i == _children.end()) {
    vcl_cerr << "warning: Group::eraseChild called, but child not found" << vcl_endl;
    return;
  }

  _children.erase(i);
  child->setLocalParent(NULL);
}

void Group::eraseAllChildren() {
  for(children_t::iterator i = _children.begin();
      i != _children.end(); ++i) {
    (*i)->setLocalParent(NULL);
    delete *i;
  }
  _children.erase(_children.begin(), _children.end());
}

void Group::moveToGroup(GraphicsNode *child) {
  // get the eventual parent (will be null if child is isolated)
  Group *old_parent = child->parent();

  if(!old_parent) {
    // isolated node
    addChild(child);
  } else {
    old_parent->removeChild(child);
    addChild(child);
  }
}

void Group::spliceLocalChild(GraphicsNode *cur_child, GraphicsNode *new_child) {
  children_t::iterator
    i = std::find(_children.begin(), _children.end(), cur_child);
  assert(i != _children.end());

  if(new_child != cur_child) {
    *i = new_child;
    new_child->setLocalParent(this);
    cur_child->setLocalParent(NULL);
  }
}

LocalPropertyNode::LocalPropertyNode() : _child(NULL) { }
LocalPropertyNode::~LocalPropertyNode() { delete _child; }
int LocalPropertyNode::numChildren() const { return 1; }
GraphicsNode *LocalPropertyNode::localChild(int i) {
  assert(i == 0); return _child;
}

void LocalPropertyNode::setLocalChild(GraphicsNode *child) {
  _child = child;
#ifdef SCENEGRAPH_DEBUG
  if(_child->getLocalParent() != NULL)
    vcl_cerr << "warning: adding local property node which"
      " already has non-null parent" << vcl_endl;
#endif
  _child->setLocalParent(this);
}

void LocalPropertyNode::spliceLocalChild(GraphicsNode *cur_child,
    GraphicsNode *new_child) {
  assert(cur_child == _child);
  if(new_child != cur_child) {
    _child = new_child;
    new_child->setLocalParent(this);
    cur_child->setLocalParent(NULL);
  }
}

void LocalPropertyNode::removeSelf() {
  _parent->spliceLocalChild(this, _child);
  _parent = NULL;
  _child = NULL;
}




StyleNode::StyleNode(const StyleSet &sty)
    : LocalPropertyNode(), _style(sty) { }

StyleSet StyleNode::getStyle() const {
  StyleSet parentStyle;
  if(_parent)
    parentStyle = _parent->getStyle();
  return parentStyle & _style;
}
const StyleSet &StyleNode::localStyleSet() const {
  return _style;
}

void StyleNode::loadStyle(const StyleSet &a) {
  _style = a;
}

void StyleNode::mergeStyle(const StyleSet &a) {
  _style &= a;
}

XFormNode::XFormNode(const XForm &xf)
  : LocalPropertyNode(), _xform(xf) {}

XForm XFormNode::getXForm() const {
  if(_parent)
    return _parent->getXForm() * _xform;
  else
    return _xform;
}

int GeometryNode::numChildren() const {
  return 0;
}

GraphicsNode *GeometryNode::localChild(int i) {
  assert(NULL == "child called for GeometryNode");
  return NULL;
}

ScaleInvariantNode::ScaleInvariantNode(double ox, double oy) { setOrigin(ox, oy); }
ScaleInvariantNode::ScaleInvariantNode(Point2D<double> origin) { setOrigin(origin); }

void ScaleInvariantNode::setOrigin(double x, double y) {
  origin_x = x;
  origin_y = y;
}
void ScaleInvariantNode::setOrigin(Point2D<double> a) { setOrigin(a.getX(), a.getY()); }



// traversal/visiting routines

bool Group::traverse(SceneGraphVisitor *v) {
  VisitResult res;

  res = v->visitGroup(this, VISIT_PRE);
  if(res == VISIT_SKIP) return true;
  if(res == VISIT_ABORT) return false;

  for(children_t::iterator
      i = _children.begin(); i != _children.end(); ++i) {
    if(!(*i)->traverse(v))
      return false;
  }

  res = v->visitGroup(this, VISIT_POST);
  return res != VISIT_ABORT;
}

bool StyleNode::traverse(SceneGraphVisitor *v) {
  VisitResult res;

  res = v->visitStyle(this, VISIT_PRE);
  if(res == VISIT_SKIP) return true;
  if(res == VISIT_ABORT) return false;

  StyleSet undo_set = v->mergeStyle(_style);
  if(!_child->traverse(v))
    return false;
  v->mergeStyle(undo_set);

  res = v->visitStyle(this, VISIT_POST);
  return res != VISIT_ABORT;
}

bool XFormNode::traverse(SceneGraphVisitor *v) {
  VisitResult res;

  res = v->visitXForm(this, VISIT_PRE);
  if(res == VISIT_SKIP) return true;
  if(res == VISIT_ABORT) return false;

  v->pushMatrix();
  v->multXForm(_xform);
  if(!_child->traverse(v))
    return false;
  v->popMatrix();

  res = v->visitXForm(this, VISIT_POST);
  return res != VISIT_ABORT;
}

bool GeometryNode::traverse(SceneGraphVisitor *v) {
  VisitResult res;
  res = v->visitGeometry(this, VISIT_PRE);
  if(res == VISIT_SKIP) return true;
  if(res == VISIT_ABORT) return false;

  draw(v);

  res = v->visitGeometry(this, VISIT_POST);
  return res != VISIT_ABORT;
}

bool ScaleInvariantNode::traverse(SceneGraphVisitor *v) {
  VisitResult res;
  res = v->visitGeometry(this, VISIT_PRE);
  if(res == VISIT_SKIP) return true;
  if(res == VISIT_ABORT) return false;

  v->unscale(origin_x, origin_y);
  draw(v);
  v->rescale();

  res = v->visitGeometry(this, VISIT_POST);
  return res != VISIT_ABORT;
}
