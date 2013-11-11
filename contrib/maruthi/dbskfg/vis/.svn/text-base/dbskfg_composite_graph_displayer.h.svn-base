// This is brcv/shpdbskfg/vis/dbskfg_composite_graph_displayer.h
#ifndef dbskfg_composite_graph_displayer_h_
#define dbskfg_composite_graph_displayer_h_

//:
// \file
// \brief  A displayer responsible for generating a tableau for
//  a dbskfg_composite_graph
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date   July 07, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

#include <dbskfg/vis/dbskfg_composite_graph_tableau_sptr.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>

class dbskfg_composite_graph_displayer : public bvis1_displayer 
{

public:

  dbskfg_composite_graph_displayer():bvis1_displayer(){}

  //: Return type string "dbskfg_composite_graph"
  /* virtual */ 
  vcl_string type() const { return "composite_graph"; }

  //: Create a tableau if the storage object is of type "dbskfg_composite_graph"
  /* virtual */ 
  vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Update a tableau with a the storage object if both are of type image
  /* virtual */
  bool update_tableau( vgui_tableau_sptr, const bpro1_storage_sptr& ) const;
 
protected:
  
  //: Perform the update once types are known
  virtual bool update_composite_graph_tableau
      (dbskfg_composite_graph_tableau_sptr, 
       const dbskfg_composite_graph_storage_sptr&) const;


};

#endif // dbskfg_composite_graph_displayer_h_
