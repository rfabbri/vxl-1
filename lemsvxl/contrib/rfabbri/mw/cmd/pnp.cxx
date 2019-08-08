/*
 * App to run p3p algo
 *
 * test it by going to pnp-test-input/simplest-case and calling pnp executable
 *
 */
#include <vul/vul_arg.h>
#include <buld/buld_arg.h>
#include <vul/vul_file.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <vector>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vul/vul_awk.h>
#include <iomanip>


#include <openMVG/numeric/eigen_alias_definition.hpp>


#if 0

static constexpr unsigned NPTS = 2;

double xy2D_[NPTS][2];//  = {
//};

double xyz3D_[NPTS][3]// {
// };

bool stdio_=true;  // by default read/write from stdio



// reads into the global variables xy2D_ and xyz3D
// 
// Format is just n rows vs 2 or 3 columns, concatenated into a single txt
//
// x0 y0
// x1 y1
// x2 y2
//  ...
// X0 Y0 Z0
// X1 Y1 Z1
//  ...
// 
// The file can also be one line, listing the above in row-major order like so:
//
// x0
// y0
// x1
// y1
// ...
// X0
// Y0
// Z0
// ...
// 
// This format is generic enough to be adapted to/from matlab
template <typename F=double>
static bool
mread(const char *fname)
{
  std::ifstream infp;
  std::istream *inp = &std::cin;
  
  if (!stdio_) {
    infp.open(fname, std::ios::in);
    if (!infp) {
      std::cerr << "I/O Error opening input " << fname << std::endl;
      return false;
    }
    inp = &infp;
  }
  
  std::istream &in = *inp;
    
  in.exceptions(std::istream::failbit | std::istream::badbit);
  unsigned i=0;
  F *dp = (F *)xy2D_;
  while (!in.eof() && dp != (F *)xy2D_+ NPTS*2) {
      try {
      in >> *dp++;
      // std::cerr << "reading " <<  *(dp-1) << std::endl;;
      if (in.eof()) {
        std::cerr << "I/O Error: Premature input termination\n";
        return false;
      }
      in >> *dp++;
      } catch (std::istream::failure &E) {
        std::cerr << "I/O Error: Invalid input conversion or other error\n";
        return false;
      }
  }
  if (dp != (F *)params_+ NPTS*2)
    std::cerr << "I/O Premature input termination\n";

//  for (unsigned i=0; i < 2*NPARAMS; ++i)
//    std::cerr << "D " << params_[i] << std::endl;

  return true;
}

*/

#endif



namespace openMVG
{
namespace euclidean_resection
{
// defined in solver_resection_p3p_nordberg
bool computePosesNordberg(
    const Mat &bearing_vectors,
    const Mat &X,
    std::vector<std::tuple<Mat3, Vec3>> &rotation_translation_solutions);
}
}


using namespace openMVG;

int 
main(int argc, char **argv)
{
  std::string world_points_file( "world_pts.txt" );
  std::string image_points_file( "image_pts.txt" );
  std::string RT_out_file_prefix( "camera_RT-" ); // rot and transl

  // Input ---------------------------------------------------------------------
  // 
  // Read the world points. Not very efficient, just prototyping.
  // 
  std::vector<vgl_point_3d<double> > world_points;
  {
    std::ifstream wpsifs( world_points_file.c_str() );
    if (!wpsifs) {
      std::cerr << " Error opening file  " << world_points_file << std::endl;
      return 1;
    }
    vul_awk wps( wpsifs );
    while( wps ){
      world_points.push_back( vgl_point_3d<double>(
        atof(wps[0]), atof(wps[1]), atof(wps[2]) ) );
      ++wps;
    }
  }
  unsigned num_world_points = world_points.size();
  assert(num_world_points == 3);

  // Read the image_points
  std::vector<vgl_point_2d<double> > image_points;
  {
    std::ifstream ipsifs( image_points_file.c_str() );
    if (!ipsifs) {
      std::cerr << " Error opening file  " << world_points_file << std::endl;
      return 1;
    }
    vul_awk ips( ipsifs );
    for( unsigned wp = 0; wp < 3; wp++ ){
      if( ips.NF() == 0 ){ ++ips; wp--; continue; }
      vgl_point_2d<double> new_ip( atof(ips[0]), atof(ips[1]) );
      image_points.push_back( new_ip );
      ++ips;
    }
  }

  // Call P3P ------------------------------------------------------------------

  // Translate to P3P structs

  // each column is a point
  Mat pts2D(3,3), pts3D(3,3);
  for (unsigned c=0; c < 3; ++c) {
    pts2D(0,c) = image_points[c].x();
    pts2D(1,c) = image_points[c].y();
    pts2D(2,c) = 1;
    pts3D(0,c) = world_points[c].x();
    pts3D(1,c) = world_points[c].y();
    pts3D(2,c) = world_points[c].z();
    std::cerr << "image:\n " << image_points[c] << "\n world:" << world_points[c] << std::endl;
  }
  
  std::vector<std::tuple<Mat3, Vec3>> rt_sols;
  euclidean_resection::computePosesNordberg(pts2D, pts3D, rt_sols);

  // Output --------------------------------------------------------------------

  std::cerr << "Number of solutions: " <<  rt_sols.size() << std::endl;
  for (unsigned s=0; s < rt_sols.size(); ++s)  {
    std::string fname = RT_out_file_prefix + std::to_string(s);
    std::ofstream c_rc_ofs(fname.c_str());
    c_rc_ofs << std::setprecision(20);
    c_rc_ofs << std::get<0>(rt_sols[s]) << std::endl << std::get<1>(rt_sols[s]);
  }
  
  return 0;
}
