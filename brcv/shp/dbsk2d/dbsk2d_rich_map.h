// This is brcv/shp/dbsk2d/dbsk2d_rich_map.h
#ifndef dbsk2d_rich_map_h_
#define dbsk2d_rich_map_h_
//:
// \file
// \brief A class to map image pixels (grid points) to curves and shocks 
// \author Amir Tamrakar
// \date 07/20/05
//
// This structure is created post shock computation.
// It uses the shocks to extract a rich variety of information from the input
// image and geometry.
//
// Some examples are :
//      1. curve_map: pointers to the nearest curve
//      2. Neighboring curve map: poiters to the neighbor of the nearest curve
//      3. distance map: distance from the nearest curves
//      4. visual fragment map: pointers to the nearest shock fragment
//      5. local coordinates wrt the fragment 
//
// 
// \verbatim
//  Modifications
//
//  Amir Tamrakar 07/20/05  Adapted this class from dbspi_curve_map
//
// \endverbatim

#include <vcl_utility.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>
#include <dbsk2d/dbsk2d_shock_fragment.h>
#include <dbsk2d/dbsk2d_shock_fragment_sptr.h>


//: This class stores 2d arrays of various information that could be of interest.
//  Hence the name.
//
// Some examples are :
//      1. curve_map: pointers to the nearest curve
//      2. Neighboring curve map: poiters to the neighbor of the nearest curve
//      3. distance map: distance from the nearest curves
//      4. visual fragment map: pointers to the nearest shock fragment
//      5. local coordinates wrt the fragment 
//
class dbsk2d_rich_map : public vbl_ref_count
{
public: 

  //: curve map element
  //
  //contains the pointer to a curve and the arclength 
  //on it to specify the exact point.
  class curve_map_element
  {
  public:
    dbsk2d_bnd_contour_sptr contour;
    double s;

    //: Default Constructor
    curve_map_element() : contour(0), s(-1) {}

    //: Constructor
    curve_map_element(dbsk2d_bnd_contour_sptr curve, double cur_s)
      : contour(curve), s(cur_s) {}

    //: Convert to bool
    operator bool() const{ return (contour != NULL); }
  };

  //: shock map element
  //
  //contains the pointer to a shock fragment and its local intrinsic coordinates 
  class shock_map_element
  {
  public:
    dbsk2d_shock_fragment_sptr fragment;
    double psi;
    double t;

    //: Default Constructor
    shock_map_element() : fragment(0), psi(-1), t(-1) {}

    //: Constructor
    shock_map_element(dbsk2d_shock_fragment_sptr frag, double cur_psi, double cur_t): 
      fragment(frag), psi(cur_psi), t(cur_t) {}

    //: Convert to bool
    operator bool() const{ return (fragment != NULL); }
  };

protected:

  //: offset in x and y where the grid starts 
  int x_offset_, y_offset_;

  //: The size of the grids
  unsigned int width_, height_;

  //: The distance map
  vbl_array_2d<float> distance_map_;

  //: The curve map
  vbl_array_2d<curve_map_element> curve_map_;

  //: The shock map
  vbl_array_2d<shock_map_element> shock_map_;

public:

  //: Constructor
  dbsk2d_rich_map(const dbsk2d_shock_graph_sptr shock_graph,
                  unsigned int width, unsigned int height,
                  int x_offset=0, int y_offset=0);

  //: Constructor: the best grid size is determined from the shock graph
  dbsk2d_rich_map(const dbsk2d_shock_graph_sptr shock_graph);

  //: Destructor
  virtual ~dbsk2d_rich_map();

  //: Return the width of the rich map
  unsigned int width() const { return width_; }

  //: Return the height of the distance map
  unsigned int height() const { return height_; }

  //: Return the offset in x where the grid starts
  int x_offset() const { return x_offset_; }  

  //: Return the offset in y where the grid starts
  int y_offset() const { return y_offset_; }

  //: Return the distance to the nearest curve at (x,y)
  float distance(int x, int y) const;

  //: Return a pointer to the closest curve at (x,y)
  dbsk2d_bnd_contour_sptr closest_curve(int x, int y) const;

  //: Return a pointer to the shock_fragment at (x,y)
  dbsk2d_shock_fragment_sptr shock_fragment(int x, int y) const;

  //: traverse the shock graph to fill the map
  void fill_the_map(const dbsk2d_shock_graph_sptr shock_graph);
  
  //: fill a shock fragment by traversing through its pixels
  void fill_a_shock_fragment(dbsk2d_shock_fragment_sptr fragment);
  
};

#endif // dbsk2d_rich_map_h_
