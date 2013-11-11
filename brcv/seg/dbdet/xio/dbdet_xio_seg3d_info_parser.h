#ifndef dbdet_xio_seg3d_info_parser_h_
#define dbdet_xio_seg3d_info_parser_h_

//: 
// \file     
// \brief    parser for XML file of dbdet_seg3d_info class
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     July 29, 2006

#include <vcl_string.h>
#include <vcl_vector.h>
#include <dbdet/dbdet_seg3d_info.h>
#include <dbdet/dbdet_seg3d_info_sptr.h>
#include <expatpp/expatpp.h>


//: parser to parse an seg3d_info xml file
class dbdet_seg3d_info_xml_parser : public expatpp 
{
public:
  dbdet_seg3d_info_xml_parser(void){};
  virtual ~dbdet_seg3d_info_xml_parser(void){};

  // =================== DATA ACCESS FUNCTIONS ==================

  //: Return pointer to seg3d_info
  dbdet_seg3d_info_sptr seg3d_info() const {return this->seg3d_info_; }

  //: Set pointer to seg3d_info object
  void set_seg3d_info(const dbdet_seg3d_info_sptr& s)
  {this->seg3d_info_ = s; }


  // ================ OVERRIDING FUNCTIONS of expatpp for PARSING =============
protected:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handle_char_data(const vcl_string& element, const vcl_string& char_data);

  //Data - used during parsing
  int depth_;                                     // depth in the xml file
  int frame_index_;                               // index of frame being read  
  int contour_index_;                             // index of contour being read    
  vcl_vector<vcl_string > opened_elements_stack_; // names of elements opened
  vcl_string char_data_;                          // accumulated read string data
  dbdet_seg3d_info_frame current_frame_;          // frame to push to seg3d_info

  // pointer to the data container
  dbdet_seg3d_info_sptr seg3d_info_;
};


#endif
