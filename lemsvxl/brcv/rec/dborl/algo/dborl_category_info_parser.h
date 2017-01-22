//:
// \file
// \brief Class that parses category information files
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/15/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//
// see dborl/algo/tests/ --> for example xml files -- should work even if only name tag is available for each class in the file
//
//
#if !defined(_DBORL_CATEGORY_INFO_PARSER_H)
#define _DBORL_CATEGORY_INFO_PARSER_H


#include <expatpp/expatpplib.h>
#if defined(APPLE)
#include <expat.h>
#endif
#include <stdio.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>

#include <borld/borld_category_info_sptr.h>

class borld_category_info_parser : public expatpp {
public:
  borld_category_info_parser(void) : version(1) {};
  ~borld_category_info_parser(void) { clear(); }
  
  vcl_vector<borld_category_info_sptr>& get_categories(void) { return cats_; }

  void clear() { cats_.clear(); current_cat_ = 0; }

protected:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

private:
  void cdataHandler(vcl_string name, vcl_string data);
  
  void reset_current_cat() { current_cat_ = 0; }

  int version;

  vcl_vector<borld_category_info_sptr> cats_;
  vcl_string cdata_;
  borld_category_info_sptr current_cat_;
};

//: inserts parsed categories at the beginning of cats
bool parse(vcl_string fname, borld_category_info_parser& parser, vcl_vector<borld_category_info_sptr>& cats);
void write_categories_xml(vcl_vector<borld_category_info_sptr>& cats, vcl_ostream& os);

#endif  // _DBORL_CATEGORY_INFO_PARSER_H
