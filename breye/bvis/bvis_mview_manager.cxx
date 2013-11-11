// This is breye/bvis/bvis_mview_manager.cxx
//
// \file

#include <bvis/bvis_mview_manager.h>
#include <bvis/bvis_displayer.h>
#include <bvis/bvis_mview_view_tableau.h>
#include <vidpro/vidpro_repository.h>
#include <bpro/bpro_mview_dbutils.h>
#include <bpro/bpro_storage_registry.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_displaybase_tableau.h>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <vcl_sstream.h>
#include <vul/vul_timer.h>

#include <bvis/bvis_gl_codec.h>

//:Isa added files
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>
#include <bpro/bpro_mview_process_manager.h>
#include <brdb/brdb_database_manager.h>
//: Constructor
bvis_mview_manager::bvis_mview_manager()
  : bvis_manager<bvis_mview_manager>()
{
  //Initialize process manager
  bpro_mview_process_manager::instance();
}
//: Manager setup particular to this sub-class
void bvis_mview_manager::configure()
{

}
//: Destructor
bvis_mview_manager::~bvis_mview_manager()
{
}


//: Access the process manager

vidpro_repository_sptr
bvis_mview_manager::repository()
{
  return 0;
}


//: Load the database

void bvis_mview_manager::load_repository()
{
  vgui_dialog load_rep_dlg("Load the database from a file");
  static vcl_string file_name = "";
  static vcl_string ext = "*.rep";
  load_rep_dlg.file("File:", ext, file_name);
  if( !load_rep_dlg.ask())
    return;
  brdb_database_manager::instance()->clear();
  brdb_database_manager::load_database(file_name);
  regenerate_all_tableaux();
  display_visible_data(true);
}

//: Add in a new repository to the existing one
void bvis_mview_manager::add_to_repository()
{
#if 0
  vgui_dialog add_to_rep_dlg("Add to the repository from a file");
  static vcl_string file_name = "";
  static vcl_string ext = "*.rep";
  add_to_rep_dlg.file("File:", ext, file_name);
  if( !add_to_rep_dlg.ask())
    return;

  while (!repository_->add_repository(file_name))
  {
    vgui_dialog retry("ERROR: the file chosen is not supported!");
    file_name.clear();
    retry.file("File:", ext, file_name);
    retry.set_ok_button ("Retry");
    retry.set_cancel_button("Close");
    if(!retry.ask())
      return;
  }
  regenerate_all_tableaux();
  display_visible_data(true);
#endif
}

//: Save the repository from a binary data file
void bvis_mview_manager::save_repository()
{
  vgui_dialog save_rep_dlg("Save the database to a file");
  static vcl_string file_name = "";
  static vcl_string ext = "*.rep";
  static bool save_visible = false;
  save_rep_dlg.file("File:", ext, file_name);
  save_rep_dlg.checkbox("Save Visible Only",save_visible);

  if( !save_rep_dlg.ask())
    return;
  brdb_database_manager::save_database(file_name);

}

//: Display the structure of the database
void bvis_mview_manager::view_repository()
{
  vcl_cout << "Direct Database Usage -\n";
 brdb_database_manager::instance()->print();
}



//: Handle events
bool 
bvis_mview_manager::handle( const vgui_event & e )
{
  return vgui_wrapper_tableau::handle(e);
}


void 
bvis_mview_manager::regenerate_all_tableaux()
{
  tableau_map_.clear();

  if(!cache_tableau_)
    return;
  vcl_set<bpro_storage_sptr> all_storages = 
    bpro_mview_dbutils::get_all_storage_classes();
  for (vcl_set<bpro_storage_sptr>::iterator asit = all_storages.begin();
       asit != all_storages.end(); asit++)
    add_to_display(*asit);
}

void bvis_mview_manager::add_new_mview_view()
{
  bvis_proc_selector_tableau_sptr selector_tab = bvis_proc_selector_tableau_new();

  bvis_view_tableau_sptr view_tab = new bvis_mview_view_tableau(selector_tab);
  view_tabs_.push_back( view_tab );
  grid_tab_->add_next(vgui_viewer2D_tableau_new(view_tab));
}
//: add a new camera view
void bvis_mview_manager::add_new_camera_view()
{
  this->add_new_mview_view();
  //get last view 
  bvis_view_tableau_sptr vt = view_tabs_[view_tabs_.size()-1];
  //we know these are mview_view_tabs
  bvis_mview_view_tableau* mvt = (bvis_mview_view_tableau*)(vt.ptr());
  vcl_set<unsigned> vids = bpro_mview_dbutils::existing_view_ids();
  unsigned max_vid = 0;
  for(vcl_set<unsigned>::iterator vit = vids.begin();
      vit != vids.end(); ++vit)
    if(*vit > max_vid)
      max_vid = *vit;
  unsigned new_view_id = max_vid+1;
  mvt->set_camera_view_id(new_view_id);
  vt->selector()->enable_processing();
  bpro_mview_dbutils::add_view_id(new_view_id);
}

