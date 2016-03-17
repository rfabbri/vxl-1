// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap4_transform.h
#ifndef dbsk2d_ishock_gap4_transform_h_
#define dbsk2d_ishock_gap4_transform_h_
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
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_segment_2d.h>

class dbsk2d_ishock_bpoint;

//: Gap Transform Remvol algorithm
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_gap4_transform: public dbsk2d_ishock_transform
{
    
public: 
    //: Constructor
    dbsk2d_ishock_gap4_transform(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>& pair,
        dbsk2d_ishock_bpoint* anchor_pt,
        int euler_spiral_id_=-2);

    //: Destructor
    /* virtual */ ~dbsk2d_ishock_gap4_transform(){};

    //: return likelihood of transform
    /* virtual */ double likelihood();

    //: Execute the transform
    /* virtual */ bool execute_transform();

    //: Get contour pair
    /* virtual */
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> get_contour_pair()
    {return vcl_make_pair(gap_line_pair_.first,anchor_pt_);}
    
    //: Test if valid transform, outside image
    /* virtual */ bool valid_transform();

    //: Get length of gap
    double length_of_gap(){
        vgl_line_segment_2d<double> line(gap_line_pair_.second->s_pt()->pt(),
                                         gap_line_pair_.second->e_pt()->pt());
        return vgl_distance(line,gap_line_pair_.first->pt());}

    //: Grap belms that euler spiral connects
    /* virtual*/ void get_belms(vcl_set<int>& set){
        set.insert(gap_line_pair_.first->id());
        set.insert(anchor_pt_->id());}

private: 
    
    // Add Straight Line Completion
    void add_connecting_line(dbsk2d_ishock_bpoint* bp1,
                             dbsk2d_ishock_bline*  bl1);


    // Stores new gap that it is working on
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> gap_line_pair_;

    // Hold new contour
    vcl_vector<dbsk2d_ishock_belm*> local_belm_list_;

    // Holds two opposing contours that were met
    vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> contour_pair_;

    // Holds contour dbsk2d contour
    dbsk2d_bnd_contour_sptr contour_;

    // Holds other point that is across from pt
    dbsk2d_ishock_bpoint* anchor_pt_;

    // Holds id of connecting line
    int euler_spiral_id_;

    // Hold visibility of original point
    bool anchor_pt_visible_;

    // Make copy ctor private
    dbsk2d_ishock_gap4_transform(const dbsk2d_ishock_gap4_transform&);

    // Make assign operator private
    dbsk2d_ishock_gap4_transform& operator
        =(const dbsk2d_ishock_gap4_transform& );

};

#endif //dbsk2d_ishock_gap4_transform_h_
