//this is /contrib/bm/dsm2/algo/dsm2_num_obs.h
#ifndef DSM2_NUM_OBS_H_
#define DSM2_NUM_OBS_H_
//:
// \file
// \date August 15, 2011
// \author Brandon A. Mayer
//
// Class based on the "Curiously Repeating Template Pattern". 
// Aka Static Polymorphism. The objective is to extend any class
// to have a variable which records a count. This is necessary
// as some statistical algorithms (e.g. calculating a running average)
// require knowledge about how many observations have been seen by
// the class
//
// \verbatim
//  Modifications
// \endverbatim

template<class base>
class dsm2_num_obs: public base
{
public:
	dsm2_num_obs(unsigned const& c = unsigned(0)):num_obs(c){}

	~dsm2_num_obs(){}

	unsigned num_obs;
};

#define DSM2_NUM_OBS_INSTANTIATE(...)\
template class dsm2_num_obs<__VA_ARGS__>

#endif //DSM2_NUM_OBS_H_