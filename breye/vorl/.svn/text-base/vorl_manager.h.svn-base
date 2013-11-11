// This is breye/vorl/vorl_manager.h
#ifndef vorl_manager_h_
#define vorl_manager_h_
//:
// \file
// \brief Manager for creating VORL compatible executables from vidpro processes
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/17/04
//
// \verbatim
//  Modifications
//  July-05-2005 Ozge Can Ozcanli added methods and capabilities to work on "objects"
//               load_video_clip
// \endverbatim

#include <vcl_string.h>
#include <vcl_utility.h>
#include <vul/vul_arg.h>
#include <vidpro/vidpro_repository_sptr.h>
#include <bpro/bpro_process_sptr.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/vidpro_process_manager.h>


//: Manager for creating VORL compatible executables from vidpro processes
class vorl_manager
{
 public:
  //: Destructor
  ~vorl_manager(){}
  
  //: Access the static instance of the tableau manager
  static vorl_manager * instance();

  //: Return the repository
  vidpro_repository_sptr repository() const 
    { return vidpro_process_manager::instance()->get_repository_sptr(); }

  //: Set the input repository to be the current repository of process_manager_
  void set_repository(const vidpro_repository_sptr& rep) 
    { vidpro_process_manager::instance()->set_repository(rep); }

  //: Set the name for this application
  void set_app_name(const vcl_string& name) { app_name_ = name; }

  //: Add the process to the processing queue
  void add_process_to_queue(const bpro_process_sptr& process);

  //: Remove the last process from the queue
  void delete_last_process();

  //: Remove all processes from the queue
  void clear_process_queue();

  //: Add the parameters of process to the program arguments list
  void add_process_to_args(const bpro_process_sptr& process);

  //: save the repository binary
  // \note if \p names are provided, save only storage classes with matching names
  bool save_repository( const vcl_string& filename, 
                        const vcl_set<vcl_string>& names = vcl_set<vcl_string>());

  //: Parse the command line parameters
  void parse_params(int argc, char** argv);

  //: Load the input video
  void load_video();

  //: Load the input video from start frame to end frame
  void load_video_clip();

  //: Runs the process on the current frame
  // \param process specific the process and related data to run
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool run_process_on_current_frame( const bpro_process_sptr& process,
                                     vcl_set<bpro_storage_sptr>* modified = NULL );

  //: Runs the process queue on the current frame
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool run_process_queue_on_current_frame( vcl_set<bpro_storage_sptr>* modified = NULL );

  //: Calls the finish function on all processes in the queue
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool finish_process_queue( vcl_set<bpro_storage_sptr>* modified = NULL );

  //: Calls the finish function on all processes in the queue on a range of frames
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool finish_process_queue( int first, int last, vcl_set<bpro_storage_sptr>* modified = NULL );

  //: Return the current frame number
  unsigned int current_frame() const;

  //: Step to the next frame
  // \retval true if at next frame
  // \retval false if there were no more frames
  bool next_frame();

  //: Step to the nth frame frame
  // \retval true if at next frame
  // \retval false if there were no more frames
  bool next_nth_frame(unsigned n);

  //: Rewind the video to the first frame to process
  void rewind();

  //: Seek to the given frame number
  void seek_to_frame(unsigned int frame);

  //: Return the position in the video as a value from 0.0 to 1.0
  float status() const;

  //: Write the status file
  // \param status ratio of completed to total processing (0.0 to 1.0)
  // \note is status is out of range (the default) then the status is
  //       the ratio of frames processed to total frames
  bool write_status(float status = -1.0) const;

  //: Write the performance file
  bool write_performance(vcl_string category = "car") const;

  //: Write the parameters to an XML file
  bool write_params();

  //: Set the performance for the current frame
  void set_performance(double false_pos, double false_neg);

  //: return the number of frames in the entire video
  int video_size();

  vcl_string get_output_dir();
  vcl_string get_output_video_dir();
  vcl_string get_output_ps_dir();
  vcl_string get_output_vrml_dir();
  vcl_string get_output_svg_dir();

  vcl_string get_video_assoc_dir();

  //: return the video id which is the parent video of the input object (or input video itself)
  int get_video_id();
  //: return the video file id (which is the necessary one to reach path)
  int get_video_fileid();
  //: return the id of input object
  int get_object_id();

  //: Return the first frame number to process
  int first_frame() const { return first_frame_; }
  //: Return the last frame number to process
  int last_frame() const { return last_frame_; }

  //: Return start frame parameter that is passed to the job
  int start_frame();
  //: Return end frame parameter that is passed to the job
  int end_frame();
  //: return the number of frames to skip
  int get_no_skip_frames();
 private:
  //: Initialize the arguments from the process
  void init_args(const bpro_parameters_sptr& param);
  
  //: Parse the command line parameters for the process
  void update_process_params();

 protected:
  //: Default Constructor
  vorl_manager();

  //: The vector of processes used in the argument list
  vcl_vector<bpro_process_sptr> processes_;

  //: The name of this application
  vcl_string app_name_;

  //: The system information for command line
  vcl_vector<vcl_pair<vcl_string,vcl_string> > info_;

  //: The performance scores
  vcl_vector<vcl_pair<double,double> > performance_scores_;

  //: Program argument list common to all
  vul_arg_info_list vorl_args_;
  //: Program argument list from process
  vul_arg_info_list process_args_;
  // VORL common arguments
  // -------------------------------------------------
  vul_arg<vcl_string> param_file_;
  vul_arg<vcl_string> video_file_;
  vul_arg<vcl_string> video_assoc_dir_;
  vul_arg<vcl_string> status_file_;
  vul_arg<vcl_string> perf_file_;
  vul_arg<vcl_string> output_dir_;
  vul_arg<vcl_string> out_video_dir_;
  vul_arg<vcl_string> out_ps_dir_;
  vul_arg<vcl_string> out_vrml_dir_;
  vul_arg<vcl_string> out_svg_dir_;
  vul_arg<int>        start_frame_;
  vul_arg<int>        end_frame_;
  vul_arg<int>        video_id_;
  vul_arg<int>        video_fileid_;
  vul_arg<int>        object_id_;
  vul_arg<bool>       params_only_;
  vul_arg<int>        skip_frames_;
  // -------------------------------------------------

  int first_frame_;
  int last_frame_;

 private:
  //: Initialize the static instance
  void initialize();
  
  //: A static instance of the manager
  static vorl_manager * instance_;

};

#endif // vorl_manager_h_










