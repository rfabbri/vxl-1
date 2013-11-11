#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include <basegui_color.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

/* A StyleSet is an object which can specify all the style properties
 * we care about. For now, only color, line width, point size, and hidden
 * work.
 *
 * A StyleSet can contain any combination of these properties, including
 * specifying no properties. This causes the scene graph to have inheritance;
 * if a StyleSet doesn't specify the color (for example), it's inherited
 * from the node above it, and so on.
 *
 * You can combine styles using &. So, for example, (Color(1,0,0) & LineWidth(10))
 * specifies a StyleSet with color and line width properties. Any style
 * which isn't specified by a node in the tree is inherited from the scene
 * graph's master style, which is modified using scene_graph->root()->loadStyle()
 * or scene_graph->root()->mergeStyle().
 *
 * The call node->mergeStyle(new_style) effectively does "node->style &= new_style",
 * so the node's style is modified to include new_style's attributes... but, any
 * attributes not specified by new_style are unchanged.
 *
 * Nodes effectively have a stack of styles, since they're just nodes in the
 * scene graph. So, if your node has a style with a green color, but the object
 * just got selected, you can do node->pushStyle(Color(1,0,0)), and it'll turn
 * red. Then, call node->popStyle(), and it'll return to green.
 */

class SceneGraphVisitor;
class GLFont;

class StyleSet {
private:
  enum Properties {
    COLOR = 1,
    FONT = 2,
    LINE_WIDTH = 4,
    POINT_SIZE = 8,
    VISIBILITY = 16
  };

  int _properties_present;
  float _r_color, _g_color, _b_color, _a_color;
  double _line_width, _point_size;
  bool _hidden;
  GLFont *_font;

public:
  StyleSet();
  StyleSet(const StyleSet &s);
  StyleSet &operator=(const StyleSet &s);

  float red_color() const   { assert(has_color()); return _r_color; }
  float green_color() const { assert(has_color()); return _g_color; }
  float blue_color() const  { assert(has_color()); return _b_color; }
  float alpha_color() const { assert(has_color()); return _a_color; }
  bool has_color() const { return _properties_present & COLOR; }

  double line_width() const { assert(has_line_width()); return _line_width; }
  bool has_line_width() const { return _properties_present & LINE_WIDTH; }

  double point_size() const { assert(has_point_size()); return _point_size; }
  bool has_point_size() const { return _properties_present & POINT_SIZE; }

  bool hidden() const { assert(has_hidden()); return _hidden; }
  bool has_hidden() const { return _properties_present & VISIBILITY; }

  GLFont *font() const { assert(has_font()); return _font; }
  bool has_font() const { return _properties_present & FONT; }

  void merge(const StyleSet &a);
  StyleSet &operator&=(const StyleSet &a) { merge(a); return *this; }
  StyleSet get_undo_style(const StyleSet &a) const;

  void apply(SceneGraphVisitor *c) const; // sets c's current drawing state
                                     // to whatever we've got

  static StyleSet defaults();
private:
  friend StyleSet Color(float r, float g, float b, float a);
  friend StyleSet LineWidth(double l);
  friend StyleSet PointSize(double l);
  friend StyleSet FontFace(GLFont *font);
  friend StyleSet Hidden(bool hide);

  StyleSet(float r, float g, float b, float a);
  StyleSet(double l);
  StyleSet(double point_size, bool dummy);
  StyleSet(bool hide);
  StyleSet(GLFont *font);
};

StyleSet Color(float r, float g, float b, float a=1.0);
StyleSet Color(StandardColor c);
StyleSet LineWidth(double l);
StyleSet PointSize(double l);
StyleSet Hidden(bool hide=true);
StyleSet FontFace(GLFont *font);

// Note that the use of the "&" operator might seem dubious to one well-versed
// in bitfields and logical attributes, where "PropertyA | PropertyB" usually
// signifies "PropertyA and PropertyB", and "PropertyA & PropertyB" signifies
// neither PropertyA nor PropertyB :) Just remember, "&" means "and".

inline StyleSet operator&(const StyleSet &a, const StyleSet &b) {
  StyleSet p = a;
  p &= b;
  return p;
}



#endif
