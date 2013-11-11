// This is breye/bvis/bvis_manager.txx
//
// \file

#include <bvis/bvis_manager.h>
#include <bvis/bvis_displayer.h>
#include <bvis/bvis_view_tableau.h>
#include <bvis/bvis_tool_manager.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_process.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_repository.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_displaybase_tableau.h>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <vcl_sstream.h>

#include <bvis/bvis_gl_codec.h>

#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>


template <class T> T* bvis_manager< T >::instance_ = 0; 
template <class T> vcl_map< vcl_string, bvis_displayer_sptr > bvis_manager< T >::displayers_;

//: Constructor
template <class T>
bvis_manager<T>::bvis_manager()
: vgui_wrapper_tableau(),
  recording_macro_bool_(false),
  cache_tableau_(false)
{
}

//: Insure only one instance is created
template <class T>
T* bvis_manager<T>::instance()
{
  if (!instance_){
    instance_ = new T();
    instance_->initialize();//generic setup
    instance_->configure();//specific to sub-class
  }
  return bvis_manager::instance_;
}

//: Initialize the instance
template <class T>
void bvis_manager<T>::initialize()
{
  grid_tab_ = vgui_grid_tableau_new(1,1);
  grid_tab_->set_frames_selectable(false);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_tab_);
  this->add_child(shell);
  this->add_new_view();
}


//: Create one view for each visible layer of the current frame
template <class T>
void bvis_manager<T>::layer_per_view()
{
  bvis_proc_selector_tableau_sptr selector_tab = active_selector();
  const vcl_vector< vcl_string >& names = selector_tab->child_names();
  vcl_vector<bvis_proc_selector_tableau_sptr> new_selectors;
  for(unsigned int i=0; i<names.size(); ++i){
    if(selector_tab->is_visible(names[i])){
      bvis_proc_selector_tableau_sptr new_selector_tab = bvis_proc_selector_tableau_new();
      for(unsigned int j=0; j<names.size(); ++j){
        new_selector_tab->add(selector_tab->get_tableau(names[j]), names[j]);
        if(i!=j){
          new_selector_tab->toggle(names[j]);
        }
      }
      new_selector_tab->set_active(names[i]);
      new_selectors.push_back(new_selector_tab);
    }
  }

  unsigned int num = new_selectors.size();
  unsigned int width = 0;
  while(width*width < num) ++width;
  unsigned int height = width;
  while((height-1)*width >= num) --height;
  while(grid_tab_->cols() < width) grid_tab_->add_column();
  while(grid_tab_->cols() > width) grid_tab_->remove_column();
  while(grid_tab_->rows() < height) grid_tab_->add_row();
  while(grid_tab_->rows() > height) grid_tab_->remove_row(); 
  view_tabs_.clear();
  grid_tab_ = vgui_grid_tableau_new(width,height);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_tab_);
  this->child.assign(shell);    

  for(unsigned int i=0; i<new_selectors.size(); ++i){
    bvis_view_tableau_sptr view_tab = instance_->view_tableau(new_selectors[i]);
    view_tabs_.push_back(view_tab);
    grid_tab_->add_next(vgui_viewer2D_tableau_new(view_tab)); 
  }

}


//: Remove the view in the active part of the grid
template <class T>
void bvis_manager<T>::remove_active_view()
{
  unsigned c,r;
  grid_tab_->get_last_selected_position(&c,&r);

  vgui_tableau_sptr ptab = grid_tab_->get_tableau_at(c,r);

  if (ptab != 0) {
    vgui_viewer2D_tableau_sptr rm_viewer;
    rm_viewer.vertical_cast( ptab );

    bvis_view_tableau_sptr rm_view;
    rm_view.vertical_cast( rm_viewer->child.child() );

    for( vcl_vector<bvis_view_tableau_sptr>::iterator v_itr = view_tabs_.begin();
      v_itr != view_tabs_.end(); ++v_itr){
        if( *v_itr == rm_view ){
          view_tabs_.erase(v_itr);
          grid_tab_->remove_at(c,r);
          break;
        }
    }
    grid_tab_->layout_grid2();
  }
}

