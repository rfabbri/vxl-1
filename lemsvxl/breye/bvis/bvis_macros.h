// This is breye/bvis/bvis_macros.h
#ifndef bvis_macros_h_
#define bvis_macros_h_
//:
// \file
// \brief Macros for use in a main function.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 2/4/04
//
// See examples\bvis_brown_eyes2_main.cxx
//
// \verbatim
//  Modifications
// \endverbatim

#define REG_DISPLAYER(disp, T) \
   T ::register_displayer( bvis_displayer_sptr(new disp() ) )

#define REG_STORAGE(store) {\
  store ## _sptr model = store ## _new(); \
  model->register_binary_io(); \
  bpro_storage_registry::register_type( model ); \
  }

#define REG_PROCESS(proc, T) \
  T ::register_process( bpro_process_sptr( new proc() ) );



#endif // bvis_macros_h_
