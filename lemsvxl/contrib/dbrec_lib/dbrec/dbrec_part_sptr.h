// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/dbrec_part_sptr.h
#ifndef dbrec_part_sptr_h_
#define dbrec_part_sptr_h_
//:
// \file
// \brief Smart pointer to a dbrec_part
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 04/23/09
//

#include <vbl/vbl_smart_ptr.h>

class dbrec_part;
typedef vbl_smart_ptr<dbrec_part> dbrec_part_sptr;
class dbrec_hierarchy;
typedef vbl_smart_ptr<dbrec_hierarchy> dbrec_hierarchy_sptr;

#endif // dbrec_part_sptr_h_

