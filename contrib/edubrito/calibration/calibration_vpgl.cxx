


#include <brct/brct_algos.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl\algo\vpgl_camera_compute.h>



int main(int argc, char *argv[])
{

  if(argc != 3){
    vcl_cout << "Usage: "<< argv[0] << " correspondence_file output_file"<< vcl_endl;
    return -1;
  }

  vcl_cout<<"\nCalibration: ";
  vcl_vector<vgl_point_3d<double> > world_points;

  // vcl_ifstream coors_file( "C:\\Documents and Settings\\Edu\\Desktop\\Research\\calibration\\COORS BASE - DSCN0017 - left1.txt" , vcl_ifstream::in );
  vcl_ifstream coors_file( argv[1] , vcl_ifstream::in );
  vcl_vector<bool> valid;
  vcl_vector<vgl_point_2d<double> > image_points;
  brct_algos::read_target_corrs(coors_file, valid, image_points, world_points);

  vcl_vector<vgl_point_2d<double> > image_pts;
  vcl_vector<vgl_point_3d<double> > world_pts;

  for(int i=0;i<valid.size();i++)
  {
    if(valid[i] == true)
    {
      image_pts.push_back(image_points[i]);
      world_pts.push_back(world_points[i]);
    }
  }

  //vnl_matrix_fixed<double,3,3> k;
  //k[0][0] = 2124; k[0][1] = 0; k[0][2] = 1280;
  //k[1][0] = 0; k[1][1] = 2124; k[1][2] = 960;
  //k[2][0] = 0; k[2][1] = 0; k[2][2] = 1;
  //vpgl_calibration_matrix<double> K(k); //vpgl_calibration_matrix( const vnl_matrix_fixed<T,3,3>& K );

  double f(7.1);
  vgl_point_2d<double> pp(1280,960);
  double sx(1/0.003342039911376);
  double sy(1/0.003342039911376);
  double skew(0);


  vpgl_calibration_matrix<double> K(f,pp,sx,sy,skew); 
  vnl_matrix_fixed<double,3,3> erase; erase=K.get_matrix(); erase.print(vcl_cout);

  vpgl_perspective_camera<double> camera; 
  vpgl_perspective_camera_compute::compute( image_pts, world_pts, K, camera );

  vpgl_proj_camera<double> camera2;
  vpgl_proj_camera_compute::compute( image_pts,world_pts, camera2 );

  //vcl_ofstream of("C:\\Documents and Settings\\Edu\\Desktop\\Research\\calibration\\Pers and Prof cameras.txt",vcl_ofstream::out);
  vcl_ofstream of(argv[2],vcl_ofstream::out);

  camera.print_summary(vcl_cout);
  //camera.print_summary(of);
  camera2.print_summary(of);


  of.close();
  coors_file.close();

  return 0; 
}