void bvis_mview_manager::remove_selected_camera_view()
{
  
}
//: Adjust the tableau smart pointers for the current frame and refresh the display
void
bvis_mview_manager::display_visible_data(bool clear_old) 
{
  // update each of the views
  for( vcl_vector<bvis_view_tableau_sptr>::iterator v_itr = view_tabs_.begin();        v_itr != view_tabs_.end(); ++v_itr ){
    if(!(*v_itr))
      continue;
    //we know these are mview_view_tabs
    bvis_mview_view_tableau* mvt = (bvis_mview_view_tableau*)((*v_itr).ptr());
    unsigned view_id = mvt->camera_view_id();
    // all storage classes in the current view
    vcl_set<bpro_storage_sptr> storage_set = 
      bpro_mview_dbutils::get_all_storage_classes(view_id);
    // all names of tableau attached to the current view
    const vcl_vector<vcl_string>& all_names = (*v_itr)->selector()->child_names();

    // Check each tableau currently drawn in this view.  For each, do one of the following
    // - remove it if there is no related storage class
    // - replace it with a cached tableau
    // - update it with the data of the storage class
    for ( vcl_vector<vcl_string>::const_iterator itr = all_names.begin();
          itr != all_names.end(); ++itr ){
      vgui_tableau_sptr tab = (*v_itr)->selector()->get_tableau(*itr);
      bpro_storage_sptr stg = 
        bpro_mview_dbutils::get_data_by_name_at(*itr, view_id);
      if(!stg){
        if(clear_old)
          (*v_itr)->selector()->remove(*itr--);
        else
          (*v_itr)->selector()->add(NULL, *itr);
        continue;
      }
      // check the cache
      if(cache_tableau_){
        typedef vcl_map< bpro_storage*, vgui_tableau_sptr >::iterator m_itr_t;
        m_itr_t search_itr = tableau_map_.find(stg.ptr());
        if(search_itr != tableau_map_.end()){
          storage_set.erase(stg);
          (*v_itr)->selector()->add(search_itr->second, *itr);
          continue;
        }
      }
      else{
        // try to update an existing tableau;
        if(update_tableau(tab,stg)){
          storage_set.erase(stg);
          tableau_map_[stg.ptr()] = tab;
        }
      }
    }
    // create any missing tableau from the storage
    for(vcl_set<bpro_storage_sptr>::iterator s_itr = storage_set.begin();
        s_itr != storage_set.end();  ++s_itr)
      {
        vcl_string name = (*s_itr)->name();

        // only create a new tableau if it hasn't already been created
        typedef vcl_map< bpro_storage*, vgui_tableau_sptr >::iterator m_itr_t;
        m_itr_t search_itr = tableau_map_.find(s_itr->ptr());
        if(search_itr != tableau_map_.end()){
          (*v_itr)->selector()->add(search_itr->second, name);
        }
        else{
          bool cacheable = false;
          vgui_tableau_sptr tab = this->make_tableau( *s_itr, cacheable );
          if (tab){
            if(cacheable)
              tableau_map_[s_itr->ptr()] = tab;
            (*v_itr)->selector()->add(tab, name);
          }
        }
      }
  }

  // redraw the display
  this->post_redraw();
  vgui::run_till_idle();
}


//: Create a new empty storage class at the selected view_id
void 
bvis_mview_manager::make_empty_storage()
{
  vgui_dialog make_storage_dlg("Make an empty storage class");
  vcl_vector<vcl_string> types;
  vcl_set<vcl_string> typeset = bpro_storage_registry::types();
  for ( vcl_set<vcl_string>::iterator itr = typeset.begin();
        itr != typeset.end();  ++itr )
    types.push_back(*itr);
  static unsigned int choice = 0;
  make_storage_dlg.choice("Type",types,choice);
  vcl_string name;
  make_storage_dlg.field("Name",name);
  if( !make_storage_dlg.ask())
    return;
  bpro_storage_sptr new_data = 
    bpro_mview_dbutils::new_data_at(types[choice],name, 0);//HARD wired JLM
  if(new_data){
    add_to_display(new_data);
    display_visible_data();
  }
}

bvis_view_tableau_sptr 
bvis_mview_manager::view_tableau(bvis_proc_selector_tableau_sptr& selector)
{
  return new bvis_mview_view_tableau(selector);
}


