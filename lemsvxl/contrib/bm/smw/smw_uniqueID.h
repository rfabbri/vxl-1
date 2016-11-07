#ifndef SMW_UNIQUEID_H_
#define SMW_UNIQUEID_H_

// this is contrib/bm/smw/smw_uniqueID.h
// Based on singular pattern. There will be
// only one instance of this class which is globally
// accessable. Each node will access this classes' 
// instance to retrieve a unique id.

class smw_uniqueID
{
public:
    static smw_uniqueID* instance();
    unsigned new_id();
protected:
    //protected constructor to keep people from trying to make
    //more than one instance of this class.
    smw_uniqueID(){}
private:
    //pointer to the sole instance of this class
    static smw_uniqueID* instance_;
    //the count which keeps track of the id's
    static unsigned next_id_;    
};

#endif //SMW_UNIQUEID_H_
