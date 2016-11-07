#include "biob_find_roster_bounding_box.h"

biob_worldpt_box biob_find_roster_bounding_box(biob_worldpt_roster_sptr roster, double margin){
  worldpt pt = roster->point(biob_worldpt_index(0));
  double minx = pt.x();
  double maxx = pt.x();
  double miny = pt.y();
  double maxy = pt.y();
  double minz = pt.z();
  double maxz = pt.z();
  for (unsigned long int i =0; i < roster->num_points(); ++i){
    pt = roster->point(biob_worldpt_index(i));
    if (pt.x() < minx) minx = pt.x();
    if (pt.x() > maxx) maxx = pt.x();
    if (pt.y() < miny) miny = pt.y();
    if (pt.y() > maxy) maxy = pt.y();
    if (pt.z() < minz) minz = pt.z();
    if (pt.z() > maxz) maxz = pt.z();
  }
  return biob_worldpt_box(worldpt(minx-margin, miny-margin, minz-margin), worldpt(maxx+margin, maxy+margin, maxz+margin));
}
