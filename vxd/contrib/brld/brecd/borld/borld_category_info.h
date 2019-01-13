//:
// \file
// \brief Class that holds category information
//
// \author Based on original code by  Ozge C Ozcanli (@lems.brown.edu)
// \date 10/15/07
//      
// \verbatim
//   Modifications
//      Ozge C. Ozcanli - added a class to hold a set of category_info objects, to represent a database's class structure
// \endverbatim

// see borld/algo/borld_category_info_parser.h for xml read/write utilities
//

#if !defined(_DBORL_CATEGORY_INFO_H)
#define _DBORL_CATEGORY_INFO_H

#include <vbl/vbl_ref_count.h>
#include <string>
#include <vector>
#include <vil/vil_rgb.h>
#include <vxl_config.h>
#include <vsl/vsl_binary_io.h>
#include <borld/borld_category_info_sptr.h>

class borld_category_info : public vbl_ref_count
{
public:
  std::string name_;
  int id_;
  vil_rgb<vxl_byte> color_;
  std::vector<std::string> prefix_list_;

  //: the default color is red
  inline borld_category_info() : name_(""), id_(-1), color_((vxl_byte)255, (vxl_byte)0, (vxl_byte)0) {}
  inline borld_category_info(std::string name) : name_(name), id_(-1), color_((vxl_byte)255, (vxl_byte)0, (vxl_byte)0) {}
  inline borld_category_info(std::string name, int id) : name_(name), id_(id), color_((vxl_byte)255, (vxl_byte)0, (vxl_byte)0) {}
  borld_category_info(borld_category_info& other);
  
  void set_name(std::string name) { name_ = name; }
  void set_id(int id) { id_ = id; }
  void set_color(vil_rgb<vxl_byte> c);
  void set_prefix_list(std::vector<std::string>& list);
  void add_prefix(std::string p);

  void write_xml(std::ostream& os);
};

//: a vector of categories
class borld_category_info_set : public vbl_ref_count 
{
public:
  borld_category_info_set() {}
  borld_category_info_set(const std::vector<borld_category_info_sptr>& cats) : cats_(cats) {}

  void add_category(borld_category_info_sptr c); 
  std::vector<borld_category_info_sptr>& categories() { return cats_; }

  //: assumes that we're given a simple file where each line is: <category_name> <category_id> or it is a comment line which start with the char: #
  bool read_cats_from_a_simple_file(std::istream& is);

  //: return the class given by the name, return NULL if not in the list
  borld_category_info_sptr find_category(const std::string class_name);

protected:
  std::vector<borld_category_info_sptr> cats_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use borld_category_info_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, borld_category_info_set const &ph);
void vsl_b_read(vsl_b_istream & is, borld_category_info_set &ph);
void vsl_b_read(vsl_b_istream& is, borld_category_info_set* ph);
void vsl_b_write(vsl_b_ostream& os, const borld_category_info_set* &ph);

#endif  //_DBORL_CATEGORY_INFO_H
