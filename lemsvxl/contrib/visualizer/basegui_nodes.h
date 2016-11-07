#ifndef NODES_H_INCLUDED
#define NODES_H_INCLUDED

#include "base_points.h"
#include "basegui_style.h"
#include "basegui_xform.h"

#include <vcl_vector.h>
#include <vcl_string.h>

#ifndef NDEBUG
#define SCENEGRAPH_DEBUG 1
#endif

class SceneGraphVisitor;

class GraphicsNode;

class LocalPropertyNode;  //: public GraphicsNode
class XFormNode;          //: public LocalPropertyNode
class StyleNode;          //: public LocalPropertyNode
class Group;              //: public GraphicsNode
class GeometryNode;       //: public GraphicsNode

// the actual drawing primitives are found in geomnodes.h
// TODO have dlist group

class GraphicsNode {
protected:
  GraphicsNode *_parent;

public:
  GraphicsNode() : _parent(NULL) { }
  virtual ~GraphicsNode();

  virtual bool traverse(SceneGraphVisitor *) = 0;
  // return false to abort the traversal

  virtual Group *parent();
  virtual void deleteSelf();
  virtual GraphicsNode *child(int i);
  virtual int numChildren() const = 0;

  // *** style management ***
  virtual StyleSet getStyle() const;

  virtual void loadStyle(const StyleSet &);
  virtual void mergeStyle(const StyleSet &);
  virtual void pushStyle(const StyleSet &);

  // *** xform management ***
  virtual XForm getXForm() const; 

  virtual void loadXForm(const XForm &x);
  virtual void multXForm(const XForm &x);
  virtual void pushXForm(const XForm &x);

  virtual void pop();
    virtual int  getInformation(vcl_string &) { return 0; };
    virtual int  getInformation(vcl_string &, Point2D<double> pt) { return 0; };

 public:
  // local api, used by the node classes for scene-graph managment
  // if there's a style node directly above this node, returns it.
  // else, returns null
  virtual StyleNode *getLocalStyleNode();
  virtual XFormNode *getLocalXFormNode();

 protected:
  // *** tree management (shouldn't be called directly) ***
  virtual GraphicsNode *localChild(int i) = 0;

  virtual GraphicsNode *getLocalParent();
  virtual void setLocalParent(GraphicsNode *p);
  virtual void spliceLocalChild(GraphicsNode *cur_child, GraphicsNode *new_child);

  // splices the new_parent into the tree, 
  // directly above this node
  virtual void pushLocalNodeAbove(LocalPropertyNode *new_parent);

 public:
  virtual int getPoints_(vcl_vector<Point2D<double> >& pts_);
  virtual bool setPoint_(int id_, const Point2D<double> pt_);
  bool supportSelect_();

  friend class Group;
  friend class SceneGraph;
  friend class LocalPropertyNode;
};

class Group : public GraphicsNode {
private:
  typedef vcl_vector<GraphicsNode*> children_t;
  children_t _children;

public:

    Group();
  virtual ~Group();

  virtual bool traverse(SceneGraphVisitor *);

  virtual int numChildren() const;
  virtual GraphicsNode *localChild(int i);
  GraphicsNode* back() { return _children.back(); }

  // child can be a stand-alone node, or a node in a tree
  // it will be added to this group and removed from wherever it lay before
  void moveToGroup(GraphicsNode *child);

  GraphicsNode *addChild(GraphicsNode *child);
  GraphicsNode *addChild(GraphicsNode *child, int location);
  void removeChild(GraphicsNode *child);
  void eraseAllChildren();

protected:
  virtual void spliceLocalChild(GraphicsNode *cur_child, GraphicsNode *new_child);
};

class LocalPropertyNode : public GraphicsNode {
protected:
  GraphicsNode *_child;

public:
  LocalPropertyNode();
  virtual ~LocalPropertyNode();

  virtual int numChildren() const;
  virtual GraphicsNode *localChild(int i);

protected:
  virtual void removeSelf();
  virtual void setLocalChild(GraphicsNode *_child);
  virtual void spliceLocalChild(GraphicsNode *cur_child, GraphicsNode *new_child);

  friend class GraphicsNode;
};


class StyleNode : public LocalPropertyNode {
private:
  StyleSet _style;

public:
  StyleNode(const StyleSet &sty);

  virtual bool traverse(SceneGraphVisitor *);

  virtual StyleSet getStyle() const;
  virtual void loadStyle(const StyleSet &a);
  virtual void mergeStyle(const StyleSet &a);

  virtual const StyleSet &localStyleSet() const;
};

class XFormNode : public LocalPropertyNode {
private:
  XForm _xform;

public:
  XFormNode(const XForm &xf);

  virtual bool traverse(SceneGraphVisitor *);
  virtual XForm getXForm() const;

  XForm &xform() { return _xform; }
  const XForm &xform() const { return _xform; }
};

class GeometryNode : public GraphicsNode {
public:
  virtual void draw(SceneGraphVisitor *v) = 0;
  virtual int numChildren() const;
  virtual GraphicsNode *localChild(int i);
  virtual bool traverse(SceneGraphVisitor *);
};

class AffineNode : public GeometryNode { 
 public:
};

class ScaleInvariantNode : public GeometryNode {
private:
  double origin_x, origin_y;
public:
  ScaleInvariantNode(double ox, double oy);
  ScaleInvariantNode(Point2D<double> origin);

  void setOrigin(double x, double y);
  void setOrigin(Point2D<double> o);

    Point2D<double> getOrigin()
      {
        return Point2D<double>(origin_x, origin_y);
       }
  virtual bool traverse(SceneGraphVisitor *);
};

#endif
