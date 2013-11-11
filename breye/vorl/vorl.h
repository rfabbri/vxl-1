// This is breye/vorl/vorl.h
#ifndef vorl_h_
#define vorl_h_
//:
// \file
// \brief include this file to make a VORL executable
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/17/04
//
// \verbatim
//  Modifications
// \endverbatim

#include "vorl_manager.h"
#include <vidpro/vidpro_repository.h>


#define REG_STORAGE(store) {\
  store ## _sptr model = store ## _new(); \
  model->register_binary_io(); \
  vidpro_repository::register_type( model ); \
  }

#define REG_PROCESS(proc) \
  vidpro_process_manager::register_process( bpro_process_sptr( new proc() ) );



#endif // vorl_h_
