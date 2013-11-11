#ifndef psm_vis_implicit_sample_h_
#define psm_vis_implicit_sample_h_


class psm_vis_implicit_sample
{
public:
  //: default constructor
  psm_vis_implicit_sample() : max_vis(0.0f), n_obs(0) {}

  float max_vis;
  unsigned int n_obs;
};

#endif

