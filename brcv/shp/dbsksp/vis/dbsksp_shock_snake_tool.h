// This is contrib/ntrinh/v2_gui/dbsksp_shock_snake_tool.h
#ifndef dbsksp_shock_snake_tool_h_
#define dbsksp_shock_snake_tool_h_
//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 28, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsksp_shock_snake_tool
// ============================================================================
//: A tool for designing an A3 v2_shock branch and its boundary
class dbsksp_shock_snake_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsksp_shock_snake_tool();

  //: Destructor
  virtual ~dbsksp_shock_snake_tool();
  
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // DATA ACESS ---------------------------------------------------------------

  //: Get the tableau this tool is working with
  dbsksp_shock_tableau_sptr tableau();

  //: Get the storage associated with the active tableau
  dbsksp_shock_storage_sptr shock_storage();

  //: Return storage of the image to segment
  vidpro1_image_storage_sptr image_storage() const 
  { return this->image_storage_; }

  //: Set the image storage
  bool set_image_storage ( const bpro1_storage_sptr& image_storage);
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  // weight for shape prior
  float lambda_;
  float chord_length_;
  unsigned num_times_;
  float max_width_;
  float min_width_;

protected:
  
  dbsksp_shock_tableau_sptr tableau_;
  dbsksp_shock_storage_sptr shock_storage_;

  // storage of image to segment
  vidpro1_image_storage_sptr image_storage_;

  
  vgui_event_condition gesture_select_target_;

};



// ============================================================================
// dbsksp_shock_snake_command
// ============================================================================

//: A vgui command to set the active group
class dbsksp_shock_snake_command : public vgui_command
{

public:
  dbsksp_shock_snake_tool* tool_;
public:  
  dbsksp_shock_snake_command(dbsksp_shock_snake_tool* tool): tool_(tool) {};
  virtual ~dbsksp_shock_snake_command(){}
  dbsksp_shock_snake_tool* tool() {return this->tool_; }

  void execute(){}
};


// ============================================================================
// dbsksp_shock_snake_choose_image_command
// ============================================================================

//: A vgui command to an image for segmentation
class dbsksp_shock_snake_choose_image_command : 
  public dbsksp_shock_snake_command
{
public:  
  dbsksp_shock_snake_choose_image_command(dbsksp_shock_snake_tool* tool): 
      dbsksp_shock_snake_command(tool){}
  virtual ~dbsksp_shock_snake_choose_image_command(){}

  void execute();
};


// ============================================================================
// dbsksp_shock_snake_set_params_command
// ============================================================================

//: A vgui command to set parameters for the shock snake
class dbsksp_shock_snake_change_params_command : 
  public dbsksp_shock_snake_command
{
public:  
  dbsksp_shock_snake_change_params_command(dbsksp_shock_snake_tool* tool): 
      dbsksp_shock_snake_command(tool){}
  virtual ~dbsksp_shock_snake_change_params_command(){}

  void execute();
};



// ============================================================================
// dbsksp_fit_two_edges_command
// ============================================================================

//: A vgui command to 
class dbsksp_fit_two_edges_command : 
  public dbsksp_shock_snake_command
{
public:  
  dbsksp_fit_two_edges_command(dbsksp_shock_snake_tool* tool): 
      dbsksp_shock_snake_command(tool){}
  virtual ~dbsksp_fit_two_edges_command(){}

  void execute();
};

// ============================================================================
// dbsksp_extend_and_fit_two_edges_command
// ============================================================================

//: A vgui command to 
class dbsksp_add_one_edge_and_deform_command : 
  public dbsksp_shock_snake_command
{
public:  
  dbsksp_add_one_edge_and_deform_command(dbsksp_shock_snake_tool* tool): 
      dbsksp_shock_snake_command(tool){}
  virtual ~dbsksp_add_one_edge_and_deform_command(){}

  void execute();
};

#endif //dbsksp_shock_snake_tool



