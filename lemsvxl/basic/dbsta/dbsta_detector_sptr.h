#ifndef dbsta_detector_sptr_h_
#define dbsta_detector_sptr_h_

#include <vbl/vbl_smart_ptr.h>
#include <dbsta/dbsta_detector.h>


typedef vbl_smart_ptr<dbsta_detector<float> > dbsta_detector_f_sptr;
typedef vbl_smart_ptr<dbsta_detector<double> > dbsta_detector_d_sptr;

#endif

