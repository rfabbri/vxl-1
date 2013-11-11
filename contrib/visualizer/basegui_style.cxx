#include "basegui_visitor.h"
#include "basegui_glfont.h"
#include "basegui_style.h"

StyleSet StyleSet::defaults() {
  return Color(BLACK) & LineWidth(1) & PointSize(3) & Hidden(false)
    & FontFace(GLFont::getFont(GLFont::DEFAULT_FACE, 10));
}

StyleSet::StyleSet() {
  _properties_present = 0;
}

StyleSet::StyleSet(const StyleSet &s) {
  _properties_present = s._properties_present;
  _r_color = s._r_color;
  _g_color = s._g_color;
  _b_color = s._b_color;
  _a_color = s._a_color;
  _line_width = s._line_width;
  _point_size = s._point_size;
  _hidden = s._hidden;
  _font = s._font;
}

StyleSet &StyleSet::operator=(const StyleSet &s) {
  _properties_present = s._properties_present;
  _r_color = s._r_color;
  _g_color = s._g_color;
  _b_color = s._b_color;
  _a_color = s._a_color;
  _line_width = s._line_width;
  _point_size = s._point_size;
  _hidden = s._hidden;
  _font = s._font;
  return *this;
}

StyleSet::StyleSet(float r, float g, float b, float a) {
  _properties_present = COLOR;
  _r_color = r;
  _g_color = g;
  _b_color = b;
  _a_color = a;
}
StyleSet::StyleSet(double l) {
  _properties_present = LINE_WIDTH;
  _line_width = l;
}
StyleSet::StyleSet(double p, bool /*dummy*/) {
  _properties_present = POINT_SIZE;
  _point_size = p;
}
StyleSet::StyleSet(bool hidden) {
  _properties_present = VISIBILITY;
  _hidden = hidden;
}
StyleSet::StyleSet(GLFont *font) {
  _properties_present = FONT;
  _font = font;
}

StyleSet Color(float r, float g, float b, float a) {
  return StyleSet(r, g, b, a);
}
StyleSet Color(StandardColor c) {
  float r, g, b;
  lookup_standard_color(c, r, g, b);
  return Color(r,g,b);
}
StyleSet LineWidth(double l) {
  return StyleSet(l);
}
StyleSet PointSize(double p) {
  return StyleSet(p, false /*false is a dummy*/);
}
StyleSet Hidden(bool hide) {
  return StyleSet(hide);
}
StyleSet FontFace(GLFont *font) {
  return StyleSet(font);
}

void StyleSet::merge(const StyleSet &a) {
  if(a.has_color()) {
    _properties_present |= COLOR;
    _r_color = a._r_color;
    _g_color = a._g_color;
    _b_color = a._b_color;
    _a_color = a._a_color;
  }
  if(a.has_point_size()) {
    _properties_present |= POINT_SIZE;
    _point_size = a._point_size;
  }
  if(a.has_line_width()) {
    _properties_present |= LINE_WIDTH;
    _line_width = a._line_width;
  }
  if(a.has_hidden()) {
    _properties_present |= VISIBILITY;
    _hidden = a._hidden;
  }
  if(a.has_font()) {
    _properties_present |= FONT;
    _font = a._font;
  }
}

StyleSet StyleSet::get_undo_style(const StyleSet &a) const {
  StyleSet undo;

  if(a.has_color() && this->has_color())
    undo &= Color(_r_color, _g_color, _b_color, _a_color);
  if(a.has_point_size() && this->has_point_size())
    undo &= PointSize(_point_size);
  if(a.has_line_width() && this->has_line_width())
    undo &= LineWidth(_line_width);
  if(a.has_hidden() && this->has_hidden())
    undo &= Hidden(_hidden);
  if(a.has_font() && this->has_font())
    undo &= FontFace(_font);
  return undo;
}

void StyleSet::apply(SceneGraphVisitor *c) const {
  if(has_color())
    c->setColor(_r_color, _g_color, _b_color, _a_color);
  if(has_point_size())
    c->setPointSize(_point_size);
  if(has_line_width())
    c->setLineWidth(_line_width);
  // _hidden isn't really listened to ... this is handled at the
  // scene-tree traversal level by not traversing the nodes
  if(has_hidden())
    c->setHidden(_hidden);
  if(has_font())
    c->setFont(_font);
}
