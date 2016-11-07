// This is brl/bpro/bpro_storage_sptr.h
#ifndef bpro_storage_sptr_h
#define bpro_storage_sptr_h

//:
// \file

class bpro_storage;

#include <vbl/vbl_smart_ptr.h>

struct bpro_storage_sptr : public vbl_smart_ptr<bpro_storage> {
  typedef vbl_smart_ptr<bpro_storage> base;

  bpro_storage_sptr() {}
  bpro_storage_sptr(bpro_storage* p): base(p) {}
  void vertical_cast(bpro_storage_sptr const& that) { *this = that; }
  void vertical_cast(bpro_storage* t) { *this = t; }
};


// Stop doxygen documenting the B class
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class T, class B = bpro_storage_sptr>
struct bpro_storage_sptr_t : public B {
  bpro_storage_sptr_t(): B() {}
  bpro_storage_sptr_t(T* p): B(p) {}
  bpro_storage_sptr_t(bpro_storage_sptr_t<T> const& r): B(r) {}
  void operator=(bpro_storage_sptr_t<T> const& r) { B::operator=(r); }
  T* operator->() const { return (T*)this->as_pointer(); }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS


#endif // bpro_storage_sptr_h
