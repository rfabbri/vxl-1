//:
// \file
// \brief Class that parses image description files, currently no header information is parsed, if files with such headers are generated,
//        this parser can easily be modified to parse any additional tag at the header or within instance descriptions
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/15/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
// see dborl/algo/tests/ --> for example xml files
//
//
#if !defined(_DBORL_IMAGE_DESC_PARSER_H)
#define _DBORL_IMAGE_DESC_PARSER_H


#include <expatpp/expatpplib.h>
#if defined(APPLE)
#include <expat.h>
#endif
#include <stdio.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>

#include <dborl/dborl_image_description_sptr.h>
#include <dborl/dborl_image_bbox_description_sptr.h>
#include <dborl/dborl_image_polygon_description_sptr.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>

class dborl_image_desc_parser : public expatpp {
public:
  dborl_image_desc_parser(void) : version(1), 
                                  category_tag_("category"), 
                                  instance_tag_("instance"), 
                                  box_tag_("bndbox"), 
                                  polygon_tag_("polygon"),
                                  description_tag_("description") {};
  ~dborl_image_desc_parser(void) { clear(); }
  
  void set_image_desc(dborl_image_description_sptr id) { idesc_ = id; }
  dborl_image_description_sptr get_image_desc(void) { return idesc_; }

  void clear() { idesc_ = 0; box_desc_ = 0; poly_desc_ = 0; current_box_ = 0; current_poly_ = 0; cats_.clear(); }

  void set_pascal_tags() { category_tag_ = "name", instance_tag_ = "object", box_tag_ = "bndbox", description_tag_ = "annotation"; }
  void set_default_tags() { category_tag_ = "category", instance_tag_ = "instance", box_tag_ = "bndbox", description_tag_ = "description"; }

  vcl_string category_tag_, instance_tag_, box_tag_, polygon_tag_, description_tag_;
protected:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

private:
  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void WriteIndent();
  
  void reset_box() { current_box_ = 0; current_minx_ = -1; current_miny_ = -1; current_maxx_ = -1; current_maxy_ = -1; }
  void reset_poly() { current_poly_ = 0; }

  int version;

  // filter response paramaters
  vcl_string cdata_;
  vcl_string current_cat_;
  vcl_vector<vcl_string> cats_;

  vsol_box_2d_sptr current_box_;
  float current_minx_, current_miny_, current_maxx_, current_maxy_;
  
  vsol_polygon_2d_sptr current_poly_;

  dborl_image_description_sptr idesc_;
  dborl_image_bbox_description_sptr box_desc_;
  dborl_image_polygon_description_sptr poly_desc_;
};

dborl_image_description_sptr dborl_image_description_parse(vcl_string fname, dborl_image_desc_parser& parser);

void parse_pascal_write_default_xml(vcl_string input_fname, vcl_string output_fname);
bool read_con_write_image_description_xml(vcl_string input_fname, vcl_string category, vcl_string output_fname);

#endif  // _DBORL_IMAGE_DESC_PARSER_H
