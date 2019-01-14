// This is mleotta/cmd/mesh/mesh_pca.cxx


#include <iostream>
#include <fstream>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <modrec/modrec_pca_vehicle.h>
#include <modrec/modrec_vehicle_parts.h>



// The Main Function
int main(int argc, char** argv)
{
  vul_arg<std::string>  a_in_parts_file("-ip", "input parts file list", "");
  vul_arg<std::string>  a_in_mesh_file("-im", "input mesh file list", "");
  vul_arg<std::string>  a_mean_parts_file("-op", "mean parts file", "");
  vul_arg<std::string>  a_mean_mesh_file("-om", "mean mesh file", "");
  vul_arg<std::string>  a_pca_file("-p", "pca file", "");
  vul_arg<std::string>  a_proj_file("-j", "projection file", "");
  vul_arg_parse(argc, argv);

  if(!a_in_parts_file.set()){
    std::cerr << "input parts file required" << std::endl;
    return -1;
  }
  if(!a_in_mesh_file.set()){
    std::cerr << "input mesh file required" << std::endl;
    return -1;
  }
  if(!a_pca_file.set()){
    std::cerr << "output pca file required" << std::endl;
    return -1;
  }

  typedef std::map<std::string, vgl_polygon<double> > pmap;
  std::vector<pmap> part_groups;
  
  // make hubcaps which are common to all vehicles
  vgl_polygon<double> h1(1),h2(1),h3(1),h4(1);
  for(unsigned i=0; i<16; ++i){
    double s = std::sin(vnl_math::pi*i/8.0)/16;
    double c = std::cos(vnl_math::pi*i/8.0)/16;
    h1.push_back(.875-s,.875+c);
    h2.push_back(.875+s,.125-c);
    h3.push_back(.125-s,.875+c);
    h4.push_back(.125+s,.125-c);
  }

  {
    std::ifstream ifs(a_in_parts_file().c_str());
    std::string fname;
    while(ifs >> fname){
      std::cout << "loading: " << fname << std::endl;
      pmap parts = modrec_read_vehicle_parts(fname);
      parts["hubcap1"] = h1;
      parts["hubcap2"] = h2;
      parts["hubcap3"] = h3;
      parts["hubcap4"] = h4;
      part_groups.push_back(parts);
    }
    ifs.close();
  }

  std::vector<imesh_mesh> meshes;
  {
    std::ifstream ifs(a_in_mesh_file().c_str());
    std::string fname;
    while(ifs >> fname){
      std::cout << "loading: " << fname << std::endl;
      imesh_mesh model_mesh;
      imesh_read_obj(fname,model_mesh);
      meshes.push_back(model_mesh);
    }
    ifs.close();
  }

  modrec_pca_vehicle pca_vehicle(meshes,part_groups);
  
  modrec_write_pca_vehicle(a_mean_mesh_file(),
                           a_mean_parts_file(),
                           a_pca_file(),
                           pca_vehicle);
  
  if(a_proj_file.set()){
    std::ofstream ofs(a_proj_file().c_str());
    for(unsigned int i=0; i<meshes.size(); ++i){
      vnl_vector<double> v = pca_vehicle.project(meshes[i].vertices(),part_groups[i]);
      ofs << v<<std::endl;
    }
    ofs.close();
  }

  return 0;
}
