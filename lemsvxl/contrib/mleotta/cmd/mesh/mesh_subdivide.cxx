// This is mleotta/cmd/mesh/mesh_subdivide.cxx


#include <iostream>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_operations.h>

#include <imesh/algo/imesh_operations.h>


// The Main Function
int main(int argc, char** argv)
{
  vul_arg<std::string>  a_in_file("-i", "input mesh file", "");
  vul_arg<std::string>  a_out_file("-o", "output mesh file", "");
  vul_arg<std::string>  a_group("-g", "group name", "");
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
  std::string in_ext = vul_file::extension(a_in_file());
  if(in_ext == ".ply2")
    imesh_read_ply2(a_in_file(),mesh);
  else if(in_ext == ".obj")
    imesh_read_obj(a_in_file(),mesh);
  else{
    std::cerr << "unknown input file extension: " << in_ext<< std::endl;
    return -1;
  }

#if 0
  mesh.build_edge_graph();

  mesh.compute_vertex_normals_from_faces();
  imesh_mesh dmesh = dual_mesh(mesh);
  dmesh.build_edge_graph();
  imesh_mesh dmesh2 = dual_mesh_with_normals(dmesh,mesh.vertices());
  imesh_write_obj(a_out_file(), dmesh2);
  return 0;
#endif

  if(a_group.set())
    imesh_quad_subdivide(mesh,mesh.faces().group_face_set(a_group()));
  else
    imesh_quad_subdivide(mesh);

  //std::auto_ptr<imesh_face_array_base> tris(imesh_triangulate(mesh.faces()));
  //mesh.set_faces(tris);

  imesh_write_obj(a_out_file(), mesh);

  return 0;
}
