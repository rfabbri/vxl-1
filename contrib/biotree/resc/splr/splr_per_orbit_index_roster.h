#if !defined(SPLR_PER_ORBIT_INDEX_ROSTER_H_)
#define SPLR_PER_ORBIT_INDEX_ROSTER_H_

#include <bio_defs.h>
#include <biob/biob_worldpt_roster.h>
#include <splr/splr_map.h>
#include <xscan/xscan_scan.h>
#include <vcl_vector.h>

//Maybe need to add io for this?

class splr_per_orbit_index_roster : public biob_worldpt_roster {
 private:
//   typedef value vcl_vector<worldpt>;
//   typedef key orbit_index;

   typedef splr_map<orbit_index, vcl_vector<worldpt> > map;
   map map_;
   xscan_scan scan_;
   worldpt_box bounding_box_;
   double spacing_;
 public:
   void populate(orbit_index t);
   splr_per_orbit_index_roster(xscan_scan scan, worldpt_box bounding_box, double spacing);
}


#endif
