#include <basegui_xpgl.h>

#include "basegui_visitor.h"
#include "basegui_nodes.h"
#include "basegui_scenegraph.h"
#include <algorithm>
#include <vector>
#include <iostream>


const StyleSet &SceneGraphVisitor::currentStyle() const {
  return _current_style;
}

StyleSet SceneGraphVisitor::mergeStyle(const StyleSet &add_style) {
  // get the style which will undo the changes
  StyleSet undo_style = _current_style.get_undo_style(add_style);

  add_style.apply(this);
  _current_style.merge(add_style);

  return undo_style;
}

void SceneGraphVisitor::loadStyle(const StyleSet &new_style) {
  new_style.apply(this);
  _current_style = new_style;
}
