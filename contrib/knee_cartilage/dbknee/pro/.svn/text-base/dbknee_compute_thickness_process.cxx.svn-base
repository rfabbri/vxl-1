// This is dbknee/pro/dbknee_compute_thickness_process.cxx

//:
// \file

#include "dbknee_compute_thickness_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbknee/dbknee_coord.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>

//: Constructor
dbknee_compute_thickness_process::
dbknee_compute_thickness_process()
{
  if( !parameters()->add( "Inner surface mesh file <inner.ply>" , 
    "-inner_mesh_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Outer surface mesh file <outer.ply>" , 
    "-outer_mesh_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Thickness-encoded output mesh <mesh_w_thickness.ply>" , 
    "-output_file", bpro1_filepath("",".ply"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_compute_thickness_process::
~dbknee_compute_thickness_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_compute_thickness_process::
clone() const
{
  return new dbknee_compute_thickness_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_compute_thickness_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_compute_thickness_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_compute_thickness_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_compute_thickness_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_compute_thickness_process::
execute()
{
  bpro1_filepath inner_mesh_file;
  parameters()->get_value( "-inner_mesh_file" , inner_mesh_file );

  bpro1_filepath outer_mesh_file;
  parameters()->get_value( "-outer_mesh_file" , outer_mesh_file );

  bpro1_filepath output_file;
  parameters()->get_value( "-output_file" , output_file );

  dbknee_compute_thickness_process::compute_thickness(
    inner_mesh_file.path, outer_mesh_file.path, output_file.path, false);
 
  return true;
}

bool dbknee_compute_thickness_process::
finish()
{
  return true;
}


//: Take in the data and execute
bool dbknee_compute_thickness_process::
compute_thickness(const vcl_string& inner_mesh_file,
                  const vcl_string& outer_mesh_file, 
                  const vcl_string& thickness_mesh_file,
                  bool show_gui)
{
  // i. Load inner mesh
  vcl_cout << "\nLoad mesh file = " << inner_mesh_file << vcl_endl;
  dbmsh3d_richmesh inner_mesh;

  // list of properties to load
  vcl_vector<vcl_string > vertex_prop_list;
  vertex_prop_list.clear();
  

  vcl_vector<vcl_string > face_prop_list;
  face_prop_list.clear();
  //////////////////////////////
  dbmsh3d_load_ply(&inner_mesh, inner_mesh_file.c_str(), vertex_prop_list, face_prop_list);
  ///////////////////////////////

  remove_erroneous_Fs_IFS(&inner_mesh);

  // ii. Load outer mesh
  vcl_cout << "\n Load outer mesh file = " << outer_mesh_file << vcl_endl;
  dbmsh3d_mesh outer_mesh;
  dbmsh3d_load_ply(&outer_mesh, outer_mesh_file.c_str());
  remove_erroneous_Fs_IFS(&outer_mesh);

  outer_mesh.IFS_to_MHE();
  outer_mesh.build_IFS_mesh();

  //outer_mesh.

  // iii. Compute distance map between the two meshes
  dbmsh3d_mesh_algos::mesh2mesh_distance_map(inner_mesh, outer_mesh);

  // iv. Save results
  vertex_prop_list.push_back("verror_abs");
  dbmsh3d_save_ply(&inner_mesh, thickness_mesh_file.c_str(), 
    vertex_prop_list, face_prop_list, BINARY);

  vcl_cout << "Done.\n";
  return false;

}



