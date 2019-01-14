#include "mw_discrete_corresp.h"

std::ostream&  operator<<(std::ostream& s, const mw_discrete_corresp &c)
{
   s  << "# of Domain points = npts1 + 1 = " << c.corresp_.size() << std::endl;

   for (unsigned long i=0; i<c.corresp_.size(); ++i) {
      s << "pt_id: " << i << " #corresp: " << c.corresp_[i].size() << std::endl;
   }

   return s;
}
