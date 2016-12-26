// This is bvis/bvis_view_tableau.h
#ifndef bvis_view_tableau_h_
#define bvis_view_tableau_h_
//:
// \file
// \brief  Tableau at the root of each view of the data in bvis2
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/13/04
//
//  Contains classes: bvis_view_tableau  bvis_view_tableau_new


#include "bvis_view_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <bvis/bvis_proc_selector_tableau_sptr.h>


class bvis_view_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use bvis_view_tableau_new.
  bvis_view_tableau();

  //: Constructor - don't use this, use bvis_view_tableau_new.
  bvis_view_tableau(const bvis_proc_selector_tableau_sptr selector);

  //: Destructor
  ~bvis_view_tableau();

  //: Returns the type of tableau ('bvis_view_tableau').
  vcl_string type_name() const;

  //: Access the selector tableau
  bvis_proc_selector_tableau_sptr selector() const;


 protected:
  //: The child of this tableau is guaranteed to be a bvis_proc_selector_tableau
  vgui_parent_child_link child_;

};

//: Create a smart-pointer to a bvis_view_tableau.
struct bvis_view_tableau_new : public bvis_view_tableau_sptr {
  typedef bvis_view_tableau_sptr base;

  //: Constructor - creates a default bvis_view_tableau.
  bvis_view_tableau_new() : base(new bvis_view_tableau()) { }

  //: Constructor - creates a bvis_view_tableau.
  bvis_view_tableau_new(const bvis_proc_selector_tableau_sptr selector)
   : base(new bvis_view_tableau(selector)) { }
};

#endif // bvis_view_tableau_h_
