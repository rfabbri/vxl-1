//:
// \file
// \brief Parses the configuration file for bvam tool.
//
#include "bvam_io_config_parser.h"
#include "bvam_io_structs.h"

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
  strm >> d;
}

bvam_io_config_parser::bvam_io_config_parser()
{
  init_params();
}

void bvam_io_config_parser::init_params()
{
}

void
bvam_io_config_parser ::cdataHandler(vcl_string name, vcl_string data)
{
  // clean up the empty chars before and after the file paths
  trim_string(data);
  if (name.compare(BVAM_MODEL_DIR_TAG) == 0 ) {
    model_dir_.assign(data);
  } else if (name.compare(BWAM_IMAGES_PATH_TAG) == 0 ) {
    image_path_.assign(data);
  } else if (name.compare(BWAM_CAMERAS_PATH_TAG) == 0 ) {
    camera_path_.assign(data);
  } else if (name.compare(BWAM_LIGHTS_PATH_TAG) == 0 ) {
    light_path_.assign(data);
  } else if (name.compare(BWAM_OUTPUT_PATH_TAG) == 0 ) {
    output_path_.assign(data);
  } else if (name.compare(BWAM_VIEW_CAMERA_PATH_TAG) == 0 ) {
    view_camera_path_.assign(data);
  }

  cdata = "";
}

void
bvam_io_config_parser::handleAtts(const XML_Char** atts)
{
}

void
bvam_io_config_parser::startElement(const char* name, const char** atts)
{
  vcl_cout<< "element=" << name << vcl_endl;

  if (vcl_strcmp(name, BVAM_PARAM_CORNER_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "x") == 0)
        convert(atts[i+1], X_);
      else if (vcl_strcmp(atts[i], "y") == 0)
        convert(atts[i+1], Y_);
      else if (vcl_strcmp(atts[i], "z") == 0)
        convert(atts[i+1], Z_);
    }
  } else if (vcl_strcmp(name, BVAM_PARAM_VOXEL_LENGTH_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "l") == 0)
        convert(atts[i+1], voxel_length_);
    }

  } else if (vcl_strcmp(name, BVAM_APPEAR_MODEL_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "m") == 0)
        convert(atts[i+1], appear_model_);
    }
  } else if (vcl_strcmp(name, BVAM_APPEAR_NUM_MODES_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "n") == 0)
        convert(atts[i+1], appear_num_modes_);
    }
  } else if (vcl_strcmp(name, BVAM_NORM_INTENS_TAG) == 0) {
    vcl_string s;
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "status") == 0)
        convert(atts[i+1], s);
      if (vcl_strcmp(atts[i], "false")==0)
        norm_intenst_ = false;
      else if (vcl_strcmp(atts[i], "true")==0)
        norm_intenst_ = true;
      else 
        vcl_cerr << "Undefined status for " << BVAM_NORM_INTENS_TAG << vcl_endl;
    }
  } 
  
  else if (vcl_strcmp(name, BWAM_SCHEDULE_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "order_by_date") == 0)
          convert(atts[i+1], order_by_date_);
      else if (vcl_strcmp(atts[i], "shuffle") == 0)
          convert(atts[i+1], start_);
    }
  }
  else if (vcl_strcmp(name, BWAM_VIEW_SCHEDULE_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "order_by_date") == 0)
          convert(atts[i+1], order_by_date_);
      else if (vcl_strcmp(atts[i], "shuffle") == 0)
          convert(atts[i+1], start_);
    }
  }
  else if (vcl_strcmp(name, BWAM_SCHEDULE_SUBSET_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "start") == 0)
          convert(atts[i+1], order_by_date_);
      else if (vcl_strcmp(atts[i], "end") == 0)
          convert(atts[i+1], end_);
      else if (vcl_strcmp(atts[i], "inc") == 0)
          convert(atts[i+1], inc_);
    }
  }

  else if (vcl_strcmp(name, BVAM_PROCESS_RENDER_EXP_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "x") == 0)
        convert(atts[i+1], xint_);
      else if (vcl_strcmp(atts[i], "y") == 0)
        convert(atts[i+1], yint_);
    }
  } 

  else if (vcl_strcmp(name, BVAM_PROCESS_RENDER_VIEW_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "x") == 0)
        convert(atts[i+1], X_);
      else if (vcl_strcmp(atts[i], "y") == 0)
        convert(atts[i+1], Y_);
    }
  } 

  else if (vcl_strcmp(name, BWAM_VIEW_CAMERA_PATH_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (vcl_strcmp(atts[i], "index") == 0)
        convert(atts[i+1], view_camera_index_);
    }
  }
}

