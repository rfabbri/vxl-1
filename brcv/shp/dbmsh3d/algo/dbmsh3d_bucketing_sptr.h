// This is shp/dbmsh3d/algo/dbmsh3d_bucketing_sptr.h
#ifndef dbmsh3d_bucketing_sptr_h_
#define dbmsh3d_bucketing_sptr_h_
//:
// \file
// \author Nhon Trinh
// \date   Feb 14, 2007
// \brief  Smart-pointer to a elements of dbmsh3d_bucketing

#include <vbl/vbl_smart_ptr.h>

// smart pointer to a face bucket
class dbmsh3d_face_bucket;
typedef vbl_smart_ptr< dbmsh3d_face_bucket > dbmsh3d_face_bucket_sptr;


#endif // dbmsh3d_bucketing_sptr_h_
