// This is dbknee/dbknee_coord_sptr.h
#ifndef dbknee_coord_sptr_h_
#define dbknee_coord_sptr_h_

//:
// \file
// \author Nhon Trinh
// \date   Jan 1, 2007
// \brief  Smart-pointer to a dbknee_coord

#include <vbl/vbl_smart_ptr.h>
class dbknee_coord_base;

typedef vbl_smart_ptr< dbknee_coord_base > dbknee_coord_base_sptr;

#endif // dbknee_coord_sptr_h_
