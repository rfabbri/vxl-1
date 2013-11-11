//      dbcfg_cfg_constructor.h
#ifndef dbcfg_cfg_constructor_h_
#define dbcfg_cfg_constructor_h_

//:
// \file
// \a constructor for the contour fragment graph
// \author Benjamin Gould
// \date 8/03/09
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbcfg_line.h"
#include "dbcfg_curve.h"
#include "dbcfg_contour_fragment_graph.h"
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_polygon.h>
#include <dbdet/sel/dbdet_edgel.h>



// -------------------------
// dbcfg_cfg_constructor
// -------------------------

template <class T>
class dbcfg_cfg_constructor {

private:

  // typedef point, points, line, and lines
  typedef vgl_point_2d<T> point_t;
  typedef vcl_vector<point_t> points_t;
  typedef dbcfg_line<T> line_t;
  typedef vcl_vector<line_t> lines_t;

  int xsize_;
  int ysize_;

  T epsilon_;

  vcl_vector<lines_t> _lines;

  // functions

  // creates a depth sorted collection of transformed lines
  void forge_transform(vcl_vector<lines_t>& final_lines);

  // merges the given line cluster into a single line
  line_t merge_line_cluster(lines_t& cluster);

public:
  // begins a new contour fragment graph constructor
  dbcfg_cfg_constructor(int xsize, int ysize, T epsilon = 0);

  // destruct the contour fragment graph constructor
  ~dbcfg_cfg_constructor();

  // adds a new line to the constructor
  void add_line(line_t& line);
  void add_line(point_t& p1, point_t& p2, int depth = 0);

  // turns the lines in this constructor into a cfg
  void transform(dbcfg_contour_fragment_graph<T>& cfg);

};



#endif dbcfg_cfg_constructor_h_
// end dbcfg_cfg_constructor.h

