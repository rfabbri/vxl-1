// This is brcv/shp/dbskfg/algo/dbskfg_detect_transforms.h
#ifndef dbskfg_detect_transforms_h_
#define dbskfg_detect_transforms_h_
//:
// \file
// \brief Algorithm to detect transforms 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/algo/dbskfg_gap_transforms.h>
#include <dbskfg/algo/dbskfg_loop_transforms.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/algo/dbskfg_region_growing_transforms.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource_sptr.h>

//: Form Composite Graph algorithm
class dbskfg_detect_transforms: public vbl_ref_count
{

public:

    //: Constructor
    dbskfg_detect_transforms
        ( dbskfg_composite_graph_sptr composite_graph,
          vil_image_resource_sptr image  );

    //: Destructor
    ~dbskfg_detect_transforms();
  
    vcl_vector<dbskfg_transform_descriptor_sptr> objects()
    {return transforms_;}

    void destroy_transforms(){transforms_.clear();
        transform_neighbors_.clear();}

    void detect_transforms(bool detect_gaps,
                           bool detect_loops,
                           double ess_completion,
                           double alpha);

    void detect_transforms_simple(bool detect_gaps,
                                  bool detect_loops,
                                  double ess_completion,
                                  double alpha);

    unsigned int apply_transforms( double threshold,
                                   dbskfg_rag_graph_sptr rag_graph);

    void apply_transforms( unsigned int transform_id,
                           dbskfg_rag_graph_sptr rag_graph);

    // Returns the number of transforms below threshold
    unsigned int transforms_valid(double threshold);

    void transform_affects_region(dbskfg_rag_node_sptr& rag_node,
                                  vcl_set<unsigned int>& rag_con_ids);

    void transform_affects_region(dbskfg_rag_node_sptr& rag_node,
                                  double ess,
                                  double alpha=0.4);

    // Set rag node to expand around 
    void set_rag_node(dbskfg_rag_node_sptr rag_node ){rag_node_ = rag_node;}

private:


    // Hold composite graph
    dbskfg_composite_graph_sptr composite_graph_;

    //: store image
    // Holds image for cost
    vil_image_resource_sptr image_; 

    // Holds a map storing all conflicting transforms
    vcl_map<unsigned int,vcl_vector<unsigned int> > transform_neighbors_;

    // Keep track of all transform objects
    vcl_vector<dbskfg_transform_descriptor_sptr> transforms_;

    // Keep a region id to expand around
    dbskfg_rag_node_sptr rag_node_;

    // Holds a gap detector object
    dbskfg_gap_transforms gap_detector_;
    
    // Holds a loop detector object
    dbskfg_loop_transforms loop_detector_;

    // Tells whether this transform affects the region
    bool transform_affects_region(dbskfg_transform_descriptor_sptr& 
     transform);

    // Make copy ctor private
    dbskfg_detect_transforms(const dbskfg_detect_transforms&);

    // Make assign operator private
    dbskfg_detect_transforms& operator
        =(const dbskfg_detect_transforms& );

   
};

#endif //dbsk2d_ishock_prune_h_
