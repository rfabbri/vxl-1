// This is breye/bvis/tool/bvis_correspond_tools.h
#ifndef bvis_correspond_tools_h_
#define bvis_correspond_tools_h_
//:
// \file
// \brief Tools that find correspondence between views
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 8/6/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <bvis/bvis_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bvis/bvis_proc_selector_tableau_sptr.h>
#include <vgui/vgui_displaylist2D_tableau.h> 


//: A tool for translating soview objects by calling their translate method
class bvis_correspond_point_tool : public bvis_tool {
public:
  
  //: Constructor
  bvis_correspond_point_tool( const vgui_event_condition& 
                              select = vgui_event_condition(vgui_LEFT, 
                                                            vgui_MODIFIER_NULL, 
                                                            true) );
  //: Destructor
  virtual ~bvis_correspond_point_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis_view_tableau_sptr& view );

  //: Add popup menu items
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: Return the corresponding points as a vector of vectors
  //  - corresponding points are in the same order in each vector
  //  - one vector is returned for each tableau in \p tabs in the same order
  //  - NULL points are returned for missing correspondences
  vcl_vector<vcl_vector<vsol_point_2d_sptr> >
    correspondences(const vcl_vector<vgui_displaylist2D_tableau_sptr>& tabs) const;

  //: Return the set of tableaux with correspondences
  vcl_set<vgui_displaylist2D_tableau_sptr> tableaux() const { return tab_set_; }
  virtual bpro_storage_sptr active_storage(){return 0;}//FIX LATER JLM
  virtual vcl_vector< vcl_string > get_input_type()//FIX LATER JLM
    {return vcl_vector< vcl_string >();}
  
  virtual vcl_vector< vcl_string > get_output_type()//FIX LATER JLM
    {return vcl_vector< vcl_string >();}

protected:
  vgui_event_condition gesture_select_;
  const vgui_style_sptr point_style_;

  //: Search the correspondences for this point and make the set active
  // \retval true if found in the correspondece set
  // \retval false if not found
  bool set_curr_corr(const bgui_vsol_soview2D_point* pt);

  //: Returns true if \p tableau is a visible child of \p selector
  bool is_visible_child( const bvis_proc_selector_tableau_sptr& selector,
                         const vgui_tableau_sptr& tableau ) const;

  bool modify_;
  bgui_vsol_soview2D_point* object_;
  vgui_displaylist2D_tableau_sptr tableau_;

  //: A correspondence is a mapping from tableau to the point in that view
  // \note a typedef should be used, but MSVC++ 6.0 has trouble with long names
  class corr_map : public vcl_map<vgui_displaylist2D_tableau_sptr, bgui_vsol_soview2D_point*>
  {
   public:
    corr_map() : vcl_map<vgui_displaylist2D_tableau_sptr, bgui_vsol_soview2D_point*>() {}
  };
  //: The set of all correspondences
  vcl_set<corr_map> correspondences_;
  //: An iterator to the current correspondence
  vcl_set<corr_map>::iterator curr_corr_;

  //: The set of all tableau containing correspondences
  vcl_set<vgui_displaylist2D_tableau_sptr> tab_set_;


};


#endif // bvis_correspond_tools_h_
