#if !defined(SPLR_UNIFORM_ORBIT_SYMMETRY_H_)
#define SPLR_UNIFORM_ORBIT_SYMMETRY_H_

#include <bio_defs.h>
#include <biob/biob_worldpt_transformation.h>

#include <splr_uniform_orbit_index_symmetry.h>

class splr_uniform_orbit_symmetry : public splr_worldpt_orbit_index_symmetry {
 public:
   splr_uniform_orbit_symmetry(xscan_uniform_orbit orbit);
   biob_worldpt_transformation operator()(orbit_index &t);
 private:
   xscan_uniform_orbit orbit_;
#endif
