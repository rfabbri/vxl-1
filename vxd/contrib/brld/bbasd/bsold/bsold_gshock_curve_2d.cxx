//:
// \file
// \author Ricardo Fabbri
#include "bsold_gshock_curve_2d.h"
#include <bsold/bsold_gshock.h>

void bsold_gshock_curve_2d::
cache_measures()
{
   m_.resize(ints_.size());
   for (unsigned i=0; i<ints_.size(); ++i)
      m_[i].compute(*(ints_[i]));
}


//: blanking parameter not supported
void bsold_gshock_curve_2d::
describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';

  strm << "=== bsold_geno_curve_2d ===\n";
  strm << "#intervals: " << ints_.size() << "\t length: " << length() << std::endl;
  for (unsigned int i=0; i<ints_.size(); ++i) {
     strm << "=== Interval " << i << " ===" << std::endl;
//     ints_[i]->print(strm);
     strm << "Cummulative length: " << lengths_[i] << std::endl;
     strm << "Stencil position: " << stencil_[i] << std::endl;
     strm << "Has Shock? " << (has_shock(i)? "true": "false") << std::endl;

     if (has_shock(i) && shock(i)) {
        strm << "#Intecepts: " << shock(i)->n_intercepts() << std::endl;
     }
  }
}
