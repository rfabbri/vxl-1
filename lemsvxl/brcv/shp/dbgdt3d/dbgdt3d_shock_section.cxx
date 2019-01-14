//: Aug 19, 2005 MingChing Chang
//  

#include <assert.h>
#include <algorithm>
#include <dbgdt3d/dbgdt3d_shock_section.h>
#include <dbgdt3d/dbgdt3d_edge.h>

void gdt_shock_section::_add_shock (gdt_shock* S) 
{
#if 0
  if (S->prjE())
    assert (S->tau() >=0 && S->tau() <= S->prjE()->len());
  else
    assert (S->tau() >=0 && S->tau() <= S->cur_edge()->len());
  assert (_find_shock(S->tau()) == NULL);
#endif
  S_map_.insert (std::pair<double, gdt_shock*>(S->tauE(), S));
}

// ######################################################################

void gdt_shock_section::put_all_shocks_to_queue (std::map<std::pair<double, int>, gdt_shock*>* shock_queue)
{
  std::map<double, gdt_shock*>::iterator sit = S_map_.begin();
  for (; sit != S_map_.end(); sit++) {
    gdt_shock* S = (*sit).second;
    std::pair<double, int> key (S->simT(), S->id()); //simtime
    shock_queue->insert (std::pair<std::pair<double, int>, gdt_shock*>(key, S));
  }
}
