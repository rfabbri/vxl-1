// This is breye/bvis/bvis_video_manager.cxx
//
// \file

#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_displayer.h>
#include <bvis/bvis_video_view_tableau.h>
#include <vidpro/vidpro_repository.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_displaybase_tableau.h>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <vcl_sstream.h>
#include <vul/vul_timer.h>

#include <bvis/bvis_gl_codec.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>

//:Isa added files
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidpro/storage/vidpro_ostream_storage.h>
#include <vidpro/storage/vidpro_ostream_storage_sptr.h>
//#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>

#include <vidl/vidl_pixel_format.h>
#include <vidl/vidl_convert.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>




//: Constructor
bvis_video_manager::bvis_video_manager()
: bvis_manager<bvis_video_manager>(),
process_manager_(*vidpro_process_manager::instance())
{
  is_playing_ = false;
  is_loop_playing_ = false;
  skip_frames_ =0;
  repository_ = process_manager_.get_repository_sptr();
  repository_->initialize();
}

//: Destructor
bvis_video_manager::~bvis_video_manager()
{
}

//: Access the process_manager

vidpro_process_manager *
bvis_video_manager::process_manager()
{
  return &process_manager_;
}

//: Access the process manager

vidpro_repository_sptr
bvis_video_manager::repository()
{
  return repository_;
}

void
bvis_video_manager::set_costum_window()
{
  vgui_dialog frames_dlg("Costum Window Size");
  vcl_stringstream prompt;
  prompt << "Enter window size ";
  int size = 0;
  frames_dlg.field(prompt.str().c_str(), size);
  if(!frames_dlg.ask())
    return;
  process_manager_.set_costum_window(size);
}


void 
bvis_video_manager::unset_costum_window()
{
  process_manager_.unset_costum_window();
}


void
bvis_video_manager::start_recording_macro()
{
  vcl_cout << "Recording Process Macro Started..." << vcl_endl;
  process_manager_.start_recording_macro();
  recording_macro_bool_ = true;
}


void 
bvis_video_manager::stop_recording_macro()
{
  vcl_cout << "Stopped Recording Process Macro." << vcl_endl;
  process_manager_.stop_recording_macro();
  recording_macro_bool_ = false;
}

//: Load the repository from a binary data file

void bvis_video_manager::load_repository()
{
  vgui_dialog load_rep_dlg("Load the repository from a file");
  static vcl_string file_name = "";
  static vcl_string ext = "*.rep";
  load_rep_dlg.file("File:", ext, file_name);
  if( !load_rep_dlg.ask())
    return;

  while (!repository_->replace_data(file_name))
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
}

//: Add in a new repository to the existing one
void bvis_video_manager::add_to_repository()
{
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
    retry.file("File:", ext ,file_name);
    retry.set_ok_button ("Retry");
    retry.set_cancel_button("Close");
    if(!retry.ask())
      return;
  }
  regenerate_all_tableaux();
  display_visible_data(true);
}

//: Save the repository from a binary data file
void bvis_video_manager::save_repository()
{
  vgui_dialog save_rep_dlg("Save the repository to a file");
  static vcl_string file_name = "";
  static vcl_string ext = "*.rep";
  static bool save_visible = false;
  save_rep_dlg.file("File:", ext, file_name);
  save_rep_dlg.checkbox("Save Visible Only",save_visible);

  if( !save_rep_dlg.ask())
    return;

  if(save_visible){
    vcl_set<vcl_string> names = this->visible_storage();
    repository_->remove_all_except(names);
    repository_->save_data(file_name);
  }
  else
    repository_->save_data(file_name);
}

//: Display the structure of the repository

void bvis_video_manager::view_repository()
{
  repository()->print_summary();
}



//: Handle events
bool 
bvis_video_manager::handle( const vgui_event & e )
{
  if(e.type == vgui_KEY_PRESS && e.modifier == vgui_MODIFIER_NULL){
    switch(e.key){
    case vgui_CURSOR_RIGHT:
      this->next_frame();
      break;
    case vgui_CURSOR_LEFT:
      this->prev_frame();
      break;
    case vgui_CURSOR_UP:
      this->first_frame();
      break;
    case vgui_CURSOR_DOWN:
      this->last_frame();
      break;
    default:
      break;
    }
  }
  return vgui_wrapper_tableau::handle(e);
}


