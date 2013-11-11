// This is dbsks/dbsks_io_circ_arc_grid.cxx

//:
// \file

#include "dbsks_io_circ_arc_grid.h"
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>



//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const dbsks_circ_arc_grid& arc_grid)
{
  // 1. save version number
  const short version = 1;
  vsl_b_write(os, version);

  // 2. save the data in the grid
  
  // x
  vsl_b_write(os, arc_grid.min_x_);
  vsl_b_write(os, arc_grid.max_x_);
  vsl_b_write(os, arc_grid.num_x_);
  vsl_b_write(os, arc_grid.step_x_);
  vsl_b_write(os, arc_grid.x_);

  // y
  vsl_b_write(os, arc_grid.min_y_);
  vsl_b_write(os, arc_grid.max_y_);
  vsl_b_write(os, arc_grid.num_y_);
  vsl_b_write(os, arc_grid.step_y_);
  vsl_b_write(os, arc_grid.y_);

  // theta - it should cover all angles [0, 2pi]
  vsl_b_write(os, arc_grid.min_theta_);
  vsl_b_write(os, arc_grid.max_theta_);
  vsl_b_write(os, arc_grid.num_theta_);
  vsl_b_write(os, arc_grid.step_theta_);
  vsl_b_write(os, arc_grid.theta_);
  
  // chord length (width of the bounding box)
  vsl_b_write(os, arc_grid.min_chord_);
  vsl_b_write(os, arc_grid.max_chord_);
  vsl_b_write(os, arc_grid.num_chord_);
  vsl_b_write(os, arc_grid.step_chord_);
  vsl_b_write(os, arc_grid.chord_);

  // height of the bounding box of the arc
  vsl_b_write(os, arc_grid.min_height_);
  vsl_b_write(os, arc_grid.max_height_);
  vsl_b_write(os, arc_grid.num_height_);
  vsl_b_write(os, arc_grid.step_height_);
  vsl_b_write(os, arc_grid.height_);

  return;
}



//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, dbsks_circ_arc_grid& arc_grid)
{  
  if (!is) return;

  // read version number
  short version;
  vsl_b_read(is, version);

  switch (version)
  {
  case 1:

    vsl_b_read(is, arc_grid.min_x_);
    vsl_b_read(is, arc_grid.max_x_);
    vsl_b_read(is, arc_grid.num_x_);
    vsl_b_read(is, arc_grid.step_x_);
    vsl_b_read(is, arc_grid.x_);

    // y
    vsl_b_read(is, arc_grid.min_y_);
    vsl_b_read(is, arc_grid.max_y_);
    vsl_b_read(is, arc_grid.num_y_);
    vsl_b_read(is, arc_grid.step_y_);
    vsl_b_read(is, arc_grid.y_);

    // theta - it should cover all angles [0, 2pi]
    vsl_b_read(is, arc_grid.min_theta_);
    vsl_b_read(is, arc_grid.max_theta_);
    vsl_b_read(is, arc_grid.num_theta_);
    vsl_b_read(is, arc_grid.step_theta_);
    vsl_b_read(is, arc_grid.theta_);
    
    // chord length (width of the bounding box)
    vsl_b_read(is, arc_grid.min_chord_);
    vsl_b_read(is, arc_grid.max_chord_);
    vsl_b_read(is, arc_grid.num_chord_);
    vsl_b_read(is, arc_grid.step_chord_);
    vsl_b_read(is, arc_grid.chord_);

    // height of the bounding box of the arc
    vsl_b_read(is, arc_grid.min_height_);
    vsl_b_read(is, arc_grid.max_height_);
    vsl_b_read(is, arc_grid.num_height_);
    vsl_b_read(is, arc_grid.step_height_);
    vsl_b_read(is, arc_grid.height_);

    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const dbsks_circ_arc_grid& arc_grid) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  return;
}




