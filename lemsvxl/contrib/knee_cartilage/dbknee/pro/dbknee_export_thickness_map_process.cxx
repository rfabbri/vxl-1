// This is dbknee/pro/dbknee_export_thickness_map_process.cxx

//:
// \file

#include "dbknee_export_thickness_map_process.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <bpro1/bpro1_parameters.h>
#include <dbknee/dbknee_coord.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

//: Constructor
dbknee_export_thickness_map_process::
dbknee_export_thickness_map_process()
{
  if( !parameters()->add( "Thickness-encoded mesh file <mesh_w_thickness.ply>" , 
    "-thickness_mesh_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Cylinder CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Band width ratio" , 
    "-band_width_ratio", double(0.2) ) ||
    !parameters()->add( "Start angle" , 
    "-start_angle", double(-130) ) ||
    !parameters()->add( "End angle" , 
    "-end_angle", double(-100) ) ||
    !parameters()->add( "Export the whole surface" , 
    "-export_full", false ) ||
    !parameters()->add( "Thickness map file <thickness_map_file.txt>" , 
    "-thickness_map_file", bpro1_filepath("",".txt"))
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbknee_export_thickness_map_process::
~dbknee_export_thickness_map_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_export_thickness_map_process::
clone() const
{
  return new dbknee_export_thickness_map_process(*this);
}


//: Provide a vector of required input types
std::vector< std::string > dbknee_export_thickness_map_process::
get_input_type()
{
  std::vector< std::string > to_return;
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbknee_export_thickness_map_process::
get_output_type()
{
  std::vector<std::string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_export_thickness_map_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_export_thickness_map_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_export_thickness_map_process::
execute()
{
  bpro1_filepath thickness_mesh_file;
  parameters()->get_value( "-thickness_mesh_file" , thickness_mesh_file );

  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );

  double band_width_ratio;
  parameters()->get_value( "-band_width_ratio" , band_width_ratio );

  double start_angle;
  parameters()->get_value( "-start_angle" , start_angle );

  double end_angle;
  parameters()->get_value( "-end_angle" , end_angle );

  bool export_full;
  parameters()->get_value( "-export_full" , export_full );

  bpro1_filepath thickness_map_file;
  parameters()->get_value( "-thickness_map_file" , thickness_map_file );

  bool show_gui = false;
  if (export_full)
  {
    std::cout << "Export full thickness map.\n";
    dbknee_export_thickness_map_process::export_full_thickness_map_to_file(
      thickness_mesh_file.path, cs_file.path, thickness_map_file.path);
    return true;
  }
  else
  {
    dbknee_export_thickness_map_process::export_thickness_map_to_file(
      thickness_mesh_file.path,
      cs_file.path, 
      band_width_ratio,
      start_angle,
      end_angle,
      thickness_map_file.path,
      show_gui);
    return true;
  }
}

bool dbknee_export_thickness_map_process::
finish()
{
  return true;
}



// Support functions

//: Take in the data and execute
bool dbknee_export_thickness_map_process::
export_thickness_map_to_file(
  const std::string& thickness_mesh_file,
  const std::string& cs_file, 
  double band_width_ratio,
  double start_angle,
  double end_angle,
  const std::string& thickness_map_file,
  bool show_gui)
{
  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);

  // i. Load the meshes
  dbmsh3d_richmesh richmesh;

  std::cout << "\n\nLoad mesh file = " << thickness_mesh_file << std::endl;

  // list of properties to load
  std::vector<std::string > vertex_prop_list;
  vertex_prop_list.push_back("verror_abs");

  std::vector<std::string > face_prop_list;
  face_prop_list.clear();

  ///////////////////////////////
  dbmsh3d_load_ply(&richmesh, thickness_mesh_file.c_str(), vertex_prop_list, face_prop_list);
  ///////////////////////////////

  std::cout << "done.\n";
  std::cout << "#v[ " << richmesh.num_vertices() 
    << " ], #f[ " << richmesh.facemap().size() << " ]\n";


  // iterate thru the vertices and output their cylindrical coordinates
  // and thickness
  std::cout << "Save thickness maps to file ... \n";

  // figure out the min and max of z for each band
  double zmin_top = coord.z_top_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_top = coord.z_top_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  double zmin_bot = coord.z_bot_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_bot = coord.z_bot_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  // place holder for the computed coordinates
  vnl_matrix<double > bot_table(richmesh.num_vertices(), 5);
  vnl_matrix<double > top_table(richmesh.num_vertices(), 5);

    //int vertex_count = 0;
  int vertex_count_bot = 0;
  int vertex_count_top = 0;

  // traverse the vertices
  richmesh.reset_vertex_traversal();
  for (dbmsh3d_vertex* vb=0; richmesh.next_vertex(vb); )
  {
    if (vb->is_a() != "dbmsh3d_richvertex")
    {
      std::cerr << "Error: loaded mesh needs to have vertices of type dbmsh3d_richvertex \n";
      std::cerr << "Current vertex type: " << vb->is_a() << std::endl;
      std::cerr << "Quit now. \n";
      return 1;
    }
    dbmsh3d_richvertex* v = static_cast<dbmsh3d_richvertex*>(vb);

    double thickness = -1;
    v->get_scalar_property("verror_abs", thickness);
    assert(thickness>=0);

    double radius = 0;
    double theta = 0;
    double z = 0;
    coord.wc_to_local_cyl(v->pt(), radius, theta, z);

    double angle = theta*180 / vnl_math::pi;
    if (angle > start_angle && angle < end_angle)
    {
      if (z > zmin_bot && z < zmax_bot)
      {
        bot_table(vertex_count_bot, 0) = v->id();
        bot_table(vertex_count_bot, 1) = radius;
        bot_table(vertex_count_bot, 2) = theta;
        bot_table(vertex_count_bot, 3) = z;
        bot_table(vertex_count_bot, 4) = thickness;
        ++vertex_count_bot;
      }

      if (z > zmin_top && z < zmax_top)
      {
        top_table(vertex_count_top, 0) = v->id();
        top_table(vertex_count_top, 1) = radius;
        top_table(vertex_count_top, 2) = theta;
        top_table(vertex_count_top, 3) = z;
        top_table(vertex_count_top, 4) = thickness;
        ++vertex_count_top;
      }
    }
  }      

  // write output table to files
  
  std::cout << "Thickness map file = " << thickness_map_file << std::endl;
  std::fstream outfile(thickness_map_file.c_str(), std::ios::out);
  outfile << "id radius theta z thickness\n";
  
  // bottom map
  bot_table.get_n_rows(0, vertex_count_bot).print(outfile);

  // top map
  top_table.get_n_rows(0, vertex_count_top).print(outfile);

  outfile.close();
  std::cout << "Done.\n";


  return false;
}



//: Take in the data and execute
bool dbknee_export_thickness_map_process::
export_full_thickness_map_to_file(const std::string& thickness_mesh_file,
                                  const std::string& cs_file,
                                  const std::string& thickness_map_file)
{
  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);

  // i. Load the meshes
  dbmsh3d_richmesh richmesh;

  std::cout << "\n\nLoad mesh file = " << thickness_mesh_file << std::endl;

  // list of properties to load
  std::vector<std::string > vertex_prop_list;
  vertex_prop_list.push_back("verror_abs");

  std::vector<std::string > face_prop_list;
  face_prop_list.clear();

  ///////////////////////////////
  dbmsh3d_load_ply(&richmesh, thickness_mesh_file.c_str(), vertex_prop_list, face_prop_list);
  ///////////////////////////////

  std::cout << "done.\n";
  std::cout << "#v[ " << richmesh.num_vertices() 
    << " ], #f[ " << richmesh.facemap().size() << " ]\n";


  // iterate thru the vertices and output their cylindrical coordinates
  // and thickness
  std::cout << "Save thickness maps to file ... \n";

  vnl_matrix<double > full_table(richmesh.num_vertices(), 5);
  int vertex_count_full = 0;

  // traverse the vertices
  richmesh.reset_vertex_traversal();
  for (dbmsh3d_vertex* vb=0; richmesh.next_vertex(vb); )
  {
    if (vb->is_a() != "dbmsh3d_richvertex")
    {
      std::cerr << "Error: loaded mesh needs to have vertices of type dbmsh3d_richvertex \n";
      std::cerr << "Current vertex type: " << vb->is_a() << std::endl;
      std::cerr << "Quit now. \n";
      return 1;
    }
    dbmsh3d_richvertex* v = static_cast<dbmsh3d_richvertex*>(vb);

    double thickness = -1;
    v->get_scalar_property("verror_abs", thickness);
    assert(thickness>=0);

    double radius = 0;
    double theta = 0;
    double z = 0;
    coord.wc_to_local_cyl(v->pt(), radius, theta, z);

    double angle = theta*180 / vnl_math::pi;

    full_table(vertex_count_full, 0) = v->id();
    full_table(vertex_count_full, 1) = radius;
    full_table(vertex_count_full, 2) = theta;
    full_table(vertex_count_full, 3) = z;
    full_table(vertex_count_full, 4) = thickness;
    ++vertex_count_full;

  }      

  // write output table to files
  
  std::cout << "Thickness map file = " << thickness_map_file << std::endl;
  std::fstream outfile(thickness_map_file.c_str(), std::ios::out);
  outfile << "id radius theta z thickness\n";
  
  // top map
  full_table.get_n_rows(0, vertex_count_full).print(outfile);

  outfile.close();
  std::cout << "Done.\n";


  return false;

  return false;
}
