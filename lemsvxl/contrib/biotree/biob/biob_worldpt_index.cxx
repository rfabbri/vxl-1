#include "biob_worldpt_index.h"

 void vsl_b_write(vsl_b_ostream & os, const biob_worldpt_index & pti){
  vsl_b_write(os, pti.index());
  }
 void vsl_print_summary(vcl_ostream & os, const biob_worldpt_index & pti){
  os << "worldpt " << pti.index() << vcl_endl;
  }

 void vsl_b_read(vsl_b_istream & is, biob_worldpt_index& pti){
     unsigned long int indexToRead;
     vsl_b_read(is, indexToRead);
     pti.index_ = indexToRead;
  }
