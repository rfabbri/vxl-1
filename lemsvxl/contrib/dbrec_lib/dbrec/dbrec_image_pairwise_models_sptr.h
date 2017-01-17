// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/dbrec_image_pairwise_models_sptr.h
#ifndef dbrec_image_pairwise_models_sptr_h_
#define dbrec_image_pairwise_models_sptr_h_
//:
// \file
// \brief Smart pointer to a dbrec_pairwise_model
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 04/23/09
//

#include <vbl/vbl_smart_ptr.h>

class dbrec_pairwise_model;
typedef vbl_smart_ptr<dbrec_pairwise_model> dbrec_pairwise_model_sptr;

class dbrec_pairwise_rot_invariant_model;
typedef vbl_smart_ptr<dbrec_pairwise_rot_invariant_model> dbrec_pairwise_rot_invariant_model_sptr;

class dbrec_pairwise_rot_invariant_discrete_model;
typedef vbl_smart_ptr<dbrec_pairwise_rot_invariant_discrete_model> dbrec_pairwise_rot_invariant_discrete_model_sptr;

#endif // dbrec_image_pairwise_models_sptr_h_

