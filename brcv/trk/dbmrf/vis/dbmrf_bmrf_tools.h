// This is breye1/bvis1/tool/dbmrf_tools.h
#ifndef dbmrf_tools_h_
#define dbmrf_tools_h_
//:
// \file
// \brief Tools that work on bmrf networks
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 2/10/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vcl_map.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_bmrf_soview2D.h>
#include <dbmrf/pro/dbmrf_bmrf_storage_sptr.h>
#include <dbmrf/pro/dbmrf_curvel_3d_storage_sptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_gamma_func_sptr.h>
#include <bmrf/bmrf_curvel_3d_sptr.h>
#include <bmrf/bmrf_curve_3d_sptr.h>

//: An abstract base class for tools in this file
class dbmrf_tool : public bvis1_tool
{
public:
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
protected:
  dbmrf_tool();
  
  vgui_easy2D_tableau_sptr tableau_;
  dbmrf_bmrf_storage_sptr storage_;
};

  
//: A tool for inspecting nodes in the network
class dbmrf_inspector_tool : public dbmrf_tool
{
public:
  dbmrf_inspector_tool();
  virtual ~dbmrf_inspector_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Add popup menu items
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);
  
  //: prune the network by probability
  void prune_by_probability();

  //: prune the network by gamma
  void prune_by_gamma();

  //: prune the network of one directional links
  void prune_directed();

protected:
  //: Print stats about the node 
  void print_arc_stats(const bmrf_node_sptr node) const;

  vgui_style_sptr neighbor_style_;
  bool draw_epipolar_line_;
  bool draw_intensity_;
  bool draw_neighbors_;
  bool color_by_intensity_;
  bool print_stats_;
  
private:
  bgui_bmrf_epi_seg_soview2D* object_;
  bmrf_node_sptr curr_node_, last_node_;
  float last_x_, last_y_;

};


  
//: A tool for inspecting 2d projections of 3d curves
class dbmrf_inspect_3d_tool : public dbmrf_tool
{
public:
  dbmrf_inspect_3d_tool();
  virtual ~dbmrf_inspect_3d_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: This is called when the tool is activated
  virtual void activate();

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
    
private:
  dbmrf_curvel_3d_storage_sptr curvel_storage_;
  bmrf_node_sptr curr_node_;
  bmrf_curve_3d_sptr curr_curve_;
};



//: A tool for creating transformed epi-segments
class dbmrf_xform_tool : public dbmrf_tool
{
public:
  dbmrf_xform_tool( const vgui_event_condition& lift = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );
  virtual ~dbmrf_xform_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

protected:
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  vgui_easy2D_tableau_sptr cached_tableau_;

private:
  bool active_;
  bmrf_node_sptr node_;
  float init_s_, init_a_;
  int start_frame_;
  bmrf_gamma_func_sptr gamma_func_;
  
};


#endif // dbmrf_tools_h_
