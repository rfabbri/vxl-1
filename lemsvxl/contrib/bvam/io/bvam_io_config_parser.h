#ifndef bvam_io_config_parser_h_
#define bvam_io_config_parser_h_

#include "bvam_io_structs.h"

#include <expatpp/expatpplib.h>
#include <vgl/vgl_point_3d.h>

#include <string>
#include <vector>
#include <utility>

class bvam_io_config_parser : public expatpp
{
 public:
  bvam_io_config_parser(void);
  ~bvam_io_config_parser(void) {}

  // getters for the parsed parameters
  std::string model_dir() { return model_dir_; }
  float voxel_length() { return voxel_length_; }
  vgl_vector_3d<unsigned int> voxel_dim() { return voxel_dim_; }
  vgl_point_3d<float> corner() { return corner_; }
  bool norm_intenst() { return norm_intenst_; }
  std::string appear_model() { return appear_model_; }
  int appear_num_modes() { return appear_num_modes_; }

  std::vector<bvam_io_process*> processes() { return processes_; }


 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(std::string name, std::string data);
  void init_params();

  //Data
  int mDepth;
  std::string cdata;
  std::string last_tag;

  // model parameters
  std::string model_dir_;
  float voxel_length_;
  vgl_vector_3d<unsigned int> voxel_dim_;
  vgl_point_3d<float> corner_;
  bool norm_intenst_;
  std::string appear_model_;
  int appear_num_modes_;

  // intermediate variables to keep values during parsing
  std::string image_path_;
  std::string camera_path_;
  std::string view_camera_path_;
  int view_camera_index_;
  std::string light_path_;
  std::string output_path_;
  std::string order_by_date_, shuffle_;
  bool sch_subset_;
  unsigned start_, end_, inc_;
  bvam_io_schedule* sch_;
  bvam_io_schedule* view_sch_;
  int xint_, yint_;
  float X_, Y_, Z_;
  std::vector<bvam_io_process *> processes_;

  void trim_string(std::string& s);
};

#endif
