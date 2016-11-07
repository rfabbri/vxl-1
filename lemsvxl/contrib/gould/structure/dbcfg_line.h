//      dbcfg_line.h
#ifndef dbcfg_line_h_
#define dbcfg_line_h_

//:
// \file
// \a line structure for use with dbcfg_cfg_constructor and the contour fragment graph
// \author Benjamin Gould
// \date 8/04/09
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>



// -------------------------
// dbcfg_line
// -------------------------

template <class T>
class dbcfg_line {

private:
  // typedef point and line
  typedef vgl_point_2d<T> point_t;
  typedef vcl_vector<point_t> points_t;
  typedef dbcfg_line<T> line_t;
  typedef vcl_vector<line_t> lines_t;

  point_t p1_;
  point_t p2_;
  int depth_;

public:
  // creates a new line
  dbcfg_line(point_t& p1, point_t& p2, int depth = 0);

  // destruct the contour fragment graph constructor
  ~dbcfg_line();

  // accessors for line segment
  double length();
  point_t p1();
  point_t p2();
  int depth();

  // handling methods
  T distance(point_t& point);
  T distance(line_t& line);
  bool intersects(line_t& line, T epsilon = 0);
  bool is_duplicate(line_t& line, T epsilon = 0);
  point_t get_intersection(point_t& point, T epsilon = 0);
  point_t get_intersection(line_t& line, T epsilon = 0);
  lines_t get_transform(vcl_vector<unsigned>& point_nums, points_t& points, T epsilon = 0);

};



#endif dbcfg_line_h_
// end dbcfg_line.h