//: Display info about the active tableau
template <class T>
void bvis_manager<T>::get_info_on_active_tableau()
{
  vgui_displaybase_tableau_sptr cur_active_tableau;
  cur_active_tableau.vertical_cast (active_tableau());

  if(!cur_active_tableau.ptr()) return;

  vcl_cout << vcl_endl << "***** Active Tableau Info: **********" << vcl_endl;
  vcl_cout << "TYPE: " << cur_active_tableau->type_name() << vcl_endl;

  //temp hack
  if (cur_active_tableau->type_name()!="bgui_vsol2D_tableau")
    return;

  vcl_vector< vcl_string > all_groups = cur_active_tableau->get_grouping_names();
  vcl_cout << "Groups: ";
  for (unsigned int i=0; i<all_groups.size(); i++)
    vcl_cout << all_groups[i] << ", ";

  vcl_cout << vcl_endl;
  vcl_cout << "*************************************" << vcl_endl;
}


//: Registers a storage type with the repository
//  Call this function with a null smart pointer of each type you plan to use
template <class T>
void bvis_manager<T>::register_displayer(bvis_displayer_sptr displayer)
{
  // map displayers by type name
  displayers_[displayer->type()] = displayer;
}




//: Access the selector tableau in the active view
template <class T>
bvis_proc_selector_tableau_sptr bvis_manager<T>::active_selector()
{
  unsigned c,r;
  grid_tab_->get_active_position(&c,&r);
  vgui_viewer2D_tableau_sptr active_viewer;
  active_viewer.vertical_cast( grid_tab_->get_tableau_at(c,r) );
  if(!active_viewer) return NULL;

  bvis_view_tableau_sptr active_view;
  active_view.vertical_cast( active_viewer->child.child() );
  if(!active_view) return NULL;

  bvis_proc_selector_tableau_sptr active_selector = active_view->selector();
  if(!active_selector) return NULL;

  return active_selector;
}

//: Access the active tableau (at the current frame)
template <class T>
vgui_tableau_sptr bvis_manager<T>::active_tableau()
{
  bvis_proc_selector_tableau_sptr active_selector = this->active_selector();
  if( !active_selector )
    return NULL;

  return active_selector->active_tableau();
}

//: Set the active tableau in the active view
template <class T>
void bvis_manager<T>::set_active_tableau(const vcl_string& name)
{
  bvis_proc_selector_tableau_sptr active_selector = this->active_selector();
  if(!active_selector)
    return;
  active_selector->set_active(name);
}

//: Find the names of the visible data in the active view
template <class T>
vcl_set<vcl_string> bvis_manager<T>::visible_storage()
{
  vcl_set<vcl_string> vis_names;
  bvis_proc_selector_tableau_sptr active_selector = this->active_selector();
  if( !active_selector )
    return vis_names;

  vcl_vector<vcl_string> all_names = active_selector->child_names();
  for( vcl_vector<vcl_string>::iterator itr = all_names.begin();
    itr != all_names.end(); ++itr )
  {
    if(active_selector->is_visible(*itr))
      vis_names.insert(*itr);
  }

  return vis_names;
}


//: Look up the storage class that created the given tableau
template <class T>
bpro_storage_sptr bvis_manager<T>::storage_from_tableau(const vgui_tableau_sptr& tab)
{
  vcl_map< bpro_storage*, vgui_tableau_sptr >::iterator itr;
  for( itr=tableau_map_.begin(); itr != tableau_map_.end(); ++itr ){
    if (itr->second == tab) break;
  }
  if( itr != tableau_map_.end() )
    return bpro_storage_sptr(itr->first);
  return NULL;
}

template <class T>  
bvis_tool_sptr bvis_manager<T>::active_tool() const
{
  return   bvis_tool_manager<T>::instance()->active_tool();
}

//: Set the current active tool
//  This function is reponsible to make sure that the tool that
//  was just activated has the right tableau and storage class
//  assigned to it
template <class T>
void bvis_manager<T>::set_active_tool(bvis_tool_sptr tool)
{
  bvis_tool_manager<T>::instance()->set_active_tool(tool);
}

template <class T>
bool bvis_manager<T>::recording_macro()
{
  return recording_macro_bool_;
}

template <class T>
void bvis_manager<T>::start_recording_macro()
{
  vcl_cout << "Recording Process Macro Started..." << vcl_endl;
  recording_macro_bool_ = true;
}

template <class T>
void bvis_manager<T>::stop_recording_macro()
{
  vcl_cout << "Stopped Recording Process Macro." << vcl_endl;
  recording_macro_bool_ = false;
}

//: Quit the application
template <class T>
void bvis_manager<T>::quit()
{
  vgui_dialog quit_dl("Quit");
  quit_dl.message(" ");
  quit_dl.message("   Are you sure you want to quit bvis2?   ");
  quit_dl.message(" ");

  if (quit_dl.ask())
    vgui::quit();
}

