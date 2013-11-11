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

#include "dborl_index_leaf.h"

void dborl_index_leaf::b_read()
{

}

void dborl_index_leaf::b_write()
{

}

void dborl_index_leaf::write_xml(vcl_ostream& os)
{
  os << "<leaf><name>" << name_ << "</name></leaf>\n";
}
