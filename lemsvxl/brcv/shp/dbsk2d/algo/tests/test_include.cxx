
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbsk2d/algo/dbsk2d_compute_shocks.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>

//# *********************************************************
//# Various Algorithms
//# *********************************************************
//
//# *********************************************************           
//# Shock computation algorithms
//# *********************************************************


// *********************************************************
// Standard lagrangian shock detector
// *********************************************************

#include <dbsk2d/algo/dbsk2d_ishock_intersection_data.h>
#include <dbsk2d/algo/dbsk2d_ishock_compute_intersection.h>  
#include <dbsk2d/algo/dbsk2d_ishock_detector.h>               
                                       

// *********************************************************
// Shock detection using bucketing
// *********************************************************

// #dbsk2d_ishock_detector_bkt.h          

// *********************************************************
// Pruning algorithms
// *********************************************************

#include <dbsk2d/algo/dbsk2d_prune_ishock.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

// *********************************************************
// Sample intrinsic shock
// *********************************************************

//#dbsk2d_convert_ishock_to_xshock.h  

#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

// *********************************************************
// Symmetry transform Algorithms
// *********************************************************

#include <dbsk2d/algo/dbsk2d_shock_transforms.h>

// *********************************************************
// SCDM
// *********************************************************

//#dbsk2d_scdm.h                     

// *********************************************************
// CEDT
// *********************************************************

//#dbsk2d_cedt.h                     

// *********************************************************
// curve algorithms
// *********************************************************
//
//#dbsk2d_ishock_curve_utils.h       
//


int main() { return 0; }
