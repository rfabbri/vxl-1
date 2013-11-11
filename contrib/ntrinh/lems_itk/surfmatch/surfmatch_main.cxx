// \: file
// \brief This programs matches two surfaces using ICP and similarity transform
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 28, 2006


#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_file_matrix.h>
#include <vul/vul_file.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>


int mesh_vis(dbmsh3d_mesh& fixed_mesh, dbmsh3d_mesh& moving_mesh);
bool match_landmarks(
  const vcl_vector<vgl_point_3d<double > >& fixed_points, 
  const vcl_vector<vgl_point_3d<double > >& moving_points,
  vgl_h_matrix_3d<double >& transform_matrix);

bool surfmatch(dbmsh3d_mesh& fixed_mesh, 
               dbmsh3d_mesh& moving_mesh, 
               vgl_h_matrix_3d<double >& transform_matrix,
               const vgl_h_matrix_3d<double >& init_matrix,
               const vcl_string& report_file);

bool surfmatch2(dbmsh3d_mesh& fixed_mesh, 
               dbmsh3d_mesh& moving_mesh, 
               vgl_h_matrix_3d<double >& transform_matrix,
               const vgl_h_matrix_3d<double >& init_matrix,
               const vcl_string& report_file);

int main(int argc, char * argv[] )
{

  if( argc < 3 )
    {
    vcl_cerr << "Arguments Missing. " << std::endl;
    vcl_cerr
      << "Usage:  " << argv[0] << " fixed_mesh_file  moving_mesh_file fixed_landmarks moving_landmarks" 
      << std::endl;
    return 1;
    }

  // Read input files

  

  // ====== corresponding points ============
  vcl_cout << "Loading corresponding points .. " << vcl_endl;
  const char* fixed_landmarks_file = argv[3];
  const char* moving_landmarks_file = argv[4];


  vcl_vector<vgl_point_3d<double > > fixed_points;
  vcl_vector<vgl_point_3d<double > > moving_points;
  vgl_h_matrix_3d<double > init_transform_matrix;

  vnl_file_matrix<double > fixed_landmarks(fixed_landmarks_file);
  vnl_file_matrix<double > moving_landmarks(moving_landmarks_file);

  if (fixed_landmarks.cols() !=3 || moving_landmarks.cols() != 3 ||
    fixed_landmarks.rows() != moving_landmarks.rows())
  {
    vcl_cout << "Both landmarks file need to 3 columns and the same number of rows" << 
      vcl_endl;
    return 1;
  }

  //
  fixed_points.reserve(fixed_landmarks.rows());
  for (unsigned i=0; i < fixed_landmarks.rows(); ++i)
  {
    vgl_point_3d<double > pt(fixed_landmarks[i][0], 
      fixed_landmarks[i][1], 
      fixed_landmarks[i][2]);
    fixed_points.push_back(pt); 
  }

  moving_points.reserve(moving_landmarks.rows());
  for (unsigned i=0; i < moving_landmarks.rows(); ++i)
  {
    vgl_point_3d<double > pt(moving_landmarks[i][0], 
      moving_landmarks[i][1], 
      moving_landmarks[i][2]);
    moving_points.push_back(pt); 
  }

  match_landmarks(fixed_points, moving_points, init_transform_matrix);


  // === fixed points ===========

  vcl_cout << "Loading fixed mesh file ..." << vcl_endl;
  const char* fixed_file = argv[1];
  dbmsh3d_mesh fixed_mesh;
  if (!dbmsh3d_load_ply(&fixed_mesh, fixed_file))
  {
    vcl_cout << "Error reading fixed mesh file" << vcl_endl;
  }

  // === moving points ====

  vcl_cout << "Loading moving mesh file ..." << vcl_endl;
  const char* moving_file = argv[2];
  dbmsh3d_mesh moving_mesh;

  if (!dbmsh3d_load_ply(&moving_mesh, moving_file))
  {
    vcl_cout << "Error reading fixed mesh file" << vcl_endl;
  }

  // // perform transformation on the moving mesh

  //// traverse thru all vertices
  //
  //for (vcl_map<int, dbmsh3d_vertex*>::iterator vit = moving_mesh.vertexmap().begin();
  //  vit != moving_mesh.vertexmap().end(); ++vit) 
  //{
  //  dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
  //  vgl_homg_point_3d<double > pt(v->get_pt());
  //  vgl_homg_point_3d<double > tp = init_transform_matrix(pt);
  //  assert(tp.w() != 0);
  //  double vx, vy, vz;
  //  tp.get_nonhomogeneous(vx, vy, vz);
  //  v->get_pt().set(vx, vy, vz);
  //}
  //

  vcl_string report_file = vul_file::dirname(fixed_file) + 
    "/reg_" +
    vul_file::strip_extension(vul_file::strip_directory(fixed_file)) + 
    "_" +
    vul_file::strip_extension(vul_file::strip_directory(moving_file)) +
    "_affine_report.txt";
  vgl_h_matrix_3d<double > transform_matrix;
  //
  surfmatch2(fixed_mesh, 
    moving_mesh, 
    transform_matrix, 
    init_transform_matrix, 
    report_file);

  //vcl_string out_mesh_file = vul_file::strip_extension(moving_file) + "_transformed.ply";
  //dbmsh3d_save_ply(&moving_mesh, out_mesh_file.c_str(), false);

  ////
  //vcl_ofstream out_file("output.txt", vcl_ios::out);
  //out_file << transform_matrix;
  //out_file.close();


  //mesh_vis(fixed_mesh, moving_mesh);
  

  return 0;

}
