// This is contrib/knee_cartilage/cmd/pt_set_correspondence

//:
// \file
// \brief A program to compute morphology of knee cartilage
// \date September 11, 2006


#include <iostream>
#include <vul/vul_timer.h>
#include <vnl/vnl_file_matrix.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>

//: 
int main( int argc, char **argv )
{
  if (argc < 3)
  {
    std::cerr << "Invalid parameters. Program syntax:\n";
    std::cerr << argv[0] << "input_meshfile output_mesh.ply\n";
    return 1;
  }

  // parse the arguments

  // mesh file to be transformed
  std::string input_mesh_file(argv[1]);
  std::string output_mesh_file(argv[2]);
  
  // i. Load the meshes
  dbmsh3d_mesh mesh;

  std::cout << "i. Load input mesh file: \n";
  vul_timer timer;
  timer.mark();

  // load the meshes
  std::cout << "Loading mesh ...";
  dbmsh3d_load(&mesh, input_mesh_file.c_str());

  std::cout << "done.\n";

  std::cout << "Total time taken = \n";
  timer.print(std::cout);

  

  std::cout << "Triangulating the mesh before saving...";
  dbmsh3d_mesh* tri_mesh = generate_tri_mesh(&mesh);

  //Save the mesh to ply format
  std::cout << "Saving the mesh in PLY format ... \n";
  dbmsh3d_save_ply(tri_mesh, output_mesh_file.c_str(), false);

  delete tri_mesh;
  std::cout << "Done.\n";
  return 0;
};


