// This is brcv/shp/dbsk2d/dbsk2d_ishock_node.h
#ifndef dbsk2d_ishock_node_h_
#define dbsk2d_ishock_node_h_
//:
// \file
// \brief Intrinsic shock node class 
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 07/05/2005    Made this into a standard shock node class
//                               and removed all subclasses of this node
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_utils.h>
#include <dbsk2d/dbsk2d_ishock_elm.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>

// Useful type definitions
typedef vcl_list<vcl_pair<double, dbsk2d_ishock_belm*> > ishock_node_belm_list;
typedef ishock_node_belm_list::iterator ishock_node_belm_list_iter;

//: Intrinsic shock node class
class dbsk2d_ishock_node : public dbsk2d_ishock_elm
{
protected:
  
  ishock_node_belm_list _bndList;  ///< list of boundary elements forming this node and 
                                   //corresponding boundary eta
  ishock_edge_list _pShockList;    ///< list of parent shocks (incoming edges)
  dbsk2d_ishock_edge* _cShock;     ///< child shock link (outgoing edge)
  dbsk2d_ishock_edge* _cShock2;    ///< second child shock link (only for sources) 
  
public:
 
  //: Constructor
  dbsk2d_ishock_node (int newid, double stime, vgl_point_2d<double> Or);

  //: Destructor
  virtual ~dbsk2d_ishock_node ();

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  //: return the list of boundary elements forming this node
  ishock_node_belm_list& bndList(){ return _bndList; }
 
  //: return the list of parent shock edges
  ishock_edge_list& pShocks(){ return _pShockList; }

  //: return child shock edge 
  dbsk2d_ishock_edge* cShock() { return _cShock; }

  //: return second child shock edge (if source)
  dbsk2d_ishock_edge* cShock2() { return _cShock2; }

  //-----------------------------------------------------------------------------
  // Set member variables
  //-----------------------------------------------------------------------------
  
  void add_pShock(dbsk2d_ishock_edge* pshock, bool in_front=false);
  void remove_pShock(dbsk2d_ishock_edge* pshock);
  void set_cShock(dbsk2d_ishock_edge* cshock) { _cShock = cshock; }
  void clear_cShock () { _cShock = NULL; }
  void set_cShock2(dbsk2d_ishock_edge* cshock) { _cShock2 = cshock; }
  void clear_cShock2 () { _cShock2 = NULL; }
  
  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  virtual bool is_a_node() { return true; }
  virtual bool is_a_link() { return false; }

  bool is_an_A3source() { return indeg()==0 && outdeg()==1; }
  bool is_a_source() { return outdeg()==2; }
  bool is_a_junct() { return indeg()>1 && outdeg()==1; }
  bool is_a_sink() { return indeg()>=1 && outdeg()==0; }
  
  //: number of boundary elements forming this node
  int nBElement() { return _bndList.size(); }

  //-----------------------------------------------------------------------------
  // Topology related functions
  //-----------------------------------------------------------------------------

  //: return the number of edges starting at this node
  int outdeg(bool exclude_hidden=false);
  //: return the number of edges ending at this node
  int indeg(bool exclude_hidden=false); 
  //: return the number of edges adjacent to this node
  int degree(bool exclude_hidden=false);

  //: return the edges connected to this node;
  ishock_edge_list adj_edges(bool exclude_hidden = false);
  
  //: return the unpruned parent of this node
  dbsk2d_ishock_edge* get_parent_edge (void);
  
  //-----------------------------------------------------------------------------
  // Dynamics of this shock
  //-----------------------------------------------------------------------------
  
  //don't know how to define this properly since at a node, there are multiple
  //dynamics from all the edges adjacent to it.

  //-----------------------------------------------------------------------------
  // Extrinsic functions
  //-----------------------------------------------------------------------------
  virtual void compute_extrinsic_locus();

  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_node_h_
