// This is breye1/vorl1/vorl1_manager.cxx
//:
// \file



#include "vorl1_manager.h"
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>



//static manager instance 
vorl1_manager* vorl1_manager::instance_ = 0;


//: Insure only one instance is created
vorl1_manager *vorl1_manager::instance()
{
  if (!instance_){
    instance_ = new vorl1_manager();
    instance_->initialize();
  }
  return vorl1_manager::instance_;
}


//: Initialize the instance
void
vorl1_manager::initialize()
{
  process_manager_.get_repository_sptr()->initialize(1);
}


//: Constructor
vorl1_manager::vorl1_manager()
 : param_file_(    vorl1_args_, "--vorl1_param",
                     "parameter output file",     ""),
   video_file_(    vorl1_args_, "--vorl1_video",
                     "video input file",          ""),
   video_assoc_dir_(    vorl1_args_, "--vorl1_video_assoc",
                     "video input associated dir",          ""),
   status_file_(   vorl1_args_, "--vorl1_status",
                     "status block file",         ""),
   perf_file_(     vorl1_args_, "--vorl1_perform",
                     "performance output file",   ""),
   output_dir_(  vorl1_args_, "--vorl1_out_dir",
                     "output directory",          ""),
   out_video_dir_( vorl1_args_, "--vorl1_out_video",
                     "results video directory",   ""),
   out_ps_dir_( vorl1_args_, "--vorl1_out_ps",
                     "results ps directory",   ""),
   out_vrml_dir_( vorl1_args_, "--vorl1_out_vrml",
                     "results vrml directory",   ""),
   out_svg_dir_( vorl1_args_, "--vorl1_out_svg",
                     "results svg directory",   ""),
   start_frame_( vorl1_args_, "--vorl1_start_frame",
                     "Frame to start processing", 0),
   end_frame_( vorl1_args_, "--vorl1_end_frame",
                     "Frame to end processing",   -1),
   video_id_( vorl1_args_, "--vorl1_video_id",
                     "Id of input video",   0),
   video_fileid_( vorl1_args_, "--vorl1_video_fileid",
                     "Id of input video file",   0),
   object_id_( vorl1_args_, "--vorl1_object_id",
                     "Id of input object",   0),
   skip_frames_( vorl1_args_, "--skip_frames",
                     "no of frames to skip",   1),
   params_only_(   vorl1_args_, "-print_params_only",
                     "print param file and exit", false)
{
}


//: Add the process to the processing queue
void 
vorl1_manager::add_process_to_queue(const bpro1_process_sptr& process)
{
  process_manager_.add_process_to_queue(process);
}


//: Remove the last process from the queue
void
vorl1_manager::delete_last_process()
{
  process_manager_.delete_last_process();
}


//: Remove all processes from the queue
void 
vorl1_manager::clear_process_queue()
{
  process_manager_.clear_process_queue();
}


//: Add the parameters of process to the program arguments list
void 
vorl1_manager::add_process_to_args(const bpro1_process_sptr& process)
{
  init_args(process->parameters());
  processes_.push_back(process);
}


//: Return the current frame number
unsigned int
vorl1_manager::current_frame() const
{
  return process_manager_.get_repository_sptr()->current_frame();
}


//: Step to the next frame
// \retval true if at next frame
// \retval false if there were no more frames
bool
vorl1_manager::next_frame()
{
  if( process_manager_.get_repository_sptr()->current_frame() >= last_frame_ )
    return false;
  return process_manager_.get_repository_sptr()->go_to_next_frame();
}


bool
vorl1_manager::next_nth_frame(unsigned  n)
{
  if( process_manager_.get_repository_sptr()->current_frame()>= last_frame_  || 
      process_manager_.get_repository_sptr()->current_frame()+n>= last_frame_ )
    return false;
  return process_manager_.get_repository_sptr()->go_to_next_n_frame(n);
}


//: Rewind the video to the first frame to process
void 
vorl1_manager::rewind()
{
  process_manager_.get_repository_sptr()->go_to_frame(first_frame_);
}


