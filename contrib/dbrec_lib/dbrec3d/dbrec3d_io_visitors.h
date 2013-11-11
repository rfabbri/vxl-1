// This is dbrec3d_visitor.h
#ifndef dbrec3d_io_visitors_h
#define dbrec3d_io_visitors_h
//:
// \file
// \brief Using a templated version of Visitor Design Pattern to allow for different algorithms to work with the meta-structure
//        without cluttering the interface of the meta-structure parts
//        e.g. a parser may define a concrete visitor class to parse using the meta-structure
//             a visualizer may define a concrete visitor class to visualize the meta-structure 
//
// \author Isabel Restrepo mir@lems.brown.edu 
// \date  23-Aug-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>

#include "dbrec3d_visitor.h"
#include "dbrec3d_parts_manager.h"
#include "dbrec3d_primitive_kernel_part.h"


//: Forward declaration of xml write and parse utilities for dbrec3d
void dbrec3d_xml_write_parts_and_contexts(vcl_string p_doc, vcl_string c_doc);
void dbrec3d_xml_parse_parts_and_contexts(vcl_string p_doc, vcl_string c_doc);

//: Writer the hierarchy of partsin XML format to be able to save a current hierarchy and load back later
//  This visitor visits the nodes recursively. It first writes the tree structure keeping track of parts, then makes a single pass on the parts and dums them individually
class dbrec3d_write_xml_visitor : public dbrec3d_visitor
{
public:
  dbrec3d_write_xml_visitor();
  
  void visit(dbrec3d_part_sptr part);

  //: write a vector of parts to a documents
  void write_hierarchy(vcl_vector<dbrec3d_part_sptr> &hierarchy, vcl_string& name);
  
  
protected:
  
  bxml_data_sptr part_data_;        // keeps one node per part
  vcl_set<unsigned> part_set_;      // keeps track of which part has been added
};

//: Parse the hierarchy of parts in XML format
class dbrec3d_parse_xml_visitor : public dbrec3d_visitor
{
public:
  dbrec3d_parse_xml_visitor(){};
  
  virtual void visit(dbrec3d_primitive_kernel_part* part){};
  //virtual void visit(dbrec3d_part_sptr part){}
  
  //: Parses one part at a time and registers them with PARTS_MANAGER
  bool parse_hierarchy(const vcl_string& name);
  
  //: Parse primitive parts
  void parse_primitive();

  //: create the structure
  bool parse_composition_structure(bxml_data_sptr d,vcl_map<unsigned, vcl_pair<dbrec3d_part_sptr, bool> >& part_map);

protected:
  
  bxml_data_sptr structural_data_;  // only keeps track of whom is whose children
  bxml_data_sptr part_data_;        // keeps one node per part
  vcl_set<unsigned> part_set_;      // keeps track of which part has been added
};

#endif
