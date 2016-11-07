// This is brcv/shp/dbsk2d/dbsk2d_ishock_belm.h
#ifndef dbsk2d_ishock_belm_h_
#define dbsk2d_ishock_belm_h_
//:
// \file
// \brief Base class for all boundary elements involved in
// intrinsic shock computation
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//   Amir Tamrakar 04/13/05      Added a separate class for ordering the shocks 
//                               on the bnd_ishock_map
//   Amir Tamrakar 05/02/2005    Merged with dbsk2d_ishock_belm since there was no 
//                               need to keep the heirarchy
//   Amir Tamrakar 01/06/2006    Removed a bunch of defunct methods from this class
//
//   Amir Tamrakr 10/30/06       Replaced asserts by exceptions because these errors
//                               cannot be handled. Once the exception is thrown, the
//                               computation must quit.
//
// \endverbatim

#include <vcl_utility.h>
#include <vcl_map.h>
#include <vcl_string.h>

#include <dbsk2d/dbsk2d_base_gui_geometry.h>
#include <dbsk2d/dbsk2d_utils.h>
#include <dbsk2d/dbsk2d_ishock_elm.h>
#include <dbsk2d/dbsk2d_ishock_bnd_key.h>

//: dbsk2d_ishock_belm types allowed
enum belm_type
{
  BPOINT, BLINE, BARC
};

// All the boundary element classes
class dbsk2d_ishock_belm;
class dbsk2d_ishock_bpoint;
class dbsk2d_ishock_bline;
class dbsk2d_ishock_barc;

//The basic shock elements this class needs to know about
class dbsk2d_ishock_elm;
class dbsk2d_ishock_node;
class dbsk2d_ishock_edge;
class dbsk2d_ishock_contact;

//useful type definitions
typedef vcl_multimap <dbsk2d_ishock_bnd_key, dbsk2d_ishock_edge*> bnd_ishock_map;
typedef bnd_ishock_map::iterator bnd_ishock_map_iter;
typedef bnd_ishock_map::reverse_iterator bnd_ishock_map_riter;

typedef vcl_list<dbsk2d_ishock_belm* > belm_list;
typedef belm_list::iterator  belm_list_iter;

//for GetInfo functions
#define IDifExists(x) ((x)==(0)?(0):(x->id()))
#define TypeifExists(x) ((x)==(0)?(0):(x->type()))

//: Base class for all boundary elements involved in intrinsic 
// shock computation. It contains the links from the boundary elements
// to the intrinsic shock elements. 
class dbsk2d_ishock_belm : public dbsk2d_base_gui_geometry
{
protected:

  //: Unique ID of this element
  int  _id;

  //: GUIElement?
  bool _bGUIElm;

  //: Type identifier
  belm_type _type;

  //: List of shocks formed by this element mapped by the boundary parameter
  bnd_ishock_map shock_map_;  

public:

  //: Constructor - type 1
  dbsk2d_ishock_belm (int id);
  
  //: Constructor - type 2
  dbsk2d_ishock_belm(int id, bool bGUI, belm_type type);

  //: Destructor
  virtual ~dbsk2d_ishock_belm (){}
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a () const=0; 

  //: Return the unique ID of this element
  int id() const { return _id; }
  
  //: Is this a GUI element?
  bool is_a_GUIelm() const { return _bGUIElm; }
  void set_GUIelm(bool bguielm) { _bGUIElm = bguielm; }

  //: is this a point?
  bool is_a_point() const { return _type == BPOINT; }

  //: is this a curve?
  bool is_a_curve() const { return (_type == BLINE || _type == BARC); }

  //: is this a line?
  bool is_a_line() const { return _type == BLINE; }

  //: is this an arc?
  bool is_an_arc() const { return _type == BARC; }

  //: Return itself if it is a GUI element, else, return its twinelm.
  virtual dbsk2d_ishock_belm* GUIelm() =0;

  //: Return Boundary type
  belm_type type() { return _type; };

  //: Return start point of this element
  virtual vgl_point_2d<double> start() const =0;

  //: Return end point of this element
  virtual vgl_point_2d<double> end() const =0;

  //: Return the boundary point at the start  
  virtual dbsk2d_ishock_bpoint* s_pt() =0;
  virtual const dbsk2d_ishock_bpoint* s_pt() const =0;

  //: Return the boundary point at the end
  virtual dbsk2d_ishock_bpoint* e_pt() =0;
  virtual const dbsk2d_ishock_bpoint* e_pt() const =0;

  //---------------------------------------------------------
  //Functions for working with the shock list (wavefront map)
  //---------------------------------------------------------

  //: Return the Bnd shock map
  bnd_ishock_map& shock_map() { return shock_map_; }

  //: number of shocks formed by this boundary
  int number_of_shocks() const { return shock_map_.size(); }
  
  //: add the shock into the shock map at the slot defined by its boundary params
  virtual void add_shock (dbsk2d_ishock_edge* shock);

  //: delete this shock from the shock map
  virtual bool delete_shock (dbsk2d_ishock_edge* sielm);

  //: delete links to extra shocks
  void delete_lr_refs(dbsk2d_ishock_edge* selm);

  //: min eta for this element
  virtual double min_eta() const=0;

  //: max eta for this element
  virtual double max_eta() const=0;

  //: test if a particular point on this element is still valid or burnt
  virtual bool is_wavefront_alive(double eta, double time);

  //: return the shock at a given eta on the element
  virtual dbsk2d_ishock_edge* get_shock_at(double eta, bool & degen_eta);

  //: return the shock starting to the left of this eta
  virtual dbsk2d_ishock_edge* get_left_neighboring_shock_at(double eta);

  //: return the shock starting to the right of this eta
  virtual dbsk2d_ishock_edge* get_right_neighboring_shock_at(double eta);

  //: check to see if the current shock invalidates its neighbor
  bool shock_invalidates_neighbor(dbsk2d_ishock_edge* shock, 
                                  dbsk2d_ishock_edge* neighbor);

  //---------------------------------------------------------
  // topology related functions
  //---------------------------------------------------------

  //: Update connectivity of line and arcs when their endpoints are changed.
  virtual void reconnect(dbsk2d_ishock_bpoint* /*oldPt*/, dbsk2d_ishock_bpoint* /*newPt*/){}

  //: return a list of belements it interacts with
  void get_interacting_belements(belm_list & belmList);

  //: get contour id
  virtual int get_contour_id(){return 0;}

  static bool throw_exception;

};

#endif // dbsk2d_ishock_belm_h_
