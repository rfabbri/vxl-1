// This is dbknee/pro/dbknee_region_thickness_process.cxx

//:
// \file

#include "dbknee_region_thickness_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbknee/dbknee_coord.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbgl/dbgl_area.h>
#include <vnl/vnl_math.h>

//: Constructor
dbknee_region_thickness_process::
dbknee_region_thickness_process()
{
  vcl_vector<vcl_string > knee_sides;
  knee_sides.push_back("left");
  knee_sides.push_back("right");

  vcl_vector<vcl_string > average_method;
  average_method.push_back("vertex-based");
  average_method.push_back("face-based");

  if( !parameters()->add( "Thickness-encode mesh file <thickness_mesh.ply>" , 
    "-data_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Cylinder CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Band width ratio" , 
    "-band_width_ratio", double(0.2) ) ||
    !parameters()->add("Left or Right knee?: " , 
    "-left_right" , knee_sides, 0) ||
    !parameters()->add( "Start angle (positive number, in degree)" , 
    "-start_angle", double(100) ) ||
    !parameters()->add( "End angle (positive number, in degree)" , 
    "-end_angle", double(130) ) ||
    !parameters()->add( "(Optional) Top region (medial side) mesh file <top_region.ply>" , 
    "-top_region_file", bpro1_filepath("",".ply") ) ||
    !parameters()->add( "(Optional) Bottom region (lateral side) mesh file <bot_region.ply>" , 
    "-bot_region_file", bpro1_filepath("",".ply") ) ||
    !parameters()->add("Method to compute average: " , 
    "-average_method" , average_method, 0)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_region_thickness_process::
~dbknee_region_thickness_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_region_thickness_process::
clone() const
{
  return new dbknee_region_thickness_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_region_thickness_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_region_thickness_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_region_thickness_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_region_thickness_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_region_thickness_process::
execute()
{
  bpro1_filepath data_file;
  parameters()->get_value( "-data_file" , data_file );

  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );

  double band_width_ratio;
  parameters()->get_value( "-band_width_ratio" , band_width_ratio );

  unsigned int left_right = 100; // = 0: left, = 1: right
  parameters()->get_value( "-left_right" , left_right);
  
  double start_angle;
  parameters()->get_value( "-start_angle" , start_angle );

  double end_angle;
  parameters()->get_value( "-end_angle" , end_angle );

  unsigned int average_method = 100; // = 0: vertex-based; = 1: face-based
  parameters()->get_value( "-average_method" , average_method);


  // Optional parameters
  bpro1_filepath top_region_file;
  parameters()->get_value( "-top_region_file" , top_region_file );

  bpro1_filepath bot_region_file;
  parameters()->get_value( "-bot_region_file" , bot_region_file );

  // change the values of start and end angles depending on left or right knee
  // left: postive angle
  // right: negative angle
  if (left_right == 1)
  {
    // right side: invert the sign
    double temp = start_angle;
    start_angle = -end_angle;
    end_angle = -temp;
  }
  else if (left_right != 0)
  {
    // unknown option:
    vcl_cout << "ERROR: Unknown knee side option.\n";
    return false;
  }
  

  double bot_thickness = 0;
  double top_thickness = 0;

  if (average_method == 0)
  {
    // compute region thickness
    dbknee_region_thickness_process::region_thickness_vertex_based(data_file.path,
      cs_file.path, band_width_ratio, start_angle, end_angle, 
      bot_thickness, top_thickness);
  }
  else
  {
    // compute region thickness
    dbknee_region_thickness_process::region_thickness_face_based(data_file.path,
      cs_file.path, band_width_ratio, start_angle, end_angle, 
      bot_thickness, top_thickness);
  }

  // crop the mesh when required
  dbknee_region_thickness_process::crop_mesh_using_cyl_cs(
    data_file.path,
    cs_file.path,
    band_width_ratio,
    start_angle,
    end_angle,
    top_region_file.path,
    bot_region_file.path);

  return true;
}


// ----------------------------------------------------------------------------
bool dbknee_region_thickness_process::
finish()
{
  return true;
}


// ----------------------------------------------------------------------------
//: Take in the data and execute
bool dbknee_region_thickness_process::
region_thickness_vertex_based(const vcl_string& thickness_mesh_file,
                 const vcl_string& cs_file,
                 double band_width_ratio,
                 double start_angle,
                 double end_angle,
                 double& bot_thickness,
                 double& top_thickness)
{
  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);

  // i. Load the meshes
  dbmsh3d_richmesh richmesh;

  vcl_cout << "\n\nLoad mesh file = " << thickness_mesh_file << vcl_endl;

  // list of properties to load
  vcl_vector<vcl_string > vertex_prop_list;
  vertex_prop_list.push_back("verror_abs");

  vcl_vector<vcl_string > face_prop_list;
  face_prop_list.clear();

  ///////////////////////////////
  dbmsh3d_load_ply(&richmesh, thickness_mesh_file.c_str(), 
    vertex_prop_list, face_prop_list);
  ///////////////////////////////

  vcl_cout << "#v[ " << richmesh.num_vertices() 
    << " ], #f[ " << richmesh.facemap().size() << " ]\n";




  // figure out the min and max of z for each band
  double zmin_top = coord.z_top_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_top = coord.z_top_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  double zmin_bot = coord.z_bot_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_bot = coord.z_bot_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();


  // iterate thru the vertices to compute regional thickness
  
  int vertex_count_bot = 0;
  int vertex_count_top = 0;
  double sum_bot = 0;
  double sum_top = 0;

  richmesh.reset_vertex_traversal();
  for (dbmsh3d_vertex* vb=0; richmesh.next_vertex(vb); )
  {
    if (vb->is_a() != "dbmsh3d_richvertex")
    {
      vcl_cerr << "Error: loaded mesh needs to have vertices of type dbmsh3d_richvertex \n";
      vcl_cerr << "Current vertex type: " << vb->is_a() << vcl_endl;
      vcl_cerr << "Quit now. \n";
      return false;
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
        sum_bot += thickness;
        ++vertex_count_bot;
      }

      if (z > zmin_top && z < zmax_top)
      {
        sum_top += thickness;
        ++vertex_count_top;
      }
    }
  }

  bot_thickness = sum_bot / vertex_count_bot;
  top_thickness = sum_top / vertex_count_top;

  vcl_cout << "\nBottom region: #v = " << vertex_count_bot 
    << "\n  Start angle = " << start_angle
    << "\n  End angle = " << end_angle
    << "\n  Band width = " << band_width_ratio
    << "\n  Mean thickness = " << bot_thickness << vcl_endl;


  vcl_cout << "\nTop region: #v = " << vertex_count_top 
    << "\n  Start angle = " << start_angle
    << "\n  End angle = " << end_angle
    << "\n  Band width = " << band_width_ratio
    << "\n  Mean thickness = " << top_thickness << vcl_endl;

  vcl_cout << "Done.\n";
  return true;
}