//: Seek to the given frame number
void
vorl1_manager::seek_to_frame(unsigned int frame)
{
  process_manager_.get_repository_sptr()->go_to_frame(frame);
}


//: Parse the command line parameters
void
vorl1_manager::parse_params(int argc, char** argv)
{
  vul_arg_include(vorl1_args_);
  //vul_arg_include(process_args_);
  vul_arg_parse(argc, argv);

  vcl_cout << "Writing params file" << vcl_endl;
  this->write_params();

  if (params_only_()){
    vcl_exit(0);
  }
  
  update_process_params();
}

//: Load the input video
void 
vorl1_manager::load_video()
{
  bpro1_process_sptr load_video_proc = new vidpro1_load_video_process;

  process_manager_.register_process(load_video_proc);
  load_video_proc->set_output_names(vcl_vector<vcl_string>(1,"video"));

  if(!video_file_.set()){
    vcl_cerr << "Error: The video file was not specified" << vcl_endl;
    vcl_exit(-1);
  }

  load_video_proc->parameters()->set_value("-video_filename",bpro1_filepath(video_file_()));
  process_manager_.run_process_on_current_frame(load_video_proc);

  vidpro1_repository_sptr rep = process_manager_.get_repository_sptr();
  first_frame_ = 0;
  last_frame_ = rep->num_frames()-1;

  if(start_frame_.set() && start_frame_() > 0 && start_frame_() <= last_frame_)
    first_frame_ = start_frame_();
  if(end_frame_.set() && end_frame_() >= first_frame_ && end_frame_() < last_frame_)
    last_frame_ = end_frame_();

  rep->go_to_frame(first_frame_);
}

//: Load the input video from start frame to end frame
void 
vorl1_manager::load_video_clip()
{
  bpro1_process_sptr load_video_proc = new vidpro1_load_video_process;

  process_manager_.register_process(load_video_proc);
  load_video_proc->set_output_names(vcl_vector<vcl_string>(1,"video"));

  if(!video_file_.set()){
    vcl_cerr << "Error: The video file was not specified" << vcl_endl;
    vcl_exit(-1);
  }

  //: now its loading just (end_frame-start_frame+1) frames
  load_video_proc->parameters()->set_value("-video_filename",bpro1_filepath(video_file_()));
  load_video_proc->parameters()->set_value("-isclipped",true);
  load_video_proc->parameters()->set_value("-sf",start_frame_());
  load_video_proc->parameters()->set_value("-ef",end_frame_());

  process_manager_.run_process_on_current_frame(load_video_proc);

  vidpro1_repository_sptr rep = process_manager_.get_repository_sptr();
  first_frame_ = 0;
  last_frame_ = rep->num_frames()-1;

  rep->go_to_frame(first_frame_);
}


//: Runs the process on the current frame
// \param process is the specific process to run
// \param modified is an optional parameter.  If specified, all storage objects
//                 modified by the process are added to the set
bool 
vorl1_manager::run_process_on_current_frame( const bpro1_process_sptr& process,
                                            vcl_set<bpro1_storage_sptr>* modified )
{
  return process_manager_.run_process_on_current_frame( process, modified );
}


//: Runs the process queue on the current frame
// \param modified is an optional parameter.  If specified, all storage objects
//                 modified by the process are added to the set
bool 
vorl1_manager::run_process_queue_on_current_frame ( vcl_set<bpro1_storage_sptr>* modified )
{
  return process_manager_.run_process_queue_on_current_frame( modified );
}


//: Calls the finish function on all processes in the queue
// \param modified is an optional parameter.  If specified, all storage objects
//                 modified by the process are added to the set
bool 
vorl1_manager::finish_process_queue( vcl_set<bpro1_storage_sptr>* modified )
{
  return process_manager_.finish_process_queue( first_frame_, last_frame_,
                                                modified );
}


//: Calls the finish function on all processes in the queue
// \param modified is an optional parameter.  If specified, all storage objects
//                 modified by the process are added to the set
bool 
vorl1_manager::finish_process_queue( int first, int last,
                                    vcl_set<bpro1_storage_sptr>* modified )
{
  return process_manager_.finish_process_queue( first,last,modified );
}


