// This is breye/bvis/bvis_video_manager.h
#ifndef bvis_video_manager_h_
#define bvis_video_manager_h_

//:
// \file
// \brief This file defines a class that acts as a gui manager.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/30/03
//
// \verbatim
//  Modifications
//   30-OCT-2003  Matt Leotta - Initial version.
//   19-MAR-2007  Eduardo Almeida - This encloses the bvis_manager video methods.
//   15-APR-2007  Eduardo Almeida - Made changes to support database design.
// \endverbatim


#include <bvis/bvis_manager.h>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_ostream.h>
#include <vidl/vidl_ostream_sptr.h>

class bvis_video_manager : public bvis_manager<bvis_video_manager> 
{

public:
  
  //: Access the process manager
  vidpro_process_manager * process_manager();
 
  //-------------------------------------------------------------
  //: Access the repository
  vidpro_repository_sptr repository();

  //: Load the repository from a binary data file
  void load_repository();

  //: Save the repository from a binary data file
  void save_repository();

  //: Create a new empty storage class
  virtual void make_empty_storage();

  //: Add additional storage items to the repository from a binary data file
  void add_to_repository();

  //: Display the structure of the repository
  void view_repository();

  virtual void add_new_view(){this->add_new_video_view();}

  void add_new_video_view(int offset = 0, bool absolute = false);

  //: Handle events
  bool handle( const vgui_event & e );

  //: Regenerate all tableau from the repository
  void regenerate_all_tableaux();

  //: Save OpenGL render areas as a movie
  void save_view_as_movie() const;

  //: Play the video sequence
  void play_video();

  //: Loop and Play the video sequence
  void loop_play_video();

  //: Stop the video if it is playing and return to initial frame
  void stop_video();

  //: Pause the video if it is playing
  void pause_video();

  //: Process each frame in the video and display the results
  void process_and_play_video();

    //: Process each frame in the video and display the results
//  void process_play_and_close();

  //: Go to the next frame
  bool next_frame();

  //: Go to the previous frame
  void prev_frame();

  //: Jump to the first frame
  void first_frame();

  //: Jump to the last frame
  void last_frame();

  //: Go to the specified frame
  void goto_frame();

  //: process the current frame only
  void process_frame();

  //--------------------------------------------------------------

  //: Add a new frame
  void add_new_frame();

  //: Return the current frame number
  int current_frame();

  //: Adjust the tableau smart pointers for the current frame and refresh the display
  virtual void display_visible_data(bool clear_old = false)
    {this->display_current_frame(clear_old);}

  void display_current_frame(bool clear_old = false);

  void redraw(const vidl_istream_sptr &vistream, vcl_string const &istream_name="");
  void redraw();

  void start_recording_macro();
  void stop_recording_macro();

  void set_costum_window();
  void unset_costum_window();

  void set_istream(const vidl_istream_sptr &vis );

  // reset the skipping frame
  bool resetskip();

  friend class bvis_manager<bvis_video_manager>;

private:
  //: Constructor
  bvis_video_manager();
  //: Destructor
  ~bvis_video_manager();

  virtual bvis_view_tableau_sptr view_tableau(bvis_proc_selector_tableau_sptr& selector);
  //: The process_manager
  vidpro_process_manager process_manager_; 

  //: A smart pointer to the repository
  vidpro_repository_sptr repository_;

  //: Is the manager playing the video
  bool is_playing_;

  //: Is the manager playing the video in a loop
  bool is_loop_playing_;

  //: varibale to skip_frames when playing
  int skip_frames_;

  //: variable to hold the video input stream
  vidl_istream_sptr vistream_;

  //: variable to hold the video output stream
  vidl_ostream_sptr vostream_;

  //: variable to delay frames when playing a video
  float time_interval_;

};

#endif // bvis_video_manager_h_
