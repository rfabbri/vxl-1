//:
// \file
// \brief Class that parses index files
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//
// see dborl/algo/tests/ --> for example xml files -- should work even if some tags/attributes are not available 
//
//

#if !defined(_DBORL_INDEX_PARSER_H)
#define _DBORL_INDEX_PARSER_H

#include <expatpp/expatpplib.h>
#if defined(APPLE)
#include <expat.h>
#endif
#include <stdio.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>

#include <dborl/dborl_index_node_sptr.h>
#include <dborl/dborl_index_leaf_sptr.h>
#include <dborl/dborl_index_sptr.h>

class dborl_index_parser : public expatpp
{
public:
  int version;
  dborl_index_sptr current_index_;
  dborl_index_node_sptr current_node_, current_parent_;
  dborl_index_leaf_sptr current_leaf_;
  vcl_string current_node_name_;
  vcl_string cdata_;

  dborl_index_parser(void) : version(1) {};
  ~dborl_index_parser(void) { clear(); }

  dborl_index_sptr get_index(void) { return current_index_; }
  void clear() { current_index_ = 0; current_node_ = 0; current_parent_ = 0; /*current_leaf_ = 0;*/ }

  static dborl_index_sptr parse(vcl_string fname, dborl_index_parser& parser);

protected:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

private:
  void cdataHandler(vcl_string name, vcl_string data);
  //void reset_current_index() { current_index_ = 0; }
  //void reset_current_node() { current_node_ = 0; }
  //void reset_current_leaf() { current_leaf_ = 0; }
};



#endif  //_DBORL_INDEX_PARSER_H
