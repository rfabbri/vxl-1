// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap_transform.h
#ifndef dbsk2d_ishock_gap_transform_h_
#define dbsk2d_ishock_gap_transform_h_
//:
// \file
// \brief Algorith to remove boundary elements and compute local shock
// \author Maruthi Narayanan
// \date 07/15/11
// 
// This file contains the function to remove a boundary element and recompute
// the shock locally

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/15/12    Initial version.
//
// \endverbatim 

#include <vcl_map.h>
#include <vcl_string.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>

class dbsk2d_ishock_bpoint;

//: Gap Transform Remvol algorithm
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_gap_transform: public dbsk2d_ishock_transform
{
    
public: 
    //: Constructor
    dbsk2d_ishock_gap_transform(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>& pair,
        int euler_spiral_id=-2);

    //: Destructor
    /* virtual */ ~dbsk2d_ishock_gap_transform(){
        init_samples_.clear();
        local_belm_list_.clear();};
                          
    //: Execute the transform
    /*virtual */ bool execute_transform();

    //: Grap belms that euler spiral connects
    /* virtual*/ void get_belms(vcl_set<int>& set){
        set.insert(gap_endpoint_.first->id());
        set.insert(gap_endpoint_.second->id());}

    //: return whether valid transform
    /*virtual*/ bool valid_transform(){return true;}

    //: return likelihood of transform
    /* virtual */ double likelihood(){return likelihood_;}

    /*virtual */ vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> 
        get_contour_pair(){return gap_endpoint_;}

    vcl_vector<vgl_point_2d<double> >& get_es_samples()
    {return init_samples_;}

private: 

    // Add Euler Spiral 
    void add_euler_spiral(dbsk2d_ishock_bpoint* bp1,
                          dbsk2d_ishock_bpoint* bp2);

    // determine likelihood
    void determine_likelihood();

    // Determine tangent pairs
    vcl_pair<double,double> get_tangent_pairs(dbsk2d_ishock_bpoint* bp1,
                                              dbsk2d_ishock_bpoint* bp2);

    // Stores new gap that it is working on
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> gap_endpoint_;

    // Hold new contour
    vcl_vector<dbsk2d_ishock_belm*> local_belm_list_;

    // Holds euler spiral samples
    vcl_vector<vgl_point_2d<double> > init_samples_;

    // Holds likelihood
    double likelihood_;

    // Holds a gap id
    int euler_spiral_id_;
    
    // Make copy ctor private
    dbsk2d_ishock_gap_transform(const dbsk2d_ishock_gap_transform&);

    // Make assign operator private
    dbsk2d_ishock_gap_transform& operator
        =(const dbsk2d_ishock_gap_transform& );
};

#endif //dbsk2d_ishock_gap_transform_h_
