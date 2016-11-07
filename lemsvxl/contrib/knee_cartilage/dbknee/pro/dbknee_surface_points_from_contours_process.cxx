// This is dbknee/pro/dbknee_surface_points_from_contours_process.cxx

//:
// \file

#include "dbknee_surface_points_from_contours_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <dbsol/dbsol_file_io.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>

#include <vnl/vnl_math.h>

//: Constructor
dbknee_surface_points_from_contours_process::
dbknee_surface_points_from_contours_process()
{
  if( !parameters()->add( "Volume segmentation file .xml>" , 
    "-xml_file", bpro1_filepath("",".xml")) ||
    !parameters()->add( "Voxel dx = " , 
    "-voxel_dx", double(0.3125) ) ||
    !parameters()->add( "Voxel dy = " , 
    "-voxel_dy", double(0.3125) ) ||
    !parameters()->add( "Voxel dz = " , 
    "-voxel_dz", double(1.5) ) ||
    !parameters()->add( "Save all points to file?" , 
    "-save_all", false ) ||
    !parameters()->add( "File for all surface points .p3d>" , 
    "-all_p3d_file", bpro1_filepath("",".p3d")) ||
    !parameters()->add( "Save inner and outer surfaces separately?" , 
    "-save_inner_outer", false ) ||
    !parameters()->add( "Femur?" , 
    "-is_femur", true ) ||
    !parameters()->add( "File for outer surface points .p3d>" , 
    "-outer_p3d_file", bpro1_filepath("",".p3d")) ||
    !parameters()->add( "File for inner surface points .p3d>" , 
    "-inner_p3d_file", bpro1_filepath("",".p3d"))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbknee_surface_points_from_contours_process::
~dbknee_surface_points_from_contours_process()
{
  
}


//: Clone the process
bpro1_process* dbknee_surface_points_from_contours_process::
clone() const
{
  return new dbknee_surface_points_from_contours_process(*this);
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbknee_surface_points_from_contours_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbknee_surface_points_from_contours_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbknee_surface_points_from_contours_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbknee_surface_points_from_contours_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbknee_surface_points_from_contours_process::
execute()
{
  bpro1_filepath xml_file;
  parameters()->get_value( "-xml_file" , xml_file );

  bpro1_filepath inner_p3d_file;
  parameters()->get_value( "-inner_p3d_file" , inner_p3d_file );

  bpro1_filepath outer_p3d_file;
  parameters()->get_value( "-outer_p3d_file" , outer_p3d_file );

  double voxel_dx;
  parameters()->get_value( "-voxel_dx" , voxel_dx );

  double voxel_dy;
  parameters()->get_value( "-voxel_dy" , voxel_dy );

  double voxel_dz;
  parameters()->get_value( "-voxel_dz" , voxel_dz );

  bool save_all = false;
  parameters()->get_value( "-save_all" , save_all );

  bpro1_filepath all_p3d_file;
  parameters()->get_value( "-all_p3d_file" , all_p3d_file );

  bool save_inner_outer = false;
  parameters()->get_value( "-save_inner_outer" , save_inner_outer );

  bool is_femur = false;
  parameters()->get_value( "-is_femur" , is_femur );
  
  double voxel_size[3];
  voxel_size[0] = voxel_dx;
  voxel_size[1] = voxel_dy;
  voxel_size[2] = voxel_dz;

  if (save_all)
  {
    dbknee_surface_points_from_contours_process::
      extract_surface_points_from_cartilage_segmentation(
    xml_file.path, voxel_size,
    all_p3d_file.path);
  }


  if (save_inner_outer)
  {
    if (is_femur)
    {
      dbknee_surface_points_from_contours_process::
        extract_surface_points_from_femoral_cartilage_segm(
        xml_file.path, voxel_size,
        inner_p3d_file.path, outer_p3d_file.path);
    }
    else
    {
      dbknee_surface_points_from_contours_process::
        extract_surface_points_from_tibial_cartilage_segm(
        xml_file.path, voxel_size,
        inner_p3d_file.path, outer_p3d_file.path);
    
    }
  }

 
  return true;
}

bool dbknee_surface_points_from_contours_process::
finish()
{
  return true;
}













//: From a segmentation in way of contour files, extract all the surface
// points and save them to a .p3d file
bool dbknee_surface_points_from_contours_process::
extract_surface_points_from_cartilage_segmentation(
  const vcl_string& seg_xml_file,
  double voxel_size[3],
  const vcl_string& all_p3d_file)
{
  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();
  //: parse an xml file
  vcl_cout << "Parse the volume segmentation file.\n";
  x_read(seg_xml_file, seg3d);



  vcl_cout << "Load the contours ...";

  // place holder for the contours
  vcl_vector<vsol_polyline_3d_sptr > all_contours;

  for (int frame_index=0; frame_index<seg3d->num_frames(); ++frame_index)
  {
    dbdet_seg3d_info_frame frame = seg3d->frame(frame_index);
    
    // ignore frame with no contours
    if (frame.contour_file_list.empty()) 
      continue;

    // load the polylines

    // load contours in the frame
    vcl_vector<vsol_spatial_object_2d_sptr > contour_list;
    contour_list.reserve(frame.contour_file_list.size());
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      // full filename
      vcl_string contour_filename = seg3d->contour_folder() + 
        "/" + frame.contour_file_list[k];

      // load the contour
      vsol_spatial_object_2d_sptr contour = dbsol_load_con_file(
        contour_filename.c_str());

      if (! contour)
      {
        vcl_cerr << "Failed to load contour file" << contour_filename << vcl_endl;
        return false;
      }
      contour_list.push_back(contour);
    }

    
    // extract the polylines
    vcl_vector<vsol_polyline_2d_sptr > poly_list;
    for (unsigned int m=0; m<contour_list.size(); ++m)
    {
      vsol_spatial_object_2d_sptr obj = contour_list[m];
      if (obj->cast_to_curve() && obj->cast_to_curve()->cast_to_polyline())
      {
        poly_list.push_back(obj->cast_to_curve()->cast_to_polyline());
      }
      else
      {
        vcl_cout << "contour is not polyline. Skip contour.\n";
        continue;
      }
    }

    // add to the overall list
    for (unsigned m=0; m < poly_list.size(); ++m)
    {
      vsol_polyline_3d_sptr poly3d = 
        dbknee_surface_points_from_contours_process::polyline_2d_to_3d(
        poly_list[m], frame_index);
      all_contours.push_back(poly3d);
    }
  }

  vcl_cout << "Writing the two point cloud files... \n";

  // output the two point clouds
  double spacing[] = {0.3125, 0.3125, 1.5};

  vcl_cout << "Writing all the contours to: " << all_p3d_file << vcl_endl;
  dbknee_surface_points_from_contours_process::
    save_contours_as_p3d(all_contours, spacing, all_p3d_file.c_str());

  vcl_cout << "Done.\n";

  return true;
}










//: From a femoral segmentation in way of contour files, extract the surface
// points for the inner and outer surfaces and save them to .p3d files
bool dbknee_surface_points_from_contours_process::extract_surface_points_from_femoral_cartilage_segm(
  const vcl_string& seg_xml_file,
  double voxel_size[3],
  const vcl_string& inner_p3d_file,
  const vcl_string& outer_p3d_file)
{
  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();
  //: parse an xml file
  vcl_cout << "Parse the volume segmentation file.\n";
  x_read(seg_xml_file, seg3d);



  vcl_cout << "Load and sort the contours into top and bot... ";

  // place holder for the contours
  vcl_vector<vsol_polyline_3d_sptr > outer_contours;
  vcl_vector<vsol_polyline_3d_sptr > inner_contours;

  for (int frame_index=0; frame_index<seg3d->num_frames(); ++frame_index)
  {
    dbdet_seg3d_info_frame frame = seg3d->frame(frame_index);
    
    // ignore frame with no contours
    if (frame.contour_file_list.empty()) 
      continue;

    // load the polylines

    // load contours in the frame
    vcl_vector<vsol_spatial_object_2d_sptr > contour_list;
    contour_list.reserve(frame.contour_file_list.size());
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      // full filename
      vcl_string contour_filename = seg3d->contour_folder() + 
        "/" + frame.contour_file_list[k];

      // load the contour
      vsol_spatial_object_2d_sptr contour = dbsol_load_con_file(
        contour_filename.c_str());

      if (! contour)
      {
        vcl_cerr << "Failed to load contour file" << contour_filename << vcl_endl;
        return false;
      }
      contour_list.push_back(contour);
    }

    
    // extract the polylines
    vcl_vector<vsol_polyline_2d_sptr > poly_list;
    for (unsigned int m=0; m<contour_list.size(); ++m)
    {
      vsol_spatial_object_2d_sptr obj = contour_list[m];
      if (obj->cast_to_curve() && obj->cast_to_curve()->cast_to_polyline())
      {
        poly_list.push_back(obj->cast_to_curve()->cast_to_polyline());
      }
      else
      {
        vcl_cout << "contour is not polyline. Skip contour.\n";
        continue;
      }
    }

    if ( poly_list.size() != 2)
    {
      vcl_cerr << "ERROR. Number of polyline in a frame is not 2. Ignore frame\n";
      vcl_cerr << "image name = " << frame.image_file << vcl_endl;
      break;
    }

    // Classify the polylines into inner and outer

    vsol_polyline_2d_sptr inner_polyline;
    vsol_polyline_2d_sptr outer_polyline;
    dbknee_surface_points_from_contours_process::determine_femur_inner_outer_contour(
      poly_list[0], poly_list[1], inner_polyline, outer_polyline);

    
    // save the classified contours
    outer_contours.push_back(
      dbknee_surface_points_from_contours_process::polyline_2d_to_3d(
      outer_polyline, frame_index));
    inner_contours.push_back(
      dbknee_surface_points_from_contours_process::polyline_2d_to_3d(
      inner_polyline, frame_index));
  }

  vcl_cout << "Writing the two point cloud files... \n";

  // output the two point clouds
  //double spacing[] = {0.3125, 0.3125, 1.5};

  vcl_cout << "Writing the inner contours to: " << inner_p3d_file << vcl_endl;
  dbknee_surface_points_from_contours_process::
    save_contours_as_p3d(inner_contours, voxel_size, inner_p3d_file.c_str());


  vcl_cout << "Writing the outer contours to: " << inner_p3d_file << vcl_endl;
  dbknee_surface_points_from_contours_process::
    save_contours_as_p3d(outer_contours, voxel_size, outer_p3d_file.c_str());

  vcl_cout << "Done.\n";

  return true;
}
  


bool dbknee_surface_points_from_contours_process::
determine_femur_inner_outer_contour(const vsol_polyline_2d_sptr& polyline1,
                              const vsol_polyline_2d_sptr& polyline2,
                              vsol_polyline_2d_sptr& inner_polyline,
                              vsol_polyline_2d_sptr& outer_polyline)
{
  // Determine inner and outer using moment moment of innertia
  
  // First determine the CG of both contours
  double x_sum = 0;
  double y_sum = 0;
  for (unsigned i=0; i<polyline1->size(); ++i)
  {
    vsol_point_2d_sptr pt = polyline1->vertex(i);
    x_sum += pt->x();
    y_sum += pt->y();
  }

  for (unsigned i=0; i<polyline2->size(); ++i)
  {
    vsol_point_2d_sptr pt = polyline2->vertex(i);
    x_sum += pt->x();
    y_sum += pt->y();
  }

  int num_pts = polyline1->size() + polyline2->size();
  vgl_point_2d<double > cg(x_sum/num_pts, y_sum/num_pts);

  // Now compute moment of inertial for each contour separately
  double moment_inertia1 = 0;
  double moment_inertia2 = 0;

  for (unsigned i=0; i<polyline1->size(); ++i)
  {
    vsol_point_2d_sptr pt = polyline1->vertex(i);
    moment_inertia1 += vnl_math_hypot(pt->x()-cg.x(), pt->y()-cg.y());
  }

  for (unsigned i=0; i<polyline2->size(); ++i)
  {
    vsol_point_2d_sptr pt = polyline2->vertex(i);
    moment_inertia2 += vnl_math_hypot(pt->x()-cg.x(), pt->y()-cg.y());
  }

  // Determine inner and outer contour based on their interia momentum
  if (moment_inertia1 < moment_inertia2)
  {
    inner_polyline = polyline1;
    outer_polyline = polyline2;
  }
  else
  {
    inner_polyline = polyline2;
    outer_polyline = polyline1;
  }
  return true;
}






// ----------------------------------------------------------------------------
//: From a tibial segmentation in way of contour files, extract the surface
// points for the inner and outer surfaces and save them to .p3d files
bool dbknee_surface_points_from_contours_process::extract_surface_points_from_tibial_cartilage_segm(
  const vcl_string& seg_xml_file,
  double voxel_size[3],
  const vcl_string& inner_p3d_file,
  const vcl_string& outer_p3d_file)
{
  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();
  //: parse an xml file
  vcl_cout << "Parse the volume segmentation file.\n";
  x_read(seg_xml_file, seg3d);

  vcl_cout << "Load and sort the contours into inner and outer contours... ";

  // place holder for the contours
  vcl_vector<vsol_polyline_3d_sptr > outer_contours;
  vcl_vector<vsol_polyline_3d_sptr > inner_contours;

  for (int frame_index=0; frame_index<seg3d->num_frames(); ++frame_index)
  {
    dbdet_seg3d_info_frame frame = seg3d->frame(frame_index);
    
    // ignore frame with no contours
    if (frame.contour_file_list.empty()) 
      continue;

    // load the polylines

    // load contours in the frame
    vcl_vector<vsol_spatial_object_2d_sptr > contour_list;
    contour_list.reserve(frame.contour_file_list.size());
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      // full filename
      vcl_string contour_filename = seg3d->contour_folder() + 
        "/" + frame.contour_file_list[k];

      // load the contour
      vsol_spatial_object_2d_sptr contour = dbsol_load_con_file(
        contour_filename.c_str());

      if (! contour)
      {
        vcl_cerr << "Failed to load contour file" << contour_filename << vcl_endl;
        return false;
      }
      contour_list.push_back(contour);
    }

    
    // extract the polylines
    vcl_vector<vsol_polyline_2d_sptr > poly_list;
    for (unsigned int m=0; m<contour_list.size(); ++m)
    {
      vsol_spatial_object_2d_sptr obj = contour_list[m];
      if (obj->cast_to_curve() && obj->cast_to_curve()->cast_to_polyline())
      {
        poly_list.push_back(obj->cast_to_curve()->cast_to_polyline());
      }
      else
      {
        vcl_cout << "contour is not polyline. Skip contour.\n";
        continue;
      }
    }

    if ( poly_list.size() != 2)
    {
      vcl_cerr << "ERROR. Number of polyline in a frame is not 2. Ignore frame\n";
      vcl_cerr << "image name = " << frame.image_file << vcl_endl;
      break;
    }

    // Classify the polylines into inner and outer

    vsol_polyline_2d_sptr inner_polyline;
    vsol_polyline_2d_sptr outer_polyline;
    dbknee_surface_points_from_contours_process::determine_tibia_inner_outer_contour(
      poly_list[0], poly_list[1], inner_polyline, outer_polyline);

    
    // save the classified contours
    outer_contours.push_back(
      dbknee_surface_points_from_contours_process::polyline_2d_to_3d(
      outer_polyline, frame_index));
    inner_contours.push_back(
      dbknee_surface_points_from_contours_process::polyline_2d_to_3d(
      inner_polyline, frame_index));
  }

  vcl_cout << "Writing the two point cloud files... \n";

  // output the two point clouds
  //double spacing[] = {0.3125, 0.3125, 1.5};

  vcl_cout << "Writing the inner contours to: " << inner_p3d_file << vcl_endl;
  dbknee_surface_points_from_contours_process::
    save_contours_as_p3d(inner_contours, voxel_size, inner_p3d_file.c_str());


  vcl_cout << "Writing the outer contours to: " << inner_p3d_file << vcl_endl;
  dbknee_surface_points_from_contours_process::
    save_contours_as_p3d(outer_contours, voxel_size, outer_p3d_file.c_str());

  vcl_cout << "Done.\n";

  return true;



}





