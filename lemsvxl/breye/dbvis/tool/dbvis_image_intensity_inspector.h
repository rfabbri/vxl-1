#ifndef   _dbvis_image_intensity_inspector_
#define   _dbvis_image_intensity_inspector_

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_event.h>
#include <bvis/bvis_view_tableau.h>
#include <bvis/bvis_tool.h>

//: tool to inspect the RGB,IHS values of images
class  dbvis_image_intensity_inspector:public bvis_tool
{
 public:
  //: Constructor
  dbvis_image_intensity_inspector();
  //: Destructor
  virtual ~dbvis_image_intensity_inspector(){}
  //: Returns the string name of this tool
  virtual vcl_string name() const{return "image inspector";}
  
  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );
  bool set_storage( const bpro_storage_sptr& storage );
  
  //: deactivate tool
  virtual void deactivate();
  virtual void activate();
  //: handle events
  virtual bool handle( const vgui_event & e,const bvis_view_tableau_sptr& selector );
  
  void 
    get_popup( const vgui_popup_params& params, 
         vgui_menu &menu );
  
protected:
  
  vgui_image_tableau_sptr tableau_;
  vidpro_image_storage_sptr storage_;

  bool rgb_;
  bool ihs_;
};



#endif
