// This is dbknee/pro/dbknee_compute_tibia_cs_process.cxx

//:
// \file

#include "dbknee_compute_tibia_cs_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbknee/dbknee_tibia_cs.h>
#include <vcl_cstdlib.h>
#include <dbgl/dbgl_area.h>

//: Constructor
dbknee_compute_tibia_cs_process::
dbknee_compute_tibia_cs_process()
{
  if( !parameters()->add( "Bone/Cartilage surface mesh <filename.ply>" , 
    "-data_file", bpro1_filepath("",".ply")) ||
    !parameters()->add( "Output CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_compute_tibia_cs_process::
~dbknee_compute_tibia_cs_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_compute_tibia_cs_process::
clone() const
{
  return new dbknee_compute_tibia_cs_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_compute_tibia_cs_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_compute_tibia_cs_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_compute_tibia_cs_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_compute_tibia_cs_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_compute_tibia_cs_process::
execute()
{
  bpro1_filepath data_file;
  parameters()->get_value( "-data_file" , data_file );

  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );

  
  bool success = 
    dbknee_compute_tibia_cs_process::
    compute_tibia_cs(data_file.path, cs_file.path);

  if (success)
  {
  
  }
  else
  {
    vcl_cout << "ERROR: Computing coordinate system failed.\n";
  }


 
  return true;
}

bool dbknee_compute_tibia_cs_process::
finish()
{
  return true;
}




//: Take in the data (mesh file, param file) and execute
bool dbknee_compute_tibia_cs_process::
compute_tibia_cs(const vcl_string& data_file,
                    const vcl_string& cs_file)
{
  vcl_cout << "Compute cylinder coordinate system from a knee cartilage mesh.\n";
  
  // i. Load input mesh file
  // --------------

  // determine file name
  if (data_file == "")
  {
    vcl_cerr << "An input data file is required to proceed.";
    return false;
  }

  vcl_cout << "Input data file = " << data_file << vcl_endl;

  // determine the file format
  vcl_string data_file_format = vul_file::extension(data_file);

  
  dbmsh3d_mesh mesh;
  
  
  dbmsh3d_load_ply(&mesh, data_file.c_str());
  mesh.IFS_to_MHE();
   

  

  // iii. Construct the coordinate system
  // ------------------------------------

 
  dbknee_tibia_cs coord;
  coord.set_mesh(&mesh);
  
  

  // Execution !
  coord.build();


  // iv. Save results
  // -----------------

  vcl_string outfile = cs_file;
  if (outfile == "")
  {
    outfile = vul_file::strip_extension(data_file + "_cs.txt");
  }

  // open file for writing
  vcl_ofstream outfp(outfile.c_str(), vcl_ios_out);
  coord.print(outfp);
  outfp.close();  


  vcl_cout << "Done.\n";

  this->region_thickness_face_based(data_file, coord);

  return true;
}








//: Take in the data and execute
// compute thickness as weighted average of closest distance to the faces
// the weights are the areas of the triangle
bool dbknee_compute_tibia_cs_process::
region_thickness_face_based(const vcl_string& thickness_mesh_file, 
    dbknee_tibia_cs coord)
{
  // load the coordinate system
  
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
  coord.wc_to_local(&richmesh);

  vgl_box_3d<double> bbox;
  detect_bounding_box(&richmesh, bbox);

  double dx = (bbox.max_x() - bbox.min_x()) * 0.2;
  double dy = (bbox.max_y() - bbox.min_y()) * 0.15;

  

  vcl_cout << "#v[ " << richmesh.num_vertices() 
    << " ], #f[ " << richmesh.facemap().size() << " ]\n";

 
  //// iterate thru the vertices to compute regional thickness

  double area_top = 0;

  double sum_weighted_thickness_top = 0;

  int face_count_top = 0;
  
  richmesh.reset_vertex_traversal();
  richmesh.reset_face_traversal();
  for (dbmsh3d_face* face=0; richmesh.next_face(face); )
  {
    // Check whether the face is in the region of interest
    vgl_point_3d<double > local = face->compute_center_pt();
    
    
    if ( vcl_abs(local.x()) > dx || vcl_abs(local.y()) > dy )
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
  
        area_top += face_area;
        sum_weighted_thickness_top += face_weighted_thickness;
        ++face_count_top;
  

  }

  

  double top_thickness;
  if (face_count_top > 0)
  {
    top_thickness = sum_weighted_thickness_top / area_top;
  }

  

  vcl_cout << "\nTop region: "
    << "\n  #f = " << face_count_top 
    << "\n  Area = " << area_top
    << "\n  Mean thickness = " << top_thickness << vcl_endl;

  vcl_cout << "Done.\n";
  return true;

}