bool dbknee_surface_points_from_contours_process::
determine_tibia_inner_outer_contour(const vsol_polyline_2d_sptr& polyline1,
                                    const vsol_polyline_2d_sptr& polyline2,
                                    vsol_polyline_2d_sptr& inner_polyline,
                                    vsol_polyline_2d_sptr& outer_polyline)
{

  // Determine inner and outer using centroid location along the y-direction
  // of the contour segments (not the points)
  // y-coordinate : inner contour > outer contour

  // first contour
  double y_moment_sum = 0;
  double w_sum = 0;
  for (unsigned i=0; i<polyline1->size()-1; ++i)
  {
    vsol_point_2d_sptr p1 = polyline1->vertex(i);
    vsol_point_2d_sptr p2 = polyline1->vertex(i+1);

    double w = vcl_abs(p2->x() - p1->x()); 
    double y = p1->middle(*p2)->y();

    y_moment_sum += y* w;
    w_sum += w;
  }
  double y_centroid1 = y_moment_sum / w_sum;


  // second contour
  y_moment_sum = 0;
  w_sum = 0;
  for (unsigned i=0; i<polyline2->size()-1; ++i)
  {
    vsol_point_2d_sptr p1 = polyline2->vertex(i);
    vsol_point_2d_sptr p2 = polyline2->vertex(i+1);

    double w = vcl_abs(p2->x() - p1->x()); 
    double y = p1->middle(*p2)->y();

    y_moment_sum += y* w;
    w_sum += w;
  }
  double y_centroid2 = y_moment_sum / w_sum;

  // Determine inner and outer contour based on their interia momentum
  if (y_centroid1 > y_centroid2)
  {
    inner_polyline = polyline1;
    outer_polyline = polyline2;
  }
  else
  {
    inner_polyline = polyline2;
    outer_polyline = polyline1;
  }
  return true;
}



