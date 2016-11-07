#if !defined(BIOB_IO_EXPLICIT_WORLDPT_ROSTER_H_)
#define BIOB_IO_EXPLICIT_WORLDPT_ROSTER_H_

#include <biob/biob_explicit_worldpt_roster.h>
#include <vsl/vsl_fwd.h>

 void vsl_b_write(vsl_b_ostream & os, 
     const biob_explicit_worldpt_roster & roster);

 void vsl_b_read(vsl_b_istream & is,
        biob_explicit_worldpt_roster & roster);

#endif
