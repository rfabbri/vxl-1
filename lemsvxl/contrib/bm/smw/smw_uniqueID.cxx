//this is contrib/bm/smw/smw_uniqueID.cxx

#include "smw/smw_uniqueID.h"

//forward declare the class because no one can call the constructor
static smw_uniqueID uniqueID;

//initialize the static members
unsigned smw_uniqueID::next_id_ = 0;
smw_uniqueID* smw_uniqueID::instance_ = 0;

smw_uniqueID* smw_uniqueID::instance()
{
    if( instance_ = 0 )
        instance_ = new smw_uniqueID;
    return instance_;
}

unsigned smw_uniqueID::new_id()
{
    unsigned temp = next_id_;
    ++next_id_;
    return temp;
}
