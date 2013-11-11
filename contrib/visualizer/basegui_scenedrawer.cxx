#include "basegui_scenedrawer.h"

SceneDrawer::SceneDrawer(SceneGraph *sg_, Group*localRootNode_)
  : _sg(sg_),_localRootNode(localRootNode_)
{
  if(_sg==NULL){
    _sg = new SceneGraph();
  }
  if(_localRootNode==NULL){
    _localRootNode = _sg->rootGroup();
  }
}
SceneDrawer::~SceneDrawer(){}

