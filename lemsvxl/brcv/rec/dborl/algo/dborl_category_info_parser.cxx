//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/15/07

#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>

#include <borld/borld_category_info.h>
#include <dborl/algo//dborl_category_info_parser.h>
#include <dborl/algo/dborl_utilities.h>

//: a function used in the parsers
template <typename T>
void convert(const char* t, T& d)
{
  std::stringstream strm(t);

  strm >> d;

}

void 
dborl_category_info_parser::cdataHandler(std::string name, std::string data)
{
  // create a vector of tokens out of cdata and convert them later
  std::vector<std::string> tokens;
  int length = data.size();
  const char * str = data.c_str();
  std::string token = "";
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
  //std::cout<< "element=" << name << std::endl; 

  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // we do nothing when a new instance starts, 
  if (std::strcmp(name, "type") == 0) {

    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "name") == 0) {
        if (std::strcmp(atts[i+1], "category-info") == 0)
          std::cout << "parsing category description file\n";
        else
          std::cout << "WARNING: dborl_category_info_parser expects attribute name: category-info, but the name is: " << atts[i+1] << std::endl;
      }
    }

  } else if (std::strcmp(name,"category")== 0) {
    current_cat_ = new borld_category_info();
  } 
  
  // clean up the char data
  cdata_ = "";
}


void 
dborl_category_info_parser::endElement(const char* name)
{
  //std::cout << "end element=" << name << std::endl;
  // our tags are category_tag_, instance_tag_, box_tag_, polygon_tag_
  // when an instance ends we add it to bbox description or polygon description
  if (std::strcmp(name,"category")== 0) {
    cats_.push_back(new borld_category_info(*current_cat_));
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
bool parse(std::string fname, dborl_category_info_parser& parser, std::vector<borld_category_info_sptr>& cats)
{
  std::FILE *xmlFile;

  if (fname.size() == 0){
    std::cout << "File not specified" << std::endl;
    return 0;
  }

  //errno_t err;

  //if ( (err = fopen_s(&xmlFile, fname.c_str(), "r") ) != 0 ) {
  xmlFile = std::fopen(fname.c_str(), "r");
  if (xmlFile == NULL){
    std::cout << fname << "-- error on opening" << std::endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     std::cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << std::endl;
     return false;
   }
   std::cout << "finished parsing!" << std::endl;

  fclose(xmlFile);

  std::vector<borld_category_info_sptr>& parsed_cats = parser.get_categories();
  cats.insert(cats.begin(), parsed_cats.begin(), parsed_cats.end());
  return true;
}

void write_categories_xml(std::vector<borld_category_info_sptr>& cats, std::ostream& os)
{
  os << "<type name = \"category-info\">\n";
  os << "\t<description>\n";

  for (unsigned i = 0; i < cats.size(); i++)
    cats[i]->write_xml(os);

  os << "\t</description>\n";
  os << "</type>\n";
}

