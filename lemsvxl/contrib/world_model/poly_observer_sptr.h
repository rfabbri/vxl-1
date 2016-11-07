// This is contrib/world_model/poly_observer_sptr.h
#ifndef poly_observer_sptr_h_
#define poly_observer_sptr_h_
//:
// \file
// \author Gamze Tunali
// \date   21 Nov 2006
// \brief  Smart-pointer to a poly_observer tableau.

#include <vgui/vgui_easy2D_tableau_sptr.h>

class poly_observer;
typedef vgui_tableau_sptr_t<poly_observer,vgui_easy2D_tableau_sptr> poly_observer_sptr;

#endif // poly_observer_sptr_h_
