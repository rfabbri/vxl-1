#ifndef _dbrl_transformation_h_
#define _dbrl_transformation_h_

#include<vbl/vbl_ref_count.h>
#include<vnl/vnl_vector.h>
#include<vcl_vector.h>
#include "dbrl_feature_sptr.h"
#include "dbrl_feature.h"
class dbrl_transformation:public vbl_ref_count
    {
    public:
        virtual ~dbrl_transformation(){}
        
    dbrl_transformation():from_features_(0){}
        
        virtual bool transform()=0;

                void set_from_features(const vcl_vector<dbrl_feature_sptr> & from_features);
        vcl_vector<dbrl_feature_sptr> & get_to_features(){return to_features_;};
        
        virtual void print_transformation(vcl_ostream &os)=0;

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

        virtual dbrl_transformation * clone() const=0;


    protected:
        vcl_vector<dbrl_feature_sptr> to_features_;
        vcl_vector<dbrl_feature_sptr> from_features_;

    };  

void vsl_add_to_binary_loader(dbrl_transformation const& b);

#endif
