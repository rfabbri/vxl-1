// This is dbsksp/vis/dbsksp_soview_shock.h
#ifndef dbsksp_soview_shock_h_
#define dbsksp_soview_shock_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief soview for shock objects (nodes and edges)
// \author Nhon Trinh
// \date Oct 3, 2006
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_bnd_arc_sptr.h>


class dbsksp_soview_shock_node : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_shock_node(const dbsksp_shock_node_sptr& node) :
       node_(node){};

  //: Destructor
  virtual ~dbsksp_soview_shock_node() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_shock_node"; }

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
  dbsksp_shock_node_sptr node() const {return this->node_; }
 
protected:
  dbsksp_shock_node_sptr node_;

};



//: Display the chord of a shock edge
class dbsksp_soview_shock_edge_chord : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_shock_edge_chord(const dbsksp_shock_edge_sptr& edge) :
       edge_(edge){};

  //: Destructor
  virtual ~dbsksp_soview_shock_edge_chord() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_shock_edge_chord"; }

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
  dbsksp_shock_edge_sptr edge() const {return this->edge_; }
 
protected:
  dbsksp_shock_edge_sptr edge_;

};



//: Display the chord of a shock edge
class dbsksp_soview_shock_geom : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_shock_geom(const dbsksp_shock_edge_sptr& edge) :
       edge_(edge){};

  //: Destructor
  virtual ~dbsksp_soview_shock_geom() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_shock_geom"; }

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
  dbsksp_shock_edge_sptr edge() const {return this->edge_; }
 
protected:
  dbsksp_shock_edge_sptr edge_;

};





// ============================================================================

//: Display contact shocks around a node
//: Display for a shock edge
class dbsksp_soview_contact_shock : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_contact_shock(const dbsksp_shock_node_sptr& node) :
       node_(node)
       {
        this->set_selectable(false);
       };

  //: Destructor
  virtual ~dbsksp_soview_contact_shock() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const 
  {return "dbsksp_soview_contact_shock"; }

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
  dbsksp_shock_node_sptr node() const {return this->node_; }
 
protected:
  dbsksp_shock_node_sptr node_;

};





// ============================================================================

//: Display a circular arc boundary of an edge
class dbsksp_soview_bnd_arc : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_bnd_arc(const dbsksp_bnd_arc_sptr& bnd_arc) :
       bnd_arc_(bnd_arc){};

  //: Destructor
  virtual ~dbsksp_soview_bnd_arc() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_bnd_arc"; }

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
  dbsksp_bnd_arc_sptr bnd_arc() const {return this->bnd_arc_; }
 
protected:
  dbsksp_bnd_arc_sptr bnd_arc_;

};



// ============================================================================

//: Display a shapelet
class dbsksp_soview_shapelet : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview_shapelet(const dbsksp_shapelet_sptr& shapelet, 
     const vgui_style_sptr& shock_chord_style = 0,
     const vgui_style_sptr& shock_geom_style = 0, 
     const vgui_style_sptr& contact_shock_style = 0, 
     const vgui_style_sptr& bnd_style = 0);

  //: Destructor
  virtual ~dbsksp_soview_shapelet() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbsksp_soview_shapelet"; }

  //: Render this shapelet on the display.
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
  dbsksp_shapelet_sptr shapelet() const {return this->shapelet_; }

  //: Set the styles to draw the shapelets
  void set_shapelet_styles(const vgui_style_sptr& shock_chord_style,
    const vgui_style_sptr& shock_geom_style, 
    const vgui_style_sptr& contact_shock_style, 
    const vgui_style_sptr& bnd_style)
  {
    this->shock_chord_style_ = shock_chord_style;
    this->shock_geom_style_ = shock_geom_style;
    this->contact_shock_style_ = contact_shock_style;
    this->bnd_style_ = bnd_style;
  }

 
protected:
  dbsksp_shapelet_sptr shapelet_;
  vgui_style_sptr shock_chord_style_;
  vgui_style_sptr shock_geom_style_;
  vgui_style_sptr contact_shock_style_;
  vgui_style_sptr bnd_style_;
};

#endif // dbsksp_soview_shock_h_
