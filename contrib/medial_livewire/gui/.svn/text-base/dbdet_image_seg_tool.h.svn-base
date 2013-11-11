// This is brcv/seg/dbdet/vis/dbdet_image_seg_tool.h
#ifndef dbdet_image_seg_tool_h_
#define dbdet_image_seg_tool_h_
//:
// \file
// \brief 
// author Nhon Trinh (ntrinh@lems.brown.edu)
// date May 29, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>


//: A base class for interactive image segmentation tools
class dbdet_image_seg_tool : public bvis1_tool
{
public:
  //: Constructor
  dbdet_image_seg_tool();

  //: Return the name of this tool
  virtual vcl_string name() const {return "image seg"; }

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: This is called when the tool is activated
  virtual void activate();

  ////: This is called when the tool is deactivated
  //virtual void deactivate();

  /*virtual bool handle(const vgui_event& e, 
                       const bvis1_view_tableau_sptr& view );
*/

  


  // =======================================================
  // DATA ACCESS
  // =======================================================
  
  //=========================================================
  //      EVENT HANDLERS
  //=========================================================

  //: select desired input and output classes from repository
  bool select_io_storage();
  
  //: Return name of image storage
  vcl_string image_name() const {return this->image_name_; }

  //: Set name of image
  void set_image_name(const vcl_string& image_name)
  {this->image_name_ = image_name; }

  //: Return name of vsol2d storage
  vcl_string vsol2d_name() const {return this->vsol2d_name_; }

  //: Set name of vsol2d storage
  void set_vsol2d_name(const vcl_string& vsol2d_name ) 
  {this->vsol2d_name_ = vsol2d_name; }

  //========================================================
  //  UTILITIES
  //========================================================

  //========================================================
  //  MEMBER VARIABLES
  //========================================================

public:

protected:
  //: name of image storage
  vcl_string image_name_;

  //: name of vsol2d storage
  vcl_string vsol2d_name_;
    
  //: active tableau
  vgui_image_tableau_sptr tableau_;

  //: target image
  vil_image_resource_sptr image_;

  //: vidpro1 storage
  vidpro1_image_storage_sptr storage_;
 
  //: return storage
  vidpro1_image_storage_sptr storage();


};



#endif // dbdet_image_seg_tool_h_
