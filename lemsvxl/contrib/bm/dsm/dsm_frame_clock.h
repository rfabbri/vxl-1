//this is /contrib/bm/dsm/dsm_frame_clock.h
#ifndef	DSM_FRAME_CLOCK_H_
#define DSM_FRAME_CLOCK_H_
//:
// \file
// \date January 6, 2011
// \author Brandon A. Mayer
//
// Clock Singleton.
//
// \verbatim
//  Modifications
// \endverbatim
#include<vsl/vsl_binary_io.h>

//forward declare this class so that it is the only that can increment time
//all other classes can get an instance and look at the time.
template <unsigned>
class dsm_manager;

//forward declare the destroyer so that dsm_frame_clock
//may have a static instance of it.
//class dsm_frame_clock_destroyer;

class dsm_frame_clock
{
public:

	static dsm_frame_clock* instance();

	~dsm_frame_clock(){this->curr_time_ = 0;};

	unsigned time(){ return curr_time_; }

	void increment_time(){++curr_time_;}

	void reset_clock(){curr_time_ = 0;}

	void b_write(vsl_b_ostream& os) const; 

	void b_read(vsl_b_istream& is);

protected:
	dsm_frame_clock():curr_time_(0){}

private:
	//static dsm_frame_clock_destroyer destroyer_;
        template<unsigned>
	friend class dsm_manager;

	static dsm_frame_clock* instance_;

	unsigned curr_time_;
	
};

//: class whoes sole purpose is to destroy the singleton instance
//class dsm_frame_clock_destroyer
//{
//public:
//	dsm_frame_clock_destroyer( dsm_frame_clock* p = 0 ){p_ = p;};
//
//	~dsm_frame_clock_destroyer()
//	{ 
//		if(this->p_!=0) 
//			delete this->p_; 
//		this->p_ = 0; 
//	};
//
//	void set_singleton(dsm_frame_clock* p){p_ = p;};
//private:
//	dsm_frame_clock* p_;
//};

#endif //DSM_FRAME_CLOCK_H_