//: Toggle caching of tableaux
template <class T>
void bvis_manager<T>::toggle_tableau_cache()
{
  cache_tableau_ = !cache_tableau_;
  tableau_map_.clear();
  vgui::out << "Tableau Caching ";
  if(cache_tableau_)
    vgui::out << "Enabled\n";
  else
    vgui::out << "Disabled\n";
}


//: Calls the appropriate type of displayer to make a tableau
template <class T>
vgui_tableau_sptr bvis_manager<T>::make_tableau(const bpro_storage_sptr& storage, bool& cacheable ) const
{
  vcl_map< vcl_string , bvis_displayer_sptr >::const_iterator itr;
  itr = displayers_.find(storage->type());
  if( itr != displayers_.end() ){
    cacheable = itr->second->cacheable();
    return itr->second->make_tableau( storage );
  }
  cacheable = false;
  return NULL;
}


//: Calls the appropriate type of displayer to update a tableau
template <class T>
bool bvis_manager<T>::update_tableau(const vgui_tableau_sptr& tableau, 
                              const bpro_storage_sptr& storage ) const
{
  vcl_map< vcl_string , bvis_displayer_sptr >::const_iterator itr;
  itr = displayers_.find(storage->type());
  if( itr != displayers_.end() )
    return itr->second->update_tableau( tableau, storage );
  return false;
}

//: Check if a storage class is stored on the database
static bool is_stored(const bpro_storage_sptr& storage)
{
  vcl_set<vcl_string> types = bpro_storage_registry::types();
  brdb_query_aptr Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
  brdb_selection_sptr selec = DATABASE->select("global_data", Q);
  if (!selec->empty()){
    return true;
  }

  for ( vcl_set<vcl_string>::iterator type_itr = types.begin(); 
    type_itr != types.end();  ++type_itr){
      Q = brdb_query_comp_new("sptr", brdb_query::EQ, storage);
      selec = DATABASE->select(*type_itr, Q);
      if (!selec->empty()){
        return true;
      }
  }
  // Storage not at the repository!
  return false;
}

//: Create a tableau for storage class and cache it
template <class T>
bool bvis_manager<T>::add_to_display(const bpro_storage_sptr& storage)
{
  if(!storage.ptr() || !is_stored(storage))
    return false;

  if(cache_tableau_){
    return true;
  }

  vcl_string name = storage->name();

  bool cacheable = false;
  vgui_tableau_sptr new_tab = this->make_tableau( storage, cacheable );
  if (!new_tab.ptr())
    return false;

  if(cacheable)
    tableau_map_[storage.ptr()] = new_tab;
  return true; 
}

template <class T>
bvis_displayer_sptr bvis_manager<T>::displayer(  vcl_string const& type )
{
  return displayers_[type];
}
template <class T>

bool bvis_manager<T>::
parameter_dialog(vcl_vector< bpro_param* >& param_vector, vgui_dialog& param_dialog)
{
    if(param_vector.empty())
      return false;

    for( vcl_vector< bpro_param* >::iterator it = param_vector.begin();
        it != param_vector.end();  ++it ) {

            if( bpro_param_type<int> * param = dynamic_cast<bpro_param_type<int> *>(*it) ) {
                param_dialog.field( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_choice_param_type * param = dynamic_cast<bpro_choice_param_type *>(*it) ) {
                param_dialog.choice( param->description().c_str() , param->choices(), param->temp_ref() );
            }
            else if( bpro_param_type<unsigned int> * param = dynamic_cast<bpro_param_type<unsigned int> *>(*it) ) {
                param_dialog.field( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_param_type<float> * param = dynamic_cast<bpro_param_type<float> *>(*it) ) {
                param_dialog.field( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_param_type<double> * param = dynamic_cast<bpro_param_type<double> *>(*it) ) {
                param_dialog.field( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_param_type<vcl_string> * param = dynamic_cast<bpro_param_type<vcl_string> *>(*it) ) {
                param_dialog.field( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_param_type<bool> * param = dynamic_cast<bpro_param_type<bool> *>(*it) ) {
                param_dialog.checkbox( param->description().c_str() , param->temp_ref() );
            }
            else if( bpro_param_type<bpro_filepath> * param = dynamic_cast<bpro_param_type<bpro_filepath> *>(*it) ) {
                param_dialog.file( param->description().c_str(), param->temp_ref().ext, param->temp_ref().path );
            }
            else{
                vcl_cerr << "No valid dialog interface for parameter: " << (*it)->name() << vcl_endl;
            }
    }
    return true;
}
#undef BVIS2_MANAGER_INSTANTIATE
#define BVIS2_MANAGER_INSTANTIATE(T) \
template class bvis_manager< T >; 
