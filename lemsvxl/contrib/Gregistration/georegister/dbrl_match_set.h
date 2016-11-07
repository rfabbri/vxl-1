#ifndef _dbrl_match_set_h_
#define _dbrl_match_set_h_

#include "dbrl_correspondence.h"
#include "dbrl_feature_sptr.h"
#include "dbrl_transformation_sptr.h"
#include "dbrl_estimator_sptr.h"
class dbrl_match_set:public vbl_ref_count
    {

    public:

        dbrl_match_set();
        dbrl_match_set(dbrl_correspondence Minit,
                       dbrl_transformation_sptr tform,
                       dbrl_estimator_sptr estimator);
        ~dbrl_match_set();


        void set_original_features(vcl_vector<dbrl_feature_sptr> f1,vcl_vector<dbrl_feature_sptr> f2);
        void set_mapped_features(vcl_vector<dbrl_feature_sptr>   f1,vcl_vector<dbrl_feature_sptr> f2);
        void set_correspondence_matrix(dbrl_correspondence & M);
        void set_transformation(dbrl_transformation_sptr & tform);
        void set_transformation_estimator(dbrl_estimator_sptr & estimator);

        dbrl_feature_sptr correspondence1to2(int i);
        dbrl_feature_sptr correspondence2to1(int i);

        vcl_map<double,dbrl_feature_sptr> correspondences1to2(int i);
        vcl_map<double,dbrl_feature_sptr> correspondences2to1(int i);

        int cindex1to2(int i);
        int cindex2to1(int i);

        dbrl_feature_sptr feature_set1_i(int i);
        dbrl_feature_sptr feature_set2_i(int i);

        dbrl_feature_sptr xformed_feature_set1_i(int i);
        dbrl_feature_sptr xformed_feature_set2_i(int i);

        int size_set1(){return feature_set1.size();}
        int size_set2(){return feature_set2.size();}


        double operator()(unsigned i,unsigned j);

        dbrl_transformation_sptr get_transformation(){return tform_;}
        dbrl_estimator_sptr get_estimator(){return estimator_;}
        dbrl_correspondence get_correspondence(){return M_;}
        vcl_vector<dbrl_feature_sptr> get_feature_set1(){return feature_set1;}
        vcl_vector<dbrl_feature_sptr> get_feature_set2(){return feature_set2;}

        vcl_vector<dbrl_feature_sptr> get_xformed_feature_set1(){return xformed_feature_set1;}
        vcl_vector<dbrl_feature_sptr> get_xformed_feature_set2(){return xformed_feature_set2;}

        //: Binary save self to stream.
        void b_write(vsl_b_ostream &os) const;

        //: Binary load self from stream.
        void b_read(vsl_b_istream &is);

        //: Return IO version number;
        short version() const;

        //: Print an ascii summary to the stream
        void print_summary(vcl_ostream &os) const;

        //: Return a platform independent string identifying the class
        virtual vcl_string is_a() const {return "dbrl_match_set";}

        virtual dbrl_match_set * clone() const;

        void normalize_point_set(vnl_matrix<double> & M, vcl_vector<dbrl_feature_sptr> & f);

    protected:

        dbrl_correspondence M_;

        //todo: some mapping functions
        dbrl_transformation_sptr tform_;

        dbrl_estimator_sptr estimator_;

        vcl_vector<dbrl_feature_sptr> feature_set1;
        vcl_vector<dbrl_feature_sptr> feature_set2;

        vcl_vector<dbrl_feature_sptr> xformed_feature_set1;
        vcl_vector<dbrl_feature_sptr> xformed_feature_set2;


    };

//: Binary save dbrl_match_set* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_match_set* p);

//: Binary load dbrl_match_set* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_match_set* &p);


#endif
