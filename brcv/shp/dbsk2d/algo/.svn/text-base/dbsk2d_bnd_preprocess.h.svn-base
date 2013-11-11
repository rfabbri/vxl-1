// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess.h
#ifndef dbsk2d_bnd_preprocess_h_
#define dbsk2d_bnd_preprocess_h_
//:
// \file
// \brief A class for pre-processing shock boundary
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 06/28/2005
//
// 
// \verbatim
//  Modifications
//   Nhon Trinh    06/28/2005    Initial version.
//            
// \endverbatim


#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

//: A class for pre-processing shock boundary
class dbsk2d_bnd_preprocess
{
  //***********************************************
  // Constants
  //***********************************************
public:
  // minimum distance between boundary elements allowed.
  // Any distance lower than this number will have to be
  // either shortened to zero (merging the points)
  // or increased to be more than the this threshold.
  static double distance_tol; // = B_EPSILON

  //: vkey.first : the edge that will be modified
  // vkey.second : position (ratio) on the edge that modifications 
  // will take place
  typedef vcl_pair<dbsk2d_bnd_edge_sptr, double > vkey;
  typedef vcl_pair<vkey, dbsk2d_bnd_vertex_sptr > vkey_vertex_pair;
  
  // key_vertex_map.val : vertex that will be inserted at vkey
  typedef vcl_multimap<vkey, dbsk2d_bnd_vertex_sptr > vkey_vertex_map;

protected:
  //********************************************
  // DATA MEMBERS
  //********************************************

  //: The boundary `this' preprocessor is working on
  dbsk2d_boundary_sptr boundary_;
  
public:
  //***********************************************
  // Constructors/Destructors/Initialize
  //***********************************************

  //: Constructor
  dbsk2d_bnd_preprocess (){};

  //: Destructor
  virtual ~dbsk2d_bnd_preprocess (){};

  
  //***********************************************
  // Interface functions
  //***********************************************

  //: Pre-process a shock boundary
  // Return false if preprocessing fails
  bool preprocess(dbsk2d_boundary_sptr boundary, bool talkative = false);

  //: Return true if the boundary needs preprocessing
  bool need_preprocessing(dbsk2d_boundary_sptr boundary);

  //: Pre-process a group of edges
  // Return false if preprocessing fails
  bool preprocess(vcl_list<dbsk2d_bnd_edge_sptr >& edges);

  
  ////: Pre-process a group of edges
  //// Return false if preprocessing fails
  //bool preprocess(vcl_list<dbsk2d_bnd_edge_sptr >& new_edges,
  //  vcl_list<dbsk2d_bnd_edge_sptr >& preproc_edges);

  
  //: Return true if the set of edges needs preprocessing
  bool need_preprocessing(vcl_list<dbsk2d_bnd_edge_sptr >& edges);


  //***********************************************
  // Data access
  //***********************************************

  //: Return smart pointer to the boundary `this' preprocessor is working on
  dbsk2d_boundary_sptr boundary() const { return this->boundary_;}

  //: Set the boundary
  void set_boundary(dbsk2d_boundary_sptr boundary) { this->boundary_ = boundary;}

  //***********************************************
  // Internal functions
  //***********************************************
public:

  //==================== FOR A GROUP OF EDGES ==========================

  // ------------ COMMON --------------------------------

  //: Remove "unlinked" edges in an edge list
  void remove_unlinked_edges(vcl_list<dbsk2d_bnd_edge_sptr >& edges);

  //: Separate out the edges into three groups - points, lines, and arcs
  // Put a zero pointer for uninterested groups
  void classify_edges(vcl_list<dbsk2d_bnd_edge_sptr >& edges,
    vcl_list<dbsk2d_bnd_edge_sptr >* bnd_pts, 
    vcl_list<dbsk2d_bnd_edge_sptr >* bnd_lines, 
    vcl_list<dbsk2d_bnd_edge_sptr >* bnd_arcs); 

  //: Convert too short lines and arcs into points
  // If the points are not stand-alone, the remove it
  void remove_short_curves(vcl_list<dbsk2d_bnd_edge_sptr >& edges);

  //: Merge vertices that are geometrically close
  void merge_close_vertices(vcl_list<dbsk2d_bnd_vertex_sptr >* affected_vertices,
    vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set1,
    vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set2 =0);


  //: Insert new vertices to the middle of the edges
  // The overall edge list will also be updated
  void insert_new_vertices(const vkey_vertex_map & new_vertex_map,
    vcl_list<dbsk2d_bnd_edge_sptr >& all_edges,
    vcl_list<dbsk2d_bnd_vertex_sptr >& affected_vertices);


