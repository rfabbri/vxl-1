// This is contrib/fine/gui/dbseg_seg_drawing_tableau.h
#ifndef dbseg_seg_drawing_tableau_h_
#define dbseg_seg_drawing_tableau_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A child of seg_tableau that does the drawing
// \author
//   (8/1/08) Eli Fine
//
//
// \verbatim
//  Modifications:
//   
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>

#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_polygon.h>

#include "dbseg_seg_drawing_tableau_sptr.h"
#include <structure/dbseg_seg_object.h>
#include "dbseg_seg_tableau.h"


class dbseg_seg_drawing_tableau : public vgui_easy2D_tableau 
{
 public:
  dbseg_seg_drawing_tableau(dbseg_seg_tableau* tab);


  ~dbseg_seg_drawing_tableau();

  bool handle(const vgui_event &e);

  //: Returns the type of this tableau ('dbseg_seg_drawing_tableau').
  vcl_string type_name() const { return "dbseg_seg_drawing_tableau";}

 protected:
    //pointer to the parent tableau    
    dbseg_seg_tableau* tab_;

    //display constants
    float outline_width_;
    float inner_contour_width_;
    float highlight_width_;

    //which region is highlighted
    int highlighted;


    void init();
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct dbseg_seg_drawing_tableau_new : public dbseg_seg_drawing_tableau_sptr
{
  typedef dbseg_seg_drawing_tableau_sptr base;

  dbseg_seg_drawing_tableau_new(dbseg_seg_tableau* tab) :
    base(new dbseg_seg_drawing_tableau(tab)) { }


  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // dbseg_seg_drawing_tableau_h_


