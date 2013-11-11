//this is /conbrib/bm/dbcl/tests/test_linear_classifier.cxx

#include"../dbcl_classifier_factory.h"

#include"../dbcl_linear_classifier_vrml_view.h"

#include<testlib/testlib_test.h>

static void test_linear_classifier()
{
    //CLASS 0 TRAINING POINTS
    double pt1_class0[] = {7.31066154849254,	6.39019544075495};
    double pt2_class0[] = {13.1134469184557,	7.46216736299165};
    double pt3_class0[] = {9.48125846722250,	6.37404383577940};
    double pt4_class0[] = {9.10861960001379,	0.303886612179980};
    double pt5_class0[] = {6.73912231161601,	5.72069152156083};

    //CLASS 0 TEST POINTS 
    double pt6_class0[] = {6.95664769688404,	9.36019843507793};
    double pt7_class0[] = {9.02395665891853,	15.8331933758989};
    double pt8_class0[] = {8.28468034495163,	9.24990316589487};
    double pt9_class0[] = {5.29205358897791,	8.56785200891780};
    double pt10_class0[] = {5.98607003931322,	10.0908970713646};


    //CLASS 1 TRAINING POINTS
    double pt1_class1[] = {0.559260302555734,	-0.787239604671919};
    double pt2_class1[] = {-4.10186064025462,	-6.89162867824175};
    double pt3_class1[] = {2.38003955542794,	4.15012819881360};
    double pt4_class1[] = {-1.63849820090183,	-4.96970481266356};
    double pt5_class1[] = {1.59331442294463,	-3.77796865450776};

    //CLASS1 TEST POINTS
    double pt6_class1[] = {-3.40942808967336,	0.659132131218082};
    double pt7_class1[] = {-6.15565940333251,	-7.87026186269576};
    double pt8_class1[] = {-0.737948899396250,	-0.797786005559114};
    double pt9_class1[] = {-1.71457352640749,	-0.509946825090409};
    double pt10_class1[] = {1.24672178848209,	0.911343365063715};

    double* training_ptrs[] = {pt1_class0, pt2_class0, pt3_class0, 
                               pt4_class0, pt5_class0, pt1_class1, 
                               pt2_class1, pt3_class1, pt4_class1,
                               pt5_class1};

    double* test_ptrs[] = {pt6_class0, pt7_class0, pt8_class0,
                           pt9_class0, pt10_class0, pt6_class1,
                           pt7_class1, pt8_class1, pt9_class1,
                           pt10_class1};

    dbcl_classifier::feature_map_type training_feature_map;
    dbcl_classifier::feature_map_type test_feature_map;

    for( unsigned i = 0; i < 10; ++i )
    {
        if(i < 5)
        {
            dbcl_temporal_feature_sptr training_feature_sptr = new dbcl_temporal_feature(i,vnl_vector<double>(training_ptrs[i],2),0);
            training_feature_map[i] = training_feature_sptr;
        }
        else
        {
            dbcl_temporal_feature_sptr training_feature_sptr = new dbcl_temporal_feature(i,vnl_vector<double>(training_ptrs[i],2),1);
            training_feature_map[i] = training_feature_sptr;
        }
        
        dbcl_temporal_feature_sptr test_feature_sptr = new dbcl_temporal_feature(i,vnl_vector<double>(test_ptrs[i],2),0);
        
        test_feature_map[i] = test_feature_sptr;

        //vcl_cout << "temporal_feature_sptr[i] = " << temporal_feature_sptr->feature_vector(0) << '\t' << temporal_feature_sptr->feature_vector(1) << '\n';
    }

    dbcl_classifier::feature_map_type::iterator fm_itr;
    dbcl_classifier::feature_map_type::iterator fm_end = training_feature_map.end();

    
    dbcl_classifier_factory factory(LINEAR_CLASSIFIER,training_feature_map,test_feature_map);

    dbcl_classifier_sptr classifier_sptr = factory.classifier();

    //dbcl_linear_classifier_sptr linear_classifier_sptr = static_cast<dbcl_linear_classifier_sptr>(classifier_sptr);
    //dbcl_linear_classifier_sptr linear_classifier_sptr = classifier_sptr;



    dbcl_linear_classifier_sptr linear_classifier_sptr = dynamic_cast<dbcl_linear_classifier*>(classifier_sptr.as_pointer());

    classifier_sptr->classify();

    vnl_matrix<double> w = linear_classifier_sptr->W();

    TEST_NEAR("w(0,0)",w(0,0),0.118081328032717,0.000001);
    TEST_NEAR("w(1,0)",w(1,0),0.085623445128345,0.000001);
    TEST_NEAR("w(2,0)",w(2,0),3.620264954550395e-04,0.00001);
    TEST_NEAR("w(0,1)",w(0,1),0.881918671967283,0.00001);
    TEST_NEAR("w(1,1)",w(1,1),-0.085623445128345,0.00001);
    TEST_NEAR("w(2,1)",w(2,1),-3.620264954550673e-04,0.00001);
        
    //vcl_cout << "w = " << '\n' << w << '\n';

    //vcl_cout << "linear_classifier_sptr = " << linear_classifier_sptr.as_pointer() <<'\n';
    //vcl_cout << "W = " << '\n' << linear_classifier_sptr->W() << '\n';
    //vcl_cout << "X = " << '\n' << linear_classifier_sptr->X() << '\n';
    //vcl_cout << "T = " << '\n' << linear_classifier_sptr->T() << '\n';

    dbcl_classifier::feature_map_type test_data = classifier_sptr->test_data();

    dbcl_classifier::feature_map_type::iterator test_data_itr;
    dbcl_classifier::feature_map_type::iterator test_data_end = test_data.end();

    unsigned cnt;
    for( cnt = 1, test_data_itr = test_data.begin(); test_data_itr != test_data_end; ++test_data_itr, ++cnt )
    {
        //vcl_cout << "test point " << test_data_itr->first << " = " << test_data_itr->second->label() << vcl_endl;
        vcl_stringstream stream;
        stream << test_data_itr->first;
        vcl_string test_string = "test label: " + stream.str();
        if( cnt < 6 )
            TEST_NEAR(test_string.c_str(),test_data_itr->second->label(),0,0.00001);
        else
            TEST_NEAR(test_string.c_str(), test_data_itr->second->label(),1,0.00001);
    }

    vcl_string filename = "linear_classifier_view.wrl";
    vcl_ofstream of(filename.c_str(),vcl_ios::out);
    dbcl_linear_classifier_vrml_view::vrml_view(of,classifier_sptr);


    //test 3 classes
    vcl_vector<vgl_point_2d<double> > class0;
    vcl_vector<vgl_point_2d<double> > class1;
    vcl_vector<vgl_point_2d<double> > class2;

    
    class0.push_back(vgl_point_2d<double>(5.53766713954610,	6.83388501459509));
    class0.push_back(vgl_point_2d<double>(4.56640797769432,	5.34262446653865));
    class0.push_back(vgl_point_2d<double>(5.72540422494611,	4.93694512681034));
    class0.push_back(vgl_point_2d<double>(6.40903448980048,	6.41719241342961));
    class0.push_back(vgl_point_2d<double>(5.48889377031179,	6.03469300991786));
    class0.push_back(vgl_point_2d<double>(5.88839563175764,	3.85292989303085));
    class0.push_back(vgl_point_2d<double>(5.32519053945620,	4.24507168083030));
    class0.push_back(vgl_point_2d<double>(5.31920673916550,	5.31285859663743));
    class0.push_back(vgl_point_2d<double>(6.09326566903948,	6.10927329761440));
    class0.push_back(vgl_point_2d<double>(4.99315067189665,	6.53263030828475));

    class1.push_back(vgl_point_2d<double>(-7.25884686100365,	-4.13782667963188));
    class1.push_back(vgl_point_2d<double>(-1.42160306027424,	-2.23056297011512));
    class1.push_back(vgl_point_2d<double>(-4.28525709617390,	-5.20496605829977));
    class1.push_back(vgl_point_2d<double>(-4.32850286639192,	-6.20748692268504));
    class1.push_back(vgl_point_2d<double>(-4.27311486661676,	-5.30344092478602));
    class1.push_back(vgl_point_2d<double>(-6.06887045816803,	-5.80949869442488));
    class1.push_back(vgl_point_2d<double>(-3.62970145990477,	-6.71151641885370));
    class1.push_back(vgl_point_2d<double>(-5.86487991732446,	-5.03005129619627));
    class1.push_back(vgl_point_2d<double>(-5.86365282198871,	-4.92264090886958));
    class1.push_back(vgl_point_2d<double>(-5.76966591375368,	-4.62862118723994));

    class2.push_back(vgl_point_2d<double>(5.31876523985898,	-6.30768829630527));
    class2.push_back(vgl_point_2d<double>(3.65011305984348,	-1.96507653366815));
    class2.push_back(vgl_point_2d<double>(4.87585565178369,	-3.51030239221454));
    class2.push_back(vgl_point_2d<double>(5.71723865132884,	-3.36976471083527));
    class2.push_back(vgl_point_2d<double>(5.29387146709666,	-5.78728280375864));
    class2.push_back(vgl_point_2d<double>(2.05571583800510,	-3.56161970718490));
    class2.push_back(vgl_point_2d<double>(4.89775755391451,	-5.24144704160736));
    class2.push_back(vgl_point_2d<double>(4.83512098079096,	-4.37229271247127));
    class2.push_back(vgl_point_2d<double>(3.78588295638459,	-6.11350074148677));
    class2.push_back(vgl_point_2d<double>(4.77441559772875,	-3.88264386118553));

    dbcl_classifier::feature_map_type training_feature_map2, test_feature_map2;
        
    unsigned training_count = 0;
    unsigned test_count = 0;

    for( unsigned i = 0; i < 30; ++i )
    {

        vnl_vector<double> feature_vector(2);
        if( i < 5 )//0-4 class0 training
        {
            feature_vector.put(0,class0[i].x());
            feature_vector.put(1,class0[i].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector,0);
            training_feature_map2[training_count] = feature_sptr;
            ++training_count;
        }
        else if( i >= 5  && i < 10 ) //5-9 class0 test
        {
            feature_vector.put(0,class0[i].x());
            feature_vector.put(1,class0[i].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector);
            test_feature_map2[test_count] = feature_sptr;
            ++test_count;
        }
        else if( i >= 10 && i < 15 ) //10-14 class1 training
        {
            feature_vector.put(0,class1[i-10].x());
            feature_vector.put(1,class1[i-10].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector,1);
            training_feature_map2[training_count] = feature_sptr;
            ++training_count;
        }
        else if( i >= 15 && i < 20 ) //15-19 class1 test
        {
            feature_vector.put(0,class1[i-10].x());
            feature_vector.put(1,class1[i-10].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector);
            test_feature_map2[test_count] = feature_sptr;
            ++test_count;
        }
        else if( i >= 20 && i < 25 ) //20-24 class2 training
        {
            feature_vector.put(0,class2[i-20].x());
            feature_vector.put(1,class2[i-20].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector,2);
            training_feature_map2[training_count] = feature_sptr;
            ++training_count;
        }
        else if( i >=25 && i < 30 ) //25-30 class2 test
        {
            feature_vector.put(0,class2[i-20].x());
            feature_vector.put(1,class2[i-20].y());
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(i,feature_vector);
            test_feature_map2[test_count] = feature_sptr;
            ++test_count;
        }   
    }//end point iteration

    dbcl_classifier_factory factory2(LINEAR_CLASSIFIER,training_feature_map2,test_feature_map2);

    dbcl_classifier_sptr classifier_sptr2 = factory2.classifier();

    classifier_sptr2->classify();

    vnl_matrix<double> w_ground_truth(3,3);
    w_ground_truth(0,0) = 0.420385270384733; w_ground_truth(0,1) = 0.538286659544522; w_ground_truth(0,2) = 0.0413280700707448;
    w_ground_truth(1,0) = 7.20851606157355e-05; w_ground_truth(1,1) = -0.0983630420651487; w_ground_truth(1,2) = 0.0982909569045330;
    w_ground_truth(2,0) = 0.0904633497032194;	w_ground_truth(2,1) = 0.00162382419728342; w_ground_truth(2,2) = -0.0920871739005029;

    double tol = 0.00001;

    dbcl_linear_classifier_sptr linear_classifier_sptr2 = dynamic_cast<dbcl_linear_classifier*>(classifier_sptr.as_pointer());

    vnl_matrix<double> w2 = linear_classifier_sptr2->W();

    TEST_NEAR("w2(0,0) = ", w2(0,0), w_ground_truth(0,0), tol);
    TEST_NEAR("w2(0,1) = ", w2(0,1), w_ground_truth(0,1), tol);
    TEST_NEAR("w2(0,2) = ", w2(0,2), w_ground_truth(0,2), tol);
    TEST_NEAR("w2(1,0) = ", w2(1,0), w_ground_truth(1,0), tol);
    TEST_NEAR("w2(1,1) = ", w2(1,1), w_ground_truth(1,1), tol);
    TEST_NEAR("w2(1,2) = ", w2(1,2), w_ground_truth(1,2), tol);
    TEST_NEAR("w2(2,0) = ", w2(2,0), w_ground_truth(2,0), tol);
    TEST_NEAR("w2(2,1) = ", w2(2,1), w_ground_truth(2,1), tol);
    TEST_NEAR("w2(2,2) = ", w2(2,2), w_ground_truth(2,2), tol);

    vcl_string filename2 = "linear_classifier_3_classes_view.wrl";
    vcl_ofstream of2(filename2.c_str(),vcl_ios::out);
    dbcl_linear_classifier_vrml_view::vrml_view(of2,classifier_sptr2);

}//end static void test_linear_classifier

TESTMAIN(test_linear_classifier);