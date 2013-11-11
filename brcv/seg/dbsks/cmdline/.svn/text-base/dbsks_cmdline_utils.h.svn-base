// This is brcv/seg/dbsks/bin/dbsks_cmdline_utils.h

#ifndef dbsks_cmdline_utils_
#define dbsks_cmdline_utils_

//:
// \file
// \brief command line utilities for shock-based segmentation
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 9, 2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim




#include <vcl_string.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector.h>

//: Convert all postscript files (typically with extension .ps) in "input_folder"
// to .png format and save to "output_folder"
bool dbsks_batch_ps_to_jpeg(const vcl_string& input_folder, const vcl_string& output_folder, 
                        const vcl_string& ps_file_extension = ".ps");

//: Create a summary XML file from
bool dbsks_create_shapematch_summary_xml(const vcl_string& output_xml,
                                         const vcl_string& input_folder,
                                         const vcl_string& image_list,
                                         const vcl_string& shapematch_extension
                                         );

//: Extract geometry of a fragment in a list of extrinsic shock graphs
bool dbsks_extract_xfrag_geom(const vcl_string& xshock_folder,
                              const vcl_string& xshock_list_file,
                              unsigned xedge_id,
                              const vcl_string& output_file);



//: Extract positive examples of boundary histogram of gradient of extrinsic shock graphs
bool dbsks_extract_positive_xfrag_bhog(const vcl_string& xshock_folder,
                                    const vcl_string& image_folder,
                                    const vcl_string& xshock_list_file,
                                    unsigned xedge_id,
                                    const vcl_string& output_file);


//: Extract negative examples of boundary histogram of gradient of extrinsic shock graphs
bool dbsks_extract_negative_xfrag_bhog(const vcl_string& xshock_folder,
                              const vcl_string& xshock_list_file,
                              unsigned xedge_id,
                              const vcl_string& image_folder,
                              const vcl_string& image_list_file,
                              const vcl_string& output_file);




//: Use a shape fragment's BHOG model and apply it to an image to detect the positive fragments
bool dbsks_detect_xfrag_using_bhog_model(const vcl_string& libsvm_xfrag_bhog_model_file,
                                         const vcl_string& xfrag_geom_file,
                                         const vcl_string& image_folder,
                                         const vcl_string& image_list_file,
                                         const vcl_string& output_file);


//// -----------------------------------------------------------------------------
////: Extract gray OCM cost
//bool dbsks_extract_xgraph_gray_ocm_cost(const vcl_string& image_list_file,
//                                   const vcl_string& xshock_folder,
//                                   const vcl_string& xshock_extension,
//                                   const vcl_string& image_folder,
//                                   const vcl_string& image_extension,
//                                   const vcl_string& edgemap_folder,
//                                   const vcl_string& edgemap_extension,
//                                   int root_vid, 
//                                   int major_child_eid,
//                                   float tol_near_zero,
//                                   const vcl_string& output_file);


//-----------------------------------------------------------------------------



//: Compute positive CCM cost to use as training data
// Assumption:
// xgraph name format: swans_aal.xgraph.0.xml (positive swan with index 0)
// image name format: swans_aal.jpg
bool dbsks_compute_pos_xgraph_ccm_cost(const vcl_string& xgraph_list_file,
                                   const vcl_string& xshock_folder,
                                   const vcl_string& image_folder,
                                   const vcl_string& image_extension,
                                   const vcl_string& edgemap_folder,
                                   const vcl_string& edgemap_extension,
                                   const vcl_string& edgeorient_folder,
                                   const vcl_string& edgeorient_extension,
                                   const vcl_string& cemv_folder,
                                   const vcl_string& cemv_extension,
                                   int root_vid, 
                                   int major_child_eid,
                                   const vcl_string& output_file);



//: Extend the front and rear legs of giraffe models
bool dbsks_extend_giraffe_legs();

//: Re-compute xgraph bbox and rewrite xml detection files
// The old detection code scaled the xgraph down but didn't recompute the bbox
// This yield wrong evaluation result. This script is to fix that
bool dbsks_recompute_detection_bbox();


//------------------------------------------------------------------------------
// Supporting functions
//------------------------------------------------------------------------------

//: Save a list of features to a file
bool dbsks_write_to_file(const vcl_string& filename, const vcl_vector<vnl_vector<double > >& data);

//: Save a list of numbers to a file
bool dbsks_write_to_file(const vcl_string& filename, const vcl_vector<double >& data);

#endif  //dbsks_cmdline_utils_
