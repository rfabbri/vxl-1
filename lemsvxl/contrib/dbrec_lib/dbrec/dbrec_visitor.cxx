//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//

#include "dbrec_visitor.h"

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_visitor as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_visitor const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_visitor &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_visitor* ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_visitor* &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

