// This is contrib/bvam/bvam_macros.h
#ifndef bvam_macros_h_
#define bvam_macros_h_
//:
// \file
// \brief Macros for use in a main function ow bvam project.
// \author Gamze Tunali, (gtunali@brown.edu)
// \date 01/16/08
//
// See bvam/apps
//
// \verbatim
//  Modifications
// \endverbatim

#include "bvam_processor.h"

#define REG_PROCESS(p) \
  bvam_process_mgr::instance()->register_process(p, bwm_processor<p>);


#endif // bvam_macros_h_

