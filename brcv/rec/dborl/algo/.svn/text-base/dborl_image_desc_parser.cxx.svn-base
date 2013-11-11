//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/15/07
#include "dborl_image_desc_parser.h"

//#include <stdio.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_polygon_description.h>
#include <dborl/dborl_image_mask_description.h>
#include <dborl/dborl_image_mask_description_sptr.h>
#include <dborl/algo/dborl_utilities.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_file_io.h>

//: a function used in the parsers
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

void 
dborl_image_desc_parser::cdataHandler(vcl_string name, vcl_string data)
{
  // create a vector of tokens out of cdata and convert them later
  vcl_vector<vcl_string> tokens;
  int length = data.size();
  const char * str = data.c_str();
  vcl_string token = "";
  for (int i=0; i<length; i++) {
    if ((str[i] == ' ') || (str[i] == '\n')) {
      if (token.size() > 0) {
        tokens.push_back(token);
        token = "";
      }
    } else {
      token += str[i];
    }
  }
  // check for the last token
  if (token.size() > 0) 
    tokens.push_back(token);
      
  // read the filter responses
  //vcl_cout << "\t\tname:" << name << ":data:" << data << ":tokens:";
  //for (unsigned i = 0; i < tokens.size(); i++)
  //  vcl_cout << tokens[i] << ":";
  //vcl_cout << "\n";

  // read the filter responses
  float resp;
  if (name.compare(polygon_tag_) == 0) {
    vcl_vector<vsol_point_2d_sptr> vertices;
    
    //: assuming <polygon>x y x y x y x y</polygon>  --> so each token is either x or y  (similar to a .con file)
    vsol_point_2d_sptr current = new vsol_point_2d();

    for (unsigned int i=0; i<tokens.size(); i++) {
      convert(tokens[i].c_str(), resp);

      if (i%2 == 0) {
        current->set_x(resp);
      } else {
        current->set_y(resp);
        vertices.push_back(new vsol_point_2d(current->get_p()));
      }
    }
    current_poly_ = new vsol_polygon_2d(vertices);

  } else if (name.compare("xmin") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      convert(tokens[0].c_str(), resp);
      current_minx_ = resp;
    }
  } else if (name.compare("ymin") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      convert(tokens[0].c_str(), resp);
      current_miny_ = resp;
    }
  } else if (name.compare("xmax") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      convert(tokens[0].c_str(), resp);
      current_maxx_ = resp;
    }
  } else if (name.compare("ymax") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      convert(tokens[0].c_str(), resp);
      current_maxy_ = resp;
    }
  } else if (name.compare(category_tag_) == 0 && tokens.size() > 0) {
    current_cat_ = tokens[0];
  }
}

void 
dborl_image_desc_parser::startElement(const char* name, const char** atts)
{
  //vcl_cout<< "element=" << name << vcl_endl; 

  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // we do nothing when a new instance starts, 
  if (vcl_strcmp(name, "type") == 0) {

    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "name") == 0) {
        if (!(vcl_strcmp(atts[i+1], "image") == 0))
          vcl_cout << "WARNING: dborl_image_desc_parser expects attribute name: image, but the name is: " << atts[i+1] << vcl_endl;
      }
    }

  } else if (vcl_strcmp(name,category_tag_.c_str())== 0) {
    current_cat_ = "";
  } else if (vcl_strcmp(name,box_tag_.c_str())== 0) {
    reset_box();
  } else if (vcl_strcmp(name,polygon_tag_.c_str())== 0) {
    reset_poly();
  }
  
  // clean up the char data
  cdata_ = "";
}


void 
dborl_image_desc_parser::endElement(const char* name)
{
  //vcl_cout << "end element=" << name << vcl_endl;
  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // when an instance ends we add it to bbox description or polygon description
  if (vcl_strcmp(name,instance_tag_.c_str())== 0) {
    cats_.push_back(current_cat_);  // save all the categories encountered
    if (current_box_) {
      if (!box_desc_) 
        box_desc_ = new dborl_image_bbox_description();
      box_desc_->add_box(current_cat_, current_box_);
    }
    if (current_poly_) {
      if (!poly_desc_)
        poly_desc_ = new dborl_image_polygon_description();
      poly_desc_->add_polygon(current_cat_, current_poly_);
    } 

  } else if (vcl_strcmp(name,box_tag_.c_str())== 0) {
      current_box_ = new vsol_box_2d();
      current_box_->add_point(current_minx_, current_miny_);
      current_box_->add_point(current_maxx_, current_maxy_);

  } else if (vcl_strcmp(name,description_tag_.c_str())== 0) {
    if (box_desc_)
      idesc_ = new dborl_image_description(box_desc_);
    else if (poly_desc_)
      idesc_ = new dborl_image_description(poly_desc_);
    else {  // if neither box nor polygon descriptions are available then assumes map description, whose image will be set later!!
      dborl_image_mask_description_sptr md = new dborl_image_mask_description();
      idesc_ = new dborl_image_description(md, cats_);
    }
  } 

  if (cdata_.size() > 0) {
        cdataHandler(name, cdata_);
      }
}

void dborl_image_desc_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0)// || len<=leadingSpace)
     return;  // called with whitespace between elements
  if (len <=leadingSpace) {
    cdata_.append(" ");
    return;
  }
  cdata_.append(s, len);
}

dborl_image_description_sptr dborl_image_description_parse(vcl_string fname, dborl_image_desc_parser& parser) 
{
  vcl_FILE *xmlFile;

  if (fname.size() == 0){
    vcl_cout << "File not specified" << vcl_endl;
    return 0;
  }

  //errno_t err;

  //if ( (err = fopen_s(&xmlFile, fname.c_str(), "r") ) != 0 ) {
  //xmlFile = vcl_fopen_s(fname.c_str(), "r");
  xmlFile = vcl_fopen(fname.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << fname << "-- error on opening" << vcl_endl;
    return 0;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }
   //vcl_cout << "finished parsing!" << vcl_endl;

  fclose(xmlFile);
  return parser.get_image_desc();
}

void parse_pascal_write_default_xml(vcl_string input_fname, vcl_string output_fname)
{
  dborl_image_desc_parser parser;
  parser.set_pascal_tags();
  dborl_image_description_sptr id = dborl_image_description_parse(input_fname, parser);

  vcl_ofstream os;
  os.open(output_fname.c_str(), vcl_ios_out);
  id->write_xml(os);
  os.close();
}

bool read_con_write_image_description_xml(vcl_string input_fname, vcl_string category, vcl_string output_fname)
{
  vcl_vector<vsol_point_2d_sptr > points;
  bool is_closed;
  if (dbsol_load_con_file(input_fname.c_str(), points, is_closed)) {
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
    dborl_image_polygon_description_sptr ip = new dborl_image_polygon_description();
    ip->add_polygon(category, poly);
    dborl_image_description_sptr id = new dborl_image_description(ip);
    
    vcl_ofstream os;
    os.open(output_fname.c_str(), vcl_ios_out);
    id->write_xml(os);
    os.close();
    return true;
  } else
    return false;
}