void 
bvis_video_manager::regenerate_all_tableaux()
{
  tableau_map_.clear();

  if(!cache_tableau_)
    return;

  int curr_frame = repository_->current_frame();
  repository_->go_to_frame(0);
  // for each frame
  do{
    vcl_set<bpro_storage_sptr> all_storages = repository_->get_all_storage_classes();
    for (vcl_set<bpro_storage_sptr>::iterator asit = all_storages.begin();
      asit != all_storages.end(); asit++)
    {
      add_to_display(*asit);
    }
  }while( repository_->go_to_next_frame() );

  repository_->go_to_frame(curr_frame);
}
void bvis_video_manager::add_new_video_view(int offset, bool absolute)
{
  bvis_proc_selector_tableau_sptr selector_tab = bvis_proc_selector_tableau_new();
  bvis_view_tableau_sptr view_tab = new bvis_video_view_tableau(selector_tab, offset, absolute);
  view_tabs_.push_back( view_tab );
  grid_tab_->add_next(vgui_viewer2D_tableau_new(view_tab));
}

//: Create a new empty storage class
void 
bvis_video_manager::make_empty_storage()
{
  vgui_dialog make_storage_dlg("Make an empty storage class");
  vcl_vector<vcl_string> types;
  vcl_set<vcl_string> typeset = repository_->types();
  for ( vcl_set<vcl_string>::iterator itr = typeset.begin();
    itr != typeset.end();  ++itr )
    types.push_back(*itr);
  static unsigned int choice = 0;
  make_storage_dlg.choice("Type",types,choice);
  vcl_string name;
  make_storage_dlg.field("Name",name);
  bool all_frames = false;
  bool global = false;
  make_storage_dlg.checkbox("Create at all frames",all_frames);
  make_storage_dlg.checkbox("Create global storage",global);
  if( !make_storage_dlg.ask())
    return;

  if(global){
    bpro_storage_sptr new_data = repository_->new_global_data(types[choice],name); 
    if(new_data){
      regenerate_all_tableaux();
      display_current_frame();
    }
  }
  else if(all_frames){
    for(int i=0; i<repository_->num_frames(); ++i)
      repository_->new_data_at(types[choice],name,i); 
    regenerate_all_tableaux();
    display_current_frame();
  }
  else{
    bpro_storage_sptr new_data = repository_->new_data(types[choice],name);
    if(new_data){
      add_to_display(new_data);
      display_current_frame();
    }
  }
}


void 
bvis_video_manager::save_view_as_movie() const
{
  vgui_dialog save_rep_dlg("Save AVI");
  static int start = 1;
  static int end = bvis_video_manager::instance()->repository()->num_frames();
  static vcl_string file_name = "";
  static vcl_string ext = "*.avi";
  save_rep_dlg.file("File:", ext, file_name);
  save_rep_dlg.field("Start Frame:",start);
  save_rep_dlg.field("End Frame:",end);
  if( !save_rep_dlg.ask())
    return;

  vidl1_movie_sptr movie = new vidl1_movie(new vidl1_clip(new bvis_gl_codec(start,end)));
  vidl1_io::save(movie, file_name.c_str(), "AVI");
}

void
bvis_video_manager::set_istream(const vidl_istream_sptr  &vis )
{
  vistream_ = vis;
}



//: Play the video sequence
// Play the video from the current frame until the end
//  unless paused or stopped first and return to first frame
void bvis_video_manager::play_video()
{
  is_playing_ = true;

  while(is_playing_ )
  {
    if( !process_manager_.go_to_next_frame() )
      this->stop_video();
    display_current_frame();
  }
}




//: Play the video sequence
void
bvis_video_manager::loop_play_video()
{
  is_loop_playing_ = true;
  while(is_loop_playing_)
  {
    if( !this->next_frame() )
      this->first_frame();
    display_current_frame();
  }
  is_loop_playing_ = false;
}

//: Stop the video if it is playing
void 
bvis_video_manager::stop_video()
{
  is_playing_ = false;
  is_loop_playing_ =false;
  this->display_current_frame();
}



