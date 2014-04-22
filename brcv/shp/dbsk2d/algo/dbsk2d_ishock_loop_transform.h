// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_loop_transform.h
#ifndef dbsk2d_ishock_loop_transform_h_
#define dbsk2d_ishock_loop_transform_h_
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
#include <vcl_utility.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>

class dbsk2d_lagrangian_ishock_detector;
class dbsk2d_ishock_belm;
class dbsk2d_ishock_edge;
class dbsk2d_ishock_node;
class dbsk2d_ishock_bline;

//: Loop Transform Remvol algorithm
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_loop_transform: public dbsk2d_ishock_transform
{
    
public: 
    //: Constructor
    dbsk2d_ishock_loop_transform(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
        dbsk2d_ishock_bpoint* bpoint);

    //: Destructor
    /* virtual */ ~dbsk2d_ishock_loop_transform(){
        contour_point_=0;
        contour_pair_.first=0;
        contour_pair_.second=0;
        higher_degree_nodes_.clear();
        contact_shock_pairs_.clear();};
  
    //: Call execute transform
    /*virtual*/ bool execute_transform();

    //: Get belms
    /* virtual */ void get_belms(vcl_set<int>& set)
    {
        vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
        for ( it = removal_bnd_elements_.begin(); 
              it != removal_bnd_elements_.end() ; ++it)
        {
            set.insert((*it).first);
        }
    }

    //: Get whether transform is valid
    /*virtual */ bool valid_transform(){return valid_transform_;}

    //: return likelihood of transform
    /* virtual */ double likelihood();

    /*virtual */ vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> 
        get_contour_pair(){return contour_pair_;}

    vcl_vector<dbsk2d_ishock_bpoint*> get_ordered_contour(){
        return ordered_contour_;}

private: 
    
    // Detect contour 
    void detect_contour();

    // Remove contour if already done
    bool remove_contour();

    // Remove interacinting bnd elements shocks
    bool reinsert_contour();

    // Keep initial start of contour
    dbsk2d_ishock_bpoint* contour_point_;

    // Keep pair of contour elmenents that define start and end of contour
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> contour_pair_;

    // Keep track whether transform is valid
    bool valid_transform_;

    // Keep ordered set of contour points
    vcl_vector<dbsk2d_ishock_bpoint*> ordered_contour_;

    // Keep track of any degree three nodes to reinitialize contact shocks
    vcl_map<unsigned int, dbsk2d_ishock_belm*> higher_degree_nodes_;

    // Keep track of contact shock pairs
    vcl_map<unsigned int,vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> >
        contact_shock_pairs_;

    // Make copy ctor private
    dbsk2d_ishock_loop_transform(const dbsk2d_ishock_loop_transform&);

    // Make assign operator private
    dbsk2d_ishock_loop_transform& operator
        =(const dbsk2d_ishock_loop_transform& );
};

#endif //dbsk2d_ishock_loop_transform_h_
