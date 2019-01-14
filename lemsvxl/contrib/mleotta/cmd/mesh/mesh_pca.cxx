// This is mleotta/cmd/mesh/mesh_pca.cxx


#include <iostream>
#include <fstream>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vnl/vnl_matrix.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_pca.h>



// The Main Function
int main(int argc, char** argv)
{
  vul_arg<std::string>  a_in_file("-i", "input file list", "");
  vul_arg<std::string>  a_mean_mesh_file("-o", "mean mesh file", "");
  vul_arg<std::string>  a_pca_file("-p", "pca file", "");
  vul_arg<std::string>  a_proj_file("-j", "projection file", "");
  vul_arg_parse(argc, argv);

  if(!a_in_file.set()){
    std::cerr << "input file required" << std::endl;
    return -1;
  }
  if(!a_mean_mesh_file.set()){
    std::cerr << "output file required" << std::endl;
    return -1;
  }


  std::vector<imesh_mesh> meshes;
  std::ifstream ifs(a_in_file().c_str());
  std::string fname;
  while(ifs >> fname){
    std::cout << "loading: " << fname << std::endl;
    imesh_mesh model_mesh;
    imesh_read_obj(fname,model_mesh);
    meshes.push_back(model_mesh);
  }
  ifs.close();

  imesh_pca_mesh pca_vehicle(meshes);
  
  imesh_write_pca(a_mean_mesh_file(),
                  a_pca_file(),
                  pca_vehicle);
  
  if(a_proj_file.set()){
    std::ofstream ofs(a_proj_file().c_str());
    for(unsigned int i=0; i<meshes.size(); ++i){
      vnl_vector<double> v = pca_vehicle.project(meshes[i].vertices());
      ofs << v<<std::endl;
    }
    ofs.close();
  }


  return 0;
}
