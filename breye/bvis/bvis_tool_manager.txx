// This is breye/bvis/bvis_tool_manager.cxx
//
// \file

#include <bvis/bvis_tool_manager.h>
#include <bvis/bvis_displayer.h>
#include <bvis/bvis_view_tableau.h>
#include <bpro/bpro_storage.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_repository.h>
#include <vidpro/vidpro_repository_sptr.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_displaybase_tableau.h>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <vcl_sstream.h>

#include <bvis/bvis_gl_codec.h>


//: Constructor
template <class T>
bvis_tool_manager<T>::bvis_tool_manager()
  : repository_(NULL),
    active_tool_(NULL)
{
}

//: Destructor
template <class T>
bvis_tool_manager<T>::~bvis_tool_manager()
{
}

//: Insure only one instance is created
template <class T>
bvis_tool_manager<T> *bvis_tool_manager<T>::instance()
{
  if (!instance_){
    instance_ = new bvis_tool_manager<T>();
    instance_->initialize();
  }
  return bvis_tool_manager<T>::instance_;
}


//: Initialize the instance
template <class T>
void bvis_tool_manager<T>::initialize()
{
  vis_manager_instance_ = bvis_manager<T>::instance();
  repository_ = vis_manager_instance_->repository();
}


//: Access the process manager
template <class T>
vidpro_repository_sptr bvis_tool_manager<T>::repository()
{
  return repository_;
}

template <class T>
void bvis_tool_manager<T>::assign_input()
{
  active_tool_->clear_input();
  vcl_vector< vcl_string > input_type_list = active_tool_->get_input_type();
  vcl_vector< vcl_string > input_names = active_tool_->input_names();

  for( unsigned int i = 0 ;
       i < input_names.size();
       i++ ) {
    //use the storage names in the tool that 
    //maps to the exact input to be used
    bpro_storage_sptr input_storage_sptr = 
      repository_->get_data_by_name( input_names[i]);
    active_tool_->add_input(input_storage_sptr);
  }
}

//We don't expect the tool to create the storage classes itself
template <class T>
void bvis_tool_manager<T>::assign_output()
{
  active_tool_->clear_output();
  vcl_vector< vcl_string > output_types = active_tool_->get_output_type();
  vcl_vector< vcl_string > output_names = active_tool_->output_names();
  for( unsigned int i = 0 ;
       i < output_types.size();
       i++ ) {
    if(i<output_names.size()){
      bpro_storage_sptr output_storage = 
        repository_->get_data_by_name( output_names[i]);
      if(!output_storage)
        output_storage=repository_->new_data(output_types[i], output_names[i]);
      if(output_storage)
        active_tool_->add_output(output_storage);
    }
  }
}
template <class T>
void bvis_tool_manager<T>::set_active_tableau()
{
  if(!active_tool_)
    return;
  bpro_storage_sptr storage = active_tool_->active_storage();
  if(storage) 
    {
      bool cacheable = false;
      vgui_tableau_sptr tab=
        vis_manager_instance_->make_tableau(storage, cacheable);
      active_tool_->set_tableau(tab);
      vis_manager_instance_->add_to_display(storage);
      vis_manager_instance_->display_visible_data();
      vis_manager_instance_->set_active_tableau(storage->name());
    }
}
//: Set the current active tool
//  This function is reponsible to make sure that the tool that
//  was just activated has the right tableau and storage class
//  assigned to it
template <class T>
void bvis_tool_manager<T>::set_active_tool(bvis_tool_sptr tool)
{
  if( active_tool_ )
    active_tool_->deactivate();

  active_tool_ = tool;

  if( active_tool_ )
    {
      //Names may be set by menu as well
      if(!active_tool_->output_names().size())
        active_tool_->generate_output_names();
      this->assign_input();
      this->assign_output();
      this->set_active_tableau();
      active_tool_->activate();
    }
}
template <class T>
void bvis_tool_manager<T>::deactivate_tool()
{
  if( active_tool_ )
    active_tool_->deactivate();
  active_tool_=0;
}

#undef BVIS2_TOOL_MANAGER_INSTANTIATE
#define BVIS2_TOOL_MANAGER_INSTANTIATE(T) \
template class bvis_tool_manager<T >; \
template <> bvis_tool_manager<T>* bvis_tool_manager<T>::instance_ = 0 


