// This is brcv/shp/dbskfg/algo/dbskfg_extract_fragments.h
#ifndef dbskfg_extract_fragments_h_
#define dbskfg_extract_fragments_h_
//:
// \file
// \brief Algorithm to extract fragments out of an image 
// \author Maruthi Narayanan
// \date 10/18/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 10/18/2010    Initial version.
//
// \endverbatim 

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_string.h>
#include <vcl_map.h>

//: Form Composite Graph algorithm
class dbskfg_extract_fragments
{

public:

    //: Constructor
    dbskfg_extract_fragments(vidpro1_vsol2D_storage_sptr input_vsol,
                             vidpro1_image_storage_sptr image_storage);

    //: Destructor
    ~dbskfg_extract_fragments();
  
    //: Extract the fragments
    void extract(bool detect_gaps,
                 bool detect_loops,
                 double ess_completion,
                 double alpha,
                 double transform_thresh,
                 double contour_ratio,
                 double prune_thres,
                 vcl_string output_folder,
                 vcl_string output_prefix);

    //: Extract the fragments
    void extract_two(bool detect_gaps,
                     bool detect_loops,
                     double ess_completion,
                     double alpha,
                     double transform_thresh,
                     double contour_ratio,
                     double prune_thres,
                     vcl_string output_folder,
                     vcl_string output_prefix);


    //: Extract the fragments
    void extract_three(bool detect_gaps,
                       bool detect_loops,
                       double ess_completion,
                       double alpha,
                       double transform_thresh,
                       double contour_ratio,
                       double prune_thres,
                       vcl_string output_folder,
                       vcl_string output_prefix);

private:


    // Holds original composite graph
    dbskfg_composite_graph_sptr composite_graph_;
    
    // Holds original rag graph
    dbskfg_rag_graph_sptr rag_graph_;

    //: store image
    vil_image_resource_sptr image_; 

    //: Keep a map off all rag nodes to start growing from
    vcl_map<double,unsigned int> start_rag_nodes_;

    // Compute first layer of graph
    void compute_first_graph(vidpro1_vsol2D_storage_sptr& input_vsol,
                             vidpro1_image_storage_sptr& image_storage);

    // Keep a helper function 
    void extract_helper(dbskfg_rag_node_sptr,
                        bool detect_gaps,
                        bool detect_loops,
                        double ess_completion,
                        double alpha,
                        double transform_thresh,
                        double contour_ratio,
                        double prune_thres,
                        vcl_string output_folder,
                        vcl_string output_prefix);

    // Determine start regions
    void determine_start_regions(double contour_ratio);

    // Make copy ctor private
    dbskfg_extract_fragments(const dbskfg_extract_fragments&);

    // Make assign operator private
    dbskfg_extract_fragments& operator
        =(const dbskfg_extract_fragments& );

    // Keep track of status
    bool status_;
};

#endif //dbsk2d_ishock_prune_h_
