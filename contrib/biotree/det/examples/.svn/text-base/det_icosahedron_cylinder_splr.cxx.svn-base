//: 
// \file  det_hair_nonmax_sup_from_response_example.cxx
// \brief  an example doing the nonmaxium suppression on filter response of hair
//         the difference of this example from det_hair_nonmax_sup_example.cxx
//         is, those one takes the filter response file detects the hair and do the
//         non maxima suppresion on it and displays the result
// \author    Kongbin Kang (kk at lems.brown.edu) & Gamze Tunali
// \date        2005-11-01
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_roster_to_grid_mapping.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_suppression_and_interpolation.h>
#include <det/det_cylinder_detect.h>
#include <xmvg/xmvg_icosafilter_response.h>

#include <proc/io/proc_io_filter_xml_parser.h>
#include <vsol/vsol_cylinder.h>
#include <geom/geom_index_structure.h>
#include <geom/geom_rectangular_probe_volume.h>
#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

/*
double get_double(char* str){
  double x;
  vcl_stringstream arg(str);
  arg >> x;
  return x;
}
*/

vgl_vector_3d<double> apply_quaternion(vnl_quaternion<double> rotation, vgl_vector_3d<double> initial_orientation){
  vnl_double_3 vnl_orientation(initial_orientation.x(), initial_orientation.y(), initial_orientation.z());
  vnl_double_3 final_orientation = rotation.rotate(vnl_orientation);
  vgl_vector_3d<double> to_return(final_orientation[0], final_orientation[1], final_orientation[2]);
  return to_return;
}

const double CYL_RADIUS = 0.5;
const double CYL_LENGTH = 1.0;


/*
double strength(xmvg_filter_response<double> response){
  double max_response_value = 0.;
  for (int i = 0; i < response.size(); ++i){
    max_response_value = vcl_max(vcl_abs(response[i]), max_response_value);
  }
  return max_response_value;
}
*/

vgl_vector_3d<double> estimate_orientation_and_strength(xmvg_filter_response<double> response, vcl_vector<vgl_vector_3d<double> > filter_orientations){
  det_cylinder_detect detector;
  vcl_vector<vgl_vector_3d<double> > v = detector.compute_v(response, filter_orientations);
  vgl_vector_3d<double> direction = detector.detect_dir(response, v, filter_orientations, true);
  return direction; //I modified detect_dir so it gives a strength when last parameter = true
}

int main(int argc, char *argv[])
{
  if(argc != 3){
    vcl_cout << "Usage: "<< argv[0] << " response_file binary_cylinder_filename\n";
    return 1;
  }

  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "failed to load response file\n";
    return 1;
  }
  double resolution = parser.resolution(); //get_double(argv[2]); 
  //for visualization:
  biob_worldpt_box active_box = parser.active_box();
  biob_grid_worldpt_roster grid(active_box, resolution);

  biob_worldpt_field<xmvg_filter_response<double> > response_field = parser.splr_response_field();
  biob_worldpt_field<vgl_vector_3d<double> > orientation_field(response_field.roster());
  biob_worldpt_field<vnl_quaternion<double> > rotation_field = parser.splr_rotation_field();
  for (unsigned i = 0; i < orientation_field.roster()->num_points(); ++i){
    //for each point, estimate orientation and strength
    //this procedure includes strength
    vgl_vector_3d<double> estimated_orientation = estimate_orientation_and_strength(response_field.values()[i], parser.filter_orientations());
    orientation_field.values()[i] = apply_quaternion(rotation_field.values()[i], estimated_orientation);
  }
  det_suppression_and_interpolation interpolator;
  double strength_threshold = 0; //1000000.0 NOT SURE WHAT THIS VALUE SHOULD BE
  /* IN FOLLOWING, CHANGED THIRD PARAMETER FROM .5*resolution TO .53*resolution, WHICH FIXED A PROBLEM IN 
     WHICH IN INTERPOLATION, A GOOD POINT WASN'T GETTING ENOUGH NEARBY POINTS TO INTERPOLATE FROM.
     THE PARAMETER IS height, WHICH IS MAXIMUM ALLOWED DISTANCE OF A POINT p FROM ITS PROJECTION IN THE PLANE OF p0.
     REMARKABLY, THERE WERE SEVERAL POINTS WHOSE DISTANCE WAS .013143...  WHY THIS NUMBER?
  */
  biob_worldpt_field<vgl_vector_3d<double> > interpolated_responses = interpolator.apply(orientation_field, 1.5*resolution, .53*resolution, 0, strength_threshold);
  
  //Write the cylinders to a stream
  vsl_b_ofstream stream(argv[2]);
  // write the version number
  vsl_b_write(stream, (int) 1);
  // write the number of cylinders
  vsl_b_write(stream, (int) interpolated_responses.roster()->num_points());
  // write the cylinders
  for (unsigned i = 0; i < interpolated_responses.roster()->num_points(); ++i){
  // set center, radius, length and orientation
    vgl_point_3d<double> location_in_world_coordinates = interpolated_responses.roster()->point(biob_worldpt_index(i));
    vgl_point_3d<double> box_base = active_box.min_point();
    //need to convert to coordinates within grid
    vgl_vector_3d<double> location_in_grid_coordinates = (1/resolution) * (location_in_world_coordinates - box_base);
    vgl_point_3d<double> location_in_grid_coordinates_pt(location_in_grid_coordinates.x(),location_in_grid_coordinates.y(),location_in_grid_coordinates.z());
    vsol_cylinder cylinder(
                           location_in_grid_coordinates_pt,
                           CYL_RADIUS, CYL_LENGTH);
    vgl_vector_3d<double> response = interpolated_responses.values()[i];
    vgl_vector_3d<double> normalized_response = response;//apparently the normalize function is mutative, so make a copy
    cylinder.set_orientation(normalize(normalized_response));
    double strength = response.length();
    // first write the strength
    vsl_b_write(stream, (double) strength);
    cylinder.b_write(stream);
  }
  stream.close();

}
