// This is shp/dbsksp/dbsksp_shapelet_sptr.h
#ifndef dbsksp_shapelet_sptr_h_
#define dbsksp_shapelet_sptr_h_

//:
// \file
// \author Nhon Trinh
// \date   Sep 30, 2006
// \brief  Smart-pointer to a dbsksp_shapelet

#include <vbl/vbl_smart_ptr.h>
class dbsksp_shapelet;
class dbsksp_twoshapelet;

typedef vbl_smart_ptr< dbsksp_shapelet > dbsksp_shapelet_sptr;
typedef vbl_smart_ptr< dbsksp_twoshapelet > dbsksp_twoshapelet_sptr;

#endif // dbsksp_shapelet_sptr_h_
