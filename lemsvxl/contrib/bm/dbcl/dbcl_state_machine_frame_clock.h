//this is /contrib/bm/dbcl/dbcl_state_machine_frame_clock.h
#ifndef dbcl_state_machine_frame_clock_h_
#define dbcl_state_machine_frame_clock_h_
//:
// \file
// \date August 31, 2010
// \author Brandon A. Mayer
//
// Clock implemented as not implemented as modified singleton so that each classifier may keep its own time but nodes can only acess the classifiers
// clock sptr. 
//
// \verbatim
//  Modifications
// \endverbatim

#include<vbl/vbl_ref_count.h>

//forward declare as friend so the classifier can increment time

class dbcl_state_machine_classifier;


class dbcl_state_machine_frame_clock: public vbl_ref_count
{
public:

    //static dbcl_state_machine_frame_clock* instance();

    //dbcl_state_machine_frame_clock* instance();

    unsigned time(){ return curr_time_; }

//protected:

private:
    friend class dbcl_state_machine_classifier;

    dbcl_state_machine_frame_clock():curr_time_(0){}

    ~dbcl_state_machine_frame_clock(){}

    void increment_time(){++curr_time_;}

    void reset_clock(){ curr_time_ = 0; }

    //static unsigned curr_time_;

    unsigned curr_time_;

    //static dbcl_state_machine_frame_clock* instance_;
};

#endif //dbcl_state_machine_frame_clock_h_