#include "bcal_camera.h"
#include <cassert>

bcal_camera::bcal_camera(int id) : id_(id)
{
}


void bcal_camera::set_lens_model(std::vector<bool> flags)
{
  assert(flags.size()==7);
  for (int i=0; i<7; i++) {
    if (flags[i])
      lm_.turn_on(i);
    else
      lm_.turn_off(i);
  }
}
