#include "mw_discrete_corresp.h"

vcl_ostream&  operator<<(vcl_ostream& s, const mw_discrete_corresp &c)
{
   s  << "# of Domain points = npts1 + 1 = " << c.corresp_.size() << vcl_endl;

   for (unsigned long i=0; i<c.corresp_.size(); ++i) {
      s << "pt_id: " << i << " #corresp: " << c.corresp_[i].size() << vcl_endl;
   }

   return s;
}
