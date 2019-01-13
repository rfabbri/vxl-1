// This is brl/bseg/bmrf/bmrf_network_builder_params.cxx
#include "bmrf_network_builder_params.h"
//:
// \file
// See bmrf_network_builder_params.h
//
//-----------------------------------------------------------------------------
#include <sstream>
#include <iostream>

//------------------------------------------------------------------------
// Constructors
//

bmrf_network_builder_params::
bmrf_network_builder_params(const bmrf_network_builder_params& tp)
  : gevd_param_mixin()
{
  InitParams(tp.eu_,
             tp.ev_,
             tp.elu_,
             tp.elv_min_,
             tp.elv_max_,
             tp.Ns_,
             tp.max_delta_recip_s_
            );
}

bmrf_network_builder_params::
bmrf_network_builder_params(const float eu,
                            const float ev,
                            const int elu,
                            const int elv_min,
                            const int elv_max,
                            const int Ns,
                            const float gamma_max)
{
  InitParams(eu,
             ev,
             elu,
             elv_min,
             elv_max,
             Ns,
             gamma_max);
}

void bmrf_network_builder_params::InitParams(float eu,
                                             float ev,
                                             int elu,
                                             int elv_min,
                                             int elv_max,
                                             int Ns,
                                             float max_delta_recip_s)
{
  eu_ = eu;
  ev_ = ev;
  elu_ =  elu;
  elv_min_ = elv_min;
  elv_max_ = elv_max;
  Ns_ = Ns;
  max_delta_recip_s_ = max_delta_recip_s;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool bmrf_network_builder_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const bmrf_network_builder_params& tp)
{
  return
  os << "bmrf_network_builder_params:\n[---\n"
     << "epi_u " << tp.eu_ << std::endl
     << "epi_v " << tp.ev_ << std::endl
     << "epi space col " << tp.elu_ << std::endl
     << "min epi space row " << tp.elv_min_ << std::endl
     << "max epi space row " << tp.elv_max_ << std::endl
     << "Samples in s " << tp.Ns_ << std::endl
     << "---]" << std::endl;
}