//: Parse the command line parameters for the process
void
vorl1_manager::update_process_params()
{
  for ( vcl_vector<vul_arg_base*>::iterator a_itr = process_args_.args_.begin();
        a_itr != process_args_.args_.end(); ++a_itr )
  {
    vcl_string option((*a_itr)->option());
    bpro1_parameters_sptr params = NULL;
    typedef vcl_vector<bpro1_process_sptr>::iterator proc_iterator;
    for ( proc_iterator p_itr = processes_.begin(); 
          p_itr != processes_.end(); ++p_itr )  
    {
      bpro1_parameters_sptr test_params = (*p_itr)->parameters();
      if(test_params->valid_parameter(option)){
        params = test_params;
        break;
      }     
    }

    if(!params){
      vcl_cerr << "Warning: couldn't find process for " << option << vcl_endl;
      continue;
    }

    // hopefully you aren't still using a compiler that does
    // not support dynamic_cast !

    vul_arg<int>* arg_int = dynamic_cast<vul_arg<int>*>(*a_itr);
    if (arg_int){
      params->set_value(arg_int->option(), (*arg_int)() );
      continue;
    }

    vul_arg<float>* arg_float = dynamic_cast<vul_arg<float>*>(*a_itr);
    if (arg_float){
      params->set_value(arg_float->option(), (*arg_float)() );
      continue;
    }

    vul_arg<bool>* arg_bool = dynamic_cast<vul_arg<bool>*>(*a_itr);
    if (arg_bool){
      params->set_value(arg_bool->option(), (*arg_bool)() );
      continue;
    }

    vul_arg<vcl_string>* arg_string = dynamic_cast<vul_arg<vcl_string>*>(*a_itr);
    if (arg_string){
      if(params->valid_parameter_type(option, vcl_string())){
        params->set_value(arg_string->option(), (*arg_string)() );
        continue;
      }
      if(params->valid_parameter_type(option, bpro1_filepath())){
        // prepend the path to associated files to this file name
  //      params->set_value(arg_string->option(), 
   //                       bpro1_filepath(get_video_assoc_dir()+"/"+(*arg_string)()) );
                params->set_value(arg_string->option(), bpro1_filepath((*arg_string)()) );

        continue;
      }  
    }

    vcl_cerr << "Warning: could not determine parameter type for "<< (*a_itr)->option() << vcl_endl;
  }
}


//: Initialize the arguments from the process
void
vorl1_manager::init_args(const bpro1_parameters_sptr& param)
{
  typedef vcl_vector< bpro1_param* > param_vector;
  param_vector params = param->get_param_list();
  for(param_vector::iterator p_itr = params.begin(); p_itr != params.end(); ++p_itr){

    if( bpro1_param_type<int> * param = dynamic_cast<bpro1_param_type<int> *>(*p_itr) ) {
      process_args_.add(new vul_arg<int>( param->name().c_str(),
                                          param->description().c_str(),
                                          param->value() ));
    }
    else if( bpro1_param_type<unsigned> * param = dynamic_cast<bpro1_param_type<unsigned> *>(*p_itr) ) {
      process_args_.add(new vul_arg<unsigned>( param->name().c_str(),
                        param->description().c_str(),
                        param->value() ));
    }
    else if( bpro1_param_type<float> * param = dynamic_cast<bpro1_param_type<float> *>(*p_itr) ) {
      process_args_.add(new vul_arg<float>( param->name().c_str(),
                                            param->description().c_str(),
                                            param->value() ));
    }
    else if( bpro1_param_type<double> * param = dynamic_cast<bpro1_param_type<double> *>(*p_itr) ) {
      process_args_.add(new vul_arg<double>( param->name().c_str(),
                        param->description().c_str(),
                        param->value() ));
    }
    else if( bpro1_param_type<vcl_string> * param = dynamic_cast<bpro1_param_type<vcl_string> *>(*p_itr) ) {
      process_args_.add(new vul_arg<vcl_string>( param->name().c_str(),
                                                 param->description().c_str(),
                                                 param->value() ));
    }
    else if( bpro1_param_type<bool> * param = dynamic_cast<bpro1_param_type<bool> *>(*p_itr) ) {
      process_args_.add(new vul_arg<bool>( param->name().c_str(),
                                           param->description().c_str(),
                                           param->value() ));
    }
    else if( bpro1_param_type<bpro1_filepath> * param = dynamic_cast<bpro1_param_type<bpro1_filepath> *>(*p_itr) ) {
      process_args_.add(new vul_arg<vcl_string>( param->name().c_str(),
                                                 param->description().c_str(),
                                                 param->value().path ));
    }
    else{
      vcl_cerr << "No valid argument type for parameter: " << (*p_itr)->name() << vcl_endl;
    }
  }
}


