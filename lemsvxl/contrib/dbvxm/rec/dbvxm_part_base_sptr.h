// This is contrib/dbvxm/rec/dbvxm_part_base_sptr.h
#ifndef dbvxm_part_base_sptr_h_
#define dbvxm_part_base_sptr_h_
//:
// \file
// \brief Smart pointer to a dbvxm_part_base
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 10/03/07
//

#include <vbl/vbl_smart_ptr.h>

class dbvxm_part_base;
class dbvxm_part_instance;

typedef vbl_smart_ptr<dbvxm_part_base> dbvxm_part_base_sptr;
typedef vbl_smart_ptr<dbvxm_part_instance> dbvxm_part_instance_sptr;

#endif // dbvxm_part_base_sptr_h_