void
bvam_io_config_parser::endElement(const char* name)
{
   // first check if the last element has some cdata
   if (cdata.size() > 0) {
     cdataHandler(name, cdata);
     cdata= "";
   }

   if (vcl_strcmp(name, BVAM_PARAM_CORNER_TAG) == 0) {
     corner_.set(X_, Y_, Z_);
   } else if (vcl_strcmp(name, BWAM_SCHEDULE_SUBSET_TAG) == 0) {
     sch_subset_ = true;
   } else if (vcl_strcmp(name, BWAM_SCHEDULE_TAG) == 0) {
     bool date, shuffle;
     date = (order_by_date_.compare("true") == 0);
     shuffle = (shuffle_.compare("true") == 0);
     sch_ = new bvam_io_schedule(date, shuffle, sch_subset_);
     if (sch_subset_)
       sch_->set_subset(start_, end_, inc_);
     sch_subset_ = false;
   } else if (vcl_strcmp(name, BWAM_VIEW_SCHEDULE_TAG) == 0) {
     bool date, shuffle;
     date = (order_by_date_.compare("true") == 0);
     shuffle = (shuffle_.compare("true") == 0);
     view_sch_ = new bvam_io_schedule(date, shuffle, sch_subset_);
     if (sch_subset_)
       view_sch_->set_subset(start_, end_, inc_);
     sch_subset_ = false;
   } else if (vcl_strcmp(name, BVAM_PROCESS_TRAIN_TAG) == 0) {
     bvam_io_process *p = new bvam_io_process_train(image_path_, camera_path_, 
      light_path_, *sch_, output_path_);
     processes_.push_back(p);
      delete sch_;
   } else if (vcl_strcmp(name, BVAM_PROCESS_DETECT_CHANGE_TAG) == 0) {
     bvam_io_process *p = new bvam_io_process_detect(image_path_, camera_path_, 
      light_path_, *sch_, output_path_);
     processes_.push_back(p);
      delete sch_;
   } else if (vcl_strcmp(name, BVAM_PROCESS_RENDER_EXP_TAG) == 0) {
     bvam_io_process *p = new bvam_io_process_render_expected(xint_, yint_, camera_path_, 
      light_path_, *sch_, output_path_);
     processes_.push_back(p);
      delete sch_;
   } else if (vcl_strcmp(name, BVAM_PROCESS_RENDER_VIEW_TAG) == 0) {
     bvam_io_process *p = new bvam_io_process_render_from_view(xint_, yint_, 
       image_path_, camera_path_, view_camera_path_, view_camera_index_, 
       *sch_, *view_sch_, output_path_);
     delete sch_;
     delete view_sch_;
     processes_.push_back(p);
   } else if (vcl_strcmp(name, BVAM_PROCESS_WRITE_RAW_TAG) == 0) {
     bvam_io_process *p = new bvam_io_process_write_raw(output_path_);
     processes_.push_back(p);
   }
}

void bvam_io_config_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements

  vcl_putchar('(');
  vcl_fwrite(s, len, 1, stdout);
  puts(")");
  cdata.append(s, len);
}

void bvam_io_config_parser::trim_string(vcl_string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}
