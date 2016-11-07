#if !defined(BIOB_ENUMERATOR_H_)
#define BIOB_ENUMERATOR_H_

#include <vbl/vbl_ref_count.h>

template <class T>
struct biob_enumerator : vbl_ref_count {
  ~biob_enumerator(){}
  virtual bool has_next(){return false;}
  virtual T next() = 0;
};

#endif
