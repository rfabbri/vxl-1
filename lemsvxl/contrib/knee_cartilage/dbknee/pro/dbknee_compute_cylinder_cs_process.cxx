// This is dbknee/pro/dbknee_compute_cylinder_cs_process.cxx

//:
// \file

#include "dbknee_compute_cylinder_cs_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbknee/dbknee_coord.h>
#include <cstdlib>

//: Constructor
dbknee_compute_cylinder_cs_process::
dbknee_compute_cylinder_cs_process()
{
  if( !parameters()->add( "Bone/Cartilage surface mesh <filename.ply>" , 
    "-data_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Parameter file to build CS <param_file.txt>" , 
    "-param_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Output CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Output cropped mesh <cropped_mesh_file.ply>" , 
    "-cropped_mesh_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "show gui" , 
    "-show_gui", false)
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbknee_compute_cylinder_cs_process::
~dbknee_compute_cylinder_cs_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_compute_cylinder_cs_process::
clone() const
{
  return new dbknee_compute_cylinder_cs_process(*this);
}


//: Provide a vector of required input types
std::vector< std::string > dbknee_compute_cylinder_cs_process::
get_input_type()
{
  std::vector< std::string > to_return;
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbknee_compute_cylinder_cs_process::
get_output_type()
{
  std::vector<std::string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_compute_cylinder_cs_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_compute_cylinder_cs_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_compute_cylinder_cs_process::
execute()
{
  bpro1_filepath data_file;
  parameters()->get_value( "-data_file" , data_file );

  bpro1_filepath param_file;
  parameters()->get_value( "-param_file" , param_file );

  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );

  bpro1_filepath cropped_mesh_file;
  parameters()->get_value( "-cropped_mesh_file" , cropped_mesh_file );

  bool show_gui;
  parameters()->get_value( "-show_gui" , show_gui );


  bool success = 
    dbknee_compute_cylinder_cs_process::
    compute_cylinder_cs(data_file.path, param_file.path, cs_file.path, cropped_mesh_file.path);

  if (success)
  {
    if (show_gui)
    {
      std::stringstream str;
      str << "dbknee_coord_cmd"
        << " -view_mesh " << data_file.path
        << " -view_cs " << cs_file.path;
      std::system(str.str().c_str());
    }
  }
  else
  {
    std::cout << "ERROR: Computing coordinate system failed.\n";
  }


 
  return true;
}

bool dbknee_compute_cylinder_cs_process::
finish()
{
  return true;
}




//: Take in the data (mesh file, param file) and execute
bool dbknee_compute_cylinder_cs_process::
compute_cylinder_cs(const std::string& data_file,
                    const std::string& param_file, 
                    const std::string& cs_file,
                    const std::string& cropped_mesh_file)
{
  std::cout << "Compute cylinder coordinate system from a knee cartilage mesh.\n";
  
  // i. Load input mesh file
  // --------------

  // determine file name
  if (data_file == "")
  {
    std::cerr << "An input data file is required to proceed."
      << "Use '-data_file' option for input data file.\n";
    return false;
  }

  std::cout << "Input data file = " << data_file << std::endl;

  // determine the file format
  std::string data_file_format = vul_file::extension(data_file);

  
  dbmsh3d_pro bndvis;
  
  // Load the point cloud depending on the format
  if (data_file_format == ".p3d")
  {
    if ( !bndvis.load_p3d(data_file) )
    {
      std::cerr << "ERROR: Could not load .p3d file " 
        << data_file << ".\n";
      return false;
    }
  }
  else if (data_file_format == ".ply")
  {
    if ( !bndvis.load_ply(data_file) )
    {
      std::cerr << "ERROR: Could not load .ply file " 
        << data_file << ".\n";
      return false;
    }
  }
  else // Unknow format --> Stop the program
  {
    std::cerr << "ERROR: Invalid file format. Quit now.\n";
    return false;
  } 

  
  
  // ii. Load the parameters neccessary to build the coordinate system
  // -----------------------------------------------------------------
  
  if (param_file == "")
  {
    std::cerr << "ERROR: No parameter file specified.\n";
    return false;
  }
  std::cout << "Parameter file = " << param_file << std::endl;


  /////////////////////////////////////////////////////////////
  std::map<std::string, double > param_map;
  dbknee_read_coord_param_file(param_file, param_map);

  if (param_map.empty())
  {
    std::cerr << "ERROR: parameter file is either non-existent or empty.\n";
    return false;
  }

  // iii. Construct the coordinate system
  // ------------------------------------

  vgl_box_3d<double> bounding_box;
  detect_bounding_box (bndvis.mesh(), bounding_box);
  dbknee_cylinder_based_coord_params coord_params(bounding_box, param_map);
  
  dbknee_cylinder_based_coord coord;
  coord.set_point_set(bndvis.mesh());
  std::cout << "Cropping box = " << coord_params.cropping_box << std::endl;
  coord.set_cropping_box(coord_params.cropping_box);

  std::cout << "Notch point = " << coord_params.notch_point << std::endl;
  coord.set_notch_point(coord_params.notch_point);


  // Execution !
  coord.build();
  coord.compute_z_of_band_centroids();


  // iv. Save results
  // -----------------

  std::string outfile = cs_file;
  if (outfile == "")
  {
    outfile = vul_file::strip_extension(data_file + "_cs.txt");
  }

  // open file for writing
  std::ofstream outfp(outfile.c_str(), std::ios::out);
  coord.print(outfp);
  outfp.close();  


  // v. Save output mesh
  if (!cropped_mesh_file.empty())
  {
    std::cout << "Save cropped_mesh_file ... \n";
    dbmsh3d_save_ply(&coord.cropped_mesh(), cropped_mesh_file.c_str(), false);
    std::cout << "Done.\n";
  }


  std::cout << "Done.\n";
  return true;
}



