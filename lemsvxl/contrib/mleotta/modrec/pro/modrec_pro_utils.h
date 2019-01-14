// This is contrib/mleotta/modrec/pro/modrec_pro_utils.h
#ifndef modrec_pro_utils_h_
#define modrec_pro_utils_h_

//:
// \file
// \brief utility functions for modrec_pro classes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 12/6/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <string>
#include <vector>

#include <imesh/imesh_mesh.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>


template<class T>
bool modrec_pro_read_param(const bxml_document& xdoc, const std::string& name, T& val)
{
  bxml_element query("param");
  query.set_attribute("name",name);
  bxml_data_sptr result = bxml_find(xdoc.root_element(), query);
  if(!result)
    return false;
  bxml_element* elm = static_cast<bxml_element*>(result.ptr());
  if(!elm->get_attribute("value", val))
    return false;

  return true;
}


void modrec_pro_read_models(const std::string& path,
                            std::vector<imesh_mesh>& models,
                            std::vector<std::string>& names);

#endif
