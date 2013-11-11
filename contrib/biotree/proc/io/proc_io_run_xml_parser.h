#ifndef proc_io_run_xml_parser_h_
#define proc_io_run_xml_parser_h_

#include <expatpp/expatpplib.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vgl/vgl_vector_3d.h>

typedef enum x {GAUSSIAN, NO_NOISE, EDGE_DETECTOR_X, EDGE_DETECTOR_Y, EDGE_DETECTOR_Z, FILTER_UNDEF} PROC_FILTER_TYPE;
typedef enum y {PARALLEL, CONE_BEAM, SPLAT_UNDEF} PROC_SPLAT_TYPE;
typedef enum z {PROC, PROC_SPLR, ALGO_UNDEF} PROC_ALGO_TYPE;

class proc_io_run_xml_parser : public expatpp {
public:
  
  proc_io_run_xml_parser(void);
  ~proc_io_run_xml_parser(void){};

  // scale is deprecated
  double scale_x() { return scale_x_; }
  double scale_y() { return scale_y_; }
  double scale_z() { return scale_z_; }

  vcl_string log() { return log_file_; }
  vcl_string scan() {return scan_file_; }
  vcl_string output_file() {return output_file_; }
  vcl_string box() { return box_file_; }
  double filter_radius() { return filter_radius_; }
  double filter_length() { return filter_length_; }
  double res() { return resolution_; }
  vcl_vector<vgl_vector_3d<double> > filter_orient() { return orientations_; }
  PROC_FILTER_TYPE filter_type() { return  filter_type_; }
  PROC_SPLAT_TYPE splatting_type() { return splatting_type_; }
  PROC_ALGO_TYPE algo_type() { return algo_type_; }

private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void WriteIndent();
  
  //Data
  int mDepth;
  vcl_string cdata;
  vcl_string last_tag;

  vcl_string log_file_;
  vcl_string scan_file_;
  vcl_string output_file_;
  vcl_string box_file_;
  double scale_x_;
  double scale_y_;
  double scale_z_;;
  double filter_radius_;
  double filter_length_;
  double resolution_;
  vcl_vector<vgl_vector_3d<double> > orientations_;
  PROC_FILTER_TYPE filter_type_;
  PROC_SPLAT_TYPE splatting_type_;
  PROC_ALGO_TYPE algo_type_;
};

#endif
