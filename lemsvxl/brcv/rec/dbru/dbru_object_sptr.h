// This is brcv/rec/dbru/dbru_object_sptr.h
#ifndef dbru_object_sptr_h_
#define dbru_object_sptr_h_
//:
// \file
// \brief Smart pointer to a dbru_object
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 6/28/05
//

#include <vbl/vbl_smart_ptr.h>

class dbru_object;
typedef vbl_smart_ptr<dbru_object> dbru_object_sptr;

#endif // dbru_object_sptr_h_