  //: Dissolve end vertices into curves(lines, arcs) when they are too close
  // Require: the vertex list is unique
  // \TODO handle cases for arcs
  void dissolve_vertices_into_curves(vcl_list<dbsk2d_bnd_edge_sptr >& tainted_edges, 
    vcl_list<dbsk2d_bnd_edge_sptr >& bnd_curves,
    const vcl_list<dbsk2d_bnd_vertex_sptr >& vertices);



  

  // ---------- PREPROCESS POINTS -----------------------------

  //: Remove unreal stand-alone points in the list of edges.
  // This operation should not affect the vertex list
  void remove_unreal_stand_alone_points(vcl_list<dbsk2d_bnd_edge_sptr >& edges);

  //: Merge close points in a group of points
  // Require: all bnd_edges in `bnd_pts' are degenerate, i.e. points
  void merge_close_points(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts);

  // -------------  PREPROCESS LINES -------------------------------
  
  //: Detect and form all intersection between `lineset1' and `lineset2'
  // if `lineset2' not given then intersect `lineset1' against itself
  // Return: `lineset1' contains all lines from both sets and `lineset2' will 
  // be empty
  // `tainted_lines' contains all lines affected by intersection and need
  // further processing
  void intersect_bnd_lines(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_lines, 
    vcl_list<dbsk2d_bnd_edge_sptr >* lineset1, 
    vcl_list<dbsk2d_bnd_edge_sptr >* lineset2=0);

  //: Remove (exact) duplicate lines - lines with same end vertices
  void remove_duplicate_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines);



  // -------------  PREPROCESS POINT-LINES -------------------------
 
  //: Remove stand-alone points if they are too close to a line
  void remove_points_close_to_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts,
    const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines);


  // -------------  PREPROCESS ARCS ---------------------------------

  //: Detect and form all intersection between `arcset1' and `arcset2'
  // if `arcset2' not given then intersect `arcset1' against itself
  // Return: `arcset1' contains all arcs from both sets and `arcset2' will 
  // be empty
  // `tainted_arcs' contains all arcs affected by intersection and need
  // further processing
  void intersect_bnd_arcs(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_arcs, 
    vcl_list<dbsk2d_bnd_edge_sptr >* arcset1, 
    vcl_list<dbsk2d_bnd_edge_sptr >* arcset2=0);


  //: Remove (exact) duplicate arcs - 
  // arcs with same end vertices and maximum distance is < threshold
  void remove_duplicate_arcs(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs);



  // -------------  PREPROCESS POINT-ARCS ------------------------- 

  //: Remove stand-alone points if they are too close to an arc
  // \TODO fix me
  void remove_points_close_to_arcs(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts,
    const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs);



  // -------------  PREPROCESS LINES-ARCS ---------------------------------

  //: Detect and form all intersection between `arcset' and `lineset'
  // `tainted_edges' contains all arcs affected by intersection and need
  // further processing
  void intersect_lines_against_arcs(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_edges, 
    vcl_list<dbsk2d_bnd_edge_sptr >* arcset, 
    vcl_list<dbsk2d_bnd_edge_sptr >* lineset);


  //: Remove arcs that share both vertices with another line and the maximum distance
  // between the arc and the line is less than distance threshold
  void remove_arcs_duplicating_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs, 
    const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines);

  
public:
  // ----------- NOT SORTED YET
  

  //==================== FOR THE WHOLE BOUNDARY ==========================
  
  //: Form bnd_contours from edges
  // require: the list of edges must be preprocessed
  void form_contours_from_edges(const vcl_list<dbsk2d_bnd_edge_sptr >& edges,
    vcl_list<dbsk2d_bnd_contour_sptr >& new_contours);

  
  //***********************************************
  // For Debugging purpose
  //***********************************************
public:

  //: Return true if the set of points need preprocessing
  bool points_need_preprocessing(const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts);
 

  //: Return true if this set of line edges need preprocessor
  bool lines_need_preprocessing(const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines);

  //: Return true if this set of line edges need preprocessor
  bool point_lines_need_preprocessing(
    const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines, 
    const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts );


  //void describe_edges(vcl_ostream& os = vcl_cout) const;
  
  //-------------------------------------------------------------------
  // PREPROCESSING
  //-------------------------------------------------------------------

  //dbsk2d_ishock_bpoint* doesBPointExist (dbsk2d_ishock_bpoint* bpoint);
  //void PreProcessBPoint (dbsk2d_ishock_bpoint* bp);
  //void PreProcessGUIElement (dbsk2d_ishock_belm* belm);

  ////: Pre-process the boundary 
  //void PreProcessBoundary (void);

  //void PreProcessBoundaryForEdgeInput(double position_accuracy, double angle_accuracy, 
  //  double operator_width, double operator_length);



  
 
};

#endif //dbsk2d_bnd_preprocess_h_
