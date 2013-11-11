// This is brcv/shp/dbsk2d/algo/dbskr_compute_scurve.h
#ifndef dbskr_compute_scurve_h_
#define dbskr_compute_scurve_h_
//:
//: \file
//: \brief Contains code to compute shock curves from shock graph paths
//: \author Amir Tamrakar
//: \date 09/22/05
// 
//: \verbatim
//  Modifications
//      Amir Tamrakar 06/10/06     using phi sampling for scurve representation 
//                                 instead of alpha, thereby getting rid of shock_dir
//
//: \endverbatim

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>      
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>  

#include "dbskr_scurve_sptr.h"

//: \relates dbsk2d_shock_graph
dbskr_scurve_sptr dbskr_compute_scurve(dbsk2d_shock_node_sptr start_node,
                                       vcl_vector<dbsk2d_shock_edge_sptr> path,
                                       bool leaf_edge=false, 
                                       bool binterpolate=true, bool bsub_sample=true, 
                                       double interpolate_ds=1.0, double subsample_ds=5.0);

/*dbskr_scurve_sptr dbskr_compute_scurve_no_A_inf_samples(dbsk2d_shock_node_sptr start_node,
                                       vcl_vector<dbsk2d_shock_edge_sptr> path,
                                       bool binterpolate=true, bool bsub_sample=true,
                                       double interpolate_ds=1.0, double subsample_ds=5.0);
                                       */

#endif //dbskr_compute_scurve_h_
