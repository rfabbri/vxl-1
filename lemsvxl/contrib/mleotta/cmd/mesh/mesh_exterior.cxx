// This is mleotta/cmd/mesh/mesh_exterior.cxx


#include <iostream>
#include <fstream>
#include <vul/vul_arg.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_operations.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_detect.h>



// The Main Function
int main(int argc, char** argv)
{
  vul_arg<std::string>  a_in_file("-i", "input mesh_file", "");
  vul_arg<std::string>  a_out_file("-o", "output mesh file", "");
  vul_arg_parse(argc, argv);

  if(!a_in_file.set()){
    std::cerr << "input file required" << std::endl;
    return -1;
  }
  if(!a_out_file.set()){
    std::cerr << "output file required" << std::endl;
    return -1;
  }

  imesh_mesh mesh;
  imesh_read_obj(a_in_file(),mesh);


  std::set<unsigned int> ext_frontfaces;
  std::set<unsigned int> ext_backfaces;
  std::set<unsigned int> ext_bifaces;
  imesh_detect_exterior_faces(mesh,ext_frontfaces, ext_backfaces, ext_bifaces);
  std::cout << "exterior:\n  front: "<<ext_frontfaces.size()
           <<"\n  back: "<<ext_backfaces.size()
           <<"\n  both: "<<ext_bifaces.size()<<std::endl;

  imesh_flip_faces(mesh, ext_backfaces);
  ext_frontfaces.insert(ext_backfaces.begin(),ext_backfaces.end());

  ext_frontfaces.insert(ext_bifaces.begin(),ext_bifaces.end());



  imesh_mesh ext_mesh = imesh_submesh_from_faces(mesh,ext_frontfaces);

  imesh_write_obj(a_out_file(), ext_mesh);




  return 0;
}