//: Set the performance for the current frame
void
vorl1_manager::set_performance(double true_pos, double true_neg)
{
  if ((true_neg == -1)){
    vcl_cerr << " Error: invalid performance true_neg score of -1, reset to 0" << vcl_endl;
    true_neg=0;
  }
    if ((true_pos == -1 )){
    vcl_cerr << " Error: invalid performance true_pos score of -1, reset to 0" << vcl_endl;
    true_pos=0;

  }

  double false_pos = 1 - true_pos;
  double false_neg = 1 - true_neg;
 performance_scores_.push_back(vcl_pair<double,double>(false_pos,false_neg));
}

//: Get the output directory.
vcl_string
vorl1_manager::get_output_dir()
{
  if (!output_dir_.set()){
    vcl_cerr << "Error: The output directory was not specified" << vcl_endl;
    return "";
  }
    return(output_dir_());
}
//: Get the output ps directory.
vcl_string
vorl1_manager::get_output_ps_dir()
{
  if (!out_ps_dir_.set()){
    vcl_cerr << "Error: The output ps  directory was not specified" << vcl_endl;
    return "";
  }
    return(out_ps_dir_());
}
//: Get the output vrml directory.
vcl_string
vorl1_manager::get_output_vrml_dir()
{
  if (!out_vrml_dir_.set()){
    vcl_cerr << "Error: The output vrml directory was not specified" << vcl_endl;
    return "";
  }
    return(out_vrml_dir_());
}
vcl_string
vorl1_manager::get_output_svg_dir()
{
  if (!out_svg_dir_.set()){
    vcl_cerr << "Error: The output svg directory was not specified" << vcl_endl;
    return "";
  }
    return(out_svg_dir_());
}

//: Get the output video directory.
vcl_string
vorl1_manager::get_output_video_dir()
{
  if (!out_video_dir_.set()){
    vcl_cerr << "Error: The output video directory was not specified" << vcl_endl;
    return "";
  }
    return(out_video_dir_());
}
//: Get the input assoc video directory.
vcl_string
vorl1_manager::get_video_assoc_dir()
{
  if (!video_assoc_dir_.set()){
    vcl_cerr << "Error: The assoc video dir was not specified" << vcl_endl;
    return "";
  }
    return(video_assoc_dir_());
}

//: return the video id which is the parent video of the input object (or input video itself)
int vorl1_manager::get_video_id() {
  if (!video_id_.set()){
    vcl_cerr << "Error: The video id was not specified" << vcl_endl;
    return 0;
  }
    return(video_id_());
}
  
//: return the video file id (which is the necessary one to reach path)
int vorl1_manager::get_video_fileid() {
  if (!video_fileid_.set()){
    vcl_cerr << "Error: The video file id was not specified" << vcl_endl;
    return 0;
  }
    return(video_fileid_());
}
  
//: return the id of input object
int vorl1_manager::get_object_id() {
  if (!object_id_.set()){
    vcl_cerr << "Error: The object id was not specified" << vcl_endl;
    return 0;
  }
    return(object_id_());
}

//: return the no of frames to skip
int vorl1_manager::get_no_skip_frames() {
  return skip_frames_();
}

//: Return start frame parameter that is passed to the job
int vorl1_manager::start_frame() {
  if (!start_frame_.set()){
    vcl_cerr << "Error: The start frame was not specified" << vcl_endl;
    return 0;
  }
    return(start_frame_());
}

