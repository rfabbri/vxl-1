// This is seg/dbsks/dbsks_xgraph_ccm_model_sptr.h
#ifndef dbsks_xgraph_ccm_model_sptr_h_
#define dbsks_xgraph_ccm_model_sptr_h_

//:
// \file
// \author Nhon Trinh
// \date   Dec 18, 2008
// \brief  Smart-pointer to a dbsks_xgraph_ccm_model

#include <vbl/vbl_smart_ptr.h>
class dbsks_xgraph_ccm_model;
class dbsks_xfrag_ccm_model;
typedef vbl_smart_ptr< dbsks_xgraph_ccm_model > dbsks_xgraph_ccm_model_sptr;
typedef vbl_smart_ptr< dbsks_xfrag_ccm_model > dbsks_xfrag_ccm_model_sptr;

#endif // dbsks_xgraph_ccm_model_sptr_h_
