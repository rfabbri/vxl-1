// This is shp/dbsksp/dbsksp_bnd_edge.h
#ifndef dbsksp_bnd_edge_h_
#define dbsksp_bnd_edge_h_

//:
// \file
// \brief A class for boundary edge
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 2, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <dbsksp/dbsksp_shock_fragment_sptr.h>

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_bnd_edge : public vbl_ref_count 
{
public:
  // =============== CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ===============
  //: constructor
  dbsksp_bnd_edge(){};

  //: destructor
  virtual ~dbsksp_bnd_edge(){};

  
  // ===============  DATA ACCESS =====================================
  
  unsigned int id() const {return this->id_; }
  void set_id(unsigned int id){ this->id_ = id; }
  
  //: Return and set shock edge
  dbsksp_shock_fragment_sptr fragment() const { return this->fragment_; }
  void set_fragment(const dbsksp_shock_fragment_sptr& fragment)
  {this->fragment_= fragment; }


  
  // ============= UTILITY FUNCTIONS ===============================


  // =============== MISCELLANEOUS =====================================

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

  // ============= MEMBER VARIABLES =====================================
protected:
  unsigned int id_;
  dbsksp_shock_fragment_sptr fragment_;

  
};
#endif // shp/dbsksp/dbsksp_bnd_edge.h


