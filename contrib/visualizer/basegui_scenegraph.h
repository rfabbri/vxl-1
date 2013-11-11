#ifndef SCENE_GRAPH_H_INCLUDED
#define SCENE_GRAPH_H_INCLUDED

#include "base_points.h"
#include "basegui_nodes.h"
#include "basegui_geomnodes.h"

class SceneGraph {
private:
  SceneGraph(const SceneGraph &);
  SceneGraph &operator =(const SceneGraph &);

protected:
  Group *_root_node;
  StyleNode *_master_style;

public:
  SceneGraph(const StyleSet &defaultStyle = StyleSet::defaults());
  ~SceneGraph();

  Group *rootGroup();
  const Group *rootGroup() const;

  const StyleSet &masterStyle() const;
  void setMasterStyle(const StyleSet &s);

  void traverse(SceneGraphVisitor *g);

  // gets a bounding rect in world space.
  // g can be anything, but this is most useful with a group
  void getWorldBoundingRect(GraphicsNode *g,
      double &x1, double &y1, double &x2, double &y2);

  // gets a bounding rect in arbitrary space... to get bounds
  // in screen space, use the viewing transformation for space_xform
  void getBoundingRect(const XForm &space_xform, GraphicsNode *g,
      double &x1, double &y1, double &x2, double &y2);
};

#endif
