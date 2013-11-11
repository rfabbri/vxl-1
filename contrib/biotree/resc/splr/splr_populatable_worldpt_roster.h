#if !defined(SPLR_POPULATABLE_WORLDPT_ROSTER_H_)
#define SPLR_POPULATABLE_WORLDPT_ROSTER_H_

#include <biob/biob_worldpt_roster.h>
#include <biob/biob_worldpt_box.h>

class splr_populatable_worldpt_roster : public biob_worldpt_roster {
  virtual void populate(biob_worldpt_box bounding_box, double spacing) = 0;
};

#endif
