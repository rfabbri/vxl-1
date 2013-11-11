// This is brcv/seg/dbdet/vis/dbdet_image_seg_tool.cxx
//:
// \file

#include "dbdet_image_seg_tool.h"

#include <vgui/vgui_dialog.h>
#include <vidpro1/vidpro1_repository.h>
#include <bgui/bgui_image_tableau.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bvis1/bvis1_manager.h>


// ------------------------------------------------------------------
//: Constructor - protected
dbdet_image_seg_tool::
dbdet_image_seg_tool() :
image_name_(""),
vsol2d_name_(""),
tableau_(0),
image_(0)
{
}

// ------------------------------------------------------------------
//: This is called when the tool is activated
void dbdet_image_seg_tool::
activate() 
{
  this->select_io_storage();
  vcl_cerr<<this->vsol2d_name_<<vcl_endl;
}


//: Set the tableau to work with
bool dbdet_image_seg_tool::
set_tableau ( const vgui_tableau_sptr& tableau)
{
  if( tableau.ptr() != 0 && 
    tableau->type_name() == "vgui_image_tableau" &&
    bvis1_manager::instance()->storage_from_tableau(tableau)->name() == 
    this->image_name()
    )
  {
    this->tableau_.vertical_cast(tableau);
    this->image_ = this->tableau_->get_image_resource();
    bgui_image_tableau_sptr bgui_tab;
    bgui_tab.vertical_cast(this->tableau_);
    bgui_tab->unset_handle_motion();
    return true;
  }

  this->tableau_ = 0;
  return true;
}


//=========================================================
//      EVENT HANDLERS
//=========================================================



bool dbdet_image_seg_tool::
select_io_storage()
{
  //get the repository
  vidpro1_repository_sptr repository_sptr = bvis1_manager::instance()->repository();

  //Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select Inputs and outputs" );

  //display input options
  //vcl_vector< vcl_string > input_type_list = process_->get_input_type();
  vcl_vector< vcl_string > input_type_list;
  input_type_list.push_back("image");
  input_type_list.push_back("vsol2D");
  io_dialog.message("Select Input(s) From Available ones:");

  //store the choices
  vcl_vector<int> input_choices(input_type_list.size());
  vcl_vector< vcl_vector <vcl_string> > available_storage_classes(input_type_list.size());
  vcl_vector< vcl_string > input_names(input_type_list.size());
  
  for( unsigned int i = 0; i < input_type_list.size(); ++i )
  {
    //for this input type allow user to select from available storage classes in the repository
    available_storage_classes[i] = repository_sptr->get_all_storage_class_names(input_type_list[i]);

    //Multiple choice - with the list of options.
    io_dialog.choice(input_type_list[i].c_str(), available_storage_classes[i], input_choices[i]); 
  }

  //display dialog
  io_dialog.ask();

  // get the names of the user-select image and vsol2D storages
  this->set_image_name(available_storage_classes[0].at(input_choices[0]));
  this->set_vsol2d_name(available_storage_classes[1].at(input_choices[1]));

  // get input storage class
  storage_.vertical_cast(repository_sptr->get_data_by_name(this->image_name()));

  // set active tableau to the selected image
  bvis1_manager::instance()->active_selector()->set_active(this->image_name());
  bvis1_manager::instance()->active_selector()->active_to_top();

  bvis1_manager::instance()->active_selector()->set_active(this->vsol2d_name());
  bvis1_manager::instance()->active_selector()->active_to_top();

  bvis1_manager::instance()->active_selector()->set_active(this->image_name());

  this->set_tableau(bvis1_manager::instance()->active_tableau());

  return false;
}


vidpro1_image_storage_sptr dbdet_image_seg_tool::storage()
{
  return storage_;
}
