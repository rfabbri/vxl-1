#ifndef btpl_imd_parser_h_
#define btpl_imd_parser_h_

#include <iostream>
#include <string>
#include <vector>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>

// Class to parse a IMD file.
class imd_parser
{
public:

  imd_parser( std::string file_name );

  // Image bounds in lat/lon.
  std::vector< vgl_point_2d<double> > region;

  // Light and camera directions.
  vgl_vector_3d<double> camera_dir;
  vgl_vector_3d<double> sun_dir;
};


#endif // btpl_imd_parser_h_
