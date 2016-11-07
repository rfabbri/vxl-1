// This is brl/bbas/bgui/bvis_proc_selector_tableau.h
#ifndef bvis_proc_selector_tableau_h_
#define bvis_proc_selector_tableau_h_
//:
// \file
// \brief  Tableau that allows process state to be set 
// \author J. L. Mundy
// \date   April 18, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>
#include <bgui/bgui_selector_tableau.h>
#include <bvis/bvis_proc_selector_tableau_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
class vgui_event;

//: Tableau that in addition to providing selection of active event processing
// and visibility ordering, also specifies process state, i.e.,the view
// is active for processing

class bvis_proc_selector_tableau : public bgui_selector_tableau
{
 public:
  //: Constructor - don't use this, use bvis_proc_selector_tableau_new.
  //  Creates an empty composite tableau.
  bvis_proc_selector_tableau();

  //: Constructor - don't use this, use bvis_proc_selector_tableau_new.
  //  Takes a vector of child tableaux.
  bvis_proc_selector_tableau(vcl_vector<vgui_tableau_sptr> const& children);

  //: Handle all events sent to this tableau.
  //  Key-press '?' prints info on this file, before being sent to the children.
  virtual bool handle(const vgui_event&);

  //: Returns the type of this tableau ('bvis_proc_selector_tableau').
  vcl_string type_name() const { return "bvis_proc_selector_tableau"; }

  //: Enable the selector for processing
  void enable_processing(){enabled_for_processing_=true;}

  //: Disable the selector for processing
  void disable_processing(){enabled_for_processing_=false;}

  //: Toggle process enable
  void toggle_processing(){enabled_for_processing_=!enabled_for_processing_;}
  //: Returns true if the selector is open for processing
  bool enabled_for_processing();
  //: Adds to base popup menu
  virtual void add_to_menu(vgui_menu &menu);
 protected:
  //: Destructor - called by bvis_proc_selector_tableau_sptr.
  virtual ~bvis_proc_selector_tableau();
  bool enabled_for_processing_;
};

//: Creates a smart-pointer to a bvis_proc_selector_tableau tableau.
struct bvis_proc_selector_tableau_new : public bvis_proc_selector_tableau_sptr
{
  typedef bvis_proc_selector_tableau_sptr base;

  //: Constructor - creates a pointer to an empty bvis_proc_selector_tableau.
  bvis_proc_selector_tableau_new() : base(new bvis_proc_selector_tableau()) { }


  //: Constructor - creates pointer to a composite with the given children.
  //  Takes a vector of child tableaux.
  bvis_proc_selector_tableau_new(vcl_vector<vgui_tableau_sptr> const& children)
    : base(new bvis_proc_selector_tableau(children)) {}
};

#endif // bvis_proc_selector_tableau_h_
