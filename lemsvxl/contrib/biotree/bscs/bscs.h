#ifndef bscs_h_
#define bscs_h_
//:
// \file
// \brief Biotree standard coordinate system, based off of 3D Cartesian coordinate system
//
// \verbatim
// \endverbatim

#include <vcsl/vcsl_spatial.h>
#include "bscs_sptr.h"

//: Biotree Standard Coordinate system
class bscs : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  bscs();

  // Destructor
  virtual ~bscs() {}

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const bscs *cast_to_bscs() const { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are the axes of `this' right handed ?
  bool is_right_handed() const { return right_handed_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set whether the coordinate system is right handed or not
  void set_right_handed(bool val) { right_handed_ = val; }

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //:  True if the axes of `this' are right handed
  bool right_handed_;
};

#endif // bscs_h_
