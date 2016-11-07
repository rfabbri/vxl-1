// This is dbknee/pro/dbknee_plot_cylinder_cs_process.cxx

//:
// \file

#include "dbknee_plot_cylinder_cs_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <dbknee/dbknee_coord.h>
#include <dbgl/algo/dbgl_intersect.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>

#include <vgl/vgl_closest_point.h>
#include <vnl/vnl_math.h>


//: compute intersection of the image plane, the cylinder, and the radial plane 
// at angle theta
vgl_point_3d<double > ellipse_point(const dbknee_cylinder_based_coord& coord,
                                    const vgl_plane_3d<double >& frame_plane,
                                    double theta);

//: Constructor
dbknee_plot_cylinder_cs_process::
dbknee_plot_cylinder_cs_process()
{
  vcl_vector<vcl_string > knee_sides;
  knee_sides.push_back("left");
  knee_sides.push_back("right");

  if( !parameters()->add( "Cylinder CS file <cs_file.txt>" , 
    "-cs_file", bpro1_filepath("",".txt")) ||
    !parameters()->add( "Band width ratio" , 
    "-band_width_ratio", double(0.2) ) ||
    !parameters()->add("Left or Right knee?: " , 
    "-left_right" , knee_sides, 0) ||
    !parameters()->add( "Frame offset (compared to order in cartilage frames" , 
    "-frame_offset", int(0) ) 
    //||
    //!parameters()->add( "End angle (positive number, in degree)" , 
    //"-end_angle", double(130) )    
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_plot_cylinder_cs_process::
~dbknee_plot_cylinder_cs_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_plot_cylinder_cs_process::
clone() const
{
  return new dbknee_plot_cylinder_cs_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_plot_cylinder_cs_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("image");
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_plot_cylinder_cs_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("vsol2D");
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_plot_cylinder_cs_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbknee_plot_cylinder_cs_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbknee_plot_cylinder_cs_process::
execute()
{
  bpro1_filepath cs_file;
  parameters()->get_value( "-cs_file" , cs_file );

  double band_width_ratio;
  parameters()->get_value( "-band_width_ratio" , band_width_ratio );

  unsigned int left_right = 100; // = 0: left, = 1: right
  parameters()->get_value( "-left_right" , left_right);
  
  
  int frame_offset;
  parameters()->get_value( "-frame_offset" , frame_offset );

  //double end_angle;
  //parameters()->get_value( "-end_angle" , end_angle );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // change the values of start and end angles depending on left or right knee
  // left: postive angle
  // right: negative angle
  double angle_sign = 1;
  if (left_right == 1)
  {
    // right side: invert the sign
    angle_sign = -1;
    //double temp = start_angle;
    //start_angle = -end_angle;
    //end_angle = -temp;
  }
  else if (left_right != 0)
  {
    // unknown option:
    vcl_cout << "ERROR: Unknown knee side option.\n";
    return false;
  }

  // load the coordinate system
  dbknee_cylinder_based_coord coord;
  coord.load_from_cs_file(cs_file.path);

  double dx = 0.3125;
  double dy = 0.3125;
  double dz = 1.5;
  // Describe the current image frame (in world coordinate)
  vgl_point_3d<double > frame_orig(0, 0, (frame_image->frame()+frame_offset) * dz);
  vgl_plane_3d<double > frame_plane(vgl_vector_3d<double >(0, 0, 1), frame_orig);

  // compute intersection of the cylinder and the image plane
  vgl_point_3d<double > ellipse_center;
  vgl_vector_3d<double > ellipse_major_axis;
  vgl_vector_3d<double > ellipse_minor_axis;

  bool intersect = dbgl_intersect::plane_infinite_cylinder(frame_plane, coord.cylinder(),
    ellipse_center, ellipse_major_axis, ellipse_minor_axis);

  // project the 3D ellipse down to the image plane and scale it down with voxel size
  vgl_point_2d<double > center(ellipse_center.x() / dx, ellipse_center.y() / dy);
  vgl_vector_2d<double > major(ellipse_major_axis.x() / dx, ellipse_major_axis.y() / dy);
  vgl_vector_2d<double > minor(ellipse_minor_axis.x() / dx, ellipse_minor_axis.y() / dy);

  vsol_conic_2d_sptr conic = new vsol_conic_2d(vsol_point_2d(center), 
    major.length(), minor.length(), signed_angle(vgl_vector_2d<double >(1, 0), major) );

  
  // determine center, start, and end point, depending on the angle
  vgl_point_3d<double > point1 = ellipse_point(coord, frame_plane, 0);
  vgl_point_3d<double > point2 = ellipse_point(coord, frame_plane, 40.0 * angle_sign);
  vgl_point_3d<double > point3 = ellipse_point(coord, frame_plane, 70.0 * angle_sign);
  vgl_point_3d<double > point4 = ellipse_point(coord, frame_plane, 100.0 * angle_sign);
  vgl_point_3d<double > point5 = ellipse_point(coord, frame_plane, 130.0 * angle_sign);


  // convert points of the ellipse to points on the image
  vsol_point_2d_sptr pc = new vsol_point_2d(center);
  vsol_point_2d_sptr p1 = new vsol_point_2d(point1.x()/dx, point1.y()/dy);
  vsol_point_2d_sptr p2 = new vsol_point_2d(point2.x()/dx, point2.y()/dy);
  vsol_point_2d_sptr p3 = new vsol_point_2d(point3.x()/dx, point3.y()/dy);
  vsol_point_2d_sptr p4 = new vsol_point_2d(point4.x()/dx, point4.y()/dy);
  vsol_point_2d_sptr p5 = new vsol_point_2d(point5.x()/dx, point5.y()/dy);

  double theta0 = conic->ellipse_angular_position(p1);
  double theta5 = conic->ellipse_angular_position(p5);
  vsol_point_2d_sptr start = new vsol_point_2d(center + vcl_cos(theta0)*major + vcl_sin(theta0)*minor);
  vsol_point_2d_sptr end = new vsol_point_2d(center + vcl_cos(theta5)*major + vcl_sin(theta5)*minor);

  if (angle_sign < 0)
  {
    conic->set_p0(start);
    conic->set_p1(end);
  }
  else
  {
    conic->set_p0(end);
    conic->set_p1(start);
  }


  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_object(conic->cast_to_spatial_object());
  //output_vsol->add_object(new vsol_line_2d(pc, p1));
  output_vsol->add_object(new vsol_line_2d(pc, p2));
  output_vsol->add_object(new vsol_line_2d(pc, p3));
  output_vsol->add_object(new vsol_line_2d(pc, p4));
  output_vsol->add_object(new vsol_line_2d(pc, p5));

  output_data_[0].push_back(output_vsol);



  return true;
}


// ----------------------------------------------------------------------------
bool dbknee_plot_cylinder_cs_process::
finish()
{
  return true;
}




//: compute intersection of the image plane, the cylinder, and the radial plane 
// at angle theta
vgl_point_3d<double > ellipse_point(const dbknee_cylinder_based_coord& coord,
              const vgl_plane_3d<double >& frame_plane,
              double theta)
{
  theta = theta / 180 * vnl_math::pi;
  vgl_cylinder<double > cylinder = coord.cylinder();
  double x0 = cylinder.radius()*vcl_cos(theta);
  vgl_point_3d<double > p1a(x0, 1, 1);
  vgl_point_3d<double > p1b(x0, 0, 1);
  vgl_point_3d<double > p1c(x0, 1, 0);
  vgl_plane_3d<double > plane1(coord.local_to_wcs(p1a), 
    coord.local_to_wcs(p1b), 
    coord.local_to_wcs(p1c));

  double y0 = cylinder.radius()*vcl_sin(theta);
  vgl_point_3d<double > p2a(1, y0, 1);
  vgl_point_3d<double > p2b(0, y0, 1);
  vgl_point_3d<double > p2c(1, y0, 0);
  vgl_plane_3d<double > plane2(coord.local_to_wcs(p2a), 
    coord.local_to_wcs(p2b), 
    coord.local_to_wcs(p2c) );

  vgl_point_3d<double > intersection_point(plane1, plane2, frame_plane);
  return intersection_point;
}

