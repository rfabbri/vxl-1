// This is dbknee/pro/dbknee_surface_mesh_process.cxx

//:
// \file

#include "dbknee_surface_mesh_process.h"
#include <bpro1/bpro1_parameters.h>
//#include <vsol/vsol_polyline_2d.h>
//#include <vsol/vsol_point_2d.h>
//#include <dbsol/dbsol_file_io.h>
//#include <dbdet/xio/dbdet_xio_seg3d_info.h>
#include <vul/vul_file.h>
#include <vul/vul_temp_filename.h>

#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>

//#include <vnl/vnl_math.h>

//: Constructor
dbknee_surface_mesh_process::
dbknee_surface_mesh_process()
{
  if( !parameters()->add( "Input point cloud file .p3d>" , 
    "-input_p3d_file", bpro1_filepath("",".p3d")) ||
    !parameters()->add( "Output surface mesh file .ply>" , 
    "-output_ply_file", bpro1_filepath("",".ply"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_surface_mesh_process::
~dbknee_surface_mesh_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_surface_mesh_process::
clone() const
{
  return new dbknee_surface_mesh_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_surface_mesh_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_surface_mesh_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_surface_mesh_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_surface_mesh_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_surface_mesh_process::
execute()
{
  bpro1_filepath xml_file;
  parameters()->get_value( "-xml_file" , xml_file );

  bpro1_filepath input_p3d_file;
  parameters()->get_value( "-input_p3d_file" , input_p3d_file );

  bpro1_filepath output_ply_file;
  parameters()->get_value( "-output_ply_file" , output_ply_file );

  dbknee_surface_mesh_process::surface_mesh(input_p3d_file.path, output_ply_file.path);

  return true;
}

bool dbknee_surface_mesh_process::
finish()
{
  return true;
}




void dbknee_surface_mesh_process::
surface_mesh(const vcl_string& input_p3d_file,
             const vcl_string& output_ply_file)
{
  // system call to process the data

  // Change directory
  vcl_string input_folder = vul_file::dirname(input_p3d_file);
  vul_file::change_directory(input_folder);

  

  // Run surface reconstruction using shock
  vcl_stringstream str1;
  vcl_string p3d_name = vul_file::strip_directory(input_p3d_file);
  str1 << "dbsk3dappw -seg 1"
    << " -f " << vul_file::strip_extension(p3d_name);
    // "SP1133R_TESTRETEST7_smooth_outer"
  vcl_system(str1.str().c_str());

  // Convert the output to a readable format
  vcl_stringstream str2;
  vcl_string output_folder = vul_file::dirname(output_ply_file);
  vcl_string default_surface_file = vul_file::strip_extension(p3d_name) + "-surface.ply";
  
  str2 << "mesh2ply" 
    << " " << default_surface_file 
    << " " << output_ply_file;
  vcl_system(str2.str().c_str());

  // Delete intermediate file
  
  vcl_string f0 = input_folder + "/" + default_surface_file;
  if (vul_file::dirname(f0) != vul_file::dirname(output_ply_file) ||
    vul_file::strip_directory(f0) != vul_file::strip_directory(output_ply_file))
  {
    vcl_remove(f0.c_str());
  }

  
  
}
