//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_mesh.h>

void gdt_manager_i_based::_get_intervals_subtree (gdt_interval* input_I, 
                                          vcl_set<gdt_interval*>* intervals_subtree)
{
  //: add itself into the set
  intervals_subtree->insert (input_I);

  //: recursion on the subtree.
  vcl_vector<gdt_interval*>::iterator it = input_I->nextIs().begin();
  for (; it != input_I->nextIs().end(); it++) {
    gdt_interval* I = (*it);
    assert (I->prev_flag()==true);

    _get_intervals_subtree (I, intervals_subtree);
  }
}






