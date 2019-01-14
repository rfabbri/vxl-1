// This is brcv/shp/dbsk2d/pro/dbsk2d_ishock_loop_transform.h
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

#include <map>
#include <string>
#include <utility>
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
        dbsk2d_ishock_bpoint* bpoint,
        dbsk2d_ishock_belm* first_link=0);

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
    /* virtual */ void get_belms(std::set<int>& set)
    {
        std::map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
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

    /*virtual */ std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> 
        get_contour_pair(){return contour_pair_;}

    std::vector<vgl_point_2d<double> > get_ordered_contour(){
        return ordered_contour_;}

private: 
    
    // Detect contour 
    void detect_contour(dbsk2d_ishock_belm* first_link=0);

    // Remove contour if already done
    bool remove_contour();

    // Remove interacinting bnd elements shocks
    bool reinsert_contour();

    // Sample points
    void sample_contour(std::vector<vgl_point_2d<double> >& foreground_grid,
                        std::vector<vgl_point_2d<double> >& background_grid);

    // Keep initial start of contour
    dbsk2d_ishock_bpoint* contour_point_;

    // Keep pair of contour elmenents that define start and end of contour
    std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> contour_pair_;

    // Keep track whether transform is valid
    bool valid_transform_;

    // Keep ordered set of contour points
    std::vector<vgl_point_2d<double> > ordered_contour_;

    // Keep track of any degree three nodes to reinitialize contact shocks
    std::map<unsigned int, dbsk2d_ishock_belm*> higher_degree_nodes_;

    // Keep track of contact shock pairs
    std::map<unsigned int,std::pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> >
        contact_shock_pairs_;

    // Make copy ctor private
    dbsk2d_ishock_loop_transform(const dbsk2d_ishock_loop_transform&);

    // Make assign operator private
    dbsk2d_ishock_loop_transform& operator
        =(const dbsk2d_ishock_loop_transform& );
};

#endif //dbsk2d_ishock_loop_transform_h_
