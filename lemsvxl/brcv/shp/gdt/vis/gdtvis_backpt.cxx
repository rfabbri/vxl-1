//: 050223 MingChing Chang

#include <gdt/vis/gdtvis_backpt.h>

// ==========================================================
//    The Homemade vis_pointer OpenInventor Classes
// ==========================================================

SO_NODE_SOURCE (gdtvis_interval_SoLineSet);
SO_NODE_SOURCE (gdtvis_welm_SoLineSet);
SO_NODE_SOURCE (gdtvis_shock_SoLineSet);

void gdtvis_init_vispt_OpenInventor_classes ()
{
  gdtvis_interval_SoLineSet::initClass ();
  gdtvis_welm_SoLineSet::initClass ();
  gdtvis_shock_SoLineSet::initClass ();
}
// ==========================================================

gdtvis_interval_SoLineSet::gdtvis_interval_SoLineSet (const gdt_interval* I)
{
  SO_NODE_CONSTRUCTOR (gdtvis_interval_SoLineSet);
  this->isBuiltIn = true;

  _element = (gdt_interval*) I;
  _element->set_vis_pointer ((void*)this);
}

gdtvis_welm_SoLineSet::gdtvis_welm_SoLineSet (const gdt_welm* W)
{
  SO_NODE_CONSTRUCTOR (gdtvis_welm_SoLineSet);
  this->isBuiltIn = true;

  _element = (gdt_welm*) W;
  _element->set_vis_pointer ((void*)this);
}

gdtvis_shock_SoLineSet::gdtvis_shock_SoLineSet (const gdt_shock* S)
{
  SO_NODE_CONSTRUCTOR (gdtvis_shock_SoLineSet);
  this->isBuiltIn = true;

  _element = (gdt_shock*) S;
  _element->set_vis_pointer ((void*) this);
}

