#ifndef __SCENE_DRAWER_H__
#define __SCENE_DRAWER_H__
#include "basegui_scenegraph.h"
#include "base_points.h"
#include "basegui_style.h"

class SceneDrawer {
 private:
    SceneGraph *_sg;
    Group *_localRootNode;  // This does not necessarily == _sg->rootNode();
 public:
  SceneDrawer(SceneGraph *sg_, Group*localRootNode_);
  ~SceneDrawer();

  SceneGraph* sg() {return _sg;}
  Group* localRootNode() {return _localRootNode;}
};

#endif
