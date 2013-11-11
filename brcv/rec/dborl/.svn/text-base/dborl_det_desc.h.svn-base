// This is rec/dborl/dborl_det_desc.h
#ifndef dborl_det_desc_h_
#define dborl_det_desc_h_

//:
// \file
// \brief An abstract class to describe on object detection solution
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 4, 2008
//
// \verbatim
//  Modifications
// \endverbatim



#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <dborl/dborl_det_desc_sptr.h>
#include <vcl_string.h>

// ============================================================================
// dborl_det_desc
// ============================================================================

//: A abstract class to describe an object detection
class dborl_det_desc: public vbl_ref_count
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dborl_det_desc(){};

  //: destructor
  virtual ~dborl_det_desc(){};

  virtual vcl_string is_a() const { return "dborl_det_desc"; }

  //: Return bounding box of the detection
  virtual vsol_box_2d_sptr bbox() = 0;

  //: Return the confidence level of the detection
  virtual double confidence() const = 0;

protected:
};


//: Return true if two detection are in decreasing confidence level
bool dborl_decreasing_confidence(const dborl_det_desc_sptr& a, const dborl_det_desc_sptr& b);

//: Return true if two detection are in increasing confidence level
bool dborl_increasing_confidence(const dborl_det_desc_sptr& a, const dborl_det_desc_sptr& b);


#endif // seg/dbsks/dborl_det_desc.h