//: Take in the data and execute
// compute thickness as weighted average of closest distance to the faces
// the weights are the areas of the triangle
bool dbknee_region_thickness_process::
region_thickness_face_based(const vcl_string& thickness_mesh_file,
                   const vcl_string& cs_file,
                   double band_width_ratio,
                   double start_angle,
                   double end_angle,
                   double& bot_thickness,
                   double& top_thickness)
{
  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);

  // i. Load the meshes
  dbmsh3d_richmesh richmesh;

  vcl_cout << "\n\nLoad mesh file = " << thickness_mesh_file << vcl_endl;

  // list of properties to load
  vcl_vector<vcl_string > vertex_prop_list;
  vertex_prop_list.push_back("verror_abs");

  vcl_vector<vcl_string > face_prop_list;
  face_prop_list.clear();

  ///////////////////////////////
  dbmsh3d_load_ply(&richmesh, thickness_mesh_file.c_str(), 
    vertex_prop_list, face_prop_list);
  ///////////////////////////////

  richmesh.IFS_to_MHE();

  vcl_cout << "#v[ " << richmesh.num_vertices() 
    << " ], #f[ " << richmesh.facemap().size() << " ]\n";




  // figure out the min and max of z for each band
  double zmin_top = coord.z_top_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_top = coord.z_top_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  double zmin_bot = coord.z_bot_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_bot = coord.z_bot_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();


  // iterate thru the vertices to compute regional thickness

  double area_top = 0;
  double area_bot = 0;

  double sum_weighted_thickness_top = 0;
  double sum_weighted_thickness_bot = 0;

  int face_count_top = 0;
  int face_count_bot = 0;
  
  richmesh.reset_vertex_traversal();
  richmesh.reset_face_traversal();
  for (dbmsh3d_face* face=0; richmesh.next_face(face); )
  {
    // Check whether the face is in the region of interest
    double radius = 0;
    double theta = 0;
    double z = 0;
    coord.wc_to_local_cyl(face->compute_center_pt(), radius, theta, z);
    double angle = theta*180 / vnl_math::pi;
    
    if ( !((angle > start_angle && angle < end_angle) && 
      ((z > zmin_bot && z < zmax_bot) || 
      (z > zmin_top && z < zmax_top))) )
    {
      continue;
    }

    // Compute thickness and area of the face
    vcl_vector<dbmsh3d_vertex* > face_vertices;
    face->get_bnd_Vs(face_vertices);
    assert(face_vertices.size() >= 3);

    double face_area = 0;
    double face_weighted_thickness = 0;
    
    // iterate thru the triangles of the face
    dbmsh3d_vertex* v1 = face_vertices[0];
    for (unsigned i=1; i < face_vertices.size()-1; ++i)
    {
      // compute area of the triangle
      dbmsh3d_vertex* v2 = face_vertices[i];
      dbmsh3d_vertex* v3 = face_vertices[i+1];
      double triangle_area = dbgl_area_triangle(v1->pt(), v2->pt(), v3->pt());
      face_area += triangle_area;

      // compute mean thickness of the 3 vertices
      if (v1->is_a() != "dbmsh3d_richvertex" || 
        v2->is_a() != "dbmsh3d_richvertex" || 
        v3->is_a() != "dbmsh3d_richvertex" )
      {
        vcl_cerr << "Error: loaded mesh needs to have vertices of type dbmsh3d_richvertex \n";
        vcl_cerr << "Quit now. \n";
        return false;
      }

      double thickness1 = 0;
      double thickness2 = 0;
      double thickness3 = 0;

      static_cast<dbmsh3d_richvertex*>(v1)->get_scalar_property("verror_abs", thickness1);
      static_cast<dbmsh3d_richvertex*>(v2)->get_scalar_property("verror_abs", thickness2);
      static_cast<dbmsh3d_richvertex*>(v3)->get_scalar_property("verror_abs", thickness3);
      double triangle_thickness = (thickness1 + thickness2 + thickness3) / 3;

      // weighted thickness for the face
      face_weighted_thickness = triangle_thickness * triangle_area;
    }

    // add to the overall sum
    if (angle > start_angle && angle < end_angle)
    {
      if (z > zmin_bot && z < zmax_bot)
      {
        area_bot += face_area;
        sum_weighted_thickness_bot += face_weighted_thickness;
        ++face_count_bot;
      }

      if (z > zmin_top && z < zmax_top)
      {
        area_top += face_area;
        sum_weighted_thickness_top += face_weighted_thickness;
        ++face_count_top;
      }
    }

  }

  if (face_count_bot > 0)
  {
    bot_thickness = sum_weighted_thickness_bot / area_bot;
  }

  if (face_count_top > 0)
  {
    top_thickness = sum_weighted_thickness_top / area_top;
  }

  vcl_cout << "\nBottom region: "
    << "\n  Start angle = " << start_angle
    << "\n  End angle = " << end_angle
    << "\n  Band width = " << band_width_ratio
    << "\n  #f = " << face_count_bot 
    << "\n  Area = " << area_bot
    << "\n  Mean thickness = " << bot_thickness << vcl_endl;


  vcl_cout << "\nTop region: "
    << "\n  Start angle = " << start_angle
    << "\n  End angle = " << end_angle
    << "\n  Band width = " << band_width_ratio
    << "\n  #f = " << face_count_top 
    << "\n  Area = " << area_top
    << "\n  Mean thickness = " << top_thickness << vcl_endl;

  vcl_cout << "Done.\n";
  return true;

}




