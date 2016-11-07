// This is dbknee/pro/dbknee_surface_points_from_contours_process.h
#ifndef dbknee_surface_points_from_contours_process_h_
#define dbknee_surface_points_from_contours_process_h_

//:
// \file
// \brief A process to compute distance between meshes at every vertex

// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 9, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h> 

#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_3d_sptr.h>

//: Process that builds a shock graph from a vsol polyline 
class dbknee_surface_points_from_contours_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_surface_points_from_contours_process();
  
  //: Destructor
  virtual ~dbknee_surface_points_from_contours_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Surface points from volume segmentation"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function

  //: From a femoral segmentation in way of contour files, extract the surface
  // points for the inner and outer surfaces and save them to .p3d files
  static bool extract_surface_points_from_femoral_cartilage_segm(
    const vcl_string& seg_xml_file,
    double voxel_size[3],
    const vcl_string& inner_p3d_file,
    const vcl_string& outer_p3d_file);

  
  
  //: From a tibial segmentation in way of contour files, extract the surface
  // points for the inner and outer surfaces and save them to .p3d files
  static bool extract_surface_points_from_tibial_cartilage_segm(
    const vcl_string& seg_xml_file,
    double voxel_size[3],
    const vcl_string& inner_p3d_file,
    const vcl_string& outer_p3d_file);

  //: From a segmentation in way of contour files, extract all the surface
  // points and save them to a .p3d file
  static bool extract_surface_points_from_cartilage_segmentation(
    const vcl_string& seg_xml_file,
    double voxel_size[3],
    const vcl_string& all_p3d_file);



  static bool determine_femur_inner_outer_contour(
    const vsol_polyline_2d_sptr& polyline1,
    const vsol_polyline_2d_sptr& polyline2,
    vsol_polyline_2d_sptr& inner_polyline,
    vsol_polyline_2d_sptr& outer_polyline);

  static bool determine_tibia_inner_outer_contour(
    const vsol_polyline_2d_sptr& polyline1,
    const vsol_polyline_2d_sptr& polyline2,
    vsol_polyline_2d_sptr& inner_polyline,
    vsol_polyline_2d_sptr& outer_polyline);

  static bool save_contours_as_p3d(
    const vcl_vector<vsol_polyline_3d_sptr >& contours,
    double spacing[3],
    const char* filename);


  // convert a polyline 2d to polyline 3d, given the z-coordinate
  static vsol_polyline_3d_sptr polyline_2d_to_3d(const vsol_polyline_2d_sptr& poly2d,
    double z);
};

#endif