//: Return end frame parameter that is passed to the job
int vorl1_manager::end_frame() {
  if (!end_frame_.set()){
    vcl_cerr << "Error: The end frame was not specified" << vcl_endl;
    return 0;
  }
    return(end_frame_());
}

//: Return the position in the video as a value from 0.0 to 1.0
float 
vorl1_manager::status() const
{
  int num_frames = last_frame_ - first_frame_ + 1;
  int curr_frame = this->repository()->current_frame() - first_frame_;
  return float(curr_frame)/float(num_frames);
}


//: Write the status file
// \param status ratio of completed to total processing (0.0 to 1.0)
// \note is status is out of range (the default) then the status is
//       the ratio of frames processed to total frames
bool
vorl1_manager::write_status(float status) const
{    
  if (!status_file_.set())
    return false;

  vcl_ofstream outstream(status_file_().c_str());
  if (!outstream){
    vcl_cerr << "error: could not create performance output file ["<<status_file_()<<"]\n";
    return false;
  }

  int num_frames = last_frame_ - first_frame_ + 1;
  int curr_frame = this->repository()->current_frame() - first_frame_;

  if( status<0.0f || status>1.0f )
    status = this->status();
 
  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<status>\n"
            << "  <basic>\n"
            << "    <info percent_completed=\""<<status*100.0f<<"\"/>\n"
            << "  </basic>\n"
            << "  <optional>\n"
            << "    <info number_of_frames_processed=\""<<curr_frame<<"\"/>\n"
            << "    <info total_number_of_frames=\""<<num_frames<<"\"/>\n"
            << "  </optional>\n"
            << "</status>\n";

  outstream.close();
  return true;
}


//: Write the performance file
bool
vorl1_manager::write_performance(vcl_string category) const
{
  if (!perf_file_.set()){
    vcl_cerr << "Error: The performance file was not specified" << vcl_endl;
    return false;
  }
  vcl_ofstream outstream(perf_file_().c_str());   
  if (!outstream)
  {
    vcl_cerr << "Error: could not create the performance file ["<<perf_file_()<<"]" << vcl_endl;
    return false;
  }

  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" 
            << "<performance>\n"
            << "  <description>Video Frame Scores</description>\n"
            << "  <frames>\n"
            << "    <video name=\""<< video_file_() 
            <<         "\" totalframes=\"" << performance_scores_.size()<<"\">\n"
            << "      <category type=\""<<category<<"\">\n";
  for (unsigned int i=0; i<performance_scores_.size(); ++i){
    double true_pos = 1-performance_scores_[i].second; 
    outstream << "        <frame index=\""<<i<<"\" score=\""<< true_pos <<"\" false_pos=\""<<performance_scores_[i].first <<"\" "
              << "false_neg=\""<<performance_scores_[i].second <<"\" />\n";
  }

  outstream << "      </category>\n"
            << "    </video>\n"
            << "  </frames>\n"
            << "</performance>" <<vcl_endl;

  return true;
}


