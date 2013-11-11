// This is shp/dbsksp/dbsks_xnode_geom.h
#ifndef dbsks_xnode_geom_h_
#define dbsks_xnode_geom_h_

//:
// \file
// \brief Class holding geometric info at an xnode
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Nov 11, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>

// =============================================================================
// dbsks_xnode_geom
// =============================================================================


//: A generative model for generating xshock node
class dbsks_xnode_geom: public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------
  
  //: constructor
  dbsks_xnode_geom(){};

  //: destructor
  virtual ~dbsks_xnode_geom(){};
  
  // DATA ACCESS ---------------------------------------------------------------

  // UTILITY -------------------------------------------------------------------

  
  // I/O =======================================================================

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

  // Member variables
public:
  double x_;
  double y_;
  double psi_;
  double phi_; // phi angle of the opposite of "parent" branch
  double radius_;
  double phi_diff_; //  = phi(child1) - phi(child2), only available in degree-3 node
};

#endif // shp/dbsksp/dbsks_xnode_geom.h


