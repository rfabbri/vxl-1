//:
// \file


#include <vector>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_inverse.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <iostream>
#include <fstream>


// Based on code originally written by Kongbin Kang (@Brown.edu) from 
// lems/bmvgd/brct/brct_algos.h
bool read_target_corrs(std::ifstream& str,
                  std::vector<bool>& valid,
                  std::vector<vgl_point_2d<double> >& image_points,
                  std::vector<vgl_point_3d<double> >& world_points)
{
  std::string temp;
  str >> temp;
  if (temp != "NUMPOINTS:")
    return false;
  int n_corrs;
  str >> n_corrs;
  for (int i = 0; i<n_corrs; i++)
  {
    str >> temp;
    if (temp != "CORRESP:")
      return false;
    bool val;
    int junk;
    vgl_point_2d<double> image_point;
    vgl_point_3d<double> world_point;
    str >> val >> junk
        >> world_point >> image_point;
    std::cout << "W " << world_point << "  I " << image_point << '\n';
    valid.push_back(val);
    image_points.push_back(image_point);
    world_points.push_back(world_point);
  }
  return true;
}


int main(int argc, char** argv)
{
  if(argc != 3){
    std::cout << "Usage: "<< argv[0] << " init_cam_file correspondence_file"<< std::endl;
    return -1;
  }

  //=========================== load the files ==========================
  std::ifstream istr(argv[1]);   
  vnl_double_3x4 cam_matrix;
  istr >> cam_matrix;
  if(istr.fail()){
    std::cerr << "Failed to read camera" << std::endl;
    return -1;
  }
  istr.close();

  istr.open(argv[2]);   
  std::vector<bool> valid;
  std::vector<vgl_point_2d<double> > i_pts;
  std::vector<vgl_point_3d<double> > w_pts;
  if(!read_target_corrs(istr, valid, i_pts, w_pts) )
  {
    std::cout << "Failed to read correspondences" << std::endl;
    return -1;
  }
  istr.close();


  std::vector<vgl_point_2d<double> > image_pts;
  std::vector<vgl_homg_point_3d<double> > world_pts;
  for( unsigned int i=0; i<valid.size(); ++i ){
    if(valid[i]){
      world_pts.push_back(vgl_homg_point_3d<double>(w_pts[i]));
      image_pts.push_back(i_pts[i]);
    }
  }

  //===========================  ==========================

  std::cout << "======== Initial Camera Matrix ========="<<std::endl;
  std::cout << cam_matrix  << std::endl;
  
  vpgl_perspective_camera<double> cam;
  if(!vpgl_perspective_decomposition(cam_matrix,cam)){
    std::cerr << "Could not decompose matrix" << std::endl;
  }

  // This is the known internal calibration calibration
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  cam.set_calibration(K);

  vpgl_perspective_camera<double> opt_cam = vpgl_optimize_camera::opt_orient_pos(cam,world_pts,image_pts);

  std::cout << "======== Original Camera Parameters ========="<<std::endl;
  std::cout << cam << std::endl;
  std::cout << "======== Optimized Camera Parameters ========="<<std::endl;
  std::cout << opt_cam << std::endl;

  std::cout << "======== Optimized Camera Matrix ========="<<std::endl;
  std::cout << opt_cam.get_matrix() << std::endl;
  return 0;
}

