// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/dbrec_part_context_sptr.h
#ifndef dbrec_part_context_sptr_h_
#define dbrec_part_context_sptr_h_
//:
// \file
// \brief Smart pointer to a dbrec_part_context
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 04/23/09
//

#include <vbl/vbl_smart_ptr.h>

class dbrec_part_context;
typedef vbl_smart_ptr<dbrec_part_context> dbrec_part_context_sptr;
class dbrec_part_ins;
typedef vbl_smart_ptr<dbrec_part_ins> dbrec_part_ins_sptr;
class dbrec_context_factory;
typedef vbl_smart_ptr<dbrec_context_factory> dbrec_context_factory_sptr;

#endif // dbrec_part_context_sptr_h_

