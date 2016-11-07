#ifndef dbsta_functor_sptr_h_
#define dbsta_functor_sptr_h_

#include <vbl/vbl_smart_ptr.h>
#include <dbsta/dbsta_functor.h>


typedef vbl_smart_ptr<dbsta_functor<float> > dbsta_functor_f_sptr;
typedef vbl_smart_ptr<dbsta_functor<double> > dbsta_functor_d_sptr;

typedef vbl_smart_ptr<dbsta_data_functor<float> > dbsta_data_functor_f_sptr;
typedef vbl_smart_ptr<dbsta_data_functor<double> > dbsta_data_functor_d_sptr;

#endif

