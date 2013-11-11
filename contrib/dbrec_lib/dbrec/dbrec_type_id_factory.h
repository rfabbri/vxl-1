//:
// \file
// \brief a singleton to generate unique ids whenever asked 
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 22, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_DBREC_TYPE_ID_FACTORY_H)
#define _DBREC_TYPE_ID_FACTORY_H

#include <vnl/vnl_random.h>

class dbrec_type_id_factory_destroyer;

class dbrec_type_id_factory {
public:
  
  static dbrec_type_id_factory* instance();

  void register_type(unsigned id);
  unsigned new_type();

  double random() { return rng_.drand32(); }
  vnl_random& rng() { return rng_; }

private:
  //: Constructor is private, this is a Singleton class, so maintains only one instance
  dbrec_type_id_factory() : current_(0) {}

  //: the instance
  static dbrec_type_id_factory* instance_;
  //: the destroyer instance to make sure memory is deallocated when the program exits
  static dbrec_type_id_factory_destroyer destroyer_;  // its not a pointer so C++ will make sure that it's descructor will be called

  //: currently available id is the current_;
  unsigned current_;

  //: maintain a global random number generator
  vnl_random rng_;

};

//: create another class whose sole purpose is to destroy the singleton instance
class dbrec_type_id_factory_destroyer {
public:
  dbrec_type_id_factory_destroyer(dbrec_type_id_factory* s = 0);
  ~dbrec_type_id_factory_destroyer();

  void set_singleton(dbrec_type_id_factory* s);
private:
  dbrec_type_id_factory* s_;
};

#endif  //_DBREC_TYPE_ID_FACTORY_H
