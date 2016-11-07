// This is contrib/ntrinh/en292/pro/vidpro1_compute_homog_2d_from_rectangle_process.cxx

//:
// \file

#include "vidpro1_compute_homog_2d_from_rectangle_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include<bpro1/bpro1_parameters.h>

#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>

#include <en292/pro/vidpro1_homog_2d_storage.h>

//: Constructor
vidpro1_compute_homog_2d_from_rectangle_process::
vidpro1_compute_homog_2d_from_rectangle_process() : bpro1_process()
{
  if( !parameters()->add( "Line x0=" , "-ht_x0" , (float)10) ||
      !parameters()->add( "Line y0=" , "-ht_y0" , (float)10) ||
      !parameters()->add( "Line x1=" , "-ht_x1" , (float)20) ||
      !parameters()->add( "Line y1=" , "-ht_y1" , (float)20)
    ) 
  {
    vcl_cerr << "ERROR: Adding parameters()" << vcl_endl;
  }
  else
  {    
  }
}


//: Destructor
vidpro1_compute_homog_2d_from_rectangle_process::
~vidpro1_compute_homog_2d_from_rectangle_process()
{
}


//: Return the name of this process
vcl_string vidpro1_compute_homog_2d_from_rectangle_process::
name()
{
  return "Compute homography 2D from rectangle";
}


//: Return the number of input frame for this process
int vidpro1_compute_homog_2d_from_rectangle_process::
input_frames()
{
  return 1;
}



//: Return the number of output frames for this process
int vidpro1_compute_homog_2d_from_rectangle_process::
output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_compute_homog_2d_from_rectangle_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_compute_homog_2d_from_rectangle_process::
get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "homog_2d" );
  return to_return;
}


//: Execute the process
bool vidpro1_compute_homog_2d_from_rectangle_process::
execute()
{
  
  vcl_cout << "\nCompute homography matrix from correspondence between a 4-polygon(image) and rectangle(world)\n";
  if ( this->input_data_.size() != 1 )
  {
    vcl_cout << "In vidpro1_compute_homog_2d_from_rectangle_process::execute() - not exactly one"
             << " input frames \n";
    return false;
  }
  clear_output();

  // get input from storage classes
  vidpro1_vsol2D_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(input_data_[0][0]);
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_data = vsol_storage->all_data();
  
  if (vsol_data.size() < 1)
    return false;

  vsol_spatial_object_2d_sptr vsol_obj = vsol_data.at(0);
  vsol_polygon_2d_sptr poly;
  if (vsol_obj->cast_to_region())
  {
    if (vsol_obj->cast_to_region()->cast_to_polygon())
    {
      poly = vsol_obj->cast_to_region()->cast_to_polygon();
    }
  }
  // if can't find a polygon, quit
  if (!poly)
  {
    vcl_cout << "Could not find a polygon in vsol2D storage" << vcl_endl;
    return false;
  }

  if (poly->size() != 4)
  {
    vcl_cout << "Number of vertices in polygon is not 4" << vcl_endl;
    return false;
  }

  // points from the image (4-polygon)
  vcl_vector< vgl_homg_point_2d< double > > image_points;
  for (unsigned int i = 0; i < poly->size(); i ++)
  {
    vgl_homg_point_2d< double > pt(poly->vertex(i)->x(), poly->vertex(i)->y()); 
    image_points.push_back(pt);
  }

  // world points - from user input
  vcl_vector< vgl_homg_point_2d< double > > world_points;
  // coordinate of a rectangle
  float x0=0, y0=0, x1=0, y1=0;
  this->parameters()->get_value("-ht_x0", x0);
  this->parameters()->get_value("-ht_y0", y0);
  this->parameters()->get_value("-ht_x1", x1);
  this->parameters()->get_value("-ht_y1", y1);

  vgl_homg_point_2d< double > p0(x0, y0);
  vgl_homg_point_2d< double > p1(x1, y0);
  vgl_homg_point_2d< double > p2(x1, y1);
  vgl_homg_point_2d< double > p3(x0, y1);

  world_points.push_back(p0);
  world_points.push_back(p1);
  world_points.push_back(p2);
  world_points.push_back(p3);
  
  vgl_h_matrix_2d_compute_4point homog_func;
  vgl_h_matrix_2d< double > H;
  
  //////////////////////////////////////////////////////
  bool success = homog_func.compute(world_points, image_points, H);
  if (!success)
  {
    vcl_cout << "Computing homography from 4 points failed \n";
    return false;
  }
  vcl_cout << "Computing homography succeeded.\nH = " << H << vcl_endl;
  /////////////////////////////////////////////////////////////////////

  // create the output storage class
  vidpro1_homog_2d_storage_sptr output_storage = vidpro1_homog_2d_storage_new();
  vgl_h_matrix_2d<double > H_return(H.get_matrix());
  output_storage->set_H(H_return);  
  output_data_[0].push_back(output_storage);

  return true;
}


//: Clone the process
bpro1_process*
vidpro1_compute_homog_2d_from_rectangle_process::clone() const
{
  return new vidpro1_compute_homog_2d_from_rectangle_process(*this);
}
    
bool
vidpro1_compute_homog_2d_from_rectangle_process::finish()
{
  return true;
}




