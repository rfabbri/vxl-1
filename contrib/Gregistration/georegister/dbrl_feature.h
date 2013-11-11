#ifndef dbrl_feature_h_
#define dbrl_feature_h_
#include <vcl_iostream.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include "dbrl_feature_sptr.h"
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>

#include<vnl/vnl_vector.h>

//class dbrl_feature;
// class dbrl_feature_sptr;
class dbrl_feature:public vbl_ref_count{

public:

    int id;
    //typedef vbl_smart_ptr<dbrl_feature> dfsptr;
    virtual ~dbrl_feature(){}

    //: computes the distance between two features
    virtual double  distance( const dbrl_feature_sptr  &f)=0;
    //: read in feature

    void setid(int id){id_=id;}

    int getid(){return id_;}
    // ==== Binary IO methods ======

    //: Binary save self to stream.
    virtual void b_write(vsl_b_ostream &os) const=0;

    //: Binary load self from stream.
    virtual void b_read(vsl_b_istream &is)=0;

    //: Return IO version number;
    virtual short version() const=0;

    //: Print an ascii summary to the stream
    virtual void print_summary(vcl_ostream &os) const=0;

    //: Return a platform independent string identifying the class
    virtual vcl_string is_a() const =0;

    virtual dbrl_feature * clone() const=0;

    virtual void print_feature(vcl_ostream &os)=0;
    vnl_vector<double> location_;

protected:
    dbrl_feature(){}
    int id_;



};
void vsl_add_to_binary_loader(dbrl_feature const& b);



#endif
