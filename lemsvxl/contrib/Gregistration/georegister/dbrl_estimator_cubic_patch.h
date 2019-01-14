#ifndef _dbrl_estimator_cubic_patch_h_
#define _dbrl_estimator_cubic_patch_h_
#include <iostream>
#include <vector>
#include "dbrl_feature_sptr.h"
#include "dbrl_correspondence.h"
#include "dbrl_transformation_sptr.h"
#include "dbrl_estimator.h"
#include <dt/dbrl_clough_tocher_patch.h>
#include <dt/partial_derivatives.h>
#include <dt/Delaunay.h>
class dbrl_estimator_cubic_patch: public dbrl_estimator
    {   
    public:
        dbrl_estimator_cubic_patch();
        virtual ~dbrl_estimator_cubic_patch(){};

        virtual dbrl_transformation_sptr estimate( std::vector<dbrl_feature_sptr> f1,
                                                   std::vector<dbrl_feature_sptr> f2,
                                                   dbrl_correspondence & M) const;
        dbrl_transformation_sptr build_cubic_patches( std::vector<dbrl_feature_sptr> f1,
                                                      std::vector<dbrl_feature_sptr> f2) const;
        virtual
            double residual(const std::vector<dbrl_feature_sptr>& f1,
                            const std::vector<dbrl_feature_sptr>& f2,
                            const dbrl_correspondence &M,
                            const dbrl_transformation_sptr& tform) const;
        std::vector<dbrl_clough_tocher_patch>  estimate_cubic(vnl_matrix<double> xy, vnl_vector<double> zs)const;

        std::vector<dbrl_feature_sptr> normalize_point_set( std::vector<dbrl_feature_sptr>  f2,
                                  dbrl_correspondence  M) const;

        //: Binary save self to stream.
        void b_write(vsl_b_ostream &os) const;

        //: Binary load self from stream.
        void b_read(vsl_b_istream &is);

        //: Return IO version number;
        short version() const;

        //: Print an ascii summary to the stream
        void print_summary(std::ostream &os) const;

        //: Return a platform independent string identifying the class
        virtual std::string is_a() const {return "dbrl_estimator_cubic_patch";}

        virtual dbrl_estimator * clone() const;
    protected:

};
//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_cubic_patch* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_cubic_patch* &p);

#endif
