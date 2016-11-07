#ifndef _dbrl_id_point_2d_storage_
#define _dbrl_id_point_2d_storage_

#include <bpro1/bpro1_storage.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include "dbrl_id_point_2d_storage_sptr.h"

class dbrl_id_point_2d_storage: public bpro1_storage
    {
    public:
        dbrl_id_point_2d_storage();
        dbrl_id_point_2d_storage(const vcl_vector<dbrl_id_point_2d_sptr>  points);
        ~dbrl_id_point_2d_storage();
        //: Returns the type string "dbrl_match_set"
        virtual vcl_string type() const { return "dbrl_id_point_2d"; }

    //: Set the dbrl_id_point_2d storage 
    void set_id_points(const vcl_vector<dbrl_id_point_2d_sptr> points);

    virtual vcl_string is_a() const {return "dbrl_id_point_2d_storage";};

    virtual bpro1_storage* clone() const;

    //: Return IO version number;
    short version() const;

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    vcl_vector<dbrl_id_point_2d_sptr> points();
    private:
        //: The vector of  id point 2d pointers
        vcl_vector<dbrl_id_point_2d_sptr> idpoints_;  

    };
//: Create a smart-pointer to a dbrl_id_point_2d_storage.
struct dbrl_id_point_2d_storage_new : public dbrl_id_point_2d_storage_sptr
{
  typedef dbrl_id_point_2d_storage_sptr base;

  //: Constructor - creates a default dbbgm_distribution_image_storage_sptr.
  dbrl_id_point_2d_storage_new() : base(new dbrl_id_point_2d_storage()) { }

  //: Constructor - creates a dbbgm_distribution_image_storage_sptr with data.
  dbrl_id_point_2d_storage_new(const vcl_vector<dbrl_id_point_2d_sptr> points)
   : base(new dbrl_id_point_2d_storage(points)) { }
};

#endif
