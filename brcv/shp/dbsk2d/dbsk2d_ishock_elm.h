// This is brcv/shp/dbsk2d/dbsk2d_ishock_elm.h
#ifndef dbsk2d_ishock_elm_h_
#define dbsk2d_ishock_elm_h_
//:
// \file
// \brief Base class for Intrinsic shock elements
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 03/14/2005    Added starting and ending bnd param data members
//                               to explicitly define the link to the bnd element.
//
//   Amir Tamrakar 01/12/2006    Cleaned it up to almost to its bare essentials
//                               Needs a little more housecleaning still
// \endverbatim

#include <vcl_list.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>
#include <dbsk2d/dbsk2d_utils.h>

//forward declaration
class dbsk2d_ishock_elm;

//some useful type definitions
typedef vcl_list<dbsk2d_ishock_elm* >     ishock_elm_list;
typedef ishock_elm_list::iterator         ishock_elm_list_iter;
typedef ishock_elm_list::reverse_iterator ishock_elm_list_riter;

//: Base class for Intrinsic shock elements
class dbsk2d_ishock_elm : public dbsk2d_base_gui_geometry
{
public:

  //: type of intrinsic model
  enum SHOCK_TYPE
  {
    SNODE, POINTPOINT, POINTLINE, POINTARC, LINELINE, LINELINE_THIRDORDER,
    LINEARC, ARCARC, CONTACTSHOCK, POINTARC_THIRDORDER, ARCARC_THIRDORDER
  };
  
protected:

  int  _id;           ///< unique id
  SHOCK_TYPE  _type;  ///< POINTPOINT, POINTLINE, etc.

  //Simulation parameters
  double _startTime;  ///< formation time (distance)
  double _simTime;    ///< for ordering the sequence of propagation
  double _endTime;    ///< end time (distance)

  //Flags
  bool  _bActive;     ///< bool, =1:active, =0:dead
  bool  _bPropagated; ///< different from _bActive because
                      //a contact shock can still be active after it propagates (?)
  bool  _bValid;      ///< this flag is set to false after dynamic validation in the 
                      //constructors of the shocks
  bool  _bHidden;     ///< I need it for simple pruning operations
                      //and for contour grouping 

  //: origin for the intrinsic coordinate system
                                // For dbsk2d_ishock_node: _origin is the extrinsic coordinate of the node
// For dbsk2d_ishock_edge: _origin is origin of the intrinsic coord
  vgl_point_2d<double> _origin; 

public:

  //: constructor
  dbsk2d_ishock_elm (int newid, double stime): dbsk2d_base_gui_geometry(), 
    _id(newid), _startTime(stime), _simTime(stime), _endTime(ISHOCK_DIST_HUGE),  
    _bActive(true), _bPropagated(false), _bValid(true), _bHidden(false) {}

  //: destructor
  virtual ~dbsk2d_ishock_elm() {}

  //-----------------------------------------------------------------------------
  // Access Member variables
  //-----------------------------------------------------------------------------

  int  id() { return _id; }
  SHOCK_TYPE type() { return _type; }

  double startTime() { return _startTime; }
  double simTime() { return _simTime; }
  double endTime() { return _endTime; }

  bool isActive() { return _bActive; }
  bool hasPropagated() { return _bPropagated; }
  bool isValid(){ return _bValid; }
  bool isHidden(){ return _bHidden; }
  bool isNotHidden(){ return !_bHidden; }
  
  vgl_point_2d<double>  origin() { return _origin; }

  //-----------------------------------------------------------------------------
  // Set member variables
  //-----------------------------------------------------------------------------

  void  setId (int id) { _id = id; }
  
  virtual void setSimTime (double stime){ _simTime = stime; }
  void setEndTime(double time) {_endTime = time;}

  void  setActive(bool bactive) { _bActive = bactive; }
  void  setPropagated(bool bprop) { _bPropagated = bprop; }
  void  setbValid(bool bVal) { _bValid = bVal;}
  void  hide(){_bHidden = true;}
  void  unhide(){_bHidden = false;}
  
  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  virtual bool is_a_node()=0; ///< is this a node?
  virtual bool is_a_link()=0; ///< is this a link?
  
};

#endif // dbsk2d_ishock_elm_h_
