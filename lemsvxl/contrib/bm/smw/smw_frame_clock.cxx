//this is /contrib/bm/smw/smw_frame_clock.cxx

#include "smw/smw_frame_clock.h"

unsigned smw_frame_clock::curr_time_ = 0;

smw_frame_clock* smw_frame_clock::instance_ = 0;

smw_frame_clock* smw_frame_clock::instance()
{
    if( instance_ == 0 )
        instance_ = new smw_frame_clock;

    return instance_;
}


// smw_frame_clock::~smw_frame_clock()
// {
//     delete instance_;
    
//     instance_ = 0;
// }

//static smw_frame_clock frame_clock;