//: Write the parameters to an XML file
bool
vorl1_manager::write_params() 
{
  if (!param_file_.set()){
    vcl_cerr << "Error: The parameter file was not specified" << vcl_endl;
    return false;
  }
  vcl_ofstream outstream(param_file_().c_str());
  if (!outstream)
  {
    vcl_cerr << "Error: could not create param output file ["<<param_file_()<<"]" << vcl_endl;
    return false;
  }

  // Write header information
  int n_args = process_args_.args_.size() + vorl1_args_.args_.size();
  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<vxl>\n"
            << "  <params app=\""<<app_name_<<"\" nparams=\""<<n_args<<"\">\n";

  vul_arg_info_list all_args;
  all_args.include(vorl1_args_);
  all_args.include(process_args_);
  // Write the arguments
  for (vcl_vector<vul_arg_base*>::iterator arg_it = all_args.args_.begin();
       arg_it != all_args.args_.end(); ++arg_it)
  {
    vcl_string command = (*arg_it)->option();
    vcl_string description = (*arg_it)->help();
    vcl_string type = (*arg_it)->type_;
    if (type == "bool")
      type = "flag";

    // The complicated check is needed to determine if a "string" is
    // really a "file" (i.e. path to a file)
    if (type == "string"){
      typedef vcl_vector<bpro1_process_sptr>::iterator proc_iterator;
      for ( proc_iterator p_itr = processes_.begin(); 
            p_itr != processes_.end(); ++p_itr )  
      {
        bpro1_parameters_sptr params = (*p_itr)->parameters();
        if(params->valid_parameter(command)){
          if(params->valid_parameter_type(command, bpro1_filepath())){
 //           type = "file";
              type = "assocfile";
          }
          break;
        }     
      }

    }

    outstream << "    <param command=\"" << command << "\" description=\""
              << description << "\" type=\"" << type << '\"';
    if (command == "--vorl1_video")
      outstream << " System_info=\"INPUT_VIDEO\"";
    if (command == "--vorl1_start_frame")
      outstream << " System_info=\"INPUT_VIDEO_START_FRAME\"";
    if (command == "--vorl1_end_frame")
      outstream << " System_info=\"INPUT_VIDEO_END_FRAME\"";
    if (command == "--vorl1_video_id")
      outstream << " System_info=\"INPUT_VIDEO_ID\"";
    if (command == "--vorl1_video_fileid")
      outstream << " System_info=\"INPUT_VIDEO_FILEID\"";
    if (command == "--vorl1_object_id")
      outstream << " System_info=\"INPUT_OBJECT_ID\"";
    if (command == "--vorl1_video_assoc")
      outstream << " System_info=\"INPUT_VIDEO_ASSOC\"";
    if (command == "--vorl1_out_dir")
      outstream << " System_info=\"OUTPUT_DIRECTORY\"";
    if (command == "--vorl1_out_video")
      outstream << " System_info=\"OUTPUT_VIDEO\"";
    if (command == "--vorl1_out_ps")
      outstream << " System_info=\"OUTPUT_PS\"";
    if (command == "--vorl1_out_svg")
      outstream << " System_info=\"OUTPUT_SVG\"";
    if (command == "--vorl1_out_vrml")
      outstream << " System_info=\"OUTPUT_VRML\"";
    if (command == "--vorl1_status")
      outstream << " System_info=\"STATUS_BLOCK\"";
    if (command == "--vorl1_perform")
      outstream << " System_info=\"PERFORMANCE_OUTPUT\"";
    if (command == "--vorl1_param")
      outstream << " System_info=\"PARAM_BLOCK\"";
    outstream << " value=\"";
    // if arg is a string, we have to get rid of ' ' around value
    vcl_ostringstream value_stream;
    (*arg_it)->print_value(value_stream);
    if (type == "string" || type == "file"|| type == "assocfile"){
      vcl_string value_string = value_stream.str();

      value_string = value_stream.str();

      value_string.replace(value_string.find("'"),1,"");
      value_string.replace(value_string.find("'"),1,"");

      outstream << value_string;
    }
    else if (type == "flag")
    {
      vcl_string value_string = value_stream.str();
      if ((value_string == "not set") || (command == "-print_params_only"))
        value_string = "off";
      else if (value_string == "set")
        value_string = "on";
      else
        value_string = "unknown";
      outstream << value_string;
    }
    else
      outstream << value_stream.str();
    outstream << "\" />\n";
  }

  // Write footer information
  outstream << "  </params>\n"
            << "</vxl>\n";

  outstream.close();
  return true;
}

bool vorl1_manager::save_repository( const vcl_string& filename, 
                                    const vcl_set<vcl_string>& names )
{
    vsl_b_ofstream bfs(filename);
    vidpro1_repository_sptr save_rep;
    if( names.empty() )
      save_rep = process_manager_.get_repository_sptr();
    else{
      save_rep = new vidpro1_repository(*(process_manager_.get_repository_sptr()));
      save_rep->remove_all_except(names);
    }
    save_rep->print_summary();
    vsl_b_write(bfs, save_rep);
    bfs.close();

    return true;
}


//: return the number of frames in the entire video
int vorl1_manager::video_size()
{
 return process_manager_.get_repository_sptr()->num_frames();
}