////: Process each frame in the video and display the results
//void
//bvis_video_manager::process_and_play_video()
//{
//
//    this->set_istream(process_manager_.get_istream()->get_istream());
//  if(!vistream_)
//   return;
//
//  vgui_dialog frames_dlg("Process and Play Video");
//  int first_frame = repository_->current_frame();   //make sure it is coordinated with the current frame of istream
//  vcl_stringstream prompt;
//  prompt << "Process from frame "<<first_frame+1<<" to ";
//  //int last_frame = repository_->num_frames();
//  int last_frame = 0;
//  frames_dlg.field(prompt.str().c_str(), last_frame);
//  frames_dlg.field("No of Frames to Skip", skip_frames_);
//  if(!frames_dlg.ask())
//    return;
//  last_frame--;
//
//  is_playing_ = true;
//
//  if (vistream_->is_seekable()) {
//    if (!vistream_->seek_frame(first_frame + skip_frames_))
//    {
//        vcl_cerr<<"ERROR: cannot skip the giving amount of frames\n";
//        return;
//    }
//    
//  }
//
// // while (is_playing_ && istream_->is_valid() &&
//  //     repository_->go_to_next_n_frame(skip_frames_) )
// 
//
//  for (int i = skip_frames_; i<=last_frame; i++)
//  {
//      if(!vistream_->advance())
//      {
//          is_playing_ = false;
//          break;
//      }
//
//      //rewind one frame to process from true starting position
//      vistream_-> seek_frame(vistream_->frame_number() -1);
//
//      //load inputs into the repository
//      //process_manager_.initialize_process_inputs();
//      vcl_set<bpro_storage_sptr> modified; 
//      process_manager_.run_process_queue_on_current_frame(&modified);
//      for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
//          itr != modified.end(); ++itr ) {
//              this->add_to_display(*itr);
//
//
//      }
//      
//      display_current_frame();
//  }
//
//
//  last_frame = vistream_->frame_number();
//  
//  vcl_cout << "finish frames " << first_frame+1 << " to " << last_frame+1 << vcl_endl;
//  // finish the process queue
//  vcl_set<bpro_storage_sptr> modified; 
//  process_manager_.finish_process_queue(first_frame,last_frame,&modified);
//
//  //close_ostream();
//
//  for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
//    itr != modified.end(); ++itr ) {
//      this->add_to_display(*itr);
//  }
//  
//
//  process_manager_.close_ostreams();
//
//  is_playing_ = false;
//  
//  display_current_frame();
//}

//: Process each frame in the video and display the results
void
bvis_video_manager::process_and_play_video()
{

  vgui_dialog frames_dlg("Process and Play Video");
  int first_frame = repository_->current_frame();   //make sure it is coordinated with the current frame of istream
  vcl_stringstream prompt;
  prompt << "Process from frame "<<first_frame+1<<" to ";
  //int last_frame = repository_->num_frames();
  int last_frame = 0;
  frames_dlg.field(prompt.str().c_str(), last_frame);
  //frames_dlg.field("No of Frames to Skip", skip_frames_);
  if(!frames_dlg.ask())
    return;
  //last_frame--;

  is_playing_ = true;
  for (int i = skip_frames_; i<last_frame; i++)
  {
    if(!process_manager_.go_to_next_frame())
      break;

    //load inputs into the repository
    //process_manager_.initialize_process_inputs();
    vcl_set<bpro_storage_sptr> modified; 
    process_manager_.run_process_queue_on_current_frame(&modified);
    for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
      itr != modified.end(); ++itr ) {
        this->add_to_display(*itr);

    }

    display_current_frame();
  }
  is_playing_ = false;

  vcl_cout << "finish frames " << first_frame+1 << " to " << last_frame << vcl_endl;

  // finish the process queue
  vcl_set<bpro_storage_sptr> modified; 
  process_manager_.finish_process_queue(first_frame, last_frame, &modified);

  for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
    itr != modified.end(); ++itr ) {
      this->add_to_display(*itr);
  }

  process_manager_.close_ostreams();
  display_current_frame();

}



//:Go to the next frame
bool bvis_video_manager::next_frame()
{
  process_manager_.go_to_next_frame();
  this->display_current_frame();
  return true;
}


//:If the video is not playing go to the previous frame
void bvis_video_manager::prev_frame()
{
  process_manager_.go_to_prev_frame();
  this->display_current_frame();
}

//: Jump to the first frame
void
bvis_video_manager::first_frame()
{
  vcl_vector<vcl_string> istream_names = repository_->get_all_storage_class_names("istream");
  vcl_vector<vcl_string>::iterator is_itr = istream_names.begin();
  for( ; is_itr != istream_names.end();is_itr++)
  {
    vidpro_istream_storage_sptr vis_sto;
    vis_sto.vertical_cast(repository_->get_data_by_name(*is_itr));
    vidl_istream_sptr vistream = vis_sto->get_istream();

    if (vistream && vistream->is_seekable()) 
    {
      vistream->seek_frame(0);
      this->redraw(vistream,*is_itr);
    }
  }
  this->display_current_frame();
}


//: Jump to the last frame
void
bvis_video_manager::last_frame()
{
  if( repository_->go_to_frame( repository_->num_frames() - 1 ) )
    display_current_frame();
}

//: Go to the specified frame
void bvis_video_manager::goto_frame()
{
  static unsigned frame = 0;
  vgui_dialog frame_dl("GoTo Frame");
  frame_dl.field("Frame Index",frame);
  if (!frame_dl.ask())
    return;
  if(repository_->go_to_frame( frame ))
    display_current_frame();
}

