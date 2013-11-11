// This is mw_discrete_corresp.h
#ifndef mw_discrete_corresp_h
#define mw_discrete_corresp_h
//:
//\file
//\brief Data structure representing correspondence between two point sets
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 04/25/2005 06:41:03 PM EDT
//

#include <vsol/vsol_point_2d.h>
#include <vcl_vector.h>
#include <vcl_list.h>

//: this is to be used within the mw_discrete_corresp class; the cost can be
//interpreted as the cost of the ; 
//\todo make it nested into mw_discrete_corresp
struct mw_attributed_point {
public:
   unsigned long pt_; //:< points are represented by some arbitrary numbering
   double cost_;
   // add more stuff here
};



//: We support many-to-one, one-to-many, and even zero-to-many and many-to-zero
// Access is directly on datastructure, since its kinda low-level
//
//The point sets are indexed in the same way as in the initialization point
// vectors
struct mw_discrete_corresp {
public:
   mw_discrete_corresp(unsigned long npts1, unsigned long npts2) 
      :
      corresp_(npts1+1)
   { }
   ~mw_discrete_corresp() {};

   //: \todo functional access

   friend vcl_ostream&  operator<<(vcl_ostream& s, const mw_discrete_corresp &c);
public:
   vcl_vector < vcl_list< mw_attributed_point > > corresp_;
};


#endif // mw_discrete_corresp_h
