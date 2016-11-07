#ifndef _dbrl_local_global_match_set_storage_
#define _dbrl_local_global_match_set_storage_

#include <bpro1/bpro1_storage.h>
#include <georegister/dbrl_match_set_sptr.h>
#include "dbrl_local_global_match_set_storage_sptr.h"

class dbrl_local_global_match_set_storage: public bpro1_storage
    {
    public:
        dbrl_local_global_match_set_storage();
        dbrl_local_global_match_set_storage(const dbrl_match_set_sptr & dms);
        ~dbrl_local_global_match_set_storage();
        //: Returns the type string "dbrl_match_set"
    virtual vcl_string type() const { return "dbrl_local_global_match_set"; }

    virtual void register_binary_io() const ;
    //: Set the dbrl_match_set storage 
    void set_match_set(const dbrl_match_set_sptr dms);

    virtual vcl_string is_a() const {return "dbrl_local_global_match_set_storage";};

    virtual bpro1_storage* clone() const;

    //: Return IO version number;
    short version() const;

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    dbrl_match_set_sptr matchset();
    private:
        //: The vector of  ctrk smart pointers
        dbrl_match_set_sptr matchset_;  

    };
//: Create a smart-pointer to a dbrl_local_global_match_set_storage.
struct dbrl_local_global_match_set_storage_new : public dbrl_local_global_match_set_storage_sptr
{
  typedef dbrl_local_global_match_set_storage_sptr base;

  //: Constructor - creates a default dbbgm_distribution_image_storage_sptr.
  dbrl_local_global_match_set_storage_new() : base(new dbrl_local_global_match_set_storage()) { }

  //: Constructor - creates a dbbgm_distribution_image_storage_sptr with data.
  dbrl_local_global_match_set_storage_new(const dbrl_match_set_sptr& dms)
   : base(new dbrl_local_global_match_set_storage(dms)) { }
};

#endif
