// This is breye/bvis/bvis_tool.h
#ifndef bvis_tool_h_
#define bvis_tool_h_
//:
// \file
// \brief To debug the scurve computations from shock edges 
// \author Mark Johnson (mrj)
// \date Thu Aug 14 13:39:20 EDT 2003
//
// \verbatim
//  Modifications
//
//    Amir Tamrakar 05/15/06       Added more functions for tool parameter changes
//    J.L. Mundy 03/24/07  Added inputs/outputs and split functionality
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_menu.h>
#include <bvis/bvis_view_tableau_sptr.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_storage_sptr.h>
class bvis_tool : public vbl_ref_count 
{
public:
  bvis_tool();
  virtual ~bvis_tool();
  //: Return the name of the tool
  virtual vcl_string name() const = 0;

  //: Handle events
  // \note This function is called once for most events \b before the event
  // is handled by the tableau under the mouse.  For draw and overlay events
  // this function is called once for each view in the grid
  virtual bool handle( const vgui_event& e, 
                       const bvis_view_tableau_sptr& view ) = 0;

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: Set the tableau associated with the current view
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau ) = 0;

  //: This is called when the tool is activated
  virtual void activate();

  //: This is called when the tool is deactivated
  virtual void deactivate();

  //: the active storage for the tool(corresponding tableau generates tool events)
  virtual bpro_storage_sptr active_storage() = 0;

  //: Clear the input vector and then resize the empty vector
  virtual void clear_input(int resize = -1);
  //: Clear the output vector and then resize the empty vector
  virtual void clear_output(int resize = -1);
  
  //: Set this vector of inputs for the indicated dataset index
  void set_input( const vcl_vector< bpro_storage_sptr >& input, 
                  const unsigned index = 0);
  //: Append the storage class to the input vector at the given frame
  void add_input( const bpro_storage_sptr& input,
                  const unsigned index = 0);

  //: Set this vector of outputs at the indicated dataset index
  void set_output( const vcl_vector< bpro_storage_sptr >& output,
                   const unsigned index = 0);
  //: Append the storage class to output vector at the dataset index
  void add_output( const bpro_storage_sptr& output,
                   const unsigned index = 0);

  //: Return the vector of output storage classes at given dataset index
  vcl_vector< bpro_storage_sptr > const& get_output(const unsigned index = 0);
  //: Return the vector of input storage class at the given frame
  vcl_vector< bpro_storage_sptr > const& get_input(const unsigned index = 0);


  //: Set the vector of names of the storage classes to be used as input
  void set_input_names(const vcl_vector<vcl_string>& names)
  { input_names_ = names;}
  //: Set the vector of names of the storage classes to be used as output
  void set_output_names(const vcl_vector<vcl_string>& names)
  { output_names_ = names;}
  //: Return the vector of names of the storage classes to be used as input
  vcl_vector< vcl_string > input_names() const
  { return input_names_;}
  //: Return the vector of names of the storage classes to be used as output
  vcl_vector< vcl_string > output_names() const
  { return output_names_;}

  //: Generate default output storage names 
  virtual void generate_output_names(){return;}

  //: Returns a vector of strings describing the storage class input types produced
  virtual vcl_vector< vcl_string > get_input_type() = 0;
  //: Returns a vector of strings describing the storage class output types produced
  virtual vcl_vector< vcl_string > get_output_type() = 0;

  //: The number of input data collections defined by input_types
  virtual unsigned int n_input_datasets() {return 1;}

  //: The number of output data collections defined by output_types
  virtual unsigned int n_output_datasets() {return 1;}

 protected:
  vcl_vector< vcl_vector< bpro_storage_sptr > >  input_data_;
  vcl_vector< vcl_vector< bpro_storage_sptr > >  output_data_;
  //: Names of the storage classes used as input
  vcl_vector< vcl_string > input_names_;
  //: Names of the storage classes used as output
  vcl_vector< vcl_string > output_names_;
};

//: Use in menus to toggle a parameter
void bvis_tool_toggle(const void* boolref);

//: Use in menus to toggle a parameter
void bvis_tool_inc(const void* intref);

//: Use in menus to toggle a parameter
void bvis_tool_dec(const void* intref);

//: Use in menus to set the value of a double parameter
void bvis_tool_set_param(const void* doubleref);

#endif //bvis_tool_h_
