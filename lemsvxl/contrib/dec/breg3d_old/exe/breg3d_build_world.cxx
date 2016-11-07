#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>

#include <vgl/vgl_point_3d.h>

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_double_3x4.h>

#include <vul/vul_timer.h>

#include <breg3d/breg3d_voxel_grid.h>
#include <breg3d/breg3d_util.h>

#include <vcl_iomanip.h>





int main(int argc, char* argv[])
{
  vgl_point_3d<double> grid_corner(2170.0, -1480.0, -30.0);
  const double vox_size = 0.5;


  breg3d_voxel_grid voxels("d:/dec/matlab/reg3d/data/seq2_voxels_cam50",300,300,120,grid_corner,vox_size);
  voxels.init_voxels();

  vcl_vector<vnl_double_3x3> Ks, Rs;
  vcl_vector<vnl_double_3x1> Ts;
  breg3d_util::read_cameras("D:/dec/matlab/reg3d/data/seq2_KRT_cameras.txt",Ks,Rs,Ts);

  // fill cameras with some data
  {
    unsigned c = 50;
  //for (unsigned i=0; i<100; i++) {
    //unsigned c = (i*23) % 100;
    // open image
    vcl_stringstream frame_fname;
    frame_fname << "d:/dec/matlab/reg3d/data/seq2/frame_" << vcl_setw(5) << vcl_setfill('0') << (int)c << ".png";
    vcl_cout << "loading " << frame_fname.str() << vcl_endl;
    vil_image_view<float> frame_view = breg3d_util::load_image(frame_fname.str());

    vnl_double_3x4 RT;
    RT.set_columns(0,Rs[c]);
    RT.set_columns(3,Ts[c]);

    vnl_matrix_fixed<double,3,4> cam_P = Ks[c]*RT;
    vpgl_proj_camera<double> camera_og(cam_P);

    vcl_cout << "updating voxels with camera " << c <<".";
    vul_timer timer;
    timer.mark();
    voxels.update_voxels(frame_view,&camera_og);
    double elapsed = timer.real();
    vcl_cout << " ..done." << vcl_endl;
    vcl_cout << " update took " << elapsed / 1000.0 << "seconds." << vcl_endl;
  }
  return 0;

}
