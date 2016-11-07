// This is brcv/shp/dbsk2d/algo/dbsk2d_compute_shocks.h
#ifndef dbsk2d_compute_shocks_h_
#define dbsk2d_compute_shocks_h_
//:
// \file
// \brief Shock computation functions
// \author Amir Tamrakar
// \date 11/29/05
// 
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_vector.h>

#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

//------------------------------------------------------------------
// Shock computation code compiled into one function for ease of use
//------------------------------------------------------------------

//: Create a boundary class from a list of vsol elements
//  \relates dbsk2d_boundary
//  \relates dbsk2d_bnd_preprocess
dbsk2d_boundary_sptr 
dbsk2d_create_boundary( vcl_vector< vsol_spatial_object_2d_sptr > vsol_list,
                        bool override_default_partitioning = true,
                        float xmin=0.0, float ymin=0.0,
                        int num_rows=1, int num_cols=1, 
                        float cell_width=1000.0, float cell_height=1000.0,
                        bool preprocess_boundary=false,
                        bool break_long_lines=false);

//: Compute intrinsic shocks from a boundary 
//  \relates dbsk2d_ishock_detector 
dbsk2d_ishock_graph_sptr 
dbsk2d_compute_ishocks (dbsk2d_boundary_sptr boundary);

//: \relates dbsk2d_ishock_detector 
//  \relates dbsk2d_prune_ishock
dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (dbsk2d_boundary_sptr bnd, 
                       float prune_threshold=1.0,
                       bool prune_shocks=true);

//: \relates dbsk2d_ishock_detector 
//  \relates dbsk2d_prune_ishock
dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (vcl_vector< vsol_spatial_object_2d_sptr > vsol_list,
                       float prune_threshold=1.0,
                       bool override_default_partitioning = true,
                       float xmin=0.0, float ymin=0.0,
                       int num_rows=1, int num_cols=1, 
                       float cell_width=1000.0, float cell_height=1000.0,
                       bool preprocess_boundary=false,
                       bool break_long_lines=false,
                       bool prune_shocks=true);

//: \relates dbsk2d_ishock_detector 
//  \relates dbsk2d_prune_ishock
dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (vsol_polygon_2d_sptr & polygon,
                       float prune_threshold=1.0);

//: \relates dbsk2d_ishock_detector 
//  \relates dbsk2d_prune_ishock
//  \relates dbsk2d_sample_ishock
dbsk2d_shock_graph_sptr 
dbsk2d_compute_xshocks (vsol_polygon_2d_sptr & polygon,
                       float prune_threshold=1.0, 
                       float sampling_resoluton=1.0);

//: test xshock computation
bool test_xshock_graph(dbsk2d_shock_graph_sptr sg);

#endif //dbsk2d_compute_shocks_h_
