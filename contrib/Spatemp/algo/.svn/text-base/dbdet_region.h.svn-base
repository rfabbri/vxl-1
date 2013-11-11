#ifndef dbdet_region_h_
#define dbdet_region_h_
#include <Spatemp/algo/dbdet_temporal_model.h>
#include <vnl/vnl_matrix.h>

//: class to hold a visula fragment
class dbdet_region{
public:
    dbdet_region(){};
    ~dbdet_region(){};
    //: collect the triangles
    vcl_vector<int> triangles_;
    
    int id_;
    //: collect the ids of the edges
    vcl_set<int> node_ids_;

    dbdet_3D_velocity_model * V;

    //: id of the first contour
    int c_id1;
    //: id of the second contour
    int c_id2;

    //vcl_map<int,int> ids_ws;

    // vcl_map<int,float> ids_weights;

    //vcl_map<int,dbdet_temporal_model*> ids_model;

    //vcl_map<int,double> label_prob_;
    void compute_V();

    vcl_map<int,dbdet_spherical_histogram_sptr > Vs;
    dbdet_spherical_histogram_sptr Vdist;

    void range_for_phi();

    //vnl_vector<float> v1;
    //vnl_vector<float> v2;

    //vnl_matrix<float> v1v1;
    //vnl_matrix<float> v1v2;
    //vnl_matrix<float> v2v2;

    vcl_vector<float> ps;

    //double l1;
    //double l2;
    //double l3;

    //float phi_min;
    //float phi_max;

    int label_id_;

    bool compute_dist();

};

#endif
