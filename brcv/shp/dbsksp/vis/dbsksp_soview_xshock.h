// This is dbsksp/vis/dbsksp_soview_xshock.h
#ifndef dbsksp_soview_xshock_h_
#define dbsksp_soview_xshock_h_

//--------------------------------------------------------------------------------
//:
// \file
// \brief soview for xshock objects (nodes and edges)
// \author Nhon Trinh
// \date August 18, 2008
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <vgl/vgl_point_2d.h>



// =============================================================================
// dbsksp_soview_xshock_node
// =============================================================================

//: Display an extrinsic node
class dbsksp_soview_xshock_node : public vgui_soview2D
{
 public:
  //: Constructor
  dbsksp_soview_xshock_node(const dbsksp_xshock_node_sptr& xedge);

  //: Destructor
  virtual ~dbsksp_soview_xshock_node() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xshock_node"; }

  //: Render this object on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_node_sptr xnode() const {return this->xnode_; }

  //: Compute extrinsic geometry
  void compute_geometry();


protected:
  //: extrinsic node corresponding to this object
  dbsksp_xshock_node_sptr xnode_;  

  //: extrinsic point of this node
  vgl_point_2d<float > pt_;

};


// =============================================================================
// dbsksp_soview_xshock_chord
// =============================================================================

//: Display the chord of an extrinsic edge
class dbsksp_soview_xshock_chord : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_xshock_chord(const dbsksp_xshock_edge_sptr& xedge);

  //: Destructor
  virtual ~dbsksp_soview_xshock_chord() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xshock_chord"; }

  //: Render this object on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_edge_sptr xedge() const {return this->xedge_; }

  //: Compute geometry (start and end point) of the chord
  void compute_geometry();

protected:
  //: extrinsic edge corresponding to this object
  dbsksp_xshock_edge_sptr xedge_;  

  // cache the geometry of the chord
  vgl_point_2d<float > start_;
  vgl_point_2d<float > end_;
};



//==============================================================================
// dbsksp_soview_xshock_curve
//==============================================================================

//: Display the chord of an extrinsic edge
class dbsksp_soview_xshock_curve : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_xshock_curve(const dbsksp_xshock_edge_sptr& xedge);

  //: Destructor
  virtual ~dbsksp_soview_xshock_curve() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xshock_curve"; }

  //: Render this object on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_edge_sptr xedge() const {return this->xedge_; }

  //: Compute geometry (start and end point) of the chord
  void compute_geometry();

protected:
  //: extrinsic edge corresponding to this object
  dbsksp_xshock_edge_sptr xedge_;  

  // cache the geometry of the curve
  vcl_vector<vgl_point_2d<float > > pts_;

  // location of centroid
  float centroid_x_;
  float centroid_y_;
};



// =============================================================================
// dbsksp_soview_xshock_contact
// =============================================================================

//: Display the contact shocks in an extrinsic shock graph
class dbsksp_soview_xshock_contact : public vgui_soview2D
{
 public:
  //: Constructor
  dbsksp_soview_xshock_contact(const dbsksp_xshock_node_sptr& xnode);

  //: Destructor
  virtual ~dbsksp_soview_xshock_contact() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xshock_contact"; }

  //: Render this object on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_node_sptr xnode() const {return this->xnode_; }

  //: Compute geometry (center + boundary points)
  void compute_geometry();

protected:
  //: extrinsic node corresponding to this object
  dbsksp_xshock_node_sptr xnode_;  

  // cache the geometry of the contact shocks
  vgl_point_2d<float > center_;
  vcl_vector<vgl_point_2d<float > > bnd_pts_;
};


// =============================================================================
// dbsksp_soview_xshock_bnd
// =============================================================================

//: Display the boundary elements of an extrinsic shock graph
class dbsksp_soview_xshock_bnd : public vgui_soview2D
{
 public:
  //: Constructor
  dbsksp_soview_xshock_bnd(const dbsksp_xshock_edge_sptr& xedge, bool is_left_bnd);

  //: Destructor
  virtual ~dbsksp_soview_xshock_bnd() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xshock_bnd"; }

  //: Render this object on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_edge_sptr xedge() const {return this->xedge_; }

  //: Compute geometry (center + boundary points)
  void compute_geometry();

protected:
  //: extrinsic edge ``this'' boundary contour corresponds to
  dbsksp_xshock_edge_sptr xedge_;  

  //: which boundary side of the shock
  bool is_left_bnd_;

  // cache the geometry of the contact shocks
  vcl_vector<vgl_point_2d<float > > bnd_pts_;
};



// =============================================================================
// dbsksp_soview_xfrag
// =============================================================================
//: Display an extrinsic shock fragment
class dbsksp_soview_xfrag : public vgui_soview2D
{
 public:
  //: Constructor
  dbsksp_soview_xfrag(const dbsksp_xshock_fragment_sptr& xfrag);

  //: Destructor
  virtual ~dbsksp_soview_xfrag() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xfrag"; }

  //: Render this node on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbsksp_xshock_fragment_sptr xfrag() const {return this->xfrag_; }

  //: Set the styles to draw the extrinsic fragment
  void set_xfrag_styles(const vgui_style_sptr& shock_chord_style,
    const vgui_style_sptr& contact_shock_style, 
    const vgui_style_sptr& bnd_style);

protected:
  //: extrinsic fragment of ``this'' soview object
  dbsksp_xshock_fragment_sptr xfrag_;  

  // drawing styles
  vgui_style_sptr bnd_style_;
  vgui_style_sptr contact_shock_style_;
  vgui_style_sptr shock_chord_style_;
};







// =============================================================================
// dbsksp_soview_xsample
// =============================================================================
//: Display an extrinsic shock fragment
class dbsksp_soview_xsample : public vgui_soview2D
{
 public:
  //: Constructor
  dbsksp_soview_xsample(const dbsksp_xshock_node_descriptor& xsample);

  //: Destructor
  virtual ~dbsksp_soview_xsample() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_xsample"; }

  //: Render this node on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y) {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Compute geometry (center + boundary points)
  void compute_geometry();

protected:
  dbsksp_xshock_node_descriptor xsample_;

  vgl_point_2d<float > shock_pt_;
  vgl_point_2d<float > bnd_pts_[2];

  // drawing styles
};


#endif // dbsksp_soview_xshock_h_
