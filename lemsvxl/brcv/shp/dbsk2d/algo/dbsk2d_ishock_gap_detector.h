// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap_detector.h
#ifndef dbsk2d_ishock_gap_detector_h_
#define dbsk2d_ishock_gap_detector_h_
//:
// \file
// \brief Algorith to detect all different types of gaps
// \author Maruthi Narayanan
// \date 07/15/11
// 
// This file is responsible for detecting all different types of gaps
//

// \verbatim
//  Modifications
//   Maruthi Narayanan 09/08/12    Initial version.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <vector>
#include <utility>
#include <map>
#include <string>
#include <set>

class dbsk2d_ishock_belm;
class dbsk2d_ishock_bpoint;
class dbsk2d_ishock_bline;
class dbsk2d_ishock_node;

//: Responsible for detecting gaps
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_gap_detector
{
    
public: 
    //: Constructor
    dbsk2d_ishock_gap_detector(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph);

    //: Destructor
    ~dbsk2d_ishock_gap_detector(){};

    //: Detect for whole shock graph
    void detect_gap1(
        std::vector<std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& 
        gap_pairs);
    
    //: Detect for one endpoint
    void detect_gap1(
        dbsk2d_ishock_belm* belm,
        std::vector<std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& 
        gap_pairs);

    //: Detect all gaps for a bunch of regions
    void detect_gap1(std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >& 
                     region_outer_nodes,
                     std::vector<
                     std::pair< dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >&
                     gap_pairs);

    //: Detect all gaps for a bunch of regions
    void detect_all_gaps(
        std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >& 
        region_outer_nodes,
        std::vector<
        std::pair< dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >&
        gap_pairs,
        std::vector<
        std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >&
        gap4_pairs);

    //: Detect for whole shock graph
    void detect_gap4(
        std::vector<std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >& 
        gap_pairs);
    
    //: Detect for endpoint
    void detect_gap4(dbsk2d_ishock_belm* belm,
                     std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>& 
                     gap4_pair);

    //: Detect for endpoint
    void detect_gap4(dbsk2d_ishock_belm* belm,
                     std::vector<
                     std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >& 
                     gap4_pair,
                     std::set<int>& contour_id);
    

private: 
    
    // Private intrinsinc shock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Private boundary class
    dbsk2d_boundary_sptr boundary_;

    //: Determine whether a point has a matching endpoint
    void gap_endpoint(
        dbsk2d_ishock_bpoint* bp, 
        std::map< std::pair<int,int>,
                 std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& 
        gaps_visited,
        bool& flag,
        std::set<int>& contour_ids);

    // Make copy ctor private
    dbsk2d_ishock_gap_detector(const dbsk2d_ishock_gap_detector&);

    // Make assign operator private
    dbsk2d_ishock_gap_detector& operator
        =(const dbsk2d_ishock_gap_detector& );
};

#endif //dbsk2d_ishock_gap_detector_h_