// convert a polyline 2d to polyline 3d, given the z-coordinate
vsol_polyline_3d_sptr dbknee_surface_points_from_contours_process::
polyline_2d_to_3d(const vsol_polyline_2d_sptr& poly2d, double z)
{
  vcl_vector<vsol_point_3d_sptr > vertices;
  vertices.reserve(poly2d->size());
  for (unsigned i=0; i<poly2d->size(); ++i)
  {
    vsol_point_2d_sptr p = poly2d->vertex(i);
    vertices.push_back(new vsol_point_3d(p->x(), p->y(), z));
  }
  return new vsol_polyline_3d(vertices);
}


//: save a set of 3d polylines as a point cloud
bool dbknee_surface_points_from_contours_process::
save_contours_as_p3d(const vcl_vector<vsol_polyline_3d_sptr >& contours,
                     double spacing[3],
                     const char* filename)
{
  // compute total number of points
  int num_pts = 0;
  for (unsigned int i=0; i<contours.size(); ++i)
  {
    if (!contours[i]) continue;
    num_pts += contours[i]->size();
  }

  // write data to file
  vcl_ofstream outfp(filename, vcl_ios_out);
  outfp << "3" << vcl_endl
    << num_pts << vcl_endl;

  double x_spacing = spacing[0];
  double y_spacing = spacing[1];
  double z_spacing = spacing[2];
  for (unsigned int i=0; i<contours.size(); ++i)
  {
    vsol_polyline_3d_sptr polyline = contours[i];
    if (!polyline) continue;

    vcl_vector<vgl_point_2d<double > > curve;
    curve.reserve(polyline->size());
    for (unsigned k=0; k<polyline->size(); ++k)
    {
      vsol_point_3d_sptr p = polyline->vertex(k);
      double x = p->x() * x_spacing;
      double y = p->y() * y_spacing;
      double z = p->z() * z_spacing;
      outfp << x << " " << y << " " << z << vcl_endl;
    }
  }

  outfp.close();

  return true;
}

