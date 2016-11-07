// This is dbdet/xio/dbdet_xio_seg3d_info_parser.cxx


#include "dbdet_xio_seg3d_info_parser.h"
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>


// ============================================================================
void dbdet_seg3d_info_xml_parser::
startElement(const char* name, const char** atts)
{
  
  this->opened_elements_stack_.push_back(name);
  ++ this->depth_;

  vcl_cout<< "element=" << name << vcl_endl; 
  for (int i=0; atts[i]; i+=2)
  {
    vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
    if (vcl_strcmp(atts[i], "frame_index") == 0)
    {
      this->frame_index_ = vcl_atoi(atts[i+1]);
    }
    else if (vcl_strcmp(atts[i], "contour_index") == 0)
    {
      this->contour_index_ = vcl_atoi(atts[i+1]);
    }
  }  
}

// ============================================================================
void dbdet_seg3d_info_xml_parser::
endElement(const char* name)
{
  --this->depth_;
  this->opened_elements_stack_.pop_back();

  // handle the string data then clear it
  this->handle_char_data(name, this->char_data_);
  this->char_data_.clear();
}

// ============================================================================
void dbdet_seg3d_info_xml_parser::
charData(const XML_Char* s, int len)
{
  // preliminary check
  const int leading_space = skipWhiteSpace(s);
  if (len==0 || len <= leading_space)
     return;  // called with whitespace between elements
  this->char_data_.append(s+leading_space, len - leading_space);
  vcl_cout << "(" << vcl_string(s, len) << ")";
}


// ============================================================================
void dbdet_seg3d_info_xml_parser::
handle_char_data(const vcl_string& element, const vcl_string& char_data)
{
  // process the data differently depending on the type of ``element"
  if (element == "volume_segmentation")
  {
    vcl_cout << "Finished parsing xml file." << vcl_endl;
  }    
  else if (element == "image_folder")
  {
    this->seg3d_info()->set_image_folder(char_data);
  }
  else if (element == "contour_folder")
  {
    this->seg3d_info()->set_contour_folder(char_data);
  }
  else if (element == "frame")
  {
    this->seg3d_info()->add_frame(this->current_frame_);
    this->current_frame_.image_file = "";
    this->current_frame_.contour_file_list.clear();
  }
  else if (element == "image_file")
  {
    this->current_frame_.image_file = char_data;
  }
  else if (element == "contour_file")
  {
    this->current_frame_.contour_file_list.push_back(char_data);
  }
  return;
}
