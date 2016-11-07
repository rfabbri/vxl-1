//this is /contrib/bm/smw/smw_frame_clock.h
#ifndef SMW_FRAME_CLOCK_H_
#define SMW_FRAME_CLOCK_H_

//Forward declare the world so that we can make the world a
//friend class so that it is the only external class that
//can increment the time.
class smw_world;

//clock will start at zero time, after that the world must increment the
//time when it sees a new image but not before the first image.

class smw_frame_clock
{
public:

    static smw_frame_clock* instance();
   
    unsigned time(){return curr_time_;}

protected:

    smw_frame_clock(){}
    
    ~smw_frame_clock(){}

private:
    friend class smw_world;

    void increment_time(){++curr_time_;}
    
    static unsigned curr_time_;
    
    static smw_frame_clock* instance_;
};

#endif //SMW_FRAME_CLOCK_H_
