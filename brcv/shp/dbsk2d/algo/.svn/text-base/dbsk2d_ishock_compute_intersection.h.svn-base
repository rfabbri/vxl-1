// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_compute_intersection.h
#ifndef dbsk2d_ishock_compute_intersection_h_
#define dbsk2d_ishock_compute_intersection_h_
//:
//: \file
//: \brief Set of intrinsic intersection computation functions
//: \author Amir Tamrakar
//: \date 02/09/05
// 
//: \verbatim
//  Modifications
//   Amir Tamrakar 02/09/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 07/07/2005    Moved to dbsk2d/algo
//
//: \endverbatim

#include <dbsk2d/dbsk2d_utils.h>

#include <dbsk2d/dbsk2d_ishock_belm.h>  
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h> 
#include <dbsk2d/dbsk2d_ishock_barc.h>  
      
#include <dbsk2d/dbsk2d_ishock_node.h>      

#include <dbsk2d/dbsk2d_ishock_contact.h>  
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>         
#include <dbsk2d/dbsk2d_ishock_pointline.h>          
#include <dbsk2d/dbsk2d_ishock_pointarc.h>           
#include <dbsk2d/dbsk2d_ishock_lineline.h>           
#include <dbsk2d/dbsk2d_ishock_linearc.h>            
#include <dbsk2d/dbsk2d_ishock_arcarc.h>             
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_pointarc_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_arcarc_thirdorder.h>  

#include <dbsk2d/algo/dbsk2d_ishock_intersection_data.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd_sptr.h>

//-------------------------------------------------
//Intersections with the lagrangian cell boundaries
//-------------------------------------------------

//: compute the intersection between a shock edge and the given cell boundary
//  \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_edge* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointpoint* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc_thirdorder* sh, dbsk2d_lagrangian_cell_bnd_sptr bnd);

//-------------------------------------------------
//Intersections between different types of shocks
//-------------------------------------------------

//: compute intersections between ishock edges
//  \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_edge* shL, dbsk2d_ishock_edge* shR);

//----------------------------------------------------------------
// Point-Point
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointpoint* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointpoint* shL, dbsk2d_ishock_pointarc* shR);

//----------------------------------------------------------------
// Contact Shocks
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointpoint* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_pointarc_thirdorder* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_lineline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_lineline_thirdorder* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_linearc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_arcarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_contact* shL, dbsk2d_ishock_arcarc_thirdorder* shR);

//----------------------------------------------------------------
// Point-Line
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointpoint* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_pointarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_lineline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointline* shL, dbsk2d_ishock_linearc* shR);

//----------------------------------------------------------------
// Point-Arc
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointpoint* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_pointarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_linearc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc* shL, dbsk2d_ishock_arcarc* shR);

//----------------------------------------------------------------
// Point-Arc-Third Order
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc_thirdorder* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_pointarc_thirdorder* shL, dbsk2d_ishock_pointarc_thirdorder* shR);


//----------------------------------------------------------------
// Line-Line
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* shL, dbsk2d_ishock_lineline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* shL, dbsk2d_ishock_lineline_thirdorder* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline* shL, dbsk2d_ishock_linearc* shR);

//----------------------------------------------------------------
// Line-Line-Third Order
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_lineline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_lineline_thirdorder* shL, dbsk2d_ishock_lineline_thirdorder* shR);

//----------------------------------------------------------------
// Line-Arc
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_pointline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_pointarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_lineline* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_linearc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_linearc* shL, dbsk2d_ishock_arcarc* shR);

//----------------------------------------------------------------
// Arc-Arc
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_pointarc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_linearc* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc* shL, dbsk2d_ishock_arcarc* shR);

//----------------------------------------------------------------
// Arc-Arc-Third Order
//----------------------------------------------------------------

//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc_thirdorder* shL, dbsk2d_ishock_contact* shR);
//: \relates dbsk2d_lagrangian_ishock_detector 
dbsk2d_ishock_intersection_data dbsk2d_ishock_compute_intersection 
  (dbsk2d_ishock_arcarc_thirdorder* shL, dbsk2d_ishock_arcarc_thirdorder* shR);

#endif //dbsk2d_ishock_compute_intersection_h_
