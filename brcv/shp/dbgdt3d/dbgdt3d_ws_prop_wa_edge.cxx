//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>

#include <dbgdt3d/dbgdt3d_manager.h>


// #####################################################################

// Description: 
//   Propagate the simtime of degenerate interval W to its cur_edge e.
//   Only one event is possible, that is to strike the other ending vertex.
// Return: void.
//
void gdt_ws_manager::get_degeW_next_event (const gdt_welm* W,
                                           double& simtime, WE_NEXTEVENT& next_event)
{
  if (W->tVS() < GDT_HUGE) {
    simtime = W->tVS();
    next_event = WENE_SV;
  }
  else {
    assert (W->tVE() < GDT_HUGE);
    simtime = W->tVE();
    next_event = WENE_EV;
  }
}

// Description: 
//   Given a wavefront element W,
//   determine the next event of W with the event's simulation time.
//   W can be in STATUS of RF or PROP.
//   Determine the next event by cases.
//
// Return: 
//     The next status (event) with its simtime.
//
void gdt_ws_manager::get_psrcW_next_event (gdt_welm* W, double& simtime, WE_NEXTEVENT& next_event)
{
  if (W->_is_RF()) {
    // The only additional event is the footpoint strike.
    if (W->stau() < W->L() && W->L() < W->etau()) {
      simtime = W->mu() + W->H();
      next_event = WENE_RF_FPT;
    }
    else if (W->tVS() < W->tVE()) { //W-vS
      simtime = W->tVS();
      next_event = WENE_SV;
    }
    else if (W->tVE() < W->tVS()) { //W-vE
      simtime = W->tVE();
      next_event = WENE_EV;
    }
  }
  else {
    // Select the closest valid event among tVS, tVE, tEL, tER, tVO.
    // For tEL, tER, tVO, need to validate it with the shock, if any.
    simtime = GDT_HUGE;
    if (W->tVS() != GDT_HUGE && W->tVS() < simtime) {
      simtime = W->tVS();
      next_event = WENE_SV;
    }
    if (W->tVE() != GDT_HUGE && W->tVE() < simtime) {
      simtime = W->tVE();
      next_event = WENE_EV;
    }
    if (W->tEL() != GDT_HUGE && W->tEL() < simtime) {
      simtime = W->tEL();
      next_event = WENE_LE_FPT;
    }
    if (W->tER() != GDT_HUGE && W->tER() < simtime) {
      simtime = W->tER();
      next_event = WENE_RE_FPT;
    }
    if (W->tVO() != GDT_HUGE && W->tVO() < simtime) {
      simtime = W->tVO();
      next_event = WENE_OV;
    }
  }

  //Note that it is possible that next_event ends up with WENE_NA.
  //In this case, the next event of W is the S-S or S-E to finalize it.
}



