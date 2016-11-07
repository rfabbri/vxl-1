// This is dbknee/pro/dbknee_separate_inner_outer_surfaces_process.cxx

//:
// \file

#include "dbknee_separate_inner_outer_surfaces_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbknee/dbknee_coord.h>

//: Constructor
dbknee_separate_inner_outer_surfaces_process::
dbknee_separate_inner_outer_surfaces_process()
{
  if( !parameters()->add( "Cartilage mesh <*.ply>" , 
    "-data_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Cylinder CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Inner surface mesh file <inner.ply>" , 
    "-inner_mesh_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Outer surface mesh file <outer.ply>" , 
    "-outer_mesh_file", bpro1_filepath("",".txt"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_separate_inner_outer_surfaces_process::
~dbknee_separate_inner_outer_surfaces_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_separate_inner_outer_surfaces_process::
clone() const
{
  return new dbknee_separate_inner_outer_surfaces_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_separate_inner_outer_surfaces_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_separate_inner_outer_surfaces_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_separate_inner_outer_surfaces_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_separate_inner_outer_surfaces_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_separate_inner_outer_surfaces_process::
execute()
{
  bpro1_filepath data_file;
  parameters()->get_value( "-data_file" , data_file );

  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );


  bpro1_filepath inner_mesh_file;
  parameters()->get_value( "-inner_mesh_file" , inner_mesh_file );

  bpro1_filepath outer_mesh_file;
  parameters()->get_value( "-outer_mesh_file" , outer_mesh_file );

  this->separate_inner_outer_surface_using_cylinder_cs(
    data_file.path, cs_file.path, 
    inner_mesh_file.path, outer_mesh_file.path, false);
 
  return true;
}

bool dbknee_separate_inner_outer_surfaces_process::
finish()
{
  return true;
}


//: Take in the data (mesh file, cs_file) and execute
bool dbknee_separate_inner_outer_surfaces_process::
separate_inner_outer_surface_using_cylinder_cs(
  const vcl_string& data_file,
  const vcl_string& cs_file,
  const vcl_string& inner_mesh_file,
  const vcl_string& outer_mesh_file,
  bool show_gui)
{
  dbmsh3d_mesh inner_mesh;
  dbmsh3d_mesh outer_mesh;
  dbmsh3d_mesh mesh;

  dbmsh3d_load_ply(&mesh, data_file.c_str());
  mesh.print_summary(vcl_cout);

  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);
  vgl_point_3d<double > axis_origin = coord.origin();
  vgl_vector_3d<double > axis_dir = coord.z_axis();

  vcl_cout << "Separating the cartilage into inner and outer meshes.\n";
  dbknee_separate_inner_outer_surfaces(mesh, 
                                        axis_origin,
                                        axis_dir,
                                        inner_mesh,
                                        outer_mesh);

  vcl_cout << "Writing inner and outer meshes to files.\n";
  dbmsh3d_save_ply(&inner_mesh, inner_mesh_file.c_str(), false);
  dbmsh3d_save_ply(&outer_mesh, outer_mesh_file.c_str(), false);
  return false;
}




