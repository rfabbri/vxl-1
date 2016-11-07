#ifndef psm_update_sample_h_
#define psm_update_sample_h_


class psm_update_sample
{
public:
  //: default constructor
  psm_update_sample() : update_multiplier(0.0f), do_update(false) {}

  // the (unnormalized) update multiplier
  float update_multiplier;
  bool do_update;
};

#endif

