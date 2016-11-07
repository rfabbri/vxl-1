// This is breye1/vorl1/vorl1.h
#ifndef vorl1_h_
#define vorl1_h_
//:
// \file
// \brief include this file to make a VORL executable
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/17/04
//
// \verbatim
//  Modifications
// \endverbatim

#include "vorl1_manager.h"
#include <vidpro1/vidpro1_repository.h>


#define REG_STORAGE(store) {\
  store ## _sptr model = store ## _new(); \
  model->register_binary_io(); \
  vidpro1_repository::register_type( model ); \
  }

#define REG_PROCESS(proc) \
  vidpro1_process_manager::register_process( bpro1_process_sptr( new proc() ) );



#endif // vorl1_h_
