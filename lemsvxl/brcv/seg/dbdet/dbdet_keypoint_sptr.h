// This is brcv/seg/dbdet/dbdet_keypoint_sptr.h
#ifndef dbdet_keypoint_sptr_h_
#define dbdet_keypoint_sptr_h_
//:
// \file
// \brief Smart pointer to a keypoint
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/22/05
//

class dbdet_keypoint;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<dbdet_keypoint> dbdet_keypoint_sptr;

#endif // dbdet_keypoint_sptr_h_
