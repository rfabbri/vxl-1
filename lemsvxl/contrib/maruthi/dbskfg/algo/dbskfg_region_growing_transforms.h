// This is brcv/shp/dbskfg/algo/dbskfg_region_growing_transforms.h
#ifndef dbskfg_region_growing_transforms_h_
#define dbskfg_region_growing_transforms_h_
//:
// \file
// \brief Algorithm to detect and apply region growing transforms
// \author Maruthi Narayanan
// \date 08/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 08/08/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>

#include <vcl_map.h>
#include <vcl_vector.h>

class dbskfg_shock_node;

//: Form Composite Graph algorithm
class dbskfg_region_growing_transforms
{

public:

    //: Constructor
    dbskfg_region_growing_transforms( dbskfg_rag_graph_sptr rag_graph);
          
    //: Destructor
    ~dbskfg_region_growing_transforms();
  
    // Grow regions
    void grow_regions(unsigned int id=0);

    // Grow regions
    void grow_regions(vcl_vector<unsigned int>& regions_to_grow);

    // Grow one region
    dbskfg_rag_node_sptr grow_region(unsigned int id);

    // set adjacency 
    void set_adjacency(bool flag){adjacency_flag_=flag;}

private:

    // Decide what type of adjacency to consider
    bool adjacency_flag_;

    // Expand rag_node
    void expand_rag_node(dbskfg_rag_node_sptr rag_node,
                         vcl_map<unsigned int,dbskfg_rag_node_sptr*>& 
                         visited_nodes);

    // Expand wavefront
    void expand_wavefront(dbskfg_shock_node* node, 
                          dbskfg_rag_node_sptr rag_node,
                          vcl_vector<dbskfg_shock_node*>& stack,
                          vcl_map<unsigned int,dbskfg_rag_node_sptr*>& 
                          visited_nodes);

    // Keep a region adjancency graph
    dbskfg_rag_graph_sptr rag_graph_;

    // Make copy ctor private
    dbskfg_region_growing_transforms(const dbskfg_region_growing_transforms&);

    // Make assign operator private
    dbskfg_region_growing_transforms& operator
        =(const dbskfg_region_growing_transforms& );

   
};

#endif //dbskfg_region_growing_transforms_h_