//: ---------------------------------------------------------------------------
bool dbknee_region_thickness_process::
crop_mesh_using_cyl_cs(const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    const vcl_string& top_mesh_file,
    const vcl_string& bot_mesh_file)
{
  // preliminary check
  if (top_mesh_file.empty() && bot_mesh_file.empty()) return false;

  vcl_cout << "Crop the thickness mesh and save.\n";

  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file);

  // i. Load the meshes
  dbmsh3d_mesh mesh;

  vcl_cout << "\n\nLoad mesh file = " << thickness_mesh_file << vcl_endl;

  ///////////////////////////////
  dbmsh3d_load_ply(&mesh, thickness_mesh_file.c_str());
  ///////////////////////////////

  vcl_cout << "#v[ " << mesh.num_vertices() 
    << " ], #f[ " << mesh.facemap().size() << " ]\n";

  // figure out the min and max of z for each band
  double zmin_top = coord.z_top_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_top = coord.z_top_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  double zmin_bot = coord.z_bot_band_centroid() - 
    0.5*band_width_ratio*coord.cylinder().length();
  double zmax_bot = coord.z_bot_band_centroid() +
    0.5*band_width_ratio*coord.cylinder().length();

  double theta_min = start_angle * vnl_math::pi / 180;
  double theta_max = end_angle * vnl_math::pi / 180;

  
  

  if (!top_mesh_file.empty())
  {
    dbmsh3d_mesh top_mesh;
    coord.crop_mesh_cylindrical(mesh, top_mesh, zmin_top, zmax_top, theta_min, theta_max);
    vcl_cout << "Save top (medial side) mesh file. top_mesh_file = "
      << top_mesh_file << vcl_endl;

    // Save mesh
    dbmsh3d_save_ply(&top_mesh, top_mesh_file.c_str(), false);
  }

  if (!bot_mesh_file.empty())
  {
    dbmsh3d_mesh bot_mesh;
    coord.crop_mesh_cylindrical(mesh, bot_mesh, zmin_bot, zmax_bot, theta_min, theta_max);
    vcl_cout << "Save bot (lateral side) mesh file. bot_mesh_file = "
      << bot_mesh_file << vcl_endl;

    // Save mesh
    dbmsh3d_save_ply(&bot_mesh, bot_mesh_file.c_str(), false);
  
  }

  return true;
}
