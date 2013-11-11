// This is lemsvxlsrc/contrib/dbrec_lib/dbrec_mic/dbrec_pca_sptr.h
#ifndef dbrec_pca_sptr_h_
#define dbrec_pca_sptr_h_
//:
// \file
// \brief Smart pointer to a dbrec_pca
// \author Ozge Can Ozcanli, (ozge@lems.brown.edu)
// \date 04/23/09
//

#include <vbl/vbl_smart_ptr.h>

class dbrec_pca;
typedef vbl_smart_ptr<dbrec_pca> dbrec_pca_sptr;

class dbrec_bayesian_pca;
typedef vbl_smart_ptr<dbrec_bayesian_pca> dbrec_bayesian_pca_sptr;

class dbrec_template_matching;
typedef vbl_smart_ptr<dbrec_template_matching> dbrec_template_matching_sptr;

#endif // dbrec_pca_sptr_h_

