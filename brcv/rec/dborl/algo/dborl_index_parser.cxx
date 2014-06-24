//:
// \file
// \brief  class that implements   for ORL 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#include "dborl_index_parser.h"
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index.h>
#include <vcl_cstring.h>
#include <vcl_sstream.h>

//: a function used in the parsers
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

void dborl_index_parser::startElement(const XML_Char* name, const XML_Char** atts)
{
  // we do nothing when a new instance starts, 
  if (vcl_strcmp(name, "file") == 0) {
    current_index_ = new dborl_index;
    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "basictype") == 0) {
        if (vcl_strcmp(atts[i+1], "index") == 0) {
          vcl_cout << "parsing index description file\n";
        } else
          vcl_cout << "WARNING: dborl_index_parser expects type attribute basictype: index, but the basictype is: " << atts[i+1] << vcl_endl;
      } if (vcl_strcmp(atts[i], "name") == 0) {
        current_index_->name_ = atts[i+1];
      } else if (vcl_strcmp(atts[i], "filetype") == 0) {
        if (current_index_->set_type_from_string(atts[i+1]) < 0) {
          vcl_cout << "WARNING: dborl_index_parser could not recognize type attribute filetype: " << atts[i+1] << vcl_endl;
        }
      }
    }
  } else if (vcl_strcmp(name,"node")== 0) {
    if (!current_node_) {
      current_node_ = new dborl_index_node(); 
      if (!current_index_->root_)
        current_index_->add_root(current_node_->cast_to_index_node_base());
    } else {
      current_parent_ = current_node_;
      current_node_ = new dborl_index_node(); 
      current_index_->add_child(current_parent_->cast_to_index_node_base(), current_node_->cast_to_index_node_base());
    }
  } else if (vcl_strcmp(name, "object") == 0) {
    for (int i = 0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "name") == 0) {
        current_node_->add_name(atts[i+1]);
      } else if (vcl_strcmp(atts[i], "path") == 0) {
        current_node_->add_path(atts[i+1]);
      }
    }
  } else if (vcl_strcmp(name,"leaf")== 0) {
    current_leaf_ = new dborl_index_leaf();
  }

  // clean up the char data
  cdata_ = "";
}

void dborl_index_parser::endElement(const XML_Char* name)
{
  if (vcl_strcmp(name,"node")== 0) {
    current_node_ = current_parent_;
    //if (current_parent_) // if 0 then we were at the root
    //  current_parent_ = (current_index_->get_parent(current_parent_->cast_to_index_node_base()))->cast_to_index_node();
  } else if (vcl_strcmp(name,"leaf")== 0) {
    current_leaf_->set_name(current_node_name_);
    current_index_->add_child(current_node_->cast_to_index_node_base(), current_leaf_->cast_to_index_node_base());
    current_leaf_ = 0;
  }

  if (cdata_.size() > 0) {
        cdataHandler(name, cdata_);
      }
}

void dborl_index_parser::charData(const XML_Char* s, int len)
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

void dborl_index_parser::cdataHandler(vcl_string name, vcl_string data)
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
  if (name.compare("name") == 0) {
    current_node_name_ = tokens[0];
    if (!current_leaf_)
      current_node_->set_name(current_node_name_);
  } 
}

dborl_index_sptr dborl_index_parser::parse(vcl_string fname, dborl_index_parser& parser) 
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
    return 0;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }
   vcl_cout << "finished parsing!" << vcl_endl;

  fclose(xmlFile);
  return parser.get_index();
}

