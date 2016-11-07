// This is shp/dbsksp/dbsksp_bnd_arc.h
#ifndef dbsksp_bnd_arc_h_
#define dbsksp_bnd_arc_h_

//:
// \file
// \brief A class for boundary edge - arc type
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 2, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <dbsksp/dbsksp_bnd_edge.h>
#include <dbgl/algo/dbgl_circ_arc.h>

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_bnd_arc : public dbsksp_bnd_edge
{
public:
  // =============== CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ===============
  //: constructor
  dbsksp_bnd_arc(){};
  dbsksp_bnd_arc(const dbgl_circ_arc& arc):arc_(arc){};

  //: destructor
  virtual ~dbsksp_bnd_arc(){};

  
  // ===============  DATA ACCESS =====================================

  //: Return and set the circular arc
  dbgl_circ_arc arc() const {return this->arc_; }
  void set_arc(const dbgl_circ_arc& arc)
  { this->arc_ = arc; }
  


  
  // ============= UTILITY FUNCTIONS ===============================


  // =============== MISCELLANEOUS =====================================

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

  // ============= MEMBER VARIABLES =====================================
protected:
  dbgl_circ_arc arc_;
  
};
#endif // shp/dbsksp/dbsksp_bnd_arc.h


