//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/15/07
#include "dborl_category_info_parser.h"

#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <dborl/dborl_category_info.h>
#include <dborl/algo/dborl_utilities.h>

//: a function used in the parsers
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

void 
dborl_category_info_parser::cdataHandler(vcl_string name, vcl_string data)
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
      
  if (name.compare("name") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      current_cat_->set_name(tokens[0]);
    }
  } else if (name.compare("id") == 0) {
    if (tokens.size() > 0 && tokens[0].size() > 0) {
      int resp1;
      convert(tokens[0].c_str(), resp1);
      current_cat_->set_id(resp1);
    }
  } else if (name.compare("color") == 0) {
    if (tokens.size() >= 3 && tokens[0].size() > 0 && tokens[1].size() > 0 && tokens[2].size() > 0) {
      int resp1, resp2, resp3;
      convert(tokens[0].c_str(), resp1);
      convert(tokens[1].c_str(), resp2);
      convert(tokens[2].c_str(), resp3);
      current_cat_->set_color(vil_rgb<vxl_byte> (resp1, resp2, resp3));
    }
  } else if (name.compare("prefix") == 0) {
    for (unsigned i = 0; i < tokens.size(); i++) {
      if (tokens[i].size() > 0) 
        current_cat_->add_prefix(tokens[i]);
    }
  }
}

void 
dborl_category_info_parser::startElement(const char* name, const char** atts)
{
  //vcl_cout<< "element=" << name << vcl_endl; 

  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // we do nothing when a new instance starts, 
  if (vcl_strcmp(name, "type") == 0) {

    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "name") == 0) {
        if (vcl_strcmp(atts[i+1], "category-info") == 0)
          vcl_cout << "parsing category description file\n";
        else
          vcl_cout << "WARNING: dborl_category_info_parser expects attribute name: category-info, but the name is: " << atts[i+1] << vcl_endl;
      }
    }

  } else if (vcl_strcmp(name,"category")== 0) {
    current_cat_ = new dborl_category_info();
  } 
  
  // clean up the char data
  cdata_ = "";
}


void 
dborl_category_info_parser::endElement(const char* name)
{
  //vcl_cout << "end element=" << name << vcl_endl;
  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // when an instance ends we add it to bbox description or polygon description
  if (vcl_strcmp(name,"category")== 0) {
    cats_.push_back(new dborl_category_info(*current_cat_));
    current_cat_ = 0;
  } 

  if (cdata_.size() > 0) {
        cdataHandler(name, cdata_);
      }
}

void dborl_category_info_parser::charData(const XML_Char* s, int len)
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

//: inserts parsed categories at the beginning of cats
bool parse(vcl_string fname, dborl_category_info_parser& parser, vcl_vector<dborl_category_info_sptr>& cats)
{
  vcl_FILE *xmlFile;

  if (fname.size() == 0){
    vcl_cout << "File not specified" << vcl_endl;
    return 0;
  }

  //errno_t err;

  //if ( (err = fopen_s(&xmlFile, fname.c_str(), "r") ) != 0 ) {
  xmlFile = vcl_fopen(fname.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << fname << "-- error on opening" << vcl_endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return false;
   }
   vcl_cout << "finished parsing!" << vcl_endl;

  fclose(xmlFile);

  vcl_vector<dborl_category_info_sptr>& parsed_cats = parser.get_categories();
  cats.insert(cats.begin(), parsed_cats.begin(), parsed_cats.end());
  return true;
}

void write_categories_xml(vcl_vector<dborl_category_info_sptr>& cats, vcl_ostream& os)
{
  os << "<type name = \"category-info\">\n";
  os << "\t<description>\n";

  for (unsigned i = 0; i < cats.size(); i++)
    cats[i]->write_xml(os);

  os << "\t</description>\n";
  os << "</type>\n";
}

