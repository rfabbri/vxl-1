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
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vul/vul_timer.h>

#include <breg3d/breg3d_voxel_grid.h>
#include <breg3d/breg3d_proj_camera_cost_function.h>
#include <breg3d/breg3d_util.h>

#include <vcl_iomanip.h>




int main(int argc, char* argv[])
{
  vgl_point_3d<double> grid_corner(2170.0, -1480.0, -30.0);
  const double vox_size = 0.5;


  breg3d_voxel_grid voxels("d:/dec/matlab/reg3d/data/seq2_voxels",300,300,120,grid_corner,vox_size);


  vcl_vector<vnl_double_3x3> Ks, Rs;
  vcl_vector<vnl_double_3x1> Ts;
  breg3d_util::read_cameras("D:/dec/matlab/reg3d/data/seq2_KRT_cameras.txt",Ks,Rs,Ts);

  // compute scores for cameras
  vnl_vector<double> startval(6);
  vnl_vector<double> inc(6);
  vnl_vector<double> nincs(6);

  startval.fill(-1.5);
  //inc.fill(0.25); 
  inc.fill(0.1);
  //nincs.fill(13);
  nincs.fill(31);

  // smaller increments for rodriguez vector components
  startval[0] = -0.015; 
  startval[1] = startval[0]; startval[2] = startval[0];
  //inc[0] = 0.00625;
  inc[0] = 0.001; 
  inc[1] = inc[0]; inc[2] = inc[0];


  vcl_vector<unsigned> camera_indices;
  //for (unsigned c=0; c < 100; c+=10)
  //  camera_indices.push_back(c);
 
  camera_indices.push_back(49);
  //camera_indices.push_back(90);
  //camera_indices.push_back(50);
  //camera_indices.push_back(40);

  for (unsigned c=0; c < camera_indices.size(); c++) {
    unsigned camera_idx = camera_indices[c];

    // write scores to a file
    vcl_stringstream outfile_fname;
    outfile_fname << "d:/dec/matlab/reg3d/data/camera_scores_xyz_" << camera_idx <<".txt";
    vcl_ofstream fout(outfile_fname.str().c_str());
    if (!fout.is_open()) {
      vcl_cerr << "ERROR opening file for write!" << vcl_endl;
      return -1;
    }

    // open image
    vcl_stringstream frame_fname;
    frame_fname << "d:/dec/matlab/reg3d/data/seq2/frame_" << vcl_setw(5) << vcl_setfill('0') << (int)camera_idx << ".png";
    vcl_cout << "loading " << frame_fname.str() << vcl_endl;
    vil_image_view<float> frame_view = breg3d_util::load_image(frame_fname.str());
    
    // perturb T
    vnl_double_3x1 Tinit(Ts[camera_idx]);
    Tinit[1][0] += 0.5;
    breg3d_proj_camera_cost_function costfun(voxels,frame_view,Ks[camera_idx],Rs[camera_idx],Tinit);

    for (unsigned dim = 0; dim < 6; dim++) {
      for (unsigned xi = 0; xi < nincs[dim]; xi++) {
        double offset = startval[dim] + inc[dim]*xi;


        vcl_cout << "computing score for camera with dim " << dim << " offset by " << offset << "..";
        vnl_vector_fixed<double,6> x;
        vnl_vector<double> fx(frame_view.size());
    
        costfun.pack_variables(Rs[camera_idx],Tinit,x);
        x[dim] += offset;

        vcl_cout << " x = < " << x << " > ";
        costfun.f(x,fx);

        double score = fx.squared_magnitude();
        fout << score << " ";
        vcl_cout << "..  score = " << score << vcl_endl;
      }
      fout << vcl_endl;
    }
    fout.close();
  }
  return 0;
}