//: process the current frame only
void
bvis_video_manager::process_frame()
{
  vcl_set<bpro_storage_sptr> modified; 
  process_manager_.run_process_queue_on_current_frame(&modified);
  for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
    itr != modified.end(); ++itr ) {
      this->add_to_display(*itr);
  }
  display_current_frame();
}


//: Return the current frame number
int 
bvis_video_manager::current_frame()
{
  return repository_->current_frame();
}


//: Add a new frame
void
bvis_video_manager::add_new_frame()
{
  repository_->add_new_frame();
  //last_frame();
  //dudu: hack

  display_current_frame(true);
}


//: Adjust the tableau smart pointers for the current frame and refresh the display
void
bvis_video_manager::display_current_frame(bool clear_old) 
{
  if(!cache_tableau_){
    tableau_map_.clear();
  }

  // update each of the views
  for( vcl_vector<bvis_view_tableau_sptr>::iterator v_itr = view_tabs_.begin();
    v_itr != view_tabs_.end(); ++v_itr ){

      // determine which frame this view should use
      bvis_view_tableau_sptr vtab = *v_itr;
      int curr_frame = ((bvis_video_view_tableau*)vtab.ptr())->frame();

      if(!repository_->valid_frame(curr_frame)){
        if(clear_old)
          (*v_itr)->selector()->clear();
        continue;
      }

      // all storage classes at the current frame
      vcl_set<bpro_storage_sptr> storage_set = repository()->get_all_storage_classes(curr_frame);
      // all names of tableau attached to the current view
      const vcl_vector<vcl_string>& all_names = (*v_itr)->selector()->child_names();

      // Check each tableau currently drawn in this view.  For each, do one of the following
      // - remove it if there is no related storage class
      // - replace it with a cached tableau
      // - update it with the data of the storage class
      for ( vcl_vector<vcl_string>::const_iterator itr = all_names.begin();
        itr != all_names.end(); ++itr ){
          vgui_tableau_sptr tab = (*v_itr)->selector()->get_tableau(*itr);
          bpro_storage_sptr stg = repository()->get_data_by_name_at(*itr,curr_frame);
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
        //vcl_string name = (*s_itr)->name();
        vcl_string name; repository_->get_storage_name(*s_itr,name);
        vcl_string type; repository_->get_storage_type(*s_itr,type);
        /*  if (type == "istream") 
        continue;*/

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


  // display the frame number in the status bar
  vgui::out << "frame " << this->current_frame()+1 << " of " << repository_->num_frames()<< '\n';

  // redraw the display
  this->post_redraw();
  vgui::run_till_idle();
}

void bvis_video_manager::redraw()
{
  redraw(vistream_);
}

void bvis_video_manager::redraw(vidl_istream_sptr const&vistream, vcl_string const &istream_name)
{

  if (vistream) {
    unsigned int frame_num = vistream->frame_number();
    if (frame_num == unsigned(-1))
      vgui::out << "invalid frame\n";
    //else
    //    vgui::out << "frame["<< frame_num <<"]\n";

    static vil_image_view<vxl_byte> img;
    vidl_frame_sptr frame = vistream->current_frame();
    if(frame)
    {
      if(vidl_convert_to_view(*frame,img,VIDL_PIXEL_COLOR_RGB))
      {
        vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(img);
        // create the storage data structure
        vidpro_image_storage_sptr img_sto = vidpro_image_storage_new();
        img_sto->set_image( image_sptr );
        img_sto->set_name("image_from_istream");

        bpro_storage_sptr sto =img_sto;

        int cf = repository_->current_frame();

        brdb_query_aptr Q = brdb_query_comp_new("name", brdb_query::EQ, vcl_string(istream_name + "_unpro" ));
        brdb_selection_sptr s = DATABASE->select("image", Q);

        if(s->empty())
        {
          DATABASE->
            add_tuple("image", new brdb_tuple(cf+1,vcl_string(istream_name + "_unpro"),sto));
          repository_->go_to_next_frame();
        }
        s->update_selected_tuple_value("sptr",sto);
      }
      else
      {
        vcl_cerr<<"Warning at bvis_manager::redraw\n"
          <<"Cannot display requested frame\n";
      }
    }


    //DATABASE->print_database();

  }

}

bool
bvis_video_manager::resetskip()
{
  skip_frames_=1;
  return true;
}
bvis_view_tableau_sptr 
bvis_video_manager::view_tableau(bvis_proc_selector_tableau_sptr& selector)
{
  return new bvis_video_view_tableau(selector, 0, false);
}

