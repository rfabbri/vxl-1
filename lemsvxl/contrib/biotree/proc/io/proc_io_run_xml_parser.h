#ifndef proc_io_run_xml_parser_h_
#define proc_io_run_xml_parser_h_

#include <expatpp/expatpplib.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <vector>
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

  std::string log() { return log_file_; }
  std::string scan() {return scan_file_; }
  std::string output_file() {return output_file_; }
  std::string box() { return box_file_; }
  double filter_radius() { return filter_radius_; }
  double filter_length() { return filter_length_; }
  double res() { return resolution_; }
  std::vector<vgl_vector_3d<double> > filter_orient() { return orientations_; }
  PROC_FILTER_TYPE filter_type() { return  filter_type_; }
  PROC_SPLAT_TYPE splatting_type() { return splatting_type_; }
  PROC_ALGO_TYPE algo_type() { return algo_type_; }

private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(std::string name, std::string data);
  void WriteIndent();
  
  //Data
  int mDepth;
  std::string cdata;
  std::string last_tag;

  std::string log_file_;
  std::string scan_file_;
  std::string output_file_;
  std::string box_file_;
  double scale_x_;
  double scale_y_;
  double scale_z_;;
  double filter_radius_;
  double filter_length_;
  double resolution_;
  std::vector<vgl_vector_3d<double> > orientations_;
  PROC_FILTER_TYPE filter_type_;
  PROC_SPLAT_TYPE splatting_type_;
  PROC_ALGO_TYPE algo_type_;
};

#endif
