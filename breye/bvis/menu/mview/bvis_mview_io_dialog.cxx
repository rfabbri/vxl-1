//This is bvis/bvis_mview_io_dialog.cxx
#include <vcl_set.h>
#include <vcl_algorithm.h>
#include "bvis_mview_io_dialog.h"
#include <vgui/vgui_dialog.h>
#include <bvis/bvis_video_manager.h>
#include <bpro/bpro_mview_dbutils.h>

bvis_mview_io_dialog::bvis_mview_io_dialog()
{
}
bvis_mview_io_dialog::~bvis_mview_io_dialog()
{
}
static vcl_string 
generate_new_name(vcl_string const& type,
                  vcl_vector < vcl_string >const& existing_names)
{
        //generate a default name that can be changed if desired
  vcl_string suggested_name = type;
  vcl_string name_str;
  int c = 0;
  bool not_unique = true;
  do{
    vcl_stringstream name_stream;
    name_stream << suggested_name << c++;
    name_str = name_stream.str();
    vcl_vector < vcl_string >::const_iterator sit;
    sit = vcl_find(existing_names.begin(), existing_names.end(), name_str);
    not_unique = sit!=existing_names.end();
  }
  while(not_unique);
  return name_str;
}
bool bvis_mview_io_dialog::
input_dialog(vcl_vector< vcl_string > const& input_type_list,
             vcl_vector< vcl_string >& input_names)
{
  if(input_type_list.empty())
    return false;
  //Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select Inputs" );
  //display input options
    
  io_dialog.message("Select Input(s) From Available ones:");
  io_dialog.message("Camera View Data Inputs:");
  //A list of all camera view ids in the database
  vcl_set<unsigned> vids = bpro_mview_dbutils::existing_view_ids();
  //the vector of selected name positions
  vcl_vector<int> input_choices(input_type_list.size());      
  input_names.resize(input_type_list.size());      
  //choices by view id
  vcl_vector<vcl_vector<vcl_string> >choice_names(input_type_list.size());
  //Iterate through all requested types
  unsigned input_no = 0;
  for(vcl_vector< vcl_string >::const_iterator tyi=input_type_list.begin();
      tyi != input_type_list.end(); ++tyi, ++input_no)
    {
      vcl_stringstream inpt;
      inpt << input_no;
      vcl_string stype = "Input(" + inpt.str() + ") <:> Type: "+ *tyi;
      io_dialog.message(stype.c_str());
      vcl_vector<vcl_string> choice_display;
      for(vcl_set<unsigned>::iterator vit = vids.begin();
          vit != vids.end(); ++vit)
      {
        //get the names of a particular type
        vcl_vector < vcl_string >  potential_inputs = 
          bpro_mview_dbutils::get_all_storage_class_names(*tyi,*vit);
        vcl_stringstream temp;
        temp << *vit;
        //copy into the choice vector
        for(vcl_vector < vcl_string >::iterator nit =
              potential_inputs.begin(); nit != potential_inputs.end();
            ++ nit)
          {
            choice_names[input_no].push_back(*nit);
            //change the names to reflect view id
            vcl_string display = "view_id: " + temp.str() + " - " + *nit;
            choice_display.push_back(display);
          }
      }
      //get additonal choices from global storage
      vcl_vector < vcl_string > pot_global_inputs =
        bpro_mview_dbutils::get_global_storage_names(*tyi);
      for(vcl_vector < vcl_string >::iterator sit = pot_global_inputs.begin();
          sit != pot_global_inputs.end(); ++sit)
        {
          choice_names[input_no].push_back(*sit);
          vcl_string display = "global " + *sit;
          choice_display.push_back(display);
        }
      //insert the choice in the menu
      io_dialog.choice((*tyi).c_str(), choice_display,
                       input_choices[input_no]);
    }
  if (!io_dialog.ask())
    return false;
  //extract the choices
  unsigned index = 0;
  for(vcl_vector<int>::iterator cit = input_choices.begin();
      cit != input_choices.end(); ++cit, ++index)
    input_names[index]=choice_names[index][*cit];
  return true;
}
bool bvis_mview_io_dialog::
output_dialog(vcl_vector< vcl_string > const& output_type_list,
              vcl_vector< vcl_string > const& output_suggested_names,
              vcl_vector< unsigned >& output_view_ids,
              vcl_vector< vcl_string >& output_names)
{            
  if(output_type_list.empty())
    return false;
  vgui_dialog io_dialog("Select Outputs");
  output_names.resize(output_type_list.size());
  
  //A list of all camera view ids in the database
  vcl_set<unsigned> vids = bpro_mview_dbutils::existing_view_ids();

  output_view_ids.resize(output_type_list.size()); 
  //The list of view_id choices as a vector
  vcl_vector<unsigned> id_choice_list;
  //The user choices
  vcl_vector<int> id_choices(output_type_list.size());

  //Generate a string display for the view_ids
  vcl_vector<vcl_string> view_id_display;
  for(vcl_set<unsigned>::iterator vit = vids.begin(); vit!=vids.end(); ++vit)
    {
      id_choice_list.push_back(*vit);
      vcl_stringstream temp;
      temp << *vit;
   vcl_string disp = "view_id : " + temp.str();
      view_id_display.push_back(disp.c_str());
    }
  view_id_display.push_back("Global Storage");
  io_dialog.message("Select View Ids for Outputs:");
  //store the choices

  //request view ids for each output
  //Iterate through the requested outputs
  unsigned output_no = 0;
  for(vcl_vector< vcl_string >::const_iterator tyi=output_type_list.begin();
      tyi != output_type_list.end(); ++tyi, ++output_no)
    {
      vcl_stringstream outpt;
      outpt << output_no;
      vcl_string stype = 
        "Select Output(" + outpt.str() + ") Type: " + *tyi;
      io_dialog.message(stype.c_str());
      io_dialog.choice("Output Location", view_id_display,
                       id_choices[output_no]);
    }

  if (!io_dialog.ask())
    return false;
  //extract the choices
  unsigned n_choices = id_choices.size();
  if(vids.size())
    for(unsigned i = 0; i<n_choices; ++i)
      output_view_ids[i]=id_choice_list[id_choices[i]];
  //Make up names for the storage of each output
  output_no = 0;
  for(vcl_vector< vcl_string >::const_iterator tyi=output_type_list.begin();
      tyi != output_type_list.end(); ++tyi, ++output_no)
    {
      vcl_vector < vcl_string >  existing_names = 
        bpro_mview_dbutils::get_all_storage_class_names(*tyi,output_view_ids[output_no]);
      vcl_string new_name = generate_new_name(*tyi, existing_names);
      output_names[output_no]=new_name;
    }
  return true;
}
