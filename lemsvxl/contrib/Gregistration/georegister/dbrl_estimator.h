#ifndef _dbrl_estmator_h_
#define _dbrl_estmator_h_

#include "dbrl_transformation_sptr.h"
#include "dbrl_correspondence.h"
#include <vbl/vbl_ref_count.h>
class dbrl_estimator: public vbl_ref_count
    {   
    public:
    dbrl_estimator(){};
    virtual ~dbrl_estimator(){};

    virtual
    dbrl_transformation_sptr estimate( std::vector<dbrl_feature_sptr> f1,
                                       std::vector<dbrl_feature_sptr> f2,
                                       dbrl_correspondence & M) const = 0;


    virtual
    double residual(const std::vector<dbrl_feature_sptr>& f1,
                    const std::vector<dbrl_feature_sptr>& f2,
                    const dbrl_correspondence &M,
                    const dbrl_transformation_sptr& tform) const = 0;
    //: Binary save self to stream.
    virtual void b_write(vsl_b_ostream &os) const=0;

    //: Binary load self from stream.
    virtual void b_read(vsl_b_istream &is)=0;

    //: Return IO version number;
    virtual short version() const=0;

    //: Print an ascii summary to the stream
    virtual void print_summary(std::ostream &os) const=0;

    //: Return a platform independent string identifying the class
    virtual std::string is_a() const =0;

    virtual dbrl_estimator * clone() const=0;

    };
void vsl_add_to_binary_loader(dbrl_estimator const& b);

#endif
