// This is dbknee/pro/dbknee_crop_mesh_process.cxx

//:
// \file

#include "dbknee_crop_mesh_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbknee/dbknee_coord.h>
#include <vcl_cstdlib.h>

//: Constructor
dbknee_crop_mesh_process::
dbknee_crop_mesh_process()
{
  vcl_vector<vcl_string > crop_direction;
  crop_direction.push_back("x");
  crop_direction.push_back("y");
  crop_direction.push_back("z");


  if( !parameters()->add( "Mesh file <filename.ply>" , 
    "-data_file", bpro1_filepath("",".ply")) ||
    !parameters()->add("Cropping direction: " , "-crop_direction" , 
    crop_direction, 0) ||
    !parameters()->add("cropping ratio = " , "-crop_ratio" , 0.5f ) ||
    !parameters()->add( "Output cropped mesh 1 <cropped_mesh_1.ply>" , 
    "-cropped_mesh_1", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Output cropped mesh 2 <cropped_mesh_2.ply>" , 
    "-cropped_mesh_2", bpro1_filepath("",".ply"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_crop_mesh_process::
~dbknee_crop_mesh_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_crop_mesh_process::
clone() const
{
  return new dbknee_crop_mesh_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_crop_mesh_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_crop_mesh_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_crop_mesh_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_crop_mesh_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_crop_mesh_process::
execute()
{
  bpro1_filepath data_file;
  parameters()->get_value( "-data_file" , data_file );

  unsigned int crop_direction = 0;
  parameters()->get_value( "-crop_direction" , crop_direction );
  
  float crop_ratio = 0.5f;
  parameters()->get_value( "-crop_ratio" , crop_ratio);

  bpro1_filepath cropped_mesh_1;
  parameters()->get_value( "-cropped_mesh_1" , cropped_mesh_1 );

  bpro1_filepath cropped_mesh_2;
  parameters()->get_value( "-cropped_mesh_2" , cropped_mesh_2 );
  
  vcl_string dir = "";
  switch (crop_direction)
  {
  case 0:
    dir = "x";
    break;
  case 1:
    dir = "y";
    break;
  case 2:
    dir = "z";
    break;
  }

  // crop and save the meshes
  this->crop_mesh(data_file.path, dir, crop_ratio, cropped_mesh_1.path, 
    cropped_mesh_2.path);

  return true;
}

bool dbknee_crop_mesh_process::
finish()
{
  return true;
}



// Support function
bool dbknee_crop_mesh_process::
crop_mesh(const vcl_string& data_file,
                const vcl_string& crop_direction,
                float crop_ratio,
                const vcl_string& cropped_mesh_1,
                const vcl_string& cropped_mesh_2)
{


  // i. Load input mesh file
  // --------------

  // determine file name
  if (data_file == "")
  {
    vcl_cerr << "An input data file is required to proceed."
      << "Use '-data_file' option for input data file.\n";
    return false;
  }

  vcl_cout << "Input data file = " << data_file << vcl_endl;

  // determine the file format
  vcl_string data_file_format = vul_file::extension(data_file);

  
  dbmsh3d_pro bndvis;
  
  if (data_file_format == ".ply")
  {
    if ( !bndvis.load_ply(data_file) )
    {
      vcl_cerr << "ERROR: Could not load .ply file " 
        << data_file << ".\n";
      return false;
    }
  }
  else // Unknow format --> Stop the program
  {
    vcl_cerr << "ERROR: Invalid file format. Quit now.\n";
    return false;
  } 

  vgl_box_3d<double> bounding_box;
  detect_bounding_box (bndvis.mesh(), bounding_box);

  // create two cropping box for the two meshes
  vgl_box_3d<double > bbox1 = bounding_box;
  vgl_box_3d<double > bbox2 = bounding_box;

  if (crop_direction == "x")
  {
    double m = (1-crop_ratio)*bbox1.min_x() + crop_ratio*bbox1.max_x();
    bbox1.set_max_x(m);
    bbox2.set_min_x(m);
  }
  else if (crop_direction == "y")
  {
    double m = (1-crop_ratio)*bbox1.min_y() + crop_ratio*bbox1.max_y();
    bbox1.set_max_y(m);
    bbox2.set_min_y(m);
  }
  else if (crop_direction == "z")
  {
    double m = (1-crop_ratio)*bbox1.min_z() + crop_ratio*bbox1.max_z();
    bbox1.set_max_z(m);
    bbox2.set_min_z(m);
  }
  else
  {
    vcl_cout << "Unknown cropping direction.\n";
  }

  // crop the meshes
  dbmsh3d_mesh mesh1;
  dbmsh3d_mesh mesh2;

  vcl_cout << "Cropping the meshes...";
  //bndvis.mesh()->IFS_to_MHE();
  dbmsh3d_mesh_algos::crop_mesh(*bndvis.mesh(), bbox1, mesh1);
  dbmsh3d_mesh_algos::crop_mesh(*bndvis.mesh(), bbox2, mesh2);
  vcl_cout << "done.\n";

  // v. Save output mesh
  vcl_cout << "Save cropped_mesh_file ... \n";
  dbmsh3d_save_ply(&mesh1, cropped_mesh_1.c_str(), false);
  dbmsh3d_save_ply(&mesh2, cropped_mesh_2.c_str(), false);
  vcl_cout << "Done.\n";


  vcl_cout << "Done.\n";
  return true;
}



