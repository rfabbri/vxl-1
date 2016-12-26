#ifndef proc_io_filter_xml_parser_h_
#define proc_io_filter_xml_parser_h_

//: 
// \file     proc_io_filter_xml_parser.h
//
// \brief    this class is a parser for filter response xml files.
//           it extracts the parameters proc sets while running filters
//           see bioproc_example_xml for how these xml elements are written
//
// \author   Gamze Tunali
// \date     2006-01-03
// 

#include <expatpp/expatpplib.h>
#if defined(APPLE)
#include <expat.h>
#endif
#include <stdio.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>


#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_quaternion.h>
#include <xmvg/xmvg_filter_response.h>
#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <biob/biob_explicit_worldpt_roster.h>
#include <biob/biob_worldpt_field.h>

class proc_io_filter_xml_parser : public expatpp {
public:
  proc_io_filter_xml_parser(void):version(2) {};
  ~proc_io_filter_xml_parser(void){};
  vcl_vector<xmvg_filter_response<double> > responses() { return responses_; }
  vcl_vector<vgl_vector_3d<double> > filter_orientations() { return orientations_; }
  xmvg_composite_filter_descriptor composite_filter_descr();
  biob_worldpt_field<xmvg_filter_response<double> > splr_response_field() {return splr_response_field_; };
  biob_worldpt_field<vnl_quaternion<double> > splr_rotation_field() {return splr_rotation_field_; }
  vgl_box_3d<double> active_box() { return active_box_; }

  /* will be fixed soon:
  biob_worldpt_field<xmvg_filter_response<double> > worldpt_field();
  */
  int filter_num() {return filter_num_; }
  int dim_x() {return response_dimx_; }
  int dim_y() {return response_dimy_; }
  int dim_z() {return response_dimz_; }
  double resolution() { return resolution_; }
protected:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

private:
  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void WriteIndent();
  
  int version;

  // filter response paramaters
  vcl_string cdata_;
  vcl_string current_elm;
  int response_dimx_;
  int response_dimy_;
  int response_dimz_;
  int filter_num_;
  int filter_id_;
  double resolution_;
  vcl_vector<xmvg_filter_response<double>  > responses_;
  vcl_vector<vgl_vector_3d<double> > orientations_;
  vgl_box_3d<double> active_box_;

  // common filter descriptor parameters
  vcl_string filter_name;
  vgl_box_3d<double> filter_box;
  vgl_point_3d<double> filter_center;
  vgl_vector_3d<double> filter_orient;
  double filter_rot_angle;
  vnl_vector_fixed<double,3> filter_rot_axis;
  vcl_vector<xmvg_filter_descriptor> descriptor_list;

  // gaussian specific filter parameters
  double filter_sigma_r;
  double filter_sigma_z;

  // no noise specific filter paramaters
  double filter_inner_rad;
  double filter_outer_rad;
  
  // splr related parameters
  //vcl_vector_3d<vgl_point_3d<double> > explicit_worldpt_roster_points;
  vnl_quaternion<double> splr_symmetry_camera0_to_camera1;
  vcl_vector<vnl_quaternion<double> > splr_rotations;
  biob_explicit_worldpt_roster explicit_worldpt_roster;
  vcl_string biob_field_type;
  biob_worldpt_field<xmvg_filter_response<double> > splr_response_field_;
  biob_worldpt_field<vnl_quaternion<double> > splr_rotation_field_;
};

bool parse(vcl_string fname, proc_io_filter_xml_parser& parser);

#endif
